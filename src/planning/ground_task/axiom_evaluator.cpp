/*
 * Copyright (C) 2025 Dominik Drexler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "tyr/planning/ground_task/axiom_evaluator.hpp"

#include "tyr/common/comparators.hpp"                         // for operator!=
#include "tyr/common/config.hpp"                              // for float_t
#include "tyr/formalism/overlay_repository.hpp"               // for Overlay...
#include "tyr/formalism/repository.hpp"                       // for Repository
#include "tyr/formalism/views.hpp"                            // for View
#include "tyr/planning/applicability.hpp"                     // for StateCo...
#include "tyr/planning/ground_task.hpp"                       // for GroundTask
#include "tyr/planning/ground_task/axiom_stratification.hpp"  // for compute...
#include "tyr/planning/ground_task/unpacked_state.hpp"

using namespace tyr::formalism;

namespace tyr::planning
{

AxiomEvaluator<GroundTask>::AxiomEvaluator(std::shared_ptr<GroundTask> task) : m_task(task), m_axiom_match_tree_strata(), m_applicable_axioms()
{
    auto axiom_strata = compute_ground_axiom_stratification(task->get_task());

    for (const auto& stratum : axiom_strata.data)
        m_axiom_match_tree_strata.emplace_back(match_tree::MatchTree<GroundAxiom>::create(stratum, task->get_task().get_context()));
}

void AxiomEvaluator<GroundTask>::compute_extended_state(UnpackedState<GroundTask>& unpacked_state)
{
    auto state_context = StateContext<GroundTask> { *m_task, unpacked_state, float_t(0) };

    for (const auto& match_tree : m_axiom_match_tree_strata)
    {
        while (true)
        {
            auto discovered_new_atom = bool { false };

            m_applicable_axioms.clear();
            match_tree->generate(state_context, m_applicable_axioms);

            for (const auto axiom : m_applicable_axioms)
            {
                const auto atom = make_view(axiom, *m_task->get_repository()).get_head().get_index();

                if (!unpacked_state.test(atom))
                    discovered_new_atom = true;

                unpacked_state.set(atom);
            }

            if (!discovered_new_atom)
                break;
        }
    }
}

}
