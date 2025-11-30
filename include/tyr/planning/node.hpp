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

#ifndef TYR_PLANNING_NODE_HPP_
#define TYR_PLANNING_NODE_HPP_

#include "tyr/common/config.hpp"
#include "tyr/planning/state.hpp"
#include "tyr/planning/state_index.hpp"

namespace tyr::planning
{
template<typename Task>
class Node
{
public:
    Node() noexcept : m_task(nullptr), m_state_metric(), m_state_index(StateIndex::max()) {}
    Node(StateIndex state_index, float_t state_metric, Task& task) noexcept : m_task(&task), m_state_metric(state_metric), m_state_index(state_index) {}

    State<Task> get_state() const { return m_task->get_state(m_state_index); }
    Task& get_task() noexcept { return *m_task; }
    float_t get_state_metric() const noexcept { return m_state_metric; }
    StateIndex get_state_index() const noexcept { return m_state_index; }

    std::vector<std::pair<View<Index<formalism::planning::GroundAction>, formalism::ScopedRepository<formalism::Repository>>, Node<Task>>>
    get_labeled_successor_nodes()
    {
        return m_task->get_labeled_successor_nodes(*this);
    }

    void get_labeled_successor_nodes(
        std::vector<std::pair<View<Index<formalism::planning::GroundAction>, formalism::ScopedRepository<formalism::Repository>>, Node<Task>>>& out_nodes)
    {
        m_task->get_labeled_successor_nodes(*this, out_nodes);
    }

private:
    Task* m_task;
    float_t m_state_metric;
    StateIndex m_state_index;
};

template<typename Task>
using NodeList = std::vector<Node<Task>>;
}

#endif
