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

#include "tyr/grounder/execution_contexts.hpp"
#include "tyr/planning/action_executor.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task/match_tree/declarations.hpp"  // for Matc...
#include "tyr/planning/lifted_task/axiom_evaluator.hpp"
#include "tyr/planning/lifted_task/node.hpp"
#include "tyr/planning/lifted_task/state_repository.hpp"
#include "tyr/planning/programs/action.hpp"
#include "tyr/planning/successor_generator.hpp"

namespace tyr::planning
{

template<>
class SuccessorGenerator<LiftedTask>
{
public:
    explicit SuccessorGenerator(std::shared_ptr<LiftedTask> task);

    Node<LiftedTask> get_initial_node();

    std::vector<LabeledNode<LiftedTask>> get_labeled_successor_nodes(const Node<LiftedTask>& node);

    void get_labeled_successor_nodes(const Node<LiftedTask>& node, std::vector<LabeledNode<LiftedTask>>& out_nodes);

    State<LiftedTask> get_state(StateIndex state_index);

private:
    std::shared_ptr<LiftedTask> m_task;

    ApplicableActionProgram m_action_program;
    grounder::ProgramExecutionContext m_action_context;
    std::vector<analysis::DomainListListList> m_parameter_domains_per_cond_effect_per_action;

    std::shared_ptr<StateRepository<LiftedTask>> m_state_repository;

    ActionExecutor m_executor;
};

}

#endif
