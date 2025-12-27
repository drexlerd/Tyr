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

#include "tyr/planning/lifted_task/state_repository.hpp"

#include "../task_utils.hpp"           // for create...
#include "tyr/common/comparators.hpp"  // for operat...
#include "tyr/common/vector.hpp"       // for View
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/overlay_repository.hpp"    // for Overla...
#include "tyr/formalism/planning/fdr_context.hpp"  // for Binary...
#include "tyr/formalism/views.hpp"
#include "tyr/planning/lifted_task.hpp"                  // for Lifted...
#include "tyr/planning/lifted_task/axiom_evaluator.hpp"  // for AxiomE...
#include "tyr/planning/state_repository.hpp"             // for StateR...

#include <tuple>           // for operat...
#include <utility>         // for move
#include <valla/slot.hpp>  // for Slot

using namespace tyr::formalism;

namespace tyr::planning
{

StateRepository<LiftedTask>::StateRepository(std::shared_ptr<LiftedTask> task) :
    m_task(task),
    m_uint_nodes(),
    m_float_nodes(),
    m_nodes_buffer(),
    m_packed_states(),
    m_unpacked_state_pool(),
    m_axiom_evaluator(std::make_shared<AxiomEvaluator<LiftedTask>>(task))
{
}

State<LiftedTask> StateRepository<LiftedTask>::get_initial_state()
{
    auto unpacked_state = get_unregistered_state();

    for (const auto atom : m_task->get_task().get_atoms<FluentTag>())
        unpacked_state->set(m_task->get_fdr_context()->get_fact(atom.get_index()));

    for (const auto fterm_value : m_task->get_task().get_fterm_values<FluentTag>())
        unpacked_state->set(fterm_value.get_fterm().get_index(), fterm_value.get_value());

    return register_state(unpacked_state);
}

State<LiftedTask> StateRepository<LiftedTask>::get_registered_state(StateIndex state_index)
{
    const auto& packed_state = m_packed_states[state_index];

    auto unpacked_state = get_unregistered_state();

    unpacked_state->get_index() = state_index;
    fill_atoms(packed_state.template get_atoms<formalism::FluentTag>(),
               m_uint_nodes,
               m_nodes_buffer,
               unpacked_state->template get_atoms<formalism::FluentTag>());
    fill_atoms(packed_state.template get_atoms<formalism::DerivedTag>(),
               m_uint_nodes,
               m_nodes_buffer,
               unpacked_state->template get_atoms<formalism::DerivedTag>());
    fill_numeric_variables(packed_state.get_numeric_variables(), m_uint_nodes, m_float_nodes, m_nodes_buffer, unpacked_state->get_numeric_variables());

    return State<LiftedTask>(*m_task, std::move(unpacked_state));
}

SharedObjectPoolPtr<UnpackedState<LiftedTask>> StateRepository<LiftedTask>::get_unregistered_state()
{
    auto state = m_unpacked_state_pool.get_or_allocate();
    state->clear();

    return state;
}

State<LiftedTask> StateRepository<LiftedTask>::register_state(SharedObjectPoolPtr<UnpackedState<LiftedTask>> state)
{
    m_axiom_evaluator->compute_extended_state(*state);

    auto fluent_atoms = create_atoms_slot(state->template get_atoms<formalism::FluentTag>(), m_nodes_buffer, m_uint_nodes);
    auto derived_atoms = create_atoms_slot(state->template get_atoms<formalism::DerivedTag>(), m_nodes_buffer, m_uint_nodes);
    auto numeric_variables = create_numeric_variables_slot(state->get_numeric_variables(), m_nodes_buffer, m_uint_nodes, m_float_nodes);

    state->set(m_packed_states.insert(PackedState<LiftedTask>(StateIndex(m_packed_states.size()), fluent_atoms, derived_atoms, numeric_variables)));

    return State<LiftedTask>(*m_task, std::move(state));
}

static_assert(StateRepositoryConcept<StateRepository<LiftedTask>, LiftedTask>);

}
