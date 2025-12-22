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

#ifndef TYR_PLANNING_GROUND_TASK_AXIOM_SCHEDULER_HPP_
#define TYR_PLANNING_GROUND_TASK_AXIOM_SCHEDULER_HPP_

#include "tyr/formalism/ground_atom_index.hpp"
#include "tyr/formalism/ground_atom_view.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/planning/ground_axiom_index.hpp"
#include "tyr/formalism/planning/ground_axiom_view.hpp"
//
#include "tyr/planning/ground_task/axiom_listeners.hpp"
#include "tyr/planning/ground_task/stratification.hpp"

#include <boost/dynamic_bitset.hpp>

namespace tyr::planning
{

class GroundAxiomSchedulerStratum
{
public:
    GroundAxiomSchedulerStratum(const GroundAxiomStratum& axioms,
                                const GroundAxiomListenerStratum& listeners,
                                const formalism::OverlayRepository<formalism::Repository>& context);

    void activate_all();

    void on_start_iteration() noexcept;

    void on_generate(Index<formalism::GroundAtom<formalism::DerivedTag>> atom);

    void on_finish_iteration();

    View<IndexList<formalism::GroundAxiom>, formalism::OverlayRepository<formalism::Repository>> get_active_axioms();

private:
    const GroundAxiomStratum& m_axioms;
    const GroundAxiomListenerStratum& m_listeners;
    const formalism::OverlayRepository<formalism::Repository>& m_context;

    boost::dynamic_bitset<> m_active_atoms;
    UnorderedSet<Index<formalism::GroundAxiom>> m_active_set;  ///< build active set
    IndexList<formalism::GroundAxiom> m_active;                ///< final active set
};

struct GroundAxiomSchedulerStrata
{
    std::vector<GroundAxiomSchedulerStratum> data;
};

extern GroundAxiomSchedulerStrata create_axiom_scheduler_strata(const GroundAxiomStrata& rules,
                                                                const GroundAxiomListenerStrata& listeners,
                                                                const formalism::OverlayRepository<formalism::Repository>& context);

}

#endif