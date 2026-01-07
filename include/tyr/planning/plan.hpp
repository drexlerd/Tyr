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

#ifndef TYR_PLANNING_PLAN_HPP_
#define TYR_PLANNING_PLAN_HPP_

#include "tyr/common/config.hpp"
#include "tyr/formalism/planning/ground_action_index.hpp"
#include "tyr/planning/state.hpp"

namespace tyr::planning
{

template<typename Task>
class Plan
{
private:
    StateList<Task> m_states;
    IndexList<formalism::planning::GroundAction> m_actions;
    float_t m_cost;

public:
    Plan(StateList<Task> states, IndexList<formalism::planning::GroundAction> actions, float_t cost);

    const StateList<Task>& get_states() const noexcept { return m_states; }
    const IndexList<formalism::planning::GroundAction>& get_actions() const noexcept { return m_actions; }
    float_t get_cost() const noexcept { return m_cost; }
    size_t get_length() const noexcept { return m_actions.size(); }
};
}

#endif
