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

#ifndef TYR_FORMALISM_PLANNING_GROUND_ACTION_DATA_HPP_
#define TYR_FORMALISM_PLANNING_GROUND_ACTION_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function_index.hpp"
#include "tyr/formalism/ground_conjunctive_condition_index.hpp"
#include "tyr/formalism/planning/ground_action_index.hpp"
#include "tyr/formalism/planning/ground_conjunctive_effect_index.hpp"

namespace tyr
{

template<>
struct Data<formalism::planning::GroundAction>
{
    using Tag = formalism::planning::GroundAction;

    Index<formalism::planning::GroundAction> index;
    Index<formalism::planning::Action> action;
    Index<formalism::GroundConjunctiveCondition> condition;
    Index<formalism::planning::GroundConjunctiveEffect> effect;

    Data() = default;
    Data(Index<formalism::planning::GroundAction> index,
         Index<formalism::planning::Action> action,
         Index<formalism::GroundConjunctiveCondition> condition,
         Index<formalism::planning::GroundConjunctiveEffect> effect) :
        index(index),
        action(action),
        condition(condition),
        effect(effect)
    {
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    auto cista_members() const noexcept { return std::tie(index, action, condition, effect); }
    auto identifying_members() const noexcept { return std::tie(action, condition, effect); }
};
}

#endif
