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

#ifndef TYR_PLANNING_ALGORITHMS_GBFS_LAZY_EVENT_HANDLER_HPP_
#define TYR_PLANNING_ALGORITHMS_GBFS_LAZY_EVENT_HANDLER_HPP_

#include "tyr/formalism/planning/ground_action_view.hpp"
#include "tyr/planning/algorithms/statistics.hpp"
#include "tyr/planning/declarations.hpp"

#include <chrono>
#include <concepts>
#include <cstdint>

namespace tyr::planning::gbfs_lazy
{

/**
 * Interface class
 */

/// @brief `IEventHandler` to react on event during GBFS search.
///
/// Inspired by boost graph library: https://www.boost.org/doc/libs/1_75_0/libs/graph/doc/AStarVisitor.html
template<TaskKind Kind>
class EventHandler
{
public:
    virtual ~EventHandler() = default;

    /// @brief React on expanding a node. This is called immediately after popping from the queue.
    virtual void on_expand_node(const Node<Kind>& node) = 0;

    /// @brief React on expanding a goal `node`.
    virtual void on_expand_goal_node(const Node<Kind>& node) = 0;

    /// @brief React on generating a successor `node` by applying an action.
    virtual void on_generate_node(const LabeledNode<Kind>& labeled_succ_node) = 0;

    /// @brief React on pruning a node.
    virtual void on_prune_node(const Node<Kind>& node) = 0;

    /// @brief React on starting a search.
    virtual void on_start_search(const Node<Kind>& node, float_t h_value) = 0;

    /// @brief React on new best h_value
    virtual void on_new_best_h_value(float_t h_value) = 0;

    /// @brief React on ending a search.
    virtual void on_end_search() = 0;

    /// @brief React on solving a search.
    virtual void on_solved(const Plan<Kind>& plan) = 0;

    /// @brief React on proving unsolvability during a search.
    virtual void on_unsolvable() = 0;

    /// @brief React on exhausting a search.
    virtual void on_exhausted() = 0;
};

/**
 * Static base class (for C++)
 *
 * Collect statistics and call implementation of derived class.
 */
template<typename Derived, TaskKind Kind>
class EventHandlerBase : public EventHandler<Kind>
{
protected:
    tyr::planning::Statistics m_statistics;
    size_t m_verbosity;

private:
    EventHandlerBase() = default;
    friend Derived;

    /// @brief Helper to cast to Derived.
    constexpr const auto& self() const { return static_cast<const Derived&>(*this); }
    constexpr auto& self() { return static_cast<Derived&>(*this); }

    bool verbosity(size_t level) const { return m_verbosity >= level; }

public:
    explicit EventHandlerBase(size_t verbosity = 0) : m_statistics(), m_verbosity(verbosity) {}

    void on_expand_node(const Node<Kind>& node) override
    {
        m_statistics.increment_num_expanded();

        if (verbosity(2))
            self().on_expand_node_impl(node);
    }

    void on_expand_goal_node(const Node<Kind>& node) override
    {
        if (verbosity(2))
            self().on_expand_goal_node_impl(node);
    }

    void on_generate_node(const LabeledNode<Kind>& labeled_succ_node) override
    {
        m_statistics.increment_num_generated();

        if (verbosity(2))
        {
            self().on_generate_node_impl(labeled_succ_node);
        }
    }

    void on_prune_node(const Node<Kind>& node) override
    {
        m_statistics.increment_num_pruned();

        if (verbosity(2))
        {
            self().on_prune_node_impl(node);
        }
    }

    void on_start_search(const Node<Kind>& node, float_t h_value) override
    {
        m_statistics = tyr::planning::Statistics();

        m_statistics.set_search_start_time_point(std::chrono::high_resolution_clock::now());

        if (verbosity(0))
        {
            self().on_start_search_impl(node, h_value);
        }
    }

    void on_new_best_h_value(float_t h_value) override
    {
        if (verbosity(0))
        {
            self().on_new_best_h_value_impl(h_value, m_statistics.get_num_expanded(), m_statistics.get_num_generated());
        }
    }

    void on_end_search() override

    {
        m_statistics.set_search_end_time_point(std::chrono::high_resolution_clock::now());

        if (verbosity(0))
            self().on_end_search_impl();
    }

    void on_solved(const Plan<Kind>& plan) override
    {
        if (verbosity(0))
        {
            self().on_solved_impl(plan);
        }
    }

    void on_unsolvable() override
    {
        if (verbosity(0))
        {
            self().on_unsolvable_impl();
        }
    }

    void on_exhausted() override
    {
        if (verbosity(0))
        {
            self().on_exhausted_impl();
        }
    }

    /**
     * Getters
     */

    const tyr::planning::Statistics& get_statistics() const { return m_statistics; }
};

template<TaskKind Kind>
class DefaultEventHandler : public EventHandlerBase<DefaultEventHandler<Kind>, Kind>
{
private:
    /* Implement EventHandlerBase interface */
    friend class EventHandlerBase<DefaultEventHandler<Kind>, Kind>;

    void on_expand_node_impl(const Node<Kind>& node) const;

    void on_expand_goal_node_impl(const Node<Kind>& node) const;

    void on_generate_node_impl(const LabeledNode<Kind>& labeled_succ_node) const;

    void on_prune_node_impl(const Node<Kind>& node) const;

    void on_start_search_impl(const Node<Kind>& node, float_t h_value) const;

    void on_new_best_h_value_impl(float_t h_value, uint64_t num_expanded_states, uint64_t num_generated_states) const;

    void on_end_search_impl() const;

    void on_solved_impl(const Plan<Kind>& plan) const;

    void on_unsolvable_impl() const;

    void on_exhausted_impl() const;

public:
    DefaultEventHandler(size_t verbosity = 0);

    static DefaultEventHandlerPtr<Kind> create(size_t verbosity = 0);
};

}

#endif
