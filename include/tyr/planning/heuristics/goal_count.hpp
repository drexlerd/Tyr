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

#ifndef TYR_PLANNING_HEURISTICS_GOAL_COUNT_HPP_
#define TYR_PLANNING_HEURISTICS_GOAL_COUNT_HPP_

#include "tyr/planning/heuristic.hpp"

namespace tyr::planning
{

template<typename Task>
class GoalCountHeuristic : public Heuristic<Task>
{
public:
    explicit GoalCountHeuristic(std::shared_ptr<const Task> task);

    static std::shared_ptr<GoalCountHeuristic<Task>> create(std::shared_ptr<const Task> task);

    void set_goal(formalism::planning::GroundConjunctiveConditionView goal) override;

    float_t evaluate(const StateView<Task>& state) override;

protected:
    std::shared_ptr<const Task> m_task;

    formalism::planning::GroundConjunctiveConditionView m_goal;
};

}

#endif
