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

#include "tyr/planning/algorithms/astar_eager.hpp"

#include "tyr/common/chrono.hpp"
#include "tyr/common/segmented_vector.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"
#include "tyr/planning/algorithms/astar_eager/event_handler.hpp"
#include "tyr/planning/algorithms/openlists/alternating.hpp"
#include "tyr/planning/algorithms/strategies/goal.hpp"
#include "tyr/planning/algorithms/strategies/pruning.hpp"
#include "tyr/planning/algorithms/utils.hpp"
#include "tyr/planning/applicability.hpp"
#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/ground_task/node.hpp"
#include "tyr/planning/ground_task/state.hpp"
#include "tyr/planning/ground_task/successor_generator.hpp"
#include "tyr/planning/ground_task/unpacked_state.hpp"
#include "tyr/planning/heuristic.hpp"
#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/lifted_task/node.hpp"
#include "tyr/planning/lifted_task/state.hpp"
#include "tyr/planning/lifted_task/successor_generator.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"
#include "tyr/planning/search_node.hpp"
#include "tyr/planning/search_space.hpp"
#include "tyr/planning/state_index.hpp"

namespace tyr::planning::astar_eager
{

/**
 * GBFS search node
 */

struct SearchNode
{
    float_t g_value;
    StateIndex parent_state;
    SearchNodeStatus status;
};

static_assert(sizeof(SearchNode) == 16);

using SearchNodeVector = SegmentedVector<SearchNode>;

static SearchNode& get_or_create_search_node(StateIndex state_index, SearchNodeVector& search_nodes)
{
    static auto default_node = SearchNode { std::numeric_limits<float_t>::infinity(), StateIndex::max(), SearchNodeStatus::NEW };

    while (uint_t(state_index) >= search_nodes.size())
    {
        search_nodes.push_back(default_node);
    }
    return search_nodes[uint_t(state_index)];
}

/**
 * GBFS queue
 */

struct QueueEntry
{
    using KeyType = std::tuple<float_t, SearchNodeStatus>;
    using ItemType = std::tuple<float_t, StateIndex>;

    float_t f_value;
    StateIndex state;
    SearchNodeStatus status;

    KeyType get_key() const { return std::make_tuple(f_value, status); }
    ItemType get_item() const { return std::make_tuple(f_value, state); }
};

static_assert(sizeof(QueueEntry) == 16);

using Queue = PriorityQueue<QueueEntry>;

template<typename Task>
SearchResult<Task> find_solution(Task& task, SuccessorGenerator<Task>& successor_generator, Heuristic<Task>& heuristic, const Options<Task>& options)
{
    const auto start_node = (options.start_node) ? options.start_node.value() : successor_generator.get_initial_node();
    const auto& start_state = start_node.get_state();
    const auto start_state_index = start_state.get_index();
    const auto event_handler = (options.event_handler) ? options.event_handler : DefaultEventHandler<Task>::create(0);
    const auto pruning_strategy = (options.pruning_strategy) ? options.pruning_strategy : PruningStrategy<Task>::create();
    const auto goal_strategy = (options.goal_strategy) ? options.goal_strategy : TaskGoalStrategy<Task>::create(task);

    auto result = SearchResult<Task>();

    /* Test static goal. */

    if (!goal_strategy->is_static_goal_satisfied())
    {
        event_handler->on_unsolvable();

        result.status = SearchStatus::UNSOLVABLE;
        return result;
    }

    auto search_nodes = SearchNodeVector();

    /* Test whether initial state is goal. */

    if (options.stop_if_goal && goal_strategy->is_dynamic_goal_satisfied(start_node))
    {
        event_handler->on_end_search();

        result.plan = Plan(start_node, LabeledNodeList<Task> {});
        result.goal_node = start_node;
        result.status = SearchStatus::SOLVED;

        event_handler->on_solved(result.plan.value());

        return result;
    }

    auto openlist = Queue();

    if (std::isnan(start_node.get_metric()))
    {
        throw std::runtime_error("find_solution(...): start node metric value is NaN.");
    }
    const auto start_h_value = heuristic.evaluate(start_state);
    const auto start_f_value = start_node.get_metric() + start_h_value;

    event_handler->on_start_search(start_node, start_f_value);

    auto& start_search_node = get_or_create_search_node(start_state_index, search_nodes);
    start_search_node.status = (start_h_value == std::numeric_limits<float_t>::infinity()) ? SearchNodeStatus::DEAD_END : SearchNodeStatus::OPEN;
    start_search_node.g_value = start_node.get_metric();

    /* Test whether start state is deadend. */

    if (start_search_node.status == SearchNodeStatus::DEAD_END)
    {
        event_handler->on_unsolvable();

        result.status = SearchStatus::UNSOLVABLE;
        return result;
    }

    auto labeled_succ_nodes = std::vector<LabeledNode<Task>> {};
    auto f_value = start_f_value;
    openlist.insert(QueueEntry { start_f_value, start_state_index, start_search_node.status });

    auto stopwatch = options.max_time ? std::optional<CountdownWatch>(options.max_time.value()) : std::nullopt;

    while (!openlist.empty())
    {
        if (stopwatch && stopwatch->has_finished())
        {
            result.status = SearchStatus::OUT_OF_TIME;
            return result;
        }

        const auto [state_f_value, state_index] = openlist.top();
        const auto state = successor_generator.get_state(state_index);

        openlist.pop();

        auto& search_node = get_or_create_search_node(state_index, search_nodes);
        auto node = Node<Task>(state, search_node.g_value);

        /* Close state. */

        if (search_node.status == SearchNodeStatus::CLOSED || search_node.status == SearchNodeStatus::DEAD_END)
            continue;

        /* Report search progress. */

        if (state_f_value > f_value)
        {
            event_handler->on_finish_f_layer(f_value);
            f_value = state_f_value;
        }

        /* Test whether state achieves the dynamic goal. */
        if (options.stop_if_goal && search_node.status == SearchNodeStatus::GOAL)
        {
            search_node.status = SearchNodeStatus::GOAL;

            event_handler->on_expand_goal_node(node);

            event_handler->on_end_search();

            result.plan = extract_total_ordered_plan(search_node, node, search_nodes, successor_generator);
            result.goal_node = node;
            result.status = SearchStatus::SOLVED;

            event_handler->on_solved(result.plan.value());

            return result;
        }

        /* Expand the successors of the node. */

        event_handler->on_expand_node(node);

        /* Ensure that the state is closed */

        search_node.status = SearchNodeStatus::CLOSED;

        successor_generator.get_labeled_successor_nodes(node, labeled_succ_nodes);

        for (const auto& labeled_succ_node : labeled_succ_nodes)
        {
            const auto& succ_node = labeled_succ_node.node;
            const auto& succ_state = succ_node.get_state();
            const auto succ_state_index = succ_state.get_index();

            auto& successor_search_node = get_or_create_search_node(succ_state_index, search_nodes);

            assert(!std::isnan(succ_node.get_metric()));

            const auto is_new_successor_state = (successor_search_node.status == SearchNodeStatus::NEW);

            if (is_new_successor_state && search_nodes.size() >= options.max_num_states)
            {
                result.status = SearchStatus::OUT_OF_STATES;
                return result;
            }

            /* Skip previously generated state. */

            if (!is_new_successor_state)
                continue;

            /* Apply pruning strategy */

            if (pruning_strategy->should_prune(succ_node))
            {
                event_handler->on_prune_node(succ_node);
                continue;
            }

            event_handler->on_generate_node(labeled_succ_node);

            /* Check whether state must be reopened or not. */

            if (succ_node.get_metric() < successor_search_node.g_value)
            {
                successor_search_node.status = SearchNodeStatus::OPEN;
                successor_search_node.parent_state = state_index;
                successor_search_node.g_value = succ_node.get_metric();

                const auto successor_is_goal_state = goal_strategy->is_dynamic_goal_satisfied(succ_node);

                if (is_new_successor_state && successor_is_goal_state)
                {
                    successor_search_node.status = SearchNodeStatus::GOAL;
                }

                const auto successor_h_value = heuristic.evaluate(succ_state);

                if (successor_h_value == std::numeric_limits<float_t>::infinity())
                {
                    successor_search_node.status = SearchNodeStatus::DEAD_END;
                    continue;
                }

                event_handler->on_generate_node_relaxed(labeled_succ_node);

                const auto successor_f_value = succ_node.get_metric() + successor_h_value;
                openlist.insert(QueueEntry { successor_f_value, succ_state_index, successor_search_node.status });
            }
            else
            {
                event_handler->on_generate_node_not_relaxed(labeled_succ_node);
            }

            event_handler->on_generate_node(labeled_succ_node);
        }
    }

    event_handler->on_end_search();
    event_handler->on_exhausted();

    result.status = SearchStatus::EXHAUSTED;
    return result;
}

template SearchResult<LiftedTask>
find_solution(LiftedTask& task, SuccessorGenerator<LiftedTask>& successor_generator, Heuristic<LiftedTask>& heuristic, const Options<LiftedTask>& options);

template SearchResult<GroundTask>
find_solution(GroundTask& task, SuccessorGenerator<GroundTask>& successor_generator, Heuristic<GroundTask>& heuristic, const Options<GroundTask>& options);

}
