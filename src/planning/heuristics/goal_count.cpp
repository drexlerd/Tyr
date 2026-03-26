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

#include "tyr/planning/heuristics/goal_count.hpp"

#include "tyr/planning/applicability.hpp"
#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/lifted_task.hpp"

namespace tyr::planning
{

template<typename Task>
GoalCountHeuristic<Task>::GoalCountHeuristic(std::shared_ptr<const Task> task) : m_task(std::move(task)), m_goal(m_task->get_task().get_goal())
{
}

template<typename Task>
std::shared_ptr<GoalCountHeuristic<Task>> GoalCountHeuristic<Task>::create(std::shared_ptr<const Task> task)
{
    return std::make_shared<GoalCountHeuristic>(std::move(task));
}

template<typename Task>
void GoalCountHeuristic<Task>::set_goal(formalism::planning::GroundConjunctiveConditionView goal)
{
    m_goal = goal;
}

template<typename Task>
float_t GoalCountHeuristic<Task>::evaluate(const StateView<Task>& state)
{
    auto unsat_counter = float_t { 0 };

    auto state_context = StateContext<Task> { *m_task, state.get_unpacked_state(), float_t { 0 } };

    for (const auto fact : m_goal.template get_facts<formalism::FluentTag>())
    {
        if (!is_applicable(fact, state_context))
            ++unsat_counter;
    }

    for (const auto fact : m_goal.template get_facts<formalism::DerivedTag>())
    {
        if (!is_applicable(fact, state_context))
            ++unsat_counter;
    }

    for (const auto numeric_constraint : m_goal.get_numeric_constraints())
    {
        if (!is_applicable(numeric_constraint, state_context))
            ++unsat_counter;
    }

    return unsat_counter;
}

template class GoalCountHeuristic<LiftedTask>;
template class GoalCountHeuristic<GroundTask>;

}
