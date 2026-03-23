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

#ifndef TYR_FORMALISM_PLANNING_CONDITIONAL_EFFECT_DATA_HPP_
#define TYR_FORMALISM_PLANNING_CONDITIONAL_EFFECT_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/planning/conditional_effect_index.hpp"
#include "tyr/formalism/planning/conjunctive_condition_index.hpp"
#include "tyr/formalism/planning/conjunctive_effect_index.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/variable_index.hpp"

namespace tyr
{

template<>
struct Data<formalism::planning::ConditionalEffect>
{
    Index<formalism::planning::ConditionalEffect> index;
    IndexList<formalism::Variable> variables;
    Index<formalism::planning::ConjunctiveCondition> condition;
    Index<formalism::planning::ConjunctiveEffect> effect;

    Data() = default;
    Data(IndexList<formalism::Variable> variables_,
         Index<formalism::planning::ConjunctiveCondition> condition_,
         Index<formalism::planning::ConjunctiveEffect> effect_) :
        index(),
        variables(std::move(variables_)),
        condition(condition_),
        effect(effect_)
    {
    }
    // Python constructor
    template<typename C>
    Data(const std::vector<View<Index<formalism::Variable>, C>>& variables_,
         View<Index<formalism::planning::ConjunctiveCondition>, C> condition_,
         View<Index<formalism::planning::ConjunctiveEffect>, C> effect_) :
        index(),
        variables(),
        condition(),
        effect()
    {
        set(variables_, variables);
        set(condition_, condition);
        set(effect_, effect);
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    void clear() noexcept
    {
        tyr::clear(index);
        tyr::clear(variables);
        tyr::clear(condition);
        tyr::clear(effect);
    }

    auto cista_members() const noexcept { return std::tie(index, variables, condition, effect); }
    auto identifying_members() const noexcept { return std::tie(variables, condition, effect); }
};

static_assert(!uses_trivial_storage_v<formalism::planning::ConditionalEffect>);

}

#endif
