/*
 * Copyright (C) 2023 Dominik Drexler and Simon Stahlberg
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

#ifndef TYR_PLANNING_SEARCH_SPACE_HPP_
#define TYR_PLANNING_SEARCH_SPACE_HPP_

#include "tyr/common/segmented_vector.hpp"
#include "tyr/planning/applicability.hpp"
#include "tyr/planning/formatter.hpp"
#include "tyr/planning/node.hpp"
#include "tyr/planning/search_node.hpp"

namespace tyr::planning
{

template<TaskKind Kind, typename SearchNode>
    requires SearchNodeConcept<SearchNode, Kind>
NodeList<Kind> extract_node_trajectory(const SegmentedVector<SearchNode>& search_nodes,
                                       const SearchNode& final_search_node,
                                       const Node<Kind>& final_node,
                                       SuccessorGenerator<Kind>& successor_generator)
{
    auto trajectory = NodeList<Kind> {};
    trajectory.push_back(final_node);

    auto cur_search_node = &final_search_node;
    auto& state_repository = *successor_generator.get_state_repository();

    while (cur_search_node->parent_state != Index<State<Kind>>::max())
    {
        const auto parent_state_index = cur_search_node->parent_state;

        cur_search_node = &search_nodes.at(uint_t(cur_search_node->parent_state));

        trajectory.push_back(Node<Kind>(state_repository.get_registered_state(parent_state_index), cur_search_node->g_value));
    }

    std::reverse(trajectory.begin(), trajectory.end());

    return trajectory;
}

template<TaskKind Kind>
LabeledNodeList<Kind> extract_labeled_node_trajectory(const NodeList<Kind>& node_trajectory, SuccessorGenerator<Kind>& successor_generator)
{
    assert(!node_trajectory.empty());

    auto labeled_node_trajectory = LabeledNodeList<Kind> {};
    auto cur_node = node_trajectory.front();
    auto labeled_succ_nodes = std::vector<LabeledNode<Kind>> {};

    for (size_t i = 1; i < node_trajectory.size(); ++i)
    {
        successor_generator.get_labeled_successor_nodes(cur_node, labeled_succ_nodes);

        for (const auto& labeled_succ_node : labeled_succ_nodes)
        {
            if (labeled_succ_node.node == node_trajectory[i])
            {
                labeled_node_trajectory.push_back(labeled_succ_node);
                cur_node = labeled_succ_node.node;
                break;
            }
        }
    }

    return labeled_node_trajectory;
}

template<TaskKind Kind, typename SearchNode>
    requires SearchNodeConcept<SearchNode, Kind>
inline Plan<Kind> extract_total_ordered_plan(const SearchNode& final_search_node,
                                             const Node<Kind>& final_node,
                                             const SegmentedVector<SearchNode>& search_nodes,
                                             SuccessorGenerator<Kind>& successor_generator)
{
    const auto node_trajetory = extract_node_trajectory(search_nodes, final_search_node, final_node, successor_generator);

    auto labeled_node_trajectory = extract_labeled_node_trajectory(node_trajetory, successor_generator);

    return Plan<Kind>(node_trajetory.front(), std::move(labeled_node_trajectory));
}

}

#endif