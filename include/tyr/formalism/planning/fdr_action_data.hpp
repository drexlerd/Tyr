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

#ifndef TYR_FORMALISM_PLANNING_FDR_ACTION_DATA_HPP_
#define TYR_FORMALISM_PLANNING_FDR_ACTION_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function_index.hpp"
#include "tyr/formalism/planning/fdr_action_index.hpp"
#include "tyr/formalism/planning/fdr_conjunctive_condition_index.hpp"
#include "tyr/formalism/planning/fdr_conjunctive_effect_index.hpp"

namespace tyr
{

template<>
struct Data<formalism::FDRAction>
{
    using Tag = formalism::FDRAction;

    Index<formalism::FDRAction> index;
    Index<formalism::Action> action;
    Index<formalism::FDRConjunctiveCondition> condition;
    IndexList<formalism::FDRConditionalEffect> effects;

    Data() = default;
    Data(Index<formalism::FDRAction> index,
         Index<formalism::Action> action,
         Index<formalism::FDRConjunctiveCondition> condition,
         IndexList<formalism::FDRConditionalEffect> effects) :
        index(index),
        action(action),
        condition(condition),
        effects(std::move(effects))
    {
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    void clear() noexcept
    {
        tyr::clear(index);
        tyr::clear(action);
        tyr::clear(condition);
        tyr::clear(effects);
    }

    auto cista_members() const noexcept { return std::tie(index, action, condition, effects); }
    auto identifying_members() const noexcept { return std::tie(action, condition, effects); }
};
}

#endif
