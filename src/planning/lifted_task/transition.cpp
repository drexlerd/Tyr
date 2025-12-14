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
#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/lifted_task/node.hpp"
#include "tyr/planning/lifted_task/state.hpp"

#include <boost/dynamic_bitset.hpp>

namespace tyr::planning
{

inline void process_effects(View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>> action,
                            Node<LiftedTask> node,
                            UnpackedState<LiftedTask>& succ_unpacked_state,
                            float_t& succ_metric_value)
{
    for (const auto cond_effect : action.get_effects())
    {
        if (is_applicable(cond_effect.get_condition(), node))
        {
            for (const auto fact : cond_effect.get_effect().get_facts())
                succ_unpacked_state.set(fact.get_data());

            for (const auto numeric_effect : cond_effect.get_effect().get_numeric_effects())
                visit([&](auto&& arg) { succ_unpacked_state.set(arg.get_fterm().get_index(), evaluate(numeric_effect, node)); }, numeric_effect.get_variant());

            if (cond_effect.get_effect().get_auxiliary_numeric_effect().has_value())
                succ_metric_value = evaluate(cond_effect.get_effect().get_auxiliary_numeric_effect().value(), node);
        }
    }
}

Node<LiftedTask> apply_action(Node<LiftedTask> node, View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>> action)
{
    const auto state = node.get_state();
    auto& task = node.get_task();

    /// --- Fetch a scratch buffer for creating the successor state.
    auto succ_unpacked_state_ptr = task.get_unpacked_state_pool().get_or_allocate();
    auto& succ_unpacked_state = *succ_unpacked_state_ptr;
    succ_unpacked_state.clear();

    // Copy state into mutable buffer
    succ_unpacked_state = state.get_unpacked_state();

    auto succ_metric_value = node.get_state_metric();

    process_effects(action, node, succ_unpacked_state, succ_metric_value);

    task.compute_extended_state(succ_unpacked_state);

    const auto succ_state_index = task.register_state(succ_unpacked_state);

    const auto succ_node = Node<LiftedTask>(succ_state_index, succ_metric_value, task);

    if (task.get_task().get_metric())
        succ_metric_value = evaluate(task.get_task().get_metric().value().get_fexpr(), succ_node);
    else
        ++succ_metric_value;  // Assume unit cost if no metric is given

    return Node<LiftedTask>(succ_state_index, succ_metric_value, task);
}
}
