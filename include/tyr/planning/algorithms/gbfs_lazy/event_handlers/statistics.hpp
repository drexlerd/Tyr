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

#ifndef TYR_PLANNING_ALGORITHMS_GBFS_LAZY_EVENT_HANDLERS_STATISTICS_HPP_
#define TYR_PLANNING_ALGORITHMS_GBFS_LAZY_EVENT_HANDLERS_STATISTICS_HPP_

#include <chrono>
#include <cstdint>
#include <map>
#include <ostream>
#include <vector>

namespace tyr::planning::gbfs_lazy
{

class Statistics
{
private:
    uint64_t m_num_generated;
    uint64_t m_num_expanded;
    uint64_t m_num_deadends;
    uint64_t m_num_pruned;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_search_start_time_point;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_search_end_time_point;

public:
    Statistics() : m_num_generated(0), m_num_expanded(0), m_num_deadends(0), m_num_pruned(0) {}

    /**
     * Setters
     */

    void increment_num_generated() { ++m_num_generated; }
    void increment_num_expanded() { ++m_num_expanded; }
    void increment_num_deadends() { ++m_num_deadends; }
    void increment_num_pruned() { ++m_num_pruned; }
    void set_search_start_time_point(std::chrono::time_point<std::chrono::high_resolution_clock> time_point) { m_search_start_time_point = time_point; }
    void set_search_end_time_point(std::chrono::time_point<std::chrono::high_resolution_clock> time_point) { m_search_end_time_point = time_point; }

    /**
     * Getters
     */

    uint64_t get_num_generated() const { return m_num_generated; }
    uint64_t get_num_expanded() const { return m_num_expanded; }
    uint64_t get_num_deadends() const { return m_num_deadends; }
    uint64_t get_num_pruned() const { return m_num_pruned; }

    std::chrono::milliseconds get_search_time_ms() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(m_search_end_time_point - m_search_start_time_point);
    }
    std::chrono::milliseconds get_current_search_time_ms() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_search_start_time_point);
    }
};

/**
 * Types
 */

using StatisticsList = std::vector<Statistics>;

}

#endif
