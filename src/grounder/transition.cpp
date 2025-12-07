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

#include "tyr/planning/transition.hpp"

#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/planning/ground_numeric_effect_operator_utils.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/grounder/applicability.hpp"
#include "tyr/grounder/declarations.hpp"
#include "tyr/grounder/facts_view.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/node.hpp"
#include "tyr/planning/state.hpp"

namespace tyr::planning
{

inline void collect_effects(View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>> action,
                            const tyr::grounder::FactsView& facts_view,
                            boost::dynamic_bitset<>& positive_effects,
                            boost::dynamic_bitset<>& negative_effects,
                            std::vector<float_t>& numeric_variables)
{
    // TODO: collect add/delete and numeric effects

    for (const auto cond_effect : action.get_effects())
    {
        if (is_applicable(cond_effect.get_condition(), facts_view))
        {
            // TODO: collect add/delete and numeric effects
            for (const auto literal : cond_effect.get_effect().get_literals()) {}

            for (const auto numeric_effect : cond_effect.get_effect().get_numeric_effects()) {}

            if (cond_effect.get_effect().get_auxiliary_numeric_effect()) {}
        }
    }
}

/// @brief Apply the action in the given node to apply its successor node.
/// This involved computing the successor state of the state underlying the given node,
/// as well as computing the metric value of the successor node given the metric value and the state in the given node.
/// @tparam Task
/// @param node
/// @param action
/// @param state_fact_sets
/// @return
template<typename Task>
Node<Task> apply_action(Node<Task> node, View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>> action)
{
    const auto state = node.get_state();
    auto& task = node.get_task();
    const auto facts_view = grounder::FactsView(state.template get_atoms<formalism::StaticTag>(),
                                                state.template get_atoms<formalism::FluentTag>(),
                                                state.template get_atoms<formalism::DerivedTag>(),
                                                state.template get_numeric_variables<formalism::StaticTag>(),
                                                state.template get_numeric_variables<formalism::FluentTag>());

    /// --- Fetch a scratch buffer for creating the successor state.
    auto succ_unpacked_state_ptr = task.get_unpacked_state_pool().get_or_allocate();
    auto& succ_unpacked_state = *succ_unpacked_state_ptr;
    succ_unpacked_state.clear();

    // Copy state into mutable buffer
    succ_unpacked_state = state.get_unpacked_state();

    // TODO: collect positive and negative effects
    auto positive_effects = boost::dynamic_bitset<>();
    auto negative_effects = boost::dynamic_bitset<>();

    collect_effects(action, facts_view, positive_effects, negative_effects, succ_unpacked_state.get_numeric_variables());

    succ_unpacked_state.template get_atoms<formalism::FluentTag>() |= positive_effects;
    succ_unpacked_state.template get_atoms<formalism::FluentTag>() -= negative_effects;

    // TODO: add positive effects to state, then delete negative ones

    // TODO: apply numeric effects in parallel
}

template Node<LiftedTask> apply_action(Node<LiftedTask> node, View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>> action);
template Node<GroundTask> apply_action(Node<GroundTask> node, View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>> action);
}
