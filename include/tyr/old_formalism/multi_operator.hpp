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

#ifndef TYR_FORMALISM_MULTI_OPERATOR_HPP_
#define TYR_FORMALISM_MULTI_OPERATOR_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/multi_operator_index.hpp"

namespace tyr::formalism
{
template<IsOp Op, typename T>
struct MultiOperator
{
    MultiOperatorIndex<Op, T> index;
    ::cista::offset::vector<T> args;

    MultiOperator() = default;
    MultiOperator(MultiOperatorIndex<Op, T> index, ::cista::offset::vector<T> args) : index(index), args(std::move(args)) {}
    MultiOperator(const MultiOperator& other) = delete;
    MultiOperator& operator=(const MultiOperator& other) = delete;
    MultiOperator(MultiOperator&& other) = default;
    MultiOperator& operator=(MultiOperator&& other) = default;

    auto cista_members() const noexcept { return std::tie(index, args); }
    auto identifying_members() const noexcept { return std::tie(args); }
};

}

#endif
