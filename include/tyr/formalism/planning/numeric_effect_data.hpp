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

#ifndef TYR_FORMALISM_PLANNING_NUMERIC_EFFECT_DATA_HPP_
#define TYR_FORMALISM_PLANNING_NUMERIC_EFFECT_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/function_expression_data.hpp"
#include "tyr/formalism/planning/function_term_index.hpp"
#include "tyr/formalism/planning/numeric_effect_index.hpp"

namespace tyr
{

template<formalism::planning::NumericEffectOpKind Op, formalism::FactKind T>
struct Data<formalism::planning::NumericEffect<Op, T>>
{
    static_assert(std::same_as<T, formalism::FluentTag> || (std::same_as<T, formalism::AuxiliaryTag> && std::same_as<Op, formalism::planning::OpIncrease>),
                  "Unsupported NumericEffect<Op, T> combination.");

    Index<formalism::planning::NumericEffect<Op, T>> index;
    Index<formalism::planning::FunctionTerm<T>> fterm;
    Data<formalism::planning::FunctionExpression> fexpr;

    Data() = default;
    Data(Index<formalism::planning::FunctionTerm<T>> fterm_, Data<formalism::planning::FunctionExpression> fexpr_) : index(), fterm(fterm_), fexpr(fexpr_) {}
    // Python constructor
    template<typename C>
    Data(View<Index<formalism::planning::FunctionTerm<T>>, C> fterm_, View<Data<formalism::planning::FunctionExpression>, C> fexpr_) : index(), fterm(), fexpr()
    {
        set(fterm_, fterm);
        set(fexpr_, fexpr);
    }
    Data(const Data& other) = default;
    Data& operator=(const Data& other) = default;
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

static_assert(!uses_trivial_storage_v<formalism::planning::NumericEffect<formalism::planning::OpAssign, formalism::FluentTag>>);

}

#endif
