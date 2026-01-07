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

#include "tyr/planning/algorithms/gbfs_lazy/event_handler.hpp"

#include "tyr/formalism/planning/formatter.hpp"
#include "tyr/planning/formatter.hpp"
#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/plan.hpp"

#include <iostream>

namespace tyr::planning::gbfs_lazy
{
template<typename Task>
void DefaultEventHandler<Task>::on_expand_node_impl(const Node<Task>& node) const
{
    std::cout << "[GBFS] ----------------------------------------\n"
              << "[GBFS] Expanding node: " << node << "\n"
              << std::endl;
}

template<typename Task>
void DefaultEventHandler<Task>::on_expand_goal_node_impl(const Node<Task>& node) const
{
}

template<typename Task>
void DefaultEventHandler<Task>::on_generate_node_impl(const LabeledNode<Task>& labeled_succ_node) const
{
    std::cout << "[GBFS] Action: " << labeled_succ_node.label << "\n";
    std::cout << "[GBFS] Successor node: " << labeled_succ_node.node << "\n" << std::endl;
}

template<typename Task>
void DefaultEventHandler<Task>::on_prune_node_impl(const Node<Task>& node) const
{
}

template<typename Task>
void DefaultEventHandler<Task>::on_start_search_impl(const Node<Task>& node, float_t h_value) const
{
    std::cout << "[GBFS] Search started.\n"
              << "[GBFS] Start node h_value: " << h_value << std::endl;
}

template<typename Task>
void DefaultEventHandler<Task>::on_new_best_h_value_impl(float_t h_value, uint64_t num_expanded_states, uint64_t num_generated_states) const
{
    std::cout << "[GBFS] New best h_value: " << h_value << " with num expanded states " << num_expanded_states << " and num generated states "
              << num_generated_states << " (" << this->get_statistics().get_current_search_time_ms().count() << " ms)" << std::endl;
}

template<typename Task>
void DefaultEventHandler<Task>::on_end_search_impl() const
{
    std::cout << "[GBFS] Search ended.\n" << this->m_statistics << std::endl;
}

template<typename Task>
void DefaultEventHandler<Task>::on_solved_impl(const Plan<Task>& plan) const
{
    std::cout << "[GBFS] Plan found.\n"
              << "[GBFS] Plan cost: " << plan.get_cost() << "\n"
              << "[GBFS] Plan length: " << plan.get_actions().size() << std::endl;
}

template<typename Task>
void DefaultEventHandler<Task>::on_unsolvable_impl() const
{
}

template<typename Task>
void DefaultEventHandler<Task>::on_exhausted_impl() const
{
}

template<typename Task>
DefaultEventHandler<Task>::DefaultEventHandler(size_t verbosity) : EventHandlerBase<DefaultEventHandler<Task>, Task>(verbosity)
{
}

template<typename Task>
DefaultEventHandlerPtr<Task> DefaultEventHandler<Task>::create(size_t verbosity)
{
    return std::make_shared<DefaultEventHandler<Task>>(verbosity);
}

template class DefaultEventHandler<LiftedTask>;
template class DefaultEventHandler<GroundTask>;

}
