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

#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"
//

#include "tyr/common/types.hpp"
#include "tyr/datalog/declarations.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/planning/action_executor.hpp"
#include "tyr/planning/applicability.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/ground_task/node.hpp"
#include "tyr/planning/ground_task/state.hpp"
#include "tyr/planning/ground_task/state_repository.hpp"
#include "tyr/planning/ground_task/unpacked_state.hpp"
#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/lifted_task/node.hpp"
#include "tyr/planning/lifted_task/state.hpp"
#include "tyr/planning/lifted_task/state_repository.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"

namespace tyr::planning
{

template<typename Task>
void process_effects(View<Index<formalism::planning::GroundAction>, formalism::planning::Repository> action,
                     UnpackedState<Task>& succ_unpacked_state,
                     StateContext<Task>& state_context,
                     DataList<formalism::planning::FDRFact<formalism::FluentTag>>& tmp_del_effects,
                     DataList<formalism::planning::FDRFact<formalism::FluentTag>>& tmp_add_effects)
{
    for (const auto cond_effect : action.get_effects())
    {
        if (is_applicable(cond_effect.get_condition(), state_context))
        {
            for (const auto fact : cond_effect.get_effect().get_facts())
                if (fact.get_value() == formalism::planning::FDRValue::none())
                    tmp_del_effects.push_back(fact.get_data());
                else
                    tmp_add_effects.push_back(fact.get_data());

            for (const auto numeric_effect : cond_effect.get_effect().get_numeric_effects())
                visit([&](auto&& arg) { succ_unpacked_state.set(arg.get_fterm().get_index(), evaluate(numeric_effect, state_context)); },
                      numeric_effect.get_variant());

            /// Collect the increment (total-cost) in the state_context
            if (cond_effect.get_effect().get_auxiliary_numeric_effect().has_value())
                state_context.auxiliary_value = evaluate(cond_effect.get_effect().get_auxiliary_numeric_effect().value(), state_context);
        }
    }
}

template<typename Task>
bool ActionExecutor::is_applicable(View<Index<formalism::planning::GroundAction>, formalism::planning::Repository> action, const StateContext<Task>& state)
{
    // Ensure that condition applicability was verified already.
    assert(tyr::planning::are_applicable_if_fires(action.get_effects(), state, m_effect_families)
           == tyr::planning::is_applicable(action, state, m_effect_families));

    return are_applicable_if_fires(action.get_effects(), state, m_effect_families);
}

template bool ActionExecutor::is_applicable(View<Index<formalism::planning::GroundAction>, formalism::planning::Repository> action,
                                            const StateContext<LiftedTask>& state);
template bool ActionExecutor::is_applicable(View<Index<formalism::planning::GroundAction>, formalism::planning::Repository> action,
                                            const StateContext<GroundTask>& state);

template<typename Task>
Node<Task> ActionExecutor::apply_action(const StateContext<Task>& state_context,
                                        View<Index<formalism::planning::GroundAction>, formalism::planning::Repository> action,
                                        StateRepository<Task>& state_repository)
{
    m_del_effects.clear();
    m_add_effects.clear();

    auto tmp_state_context = state_context;
    auto& task = tmp_state_context.task;

    auto succ_unpacked_state_ptr = state_repository.get_unregistered_state();
    auto& succ_unpacked_state = *succ_unpacked_state_ptr;
    succ_unpacked_state.assign_unextended_part(tmp_state_context.unpacked_state);

    process_effects(action, succ_unpacked_state, tmp_state_context, m_del_effects, m_add_effects);

    for (const auto fact : m_del_effects)
        succ_unpacked_state.set(fact);
    for (const auto fact : m_add_effects)
        succ_unpacked_state.set(fact);

    auto succ_state = state_repository.register_state(succ_unpacked_state_ptr);

    auto succ_state_context = StateContext { task, succ_unpacked_state, tmp_state_context.auxiliary_value };
    if (task.get_task().get_metric())
        succ_state_context.auxiliary_value = evaluate(task.get_task().get_metric().value().get_fexpr(), succ_state_context);
    else
        ++succ_state_context.auxiliary_value;  // Assume unit cost if no metric is given

    return Node<Task>(succ_state, succ_state_context.auxiliary_value);
}

template Node<LiftedTask> ActionExecutor::apply_action(const StateContext<LiftedTask>& state_context,
                                                       View<Index<formalism::planning::GroundAction>, formalism::planning::Repository> action,
                                                       StateRepository<LiftedTask>& state_repository);
template Node<GroundTask> ActionExecutor::apply_action(const StateContext<GroundTask>& state_context,
                                                       View<Index<formalism::planning::GroundAction>, formalism::planning::Repository> action,
                                                       StateRepository<GroundTask>& state_repository);
}
