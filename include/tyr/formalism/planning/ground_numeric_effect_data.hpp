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

#ifndef TYR_FORMALISM_PLANNING_GROUND_NUMERIC_EFFECT_DATA_HPP_
#define TYR_FORMALISM_PLANNING_GROUND_NUMERIC_EFFECT_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/ground_function_expression_data.hpp"
#include "tyr/formalism/planning/ground_function_term_index.hpp"
#include "tyr/formalism/planning/ground_numeric_effect_index.hpp"

namespace tyr
{

template<formalism::planning::NumericEffectOpKind Op, formalism::FactKind T>
struct Data<formalism::planning::GroundNumericEffect<Op, T>>
{
    static_assert(std::same_as<T, formalism::FluentTag> || (std::same_as<T, formalism::AuxiliaryTag> && std::same_as<Op, formalism::planning::OpIncrease>),
                  "Unsupported NumericEffect<Op, T> combination.");

    Index<formalism::planning::GroundNumericEffect<Op, T>> index;
    Index<formalism::planning::GroundFunctionTerm<T>> fterm;
    Data<formalism::planning::GroundFunctionExpression> fexpr;

    Data() = default;
    Data(Index<formalism::planning::GroundNumericEffect<Op, T>> index,
         Index<formalism::planning::GroundFunctionTerm<T>> fterm,
         Data<formalism::planning::GroundFunctionExpression> fexpr) :
        index(index),
        fterm(fterm),
        fexpr(fexpr)
    {
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    void clear() noexcept
    {
        tyr::clear(index);
        tyr::clear(fterm);
        tyr::clear(fexpr);
    }

    auto cista_members() const noexcept { return std::tie(index, fterm, fexpr); }
    auto identifying_members() const noexcept { return std::tie(Op::kind, fterm, fexpr); }
};
}

#endif
