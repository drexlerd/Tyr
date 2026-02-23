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

#ifndef TYR_PLANNING_ALGORITHMS_STRATEGIES_GOAL_HPP_
#define TYR_PLANNING_ALGORITHMS_STRATEGIES_GOAL_HPP_

#include "tyr/planning/applicability.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/node.hpp"

#include <memory>

namespace tyr::planning
{

template<typename Task>
class GoalStrategy
{
public:
    virtual ~GoalStrategy() = default;

    virtual bool is_static_goal_satisfied() = 0;
    virtual bool is_dynamic_goal_satisfied(const Node<Task>& node) = 0;
};

template<typename Task>
class TaskGoalStrategy : public GoalStrategy<Task>
{
public:
    TaskGoalStrategy(const Task& task) : m_task(task) {}

    static std::shared_ptr<TaskGoalStrategy<Task>> create(const Task& task) { return std::make_shared<TaskGoalStrategy<Task>>(task); }

    bool is_static_goal_satisfied() override { return is_statically_applicable(m_task.get_task().get_goal(), m_task.get_static_atoms_bitset()); }
    bool is_dynamic_goal_satisfied(const Node<Task>& node) override
    {
        const auto& state = node.get_state();
        const auto state_context = StateContext { m_task, state.get_unpacked_state(), node.get_metric() };
        return is_dynamically_applicable(m_task.get_task().get_goal(), state_context);
    }

private:
    const Task& m_task;
};
}

#endif
