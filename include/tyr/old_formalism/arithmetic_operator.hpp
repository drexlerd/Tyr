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

#ifndef TYR_FORMALISM_ARITHMETIC_OPERATOR_HPP_
#define TYR_FORMALISM_ARITHMETIC_OPERATOR_HPP_

#include "tyr/formalism/binary_operator_index.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/multi_operator_index.hpp"

namespace tyr::formalism
{
template<typename T>
struct ArithmeticOperator
{
    using Variant = ::cista::offset::variant<BinaryOperatorIndex<OpAdd, T>,
                                             BinaryOperatorIndex<OpSub, T>,
                                             BinaryOperatorIndex<OpMul, T>,
                                             BinaryOperatorIndex<OpDiv, T>,
                                             MultiOperatorIndex<OpAdd, T>,
                                             MultiOperatorIndex<OpMul, T>>;

    Variant value;

    ArithmeticOperator() = default;
    ArithmeticOperator(Variant value) : value(value) {}

    friend bool operator==(const ArithmeticOperator& lhs, const ArithmeticOperator& rhs) { return EqualTo<Variant> {}(lhs.value, rhs.value); }

    auto cista_members() const noexcept { return std::tie(value); }
    auto identifying_members() const noexcept { return std::tie(value); }
};

template<typename T>
using ArithmeticOperatorList = ::cista::offset::vector<ArithmeticOperator<T>>;
}

#endif
