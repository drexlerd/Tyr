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

#ifndef TYR_FORMALISM_PLANNING_ACTION_DATA_HPP_
#define TYR_FORMALISM_PLANNING_ACTION_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/formalism/conjunctive_condition_index.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function_index.hpp"
#include "tyr/formalism/planning/action_index.hpp"
#include "tyr/formalism/planning/conjunctive_effect_index.hpp"

namespace tyr
{

template<>
struct Data<formalism::planning::Action>
{
    using Tag = formalism::planning::Action;

    Index<formalism::planning::Action> index;
    ::cista::offset::string name;
    uint_t original_arity;
    Index<formalism::ConjunctiveCondition> condition;
    Index<formalism::planning::ConjunctiveEffect> effect;

    Data() = default;
    Data(Index<formalism::planning::Action> index,
         ::cista::offset::string name,
         uint_t original_arity,
         Index<formalism::ConjunctiveCondition> condition,
         Index<formalism::planning::ConjunctiveEffect> effect) :
        index(index),
        name(std::move(name)),
        condition(condition),
        effect(effect)
    {
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    auto cista_members() const noexcept { return std::tie(index, name, original_arity, condition, effect); }
    auto identifying_members() const noexcept { return std::tie(name, original_arity, condition, effect); }
};
}

#endif
