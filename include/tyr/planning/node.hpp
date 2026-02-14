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

#ifndef TYR_PLANNING_NODE_HPP_
#define TYR_PLANNING_NODE_HPP_

#include "tyr/common/config.hpp"
#include "tyr/common/declarations.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/planning/ground_action_view.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/planning/state.hpp"
#include "tyr/planning/state_index.hpp"

#include <concepts>
#include <ranges>

namespace tyr::planning
{
template<typename Task>
class Node
{
    static_assert(dependent_false<Task>::value, "Node is not defined for type T.");
};

template<typename Task>
using NodeList = std::vector<Node<Task>>;

template<typename Task>
struct LabeledNode
{
    View<Index<formalism::planning::GroundAction>, formalism::planning::Repository> label;
    Node<Task> node;
};

template<typename Task>
using LabeledNodeList = std::vector<LabeledNode<Task>>;

template<typename T, typename Task>
concept NodeConcept = requires(const T& cn) {
    { cn.get_state() } -> std::same_as<const State<Task>&>;
    { cn.get_task() } -> std::same_as<const Task&>;
    { cn.get_metric() } -> std::same_as<float_t>;
};
}

#endif
