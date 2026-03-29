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

#include "tyr/common/comparators.hpp"                    // for operat...
#include "tyr/common/vector.hpp"                         // for View
#include "tyr/formalism/planning/declarations.hpp"       // for Index
#include "tyr/formalism/planning/views.hpp"              // for View
#include "tyr/planning/ground_task.hpp"                  // for Ground...
#include "tyr/planning/ground_task/axiom_evaluator.hpp"  // for AxiomE...
#include "tyr/planning/state_repository.hpp"
#include "tyr/planning/task_utils.hpp"  // for create...

#include <algorithm>                 // for fill
#include <assert.h>                  // for assert
#include <boost/dynamic_bitset.hpp>  // for dynami...
#include <gtl/phmap.hpp>             // for operat...
#include <tuple>                     // for operat...
#include <utility>                   // for move
#include <valla/slot.hpp>            // for Slot

namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;

namespace tyr::planning
{

StateRepository<GroundTag>::StateRepository(std::shared_ptr<Task<GroundTag>> task, ExecutionContextPtr execution_context) :
    m_task(task),
    m_context(*m_task),
    m_fluent_backend(m_context),
    m_derived_backend(m_context),
    m_numeric_backend(m_context),
    m_packed_states(),
    m_unpacked_state_pool(),
    m_axiom_evaluator(std::make_shared<AxiomEvaluator<GroundTag>>(task, execution_context))
{
}

std::shared_ptr<StateRepository<GroundTag>> StateRepository<GroundTag>::create(std::shared_ptr<Task<GroundTag>> task, ExecutionContextPtr execution_context)
{
    return std::make_shared<StateRepository<GroundTag>>(std::move(task), std::move(execution_context));
}

StateView<GroundTag> StateRepository<GroundTag>::get_initial_state()
{
    auto unpacked_state = get_unregistered_state();

    for (const auto fact : m_task->get_task().get_fluent_facts())
        unpacked_state->set(fact.get_data());

    for (const auto fterm_value : m_task->get_task().get_fterm_values<f::FluentTag>())
        unpacked_state->set(fterm_value.get_fterm().get_index(), fterm_value.get_value());

    return register_state(unpacked_state);
}

StateView<GroundTag> StateRepository<GroundTag>::get_registered_state(Index<State<GroundTag>> state_index)
{
    const auto& packed_state = m_packed_states[state_index];

    auto unpacked_state = get_unregistered_state();

    unpacked_state->set(state_index);
    m_fluent_backend.unpack(packed_state.template get_atoms<f::FluentTag>(), unpacked_state->template get_atoms<f::FluentTag>());
    m_derived_backend.unpack(packed_state.template get_atoms<f::DerivedTag>(), unpacked_state->template get_atoms<f::DerivedTag>());
    m_numeric_backend.unpack(packed_state.get_numeric_variables(), unpacked_state->get_numeric_variables());

    return StateView<GroundTag>(shared_from_this(), std::move(unpacked_state));
}

StateView<GroundTag> StateRepository<GroundTag>::create_state(const std::vector<Data<fp::FDRFact<f::FluentTag>>>& fluent_facts,
                                                              const std::vector<std::pair<Index<fp::GroundFunctionTerm<f::FluentTag>>, float_t>>& fterm_values)
{
    auto unpacked_state = get_unregistered_state();

    for (const auto& fact : fluent_facts)
        unpacked_state->set(fact);
    for (const auto& [fterm, value] : fterm_values)
        unpacked_state->set(fterm, value);

    return register_state(std::move(unpacked_state));
}

StateView<GroundTag> StateRepository<GroundTag>::create_state(const std::vector<fp::FDRFactView<f::FluentTag>>& fluent_facts,
                                                              const std::vector<fp::GroundFunctionTermViewValuePair<f::FluentTag>>& fterm_values)
{
    auto unpacked_state = get_unregistered_state();

    for (const auto& fact : fluent_facts)
        unpacked_state->set(fact.get_data());
    for (const auto& [fterm, value] : fterm_values)
        unpacked_state->set(fterm.get_index(), value);

    return register_state(std::move(unpacked_state));
}

SharedObjectPoolPtr<UnpackedState<GroundTag>> StateRepository<GroundTag>::get_unregistered_state()
{
    auto state = m_unpacked_state_pool.get_or_allocate();
    state->clear();

    state->resize_fluent_facts(m_task->get_task().get_fluent_variables().size());
    state->resize_derived_atoms(m_task->get_task().get_atoms<f::DerivedTag>().size());

    return state;
}

StateView<GroundTag> StateRepository<GroundTag>::register_state(SharedObjectPoolPtr<UnpackedState<GroundTag>> state)
{
    m_axiom_evaluator->compute_extended_state(*state);

    state->set(m_packed_states
                   .insert(Data<State<GroundTag>>(Index<State<GroundTag>>(m_packed_states.size()),
                                                  m_fluent_backend.insert(state->template get_atoms<f::FluentTag>()),
                                                  m_derived_backend.insert(state->template get_atoms<f::DerivedTag>()),
                                                  m_numeric_backend.insert(state->get_numeric_variables())))
                   .first);

    return StateView<GroundTag>(shared_from_this(), std::move(state));
}

size_t StateRepository<GroundTag>::memory_usage() const noexcept
{
    size_t bytes = 0;
    bytes += m_context.memory_usage();
    bytes += m_packed_states.memory_usage();
    return bytes;
}

static_assert(StateRepositoryConcept<StateRepository<GroundTag>, GroundTag>);

}
