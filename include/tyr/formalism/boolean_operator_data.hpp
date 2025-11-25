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

#ifndef TYR_FORMALISM_BOOLEAN_OPERATOR_DATA_HPP_
#define TYR_FORMALISM_BOOLEAN_OPERATOR_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/formalism/binary_operator_index.hpp"
#include "tyr/formalism/declarations.hpp"

namespace tyr
{
template<typename T>
struct Data<formalism::BooleanOperator<T>>
{
    using Tag = formalism::BooleanOperator<T>;

    using Variant = ::cista::offset::variant<Index<formalism::BinaryOperator<formalism::OpEq, T>>,
                                             Index<formalism::BinaryOperator<formalism::OpLe, T>>,
                                             Index<formalism::BinaryOperator<formalism::OpLt, T>>,
                                             Index<formalism::BinaryOperator<formalism::OpGe, T>>,
                                             Index<formalism::BinaryOperator<formalism::OpGt, T>>>;

    Variant value;
    uint_t arity;

    Data() = default;
    Data(Variant value) : value(value) {}

    friend bool operator==(const Data& lhs, const Data& rhs) { return EqualTo<Variant> {}(lhs.value, rhs.value); }
    friend bool operator<(const Data& lhs, const Data& rhs) { return lhs.value < rhs.value; }

    auto cista_members() const noexcept { return std::tie(value, arity); }
    auto identifying_members() const noexcept { return std::tie(value, arity); }
};
}

#endif
