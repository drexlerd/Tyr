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

#ifndef TYR_PLANNING_ALGORITHMS_GBFS_LAZY_EVENT_HANDLERS_MINIMAL_HPP_
#define TYR_PLANNING_ALGORITHMS_GBFS_LAZY_EVENT_HANDLERS_MINIMAL_HPP_

#include "tyr/planning/algorithms/gbfs_lazy/event_handlers/interface.hpp"

namespace tyr::planning::gbfs_lazy
{

/**
 * Implementation class
 */
template<typename Task>
class DefaultEventHandler : public EventHandlerBase<DefaultEventHandler<Task>, Task>
{
private:
    /* Implement EventHandlerBase interface */
    friend class EventHandlerBase<DefaultEventHandler<Task>, Task>;

    void on_expand_node_impl(const Node<Task>& node) const;

    void on_expand_goal_node_impl(const Node<Task>& node) const;

    void on_generate_node_impl(const LabeledNode<Task>& labeled_succ_node) const;

    void on_prune_node_impl(const Node<Task>& node) const;

    void on_start_search_impl(const Node<Task>& node, float_t h_value) const;

    void on_new_best_h_value_impl(float_t h_value, uint64_t num_expanded_states, uint64_t num_generated_states) const;

    void on_end_search_impl() const;

    void on_solved_impl(const Plan<Task>& plan) const;

    void on_unsolvable_impl() const;

    void on_exhausted_impl() const;

public:
    DefaultEventHandler(bool quiet = true);

    static DefaultEventHandlerPtr<Task> create(bool quiet = true);
};

}

#endif
