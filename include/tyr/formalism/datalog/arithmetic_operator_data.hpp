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

#ifndef TYR_FORMALISM_DATALOG_ARITHMETIC_OPERATOR_DATA_HPP_
#define TYR_FORMALISM_DATALOG_ARITHMETIC_OPERATOR_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/datalog/binary_operator_index.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/multi_operator_index.hpp"
#include "tyr/formalism/datalog/unary_operator_index.hpp"

namespace tyr
{
template<typename T>
struct Data<formalism::datalog::ArithmeticOperator<T>>
{
    using Variant = ::cista::offset::variant<Index<formalism::datalog::UnaryOperator<formalism::OpSub, T>>,
                                             Index<formalism::datalog::BinaryOperator<formalism::OpAdd, T>>,
                                             Index<formalism::datalog::BinaryOperator<formalism::OpSub, T>>,
                                             Index<formalism::datalog::BinaryOperator<formalism::OpMul, T>>,
                                             Index<formalism::datalog::BinaryOperator<formalism::OpDiv, T>>,
                                             Index<formalism::datalog::MultiOperator<formalism::OpAdd, T>>,
                                             Index<formalism::datalog::MultiOperator<formalism::OpMul, T>>>;

    Variant value;

    Data() = default;
    Data(Variant value) : value(value) {}

    void clear() noexcept { tyr::clear(value); }

    auto cista_members() const noexcept { return std::tie(value); }
    auto identifying_members() const noexcept { return std::tie(value); }
};
}

#endif
