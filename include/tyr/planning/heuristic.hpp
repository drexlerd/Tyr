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

#ifndef TYR_PLANNING_HEURISTIC_HPP_
#define TYR_PLANNING_HEURISTIC_HPP_

#include "tyr/common/config.hpp"
#include "tyr/common/declarations.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/ground_action_index.hpp"
#include "tyr/planning/declarations.hpp"

namespace tyr::planning
{

template<typename Task>
class Heuristic
{
public:
    virtual ~Heuristic() = default;

    virtual void set_goal(View<Index<formalism::planning::GroundConjunctiveCondition>, formalism::planning::Repository> goal) = 0;

    virtual float_t evaluate(const State<Task>& state) = 0;

    virtual const UnorderedSet<Index<formalism::planning::GroundAction>>& get_preferred_actions()
    {
        static const auto actions = UnorderedSet<Index<formalism::planning::GroundAction>> {};
        return actions;
    }
};

}

#endif
