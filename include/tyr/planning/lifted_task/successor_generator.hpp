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

#ifndef TYR_PLANNING_LIFTED_TASK_SUCCESSOR_GENERATOR_HPP_
#define TYR_PLANNING_LIFTED_TASK_SUCCESSOR_GENERATOR_HPP_

#include "tyr/common/onetbb.hpp"
#include "tyr/datalog/policies/annotation.hpp"
#include "tyr/datalog/policies/termination.hpp"
#include "tyr/datalog/workspaces/program.hpp"
#include "tyr/formalism/planning/ground_action_view.hpp"
#include "tyr/planning/action_executor.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task/match_tree/declarations.hpp"  // for Matc...
#include "tyr/planning/lifted_task/axiom_evaluator.hpp"
#include "tyr/planning/lifted_task/node.hpp"
#include "tyr/planning/lifted_task/state_repository.hpp"
#include "tyr/planning/successor_generator.hpp"

namespace tyr::planning
{

template<>
class SuccessorGenerator<LiftedTag>
{
public:
    explicit SuccessorGenerator(std::shared_ptr<Task<LiftedTag>> task, ExecutionContextPtr execution_context);

    static std::shared_ptr<SuccessorGenerator<LiftedTag>> create(std::shared_ptr<Task<LiftedTag>> task, ExecutionContextPtr execution_context);

    Node<LiftedTag> get_initial_node();

    std::vector<LabeledNode<LiftedTag>> get_labeled_successor_nodes(const Node<LiftedTag>& node);
    void get_labeled_successor_nodes(const Node<LiftedTag>& node, std::vector<LabeledNode<LiftedTag>>& out_nodes);

    Node<LiftedTag> get_successor_node(const Node<LiftedTag>& node, formalism::planning::GroundActionView action);

    Node<LiftedTag> get_node(Index<State<LiftedTag>> state_index);

    /**
     * Expert API
     */

    const auto& get_state_repository() const noexcept { return m_state_repository; }
    const auto& get_workspace() const noexcept { return m_workspace; }

private:
    std::shared_ptr<Task<LiftedTag>> m_task;
    ExecutionContextPtr m_execution_context;

    datalog::ProgramWorkspace<datalog::NoOrAnnotationPolicy, datalog::NoAndAnnotationPolicy, datalog::NoTerminationPolicy> m_workspace;

    std::shared_ptr<StateRepository<LiftedTag>> m_state_repository;

    ActionExecutor m_executor;
};

}

#endif
