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

#include "tyr/planning/ground_task/state_repository.hpp"

#include "../task_utils.hpp"                             // for create...
#include "tyr/common/comparators.hpp"                    // for operat...
#include "tyr/common/vector.hpp"                         // for View
#include "tyr/formalism/declarations.hpp"                // for Index
#include "tyr/formalism/overlay_repository.hpp"          // for Overla...
#include "tyr/formalism/views.hpp"                       // for View
#include "tyr/planning/ground_task.hpp"                  // for Ground...
#include "tyr/planning/ground_task/axiom_evaluator.hpp"  // for AxiomE...
#include "tyr/planning/state_repository.hpp"

#include <algorithm>                 // for fill
#include <assert.h>                  // for assert
#include <boost/dynamic_bitset.hpp>  // for dynami...
#include <gtl/phmap.hpp>             // for operat...
#include <tuple>                     // for operat...
#include <utility>                   // for move
#include <valla/slot.hpp>            // for Slot

using namespace tyr::formalism;

namespace tyr::planning
{

static auto create_fluent_layout(View<Index<formalism::FDRTask>, formalism::OverlayRepository<formalism::Repository>> fdr_task)
{
    auto ranges = std::vector<uint_t> {};
    for (const auto variable : fdr_task.get_fluent_variables())
    {
        // Ensure fluent variable indice are dense, i.e., 0,1,2,...
        assert(uint_t(variable.get_index()) == ranges.size());
        ranges.push_back(variable.get_domain_size());
    }

    return create_bit_packed_array_layout(ranges);
}

static auto create_derived_layout(View<Index<formalism::FDRTask>, formalism::OverlayRepository<formalism::Repository>> fdr_task)
{
    // Ensure derived atom indices are dense, i.e., 0,1,2,...
    for (uint_t i = 0; i < fdr_task.get_atoms<DerivedTag>().size(); ++i)
        assert(i == uint_t(fdr_task.get_atoms<DerivedTag>()[i].get_index()));

    return create_bitset_layout<uint_t>(fdr_task.get_atoms<DerivedTag>().size());
}

StateRepository<GroundTask>::StateRepository(std::shared_ptr<GroundTask> task) :
    m_task(task),
    m_fluent_layout(create_fluent_layout(m_task->get_task())),
    m_derived_layout(create_derived_layout(m_task->get_task())),
    m_uint_nodes(),
    m_float_nodes(),
    m_nodes_buffer(),
    m_packed_states(),
    m_fluent_repository(m_fluent_layout.total_blocks),
    m_derived_repository(m_derived_layout.total_blocks),
    m_fluent_buffer(m_fluent_layout.total_blocks),
    m_derived_buffer(m_derived_layout.total_blocks),
    m_unpacked_state_pool(),
    m_axiom_evaluator(std::make_shared<AxiomEvaluator<GroundTask>>(task))
{
}

State<GroundTask> StateRepository<GroundTask>::get_initial_state()
{
    auto unpacked_state = get_unregistered_state();

    for (const auto fact : m_task->get_task().get_fluent_facts())
        unpacked_state->set(fact.get_data());

    for (const auto fterm_value : m_task->get_task().get_fterm_values<FluentTag>())
        unpacked_state->set(fterm_value.get_fterm().get_index(), fterm_value.get_value());

    return register_state(unpacked_state);
}

State<GroundTask> StateRepository<GroundTask>::get_registered_state(StateIndex state_index)
{
    const auto& packed_state = m_packed_states[state_index];

    auto unpacked_state = get_unregistered_state();

    unpacked_state->get_index() = state_index;
    const auto fluent_ptr = m_fluent_repository[packed_state.get_facts<FluentTag>()];
    auto& fluent_values = unpacked_state->get_fluent_values();
    for (uint_t i = 0; i < m_fluent_layout.layouts.size(); ++i)
        fluent_values[i] = FDRValue { uint_t(BitPackedElementReference(m_fluent_layout.layouts[i], fluent_ptr)) };

    const auto derived_ptr = m_derived_repository[packed_state.get_facts<DerivedTag>()];
    auto& derived_atoms = unpacked_state->get_derived_atoms();
    for (uint_t i = 0; i < m_derived_layout.total_bits; ++i)
        derived_atoms[i] = bool(BitReference(i, derived_ptr));

    fill_numeric_variables(packed_state.get_numeric_variables(), m_uint_nodes, m_float_nodes, m_nodes_buffer, unpacked_state->get_numeric_variables());

    return State<GroundTask>(*m_task, std::move(unpacked_state));
}

SharedObjectPoolPtr<UnpackedState<GroundTask>> StateRepository<GroundTask>::get_unregistered_state()
{
    auto state = m_unpacked_state_pool.get_or_allocate();
    state->clear();

    state->resize_fluent_facts(m_task->get_task().get_fluent_variables().size());
    state->resize_derived_atoms(m_task->get_task().get_atoms<DerivedTag>().size());

    return state;
}

State<GroundTask> StateRepository<GroundTask>::register_state(SharedObjectPoolPtr<UnpackedState<GroundTask>> state)
{
    m_axiom_evaluator->compute_extended_state(*state);

    assert(m_fluent_buffer.size() == m_fluent_layout.total_blocks);
    assert(m_derived_buffer.size() == m_derived_layout.total_blocks);

    std::fill(m_fluent_buffer.begin(), m_fluent_buffer.end(), uint_t(0));
    for (uint_t i = 0; i < m_fluent_layout.layouts.size(); ++i)
        BitPackedElementReference(m_fluent_layout.layouts[i], m_fluent_buffer.data()) = uint_t(state->get_fluent_values()[i]);
    const auto fluent_facts_index = m_fluent_repository.insert(m_fluent_buffer);

    std::fill(m_derived_buffer.begin(), m_derived_buffer.end(), uint_t(0));
    for (uint_t i = 0; i < m_derived_layout.total_bits; ++i)
        BitReference(i, m_derived_buffer.data()) = state->get_derived_atoms().test(i);
    const auto derived_atoms_index = m_derived_repository.insert(m_derived_buffer);

    auto numeric_variables_slot = create_numeric_variables_slot(state->get_numeric_variables(), m_nodes_buffer, m_uint_nodes, m_float_nodes);

    state->set(
        m_packed_states.insert(PackedState<GroundTask>(StateIndex(m_packed_states.size()), fluent_facts_index, derived_atoms_index, numeric_variables_slot)));

    return State<GroundTask>(*m_task, std::move(state));
}

static_assert(StateRepositoryConcept<StateRepository<GroundTask>, GroundTask>);

}
