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

#ifndef TYR_PLANNING_GROUND_TASK_NODE_HPP_
#define TYR_PLANNING_GROUND_TASK_NODE_HPP_

#include "tyr/common/config.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task/state.hpp"
#include "tyr/planning/node.hpp"

/**
 * Definitions
 */

namespace tyr::planning
{
template<>
class Node<GroundTask>
{
public:
    using TaskType = GroundTask;

    Node(State<GroundTask> state, float_t metric) noexcept : m_state(std::move(state)), m_metric(metric) {}

    const State<GroundTask>& get_state() const noexcept { return m_state; }
    const GroundTask& get_task() const noexcept { return m_state.get_task(); }
    float_t get_metric() const noexcept { return m_metric; }

private:
    State<GroundTask> m_state;
    float_t m_metric;
};

}

#endif
