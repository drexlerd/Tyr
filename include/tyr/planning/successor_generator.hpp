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

#ifndef TYR_PLANNING_SUCCESSOR_GENERATOR_HPP_
#define TYR_PLANNING_SUCCESSOR_GENERATOR_HPP_

#include "tyr/common/declarations.hpp"
#include "tyr/common/onetbb.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/node.hpp"
#include "tyr/planning/state_index.hpp"

namespace tyr::planning
{

template<TaskKind Kind>
class SuccessorGenerator
{
    static_assert(dependent_false<Kind>::value, "SuccessorGenerator is not defined for type T.");
};

template<typename T, typename Kind>
concept SuccessorGeneratorConcept = requires(T& r,
                                             std::shared_ptr<Task<Kind>> task,
                                             std::shared_ptr<ExecutionContext> execution_context,
                                             Index<State<Kind>> state_index,
                                             const Node<Kind>& node,
                                             std::vector<LabeledNode<Kind>>& labeled_successor_nodes,
                                             formalism::planning::GroundActionView action) {
    requires TaskKind<Kind>;
    { r.get_initial_node() } -> std::same_as<Node<Kind>>;
    { r.get_labeled_successor_nodes(node) } -> std::same_as<std::vector<LabeledNode<Kind>>>;
    { r.get_labeled_successor_nodes(node, labeled_successor_nodes) } -> std::same_as<void>;
    { r.get_successor_node(node, action) } -> std::same_as<Node<Kind>>;
    { r.get_node(state_index) } -> std::same_as<Node<Kind>>;
};

}

#endif
