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

#ifndef TYR_FORMALISM_MULTI_OPERATOR_DATA_HPP_
#define TYR_FORMALISM_MULTI_OPERATOR_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/multi_operator_index.hpp"

namespace tyr
{
template<formalism::IsOp Op, typename ArgTag>
struct Data<formalism::MultiOperator<Op, ArgTag>>
{
    using Tag = formalism::MultiOperator<Op, ArgTag>;

    using ArgStorageType = std::conditional_t<HasTag<Index<ArgTag>>, Index<ArgTag>, Data<ArgTag>>;

    Index<formalism::MultiOperator<Op, ArgTag>> index;
    ::cista::offset::vector<ArgStorageType> args;

    Data() = default;
    Data(Index<formalism::MultiOperator<Op, ArgTag>> index, ::cista::offset::vector<ArgStorageType> args) : index(index), args(std::move(args)) {}
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    auto cista_members() const noexcept { return std::tie(index, args); }
    auto identifying_members() const noexcept { return std::tie(args); }
};

}

#endif
