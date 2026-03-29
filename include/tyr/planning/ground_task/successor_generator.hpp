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

#include "tyr/planning/ground_task/node.hpp"        // for Node
#include "tyr/planning/ground_task/state_view.hpp"  // for State
//
#include "tyr/formalism/planning/ground_action_index.hpp"  // for Index
#include "tyr/formalism/planning/ground_action_view.hpp"
#include "tyr/planning/action_executor.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/successor_generator.hpp"

namespace tyr::planning
{

template<>
class SuccessorGenerator<GroundTag>
{
public:
    explicit SuccessorGenerator(std::shared_ptr<Task<GroundTag>> task, ExecutionContextPtr execution_context);

    static std::shared_ptr<SuccessorGenerator<GroundTag>> create(std::shared_ptr<Task<GroundTag>> task, ExecutionContextPtr execution_context);

    Node<GroundTag> get_initial_node();

    std::vector<LabeledNode<GroundTag>> get_labeled_successor_nodes(const Node<GroundTag>& node);
    void get_labeled_successor_nodes(const Node<GroundTag>& node, std::vector<LabeledNode<GroundTag>>& out_nodes);

    Node<GroundTag> get_successor_node(const Node<GroundTag>& node, formalism::planning::GroundActionView action);

    Node<GroundTag> get_node(Index<State<GroundTag>> state_index);

    /**
     * Expert API
     */

    const auto& get_state_repository() const noexcept { return m_state_repository; }

private:
    std::shared_ptr<Task<GroundTag>> m_task;

    IndexList<formalism::planning::GroundAction> m_applicable_actions;

    std::shared_ptr<StateRepository<GroundTag>> m_state_repository;

    ActionExecutor m_executor;
};

}

#endif
