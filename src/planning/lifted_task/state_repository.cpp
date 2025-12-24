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

#include "../task_utils.hpp"
#include "tyr/planning/lifted_task.hpp"

using namespace tyr::formalism;

namespace tyr::planning
{

StateRepository<LiftedTask>::StateRepository(LiftedTask& task, formalism::BinaryFDRContext<formalism::OverlayRepository<formalism::Repository>> fdr_context) :
    m_task(task),
    m_fdr_context(std::move(fdr_context)),
    m_uint_nodes(),
    m_float_nodes(),
    m_nodes_buffer(),
    m_packed_states(),
    m_unpacked_state_pool()
{
}

State<LiftedTask> StateRepository<LiftedTask>::get_initial_state()
{
    auto unpacked_state = get_unregistered_state();

    for (const auto atom : m_task.get_task().get_atoms<FluentTag>())
        unpacked_state->set(m_fdr_context.get_fact(atom.get_index()));

    for (const auto fterm_value : m_task.get_task().get_fterm_values<FluentTag>())
        unpacked_state->set(fterm_value.get_fterm().get_index(), fterm_value.get_value());

    m_task.compute_extended_state(*unpacked_state);

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

    return State<LiftedTask>(m_task, std::move(unpacked_state));
}

SharedObjectPoolPtr<UnpackedState<LiftedTask>> StateRepository<LiftedTask>::get_unregistered_state()
{
    auto state = m_unpacked_state_pool.get_or_allocate();
    state->clear();

    return state;
}

State<LiftedTask> StateRepository<LiftedTask>::register_state(SharedObjectPoolPtr<UnpackedState<LiftedTask>> state)
{
    auto fluent_atoms = create_atoms_slot(state->template get_atoms<formalism::FluentTag>(), m_nodes_buffer, m_uint_nodes);
    auto derived_atoms = create_atoms_slot(state->template get_atoms<formalism::DerivedTag>(), m_nodes_buffer, m_uint_nodes);
    auto numeric_variables = create_numeric_variables_slot(state->get_numeric_variables(), m_nodes_buffer, m_uint_nodes, m_float_nodes);

    state->set(m_packed_states.insert(PackedState<LiftedTask>(StateIndex(m_packed_states.size()), fluent_atoms, derived_atoms, numeric_variables)));

    return State<LiftedTask>(m_task, std::move(state));
}

formalism::BinaryFDRContext<formalism::OverlayRepository<formalism::Repository>>& StateRepository<LiftedTask>::get_fdr_context() { return m_fdr_context; }

static_assert(StateRepositoryConcept<StateRepository<LiftedTask>, LiftedTask>);

}
