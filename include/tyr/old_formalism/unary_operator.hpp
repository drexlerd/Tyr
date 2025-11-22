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

#ifndef TYR_FORMALISM_UNARY_OPERATOR_HPP_
#define TYR_FORMALISM_UNARY_OPERATOR_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/unary_operator_index.hpp"

namespace tyr::formalism
{
template<IsOp Op, typename T>
struct UnaryOperator
{
    UnaryOperatorIndex<Op, T> index;
    T arg;

    UnaryOperator() = default;
    UnaryOperator(UnaryOperatorIndex<Op, T> index, T lhs, T rhs) : index(index), arg(arg) {}
    UnaryOperator(const UnaryOperator& other) = delete;
    UnaryOperator& operator=(const UnaryOperator& other) = delete;
    UnaryOperator(UnaryOperator&& other) = default;
    UnaryOperator& operator=(UnaryOperator&& other) = default;

    auto cista_members() const noexcept { return std::tie(index, arg); }
    auto identifying_members() const noexcept { return std::tie(arg); }
};

}

#endif
