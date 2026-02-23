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

#ifndef TYR_PLANNING_ALGORITHMS_GBFS_LAZY_HPP_
#define TYR_PLANNING_ALGORITHMS_GBFS_LAZY_HPP_

#include "tyr/planning/algorithms/utils.hpp"
#include "tyr/planning/declarations.hpp"

#include <memory>
#include <optional>
#include <vector>

namespace tyr::planning::gbfs_lazy
{

template<typename Task>
struct Options
{
    std::optional<Node<Task>> start_node = std::nullopt;
    EventHandlerPtr<Task> event_handler = nullptr;
    PruningStrategyPtr<Task> pruning_strategy = nullptr;
    GoalStrategyPtr<Task> goal_strategy = nullptr;
    uint_t max_num_states = std::numeric_limits<uint_t>::max();
    std::optional<std::chrono::steady_clock::duration> max_time = std::nullopt;
    uint_t prefered_queue_weight = 1000;

    Options() = default;
};

template<typename Task>
SearchResult<Task>
find_solution(Task& task, SuccessorGenerator<Task>& successor_generator, Heuristic<Task>& heuristic, const Options<Task>& options = Options<Task>());

}

#endif
