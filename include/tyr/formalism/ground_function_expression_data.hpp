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

#ifndef TYR_FORMALISM_GROUND_FUNCTION_EXPRESSION_DATA_HPP_
#define TYR_FORMALISM_GROUND_FUNCTION_EXPRESSION_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/arithmetic_operator_data.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_function_term_index.hpp"

namespace tyr
{
template<>
struct Data<formalism::GroundFunctionExpression>
{
    using Tag = formalism::GroundFunctionExpression;

    using Variant = ::cista::offset::variant<float_t,
                                             Data<formalism::ArithmeticOperator<Data<formalism::GroundFunctionExpression>>>,
                                             Index<formalism::GroundFunctionTerm<formalism::StaticTag>>,
                                             Index<formalism::GroundFunctionTerm<formalism::FluentTag>>,
                                             Index<formalism::GroundFunctionTerm<formalism::AuxiliaryTag>>>;

    Variant value;

    Data() = default;
    Data(Variant value) : value(value) {}

    void clear() noexcept { tyr::clear(value); }

    auto cista_members() const noexcept { return std::tie(value); }
    auto identifying_members() const noexcept { return std::tie(value); }
};

}

#endif
