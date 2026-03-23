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
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/binding_index.hpp"
#include "tyr/formalism/function_index.hpp"
#include "tyr/formalism/planning/action_index.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/ground_action_index.hpp"
#include "tyr/formalism/planning/ground_conjunctive_condition_index.hpp"
#include "tyr/formalism/planning/ground_conjunctive_effect_index.hpp"

namespace tyr
{

template<>
struct Data<formalism::planning::GroundAction>
{
    Index<formalism::planning::GroundAction> index;
    Index<formalism::RelationBinding<formalism::planning::Action>> binding;
    Index<formalism::planning::GroundConjunctiveCondition> condition;
    IndexList<formalism::planning::GroundConditionalEffect> effects;

    Data() = default;
    Data(Index<formalism::RelationBinding<formalism::planning::Action>> binding_,
         Index<formalism::planning::GroundConjunctiveCondition> condition_,
         IndexList<formalism::planning::GroundConditionalEffect> effects_) :
        index(),
        binding(binding_),
        condition(condition_),
        effects(std::move(effects_))
    {
    }
    // Python constructor
    template<typename C>
    Data(View<Index<formalism::RelationBinding<formalism::planning::Action>>, C> binding_,
         View<Index<formalism::planning::GroundConjunctiveCondition>, C> condition_,
         const std::vector<View<Index<formalism::planning::GroundConditionalEffect>, C>>& effects_) :
        index(),
        binding(),
        condition(),
        effects()
    {
        set(binding_, binding);
        set(condition_, condition);
        set(effects_, effects);
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    void clear() noexcept
    {
        tyr::clear(index);
        tyr::clear(binding);
        tyr::clear(condition);
        tyr::clear(effects);
    }

    auto cista_members() const noexcept { return std::tie(index, binding, condition, effects); }
    // Have to include effects because row only binds objects to non-effect quantified variables.
    auto identifying_members() const noexcept { return std::tie(binding, condition, effects); }
};

static_assert(!uses_trivial_storage_v<formalism::planning::GroundAction>);
}

#endif
