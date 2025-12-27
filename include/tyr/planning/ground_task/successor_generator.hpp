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

#ifndef TYR_PLANNING_GROUND_TASK_SUCCESSOR_GENERATOR_HPP_
#define TYR_PLANNING_GROUND_TASK_SUCCESSOR_GENERATOR_HPP_

#include "tyr/planning/ground_task/node.hpp"   // for Node
#include "tyr/planning/ground_task/state.hpp"  // for State
//
#include "tyr/formalism/planning/ground_action_index.hpp"  // for Index
#include "tyr/planning/action_executor.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task/match_tree/declarations.hpp"  // for Matc...
#include "tyr/planning/successor_generator.hpp"

namespace tyr::planning
{

template<>
class SuccessorGenerator<GroundTask>
{
public:
    explicit SuccessorGenerator(std::shared_ptr<GroundTask> task);

    Node<GroundTask> get_initial_node();

    std::vector<LabeledNode<GroundTask>> get_labeled_successor_nodes(const Node<GroundTask>& node);

    void get_labeled_successor_nodes(const Node<GroundTask>& node, std::vector<LabeledNode<GroundTask>>& out_nodes);

    State<GroundTask> get_state(StateIndex state_index);

private:
    std::shared_ptr<GroundTask> m_task;

    match_tree::MatchTreePtr<formalism::GroundAction> m_action_match_tree;
    IndexList<formalism::GroundAction> m_applicable_actions;

    std::shared_ptr<StateRepository<GroundTask>> m_state_repository;

    ActionExecutor m_executor;
};

}

#endif
