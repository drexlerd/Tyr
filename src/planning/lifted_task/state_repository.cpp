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

#include "tyr/common/comparators.hpp"  // for operat...
#include "tyr/common/vector.hpp"       // for View
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/fdr_context.hpp"  // for Binary...
#include "tyr/formalism/planning/views.hpp"
#include "tyr/planning/lifted_task.hpp"                  // for Lifted...
#include "tyr/planning/lifted_task/axiom_evaluator.hpp"  // for AxiomE...
#include "tyr/planning/state_repository.hpp"             // for StateR...
#include "tyr/planning/task_utils.hpp"                   // for create...

#include <tuple>           // for operat...
#include <utility>         // for move
#include <valla/slot.hpp>  // for Slot

namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;

namespace tyr::planning
{

StateRepository<LiftedTag>::StateRepository(std::shared_ptr<Task<LiftedTag>> task, ExecutionContextPtr execution_context) :
    m_task(task),
    m_context(),
    m_fluent_backend(m_context),
    m_derived_backend(m_context),
    m_numeric_backend(m_context),
    m_unpacked_state_pool(),
    m_axiom_evaluator(std::make_shared<AxiomEvaluator<LiftedTag>>(task, execution_context))
{
}

std::shared_ptr<StateRepository<LiftedTag>> StateRepository<LiftedTag>::create(std::shared_ptr<Task<LiftedTag>> task, ExecutionContextPtr execution_context)
{
    return std::make_shared<StateRepository<LiftedTag>>(std::move(task), std::move(execution_context));
}

StateView<LiftedTag> StateRepository<LiftedTag>::get_initial_state()
{
    auto unpacked_state = get_unregistered_state();

    for (const auto atom : m_task->get_task().get_atoms<f::FluentTag>())
        unpacked_state->set(m_task->get_fdr_context()->get_fact(atom.get_index()));

    for (const auto fterm_value : m_task->get_task().get_fterm_values<f::FluentTag>())
        unpacked_state->set(fterm_value.get_fterm().get_index(), fterm_value.get_value());

    return register_state(unpacked_state);
}

StateView<LiftedTag> StateRepository<LiftedTag>::get_registered_state(Index<State<LiftedTag>> state_index)
{
    const auto& packed_state = m_packed_states[state_index];

    auto unpacked_state = get_unregistered_state();

    unpacked_state->set(state_index);
    m_fluent_backend.unpack(packed_state.template get_atoms<f::FluentTag>(), unpacked_state->template get_atoms<f::FluentTag>());
    m_derived_backend.unpack(packed_state.template get_atoms<f::DerivedTag>(), unpacked_state->template get_atoms<f::DerivedTag>());
    m_numeric_backend.unpack(packed_state.get_numeric_variables(), unpacked_state->get_numeric_variables());

    return StateView<LiftedTag>(shared_from_this(), std::move(unpacked_state));
}

StateView<LiftedTag> StateRepository<LiftedTag>::create_state(const std::vector<Data<fp::FDRFact<f::FluentTag>>>& fluent_facts,
                                                              const std::vector<std::pair<Index<fp::GroundFunctionTerm<f::FluentTag>>, float_t>>& fterm_values)
{
    auto unpacked_state = get_unregistered_state();

    for (const auto& fact : fluent_facts)
        unpacked_state->set(fact);
    for (const auto& [fterm, value] : fterm_values)
        unpacked_state->set(fterm, value);

    return register_state(std::move(unpacked_state));
}

StateView<LiftedTag> StateRepository<LiftedTag>::create_state(const std::vector<fp::FDRFactView<f::FluentTag>>& fluent_facts,
                                                              const std::vector<std::pair<fp::GroundFunctionTermView<f::FluentTag>, float_t>>& fterm_values)
{
    auto unpacked_state = get_unregistered_state();

    for (const auto& fact : fluent_facts)
        unpacked_state->set(fact.get_data());
    for (const auto& [fterm, value] : fterm_values)
        unpacked_state->set(fterm.get_index(), value);

    return register_state(std::move(unpacked_state));
}

SharedObjectPoolPtr<UnpackedState<LiftedTag>> StateRepository<LiftedTag>::get_unregistered_state()
{
    auto state = m_unpacked_state_pool.get_or_allocate();
    state->clear();

    return state;
}

StateView<LiftedTag> StateRepository<LiftedTag>::register_state(SharedObjectPoolPtr<UnpackedState<LiftedTag>> state)
{
    m_axiom_evaluator->compute_extended_state(*state);

    state->set(m_packed_states
                   .insert(Data<State<LiftedTag>>(Index<State<LiftedTag>>(m_packed_states.size()),
                                                  m_fluent_backend.insert(state->template get_atoms<f::FluentTag>()),
                                                  m_derived_backend.insert(state->template get_atoms<f::DerivedTag>()),
                                                  m_numeric_backend.insert(state->get_numeric_variables())))
                   .first);

    return StateView<LiftedTag>(shared_from_this(), std::move(state));
}

size_t StateRepository<LiftedTag>::memory_usage() const noexcept
{
    size_t bytes = 0;
    bytes += m_context.memory_usage();
    bytes += m_packed_states.memory_usage();
    return bytes;
}

static_assert(StateRepositoryConcept<StateRepository<LiftedTag>, LiftedTag>);

}
