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

#ifndef TYR_FORMALISM_PLANNING_FUNCTION_EXPRESSION_DATA_HPP_
#define TYR_FORMALISM_PLANNING_FUNCTION_EXPRESSION_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/function_term_index.hpp"
#include "tyr/formalism/planning/arithmetic_operator_data.hpp"
#include "tyr/formalism/planning/declarations.hpp"

namespace tyr
{
template<>
struct Data<formalism::planning::FunctionExpression>
{
    using Variant = ::cista::offset::variant<float_t,
                                             Data<formalism::planning::ArithmeticOperator<Data<formalism::planning::FunctionExpression>>>,
                                             Index<formalism::FunctionTerm<formalism::StaticTag>>,
                                             Index<formalism::FunctionTerm<formalism::FluentTag>>>;

    Variant value;

    Data() = default;
    Data(Variant value) : value(value) {}

    void clear() noexcept { tyr::clear(value); }

    auto cista_members() const noexcept { return std::tie(value); }
    auto identifying_members() const noexcept { return std::tie(value); }
};

}

#endif
