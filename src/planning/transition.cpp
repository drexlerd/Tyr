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

#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/views.hpp"
//

#include "transition.hpp"
#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/common/types.hpp"
#include "tyr/common/variant.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/planning/ground_numeric_effect_operator_utils.hpp"
#include "tyr/grounder/declarations.hpp"
#include "tyr/planning/applicability.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/ground_task/node.hpp"
#include "tyr/planning/ground_task/state.hpp"
#include "tyr/planning/ground_task/unpacked_state.hpp"
#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/lifted_task/node.hpp"
#include "tyr/planning/lifted_task/state.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"

#include <boost/dynamic_bitset.hpp>

namespace tyr::planning
{

template<typename Task>
void process_effects(View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>> action,
                     UnpackedState<Task>& succ_unpacked_state,
                     StateContext<Task>& state_context)
{
    for (const auto cond_effect : action.get_effects())
    {
        if (is_applicable(cond_effect.get_condition(), state_context))
        {
            for (const auto fact : cond_effect.get_effect().get_facts())
                succ_unpacked_state.set(fact.get_data());

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
Node<Task> apply_action(const StateContext<Task>& state_context,
                        View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>> action)
{
    auto tmp_state_context = state_context;
    auto& task = tmp_state_context.task;

    auto succ_unpacked_state_ptr = task.get_unpacked_state_pool().get_or_allocate();
    auto& succ_unpacked_state = *succ_unpacked_state_ptr;
    succ_unpacked_state.assign_unextended_part(tmp_state_context.unpacked_state);
    succ_unpacked_state.clear_extended_part();

    process_effects(action, succ_unpacked_state, tmp_state_context);

    task.compute_extended_state(succ_unpacked_state);

    const auto succ_state_index = task.register_state(succ_unpacked_state);

    auto succ_state_context = StateContext { task, succ_unpacked_state, tmp_state_context.auxiliary_value };
    if (task.get_task().get_metric())
        succ_state_context.auxiliary_value = evaluate(task.get_task().get_metric().value().get_fexpr(), succ_state_context);
    else
        ++succ_state_context.auxiliary_value;  // Assume unit cost if no metric is given

    return Node<Task>(succ_state_index, succ_state_context.auxiliary_value, task);
}

template Node<LiftedTask> apply_action(const StateContext<LiftedTask>& state_context,
                                       View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>> action);
template Node<GroundTask> apply_action(const StateContext<GroundTask>& state_context,
                                       View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>> action);
}
