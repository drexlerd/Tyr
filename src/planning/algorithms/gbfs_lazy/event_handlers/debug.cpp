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

#include "tyr/planning/algorithms/gbfs_lazy/event_handlers/debug.hpp"

#include "tyr/formalism/planning/formatter.hpp"
#include "tyr/planning/formatter.hpp"
#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/lifted_task.hpp"

#include <iostream>

namespace tyr::planning::gbfs_lazy
{
template<typename Task>
void DebugEventHandler<Task>::on_expand_node_impl(const Node<Task>& node) const
{
}

template<typename Task>
void DebugEventHandler<Task>::on_expand_goal_node_impl(const Node<Task>& node) const
{
}

template<typename Task>
void DebugEventHandler<Task>::on_generate_node_impl(const LabeledNode<Task>& labeled_succ_node) const
{
}

template<typename Task>
void DebugEventHandler<Task>::on_prune_node_impl(const Node<Task>& node) const
{
}

template<typename Task>
void DebugEventHandler<Task>::on_start_search_impl(const Node<Task>& node, float_t h_value) const
{
}

template<typename Task>
void DebugEventHandler<Task>::on_new_best_h_value_impl(float_t h_value, uint64_t num_expanded_states, uint64_t num_generated_states) const
{
}

template<typename Task>
void DebugEventHandler<Task>::on_end_search_impl() const
{
    std::cout << "[GBFS] Search ended.\n" << this->m_statistics << std::endl;
}

template<typename Task>
void DebugEventHandler<Task>::on_solved_impl(const Plan<Task>& plan) const
{
}

template<typename Task>
void DebugEventHandler<Task>::on_unsolvable_impl() const
{
}

template<typename Task>
void DebugEventHandler<Task>::on_exhausted_impl() const
{
}

template<typename Task>
DebugEventHandler<Task>::DebugEventHandler(bool quiet) : EventHandlerBase<DebugEventHandler<Task>, Task>(quiet)
{
}

template<typename Task>
DebugEventHandlerPtr<Task> DebugEventHandler<Task>::create(bool quiet)
{
    return std::make_shared<DebugEventHandler<Task>>(quiet);
}

template class DebugEventHandler<LiftedTask>;
template class DebugEventHandler<GroundTask>;

}