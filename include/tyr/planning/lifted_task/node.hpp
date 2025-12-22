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

#ifndef TYR_PLANNING_LIFTED_TASK_NODE_HPP_
#define TYR_PLANNING_LIFTED_TASK_NODE_HPP_

#include "tyr/common/config.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/node.hpp"
#include "tyr/planning/state_index.hpp"

/**
 * Forward declarations
 */

namespace tyr::formalism
{
template<typename T>
class OverlayRepository;

class Repository;

struct GroundAction;
}

/**
 * Definitions
 */

namespace tyr::planning
{
template<>
class Node<LiftedTask>
{
public:
    using TaskType = LiftedTask;

    Node() noexcept : m_task(nullptr), m_state_metric(), m_state_index(StateIndex::max()) {}
    Node(StateIndex state_index, float_t state_metric, LiftedTask& task) noexcept : m_task(&task), m_state_metric(state_metric), m_state_index(state_index) {}

    State<LiftedTask> get_state() const;
    LiftedTask& get_task() noexcept { return *m_task; }
    const LiftedTask& get_task() const noexcept { return *m_task; }
    float_t get_state_metric() const noexcept { return m_state_metric; }
    StateIndex get_state_index() const noexcept { return m_state_index; }

    std::vector<std::pair<View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>>, Node<LiftedTask>>>
    get_labeled_successor_nodes();

    void get_labeled_successor_nodes(
        std::vector<std::pair<View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>>, Node<LiftedTask>>>& out_nodes);

private:
    LiftedTask* m_task;
    float_t m_state_metric;
    StateIndex m_state_index;
};

// static_assert(NodeConcept<Node<LiftedTask>, LiftedTask>);

}

#endif
