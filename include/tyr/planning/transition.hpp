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

#ifndef TYR_PLANNING_TRANSITION_HPP_
#define TYR_PLANNING_TRANSITION_HPP_

#include "tyr/common/types.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/grounder/declarations.hpp"
#include "tyr/planning/declarations.hpp"

namespace tyr::planning
{
/// @brief Apply the action in the given node to apply its successor node.
/// This involved computing the successor state of the state underlying the given node,
/// as well as computing the metric value of the successor node given the metric value and the state in the given node.
/// @tparam Task
/// @param node
/// @param action
/// @param state_fact_sets
/// @return
template<typename Task>
Node<Task> apply_action(Node<Task> node, View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>> action);
}

#endif