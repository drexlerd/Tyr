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

#include "tyr/planning/ground_task/successor_generator.hpp"

#include "../metric.hpp"
#include "tyr/formalism/overlay_repository.hpp"  // for View
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"  // for View
#include "tyr/planning/applicability.hpp"    // for StateC...
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/ground_task/match_tree/match_tree.hpp"
#include "tyr/planning/ground_task/state_repository.hpp"
#include "tyr/planning/state_index.hpp"
#include "tyr/planning/task_utils.hpp"

namespace tyr::planning
{

SuccessorGenerator<GroundTask>::SuccessorGenerator(std::shared_ptr<GroundTask> task) :
    m_task(task),
    m_applicable_actions(),
    m_state_repository(std::make_shared<StateRepository<GroundTask>>(task)),
    m_executor()
{
}

Node<GroundTask> SuccessorGenerator<GroundTask>::get_initial_node()
{
    auto initial_state = m_state_repository->get_initial_state();

    const auto state_context = StateContext<GroundTask>(*m_task, initial_state.get_unpacked_state(), 0);

    const auto state_metric = evaluate_metric(m_task->get_task().get_metric(), m_task->get_task().get_auxiliary_fterm_value(), state_context);

    return Node<GroundTask>(std::move(initial_state), state_metric);
}

std::vector<LabeledNode<GroundTask>> SuccessorGenerator<GroundTask>::get_labeled_successor_nodes(const Node<GroundTask>& node)
{
    auto result = std::vector<LabeledNode<GroundTask>> {};

    get_labeled_successor_nodes(node, result);

    return result;
}

void SuccessorGenerator<GroundTask>::get_labeled_successor_nodes(const Node<GroundTask>& node, std::vector<LabeledNode<GroundTask>>& out_nodes)
{
    out_nodes.clear();

    const auto state = node.get_state();

    const auto state_context = StateContext<GroundTask>(*m_task, state.get_unpacked_state(), node.get_metric());

    m_task->get_action_match_tree()->generate(state_context, m_applicable_actions);

    for (const auto ground_action : make_view(m_applicable_actions, *m_task->get_repository()))
    {
        if (m_executor.is_applicable(ground_action, state_context))
            out_nodes.emplace_back(ground_action, m_executor.apply_action(state_context, ground_action, *m_state_repository));
    }
}

State<GroundTask> SuccessorGenerator<GroundTask>::get_state(StateIndex state_index) { return m_state_repository->get_registered_state(state_index); }

}
