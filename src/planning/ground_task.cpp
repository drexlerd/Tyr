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

#include "tyr/planning/ground_task.hpp"

#include "tyr/common/comparators.hpp"                         // for operat...
#include "tyr/common/dynamic_bitset.hpp"                      // for set
#include "tyr/common/vector.hpp"                              // for View, set
#include "tyr/formalism/overlay_repository.hpp"               // for Overla...
#include "tyr/formalism/planning/fdr_context.hpp"             // for Genera...
#include "tyr/formalism/repository.hpp"                       // for Reposi...
#include "tyr/formalism/views.hpp"                            // for Index
#include "tyr/planning/ground_task/axiom_stratification.hpp"  // for compute...

#include <tuple>    // for operat...
#include <utility>  // for move

using namespace tyr::formalism;

namespace tyr::planning
{

GroundTask::GroundTask(DomainPtr domain,
                       RepositoryPtr m_repository,
                       OverlayRepositoryPtr<Repository> overlay_repository,
                       View<Index<FDRTask>, OverlayRepository<Repository>> fdr_task,
                       GeneralFDRContext<OverlayRepository<Repository>> fdr_context) :
    m_domain(std::move(domain)),
    m_repository(std::move(m_repository)),
    m_overlay_repository(std::move(overlay_repository)),
    m_fdr_task(fdr_task),
    m_static_atoms_bitset(),
    m_static_numeric_variables(),
    m_action_match_tree(match_tree::MatchTree<GroundAction>::create(m_fdr_task.get_ground_actions().get_data(), m_fdr_task.get_context())),
    m_axiom_match_tree_strata()
{
    // std::cout << m_fdr_task << std::endl;

    for (const auto atom : m_fdr_task.template get_atoms<StaticTag>())
        set(uint_t(atom.get_index()), true, m_static_atoms_bitset);

    for (const auto fterm_value : m_fdr_task.template get_fterm_values<StaticTag>())
        set(uint_t(fterm_value.get_fterm().get_index()), fterm_value.get_value(), m_static_numeric_variables, std::numeric_limits<float_t>::quiet_NaN());

    auto axiom_strata = compute_ground_axiom_stratification(m_fdr_task);

    for (const auto& stratum : axiom_strata.data)
        m_axiom_match_tree_strata.emplace_back(match_tree::MatchTree<GroundAxiom>::create(stratum, m_fdr_task.get_context()));
}

template<FactKind T>
size_t GroundTask::get_num_atoms() const noexcept
{
    return m_fdr_task.template get_atoms<T>().size();
}

template size_t GroundTask::get_num_atoms<FluentTag>() const noexcept;
template size_t GroundTask::get_num_atoms<DerivedTag>() const noexcept;

size_t GroundTask::get_num_actions() const noexcept { return m_fdr_task.get_ground_actions().size(); }

size_t GroundTask::get_num_axioms() const noexcept { return m_fdr_task.get_ground_axioms().size(); }
}
