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

#ifndef TYR_FORMALISM_DATALOG_BOOLEAN_OPERATOR_DATA_HPP_
#define TYR_FORMALISM_DATALOG_BOOLEAN_OPERATOR_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/datalog/binary_operator_index.hpp"
#include "tyr/formalism/datalog/declarations.hpp"

namespace tyr
{
template<typename T>
struct Data<formalism::datalog::BooleanOperator<T>>
{
    using Variant = ::cista::offset::variant<Index<formalism::datalog::BinaryOperator<formalism::OpEq, T>>,
                                             Index<formalism::datalog::BinaryOperator<formalism::OpNe, T>>,
                                             Index<formalism::datalog::BinaryOperator<formalism::OpLe, T>>,
                                             Index<formalism::datalog::BinaryOperator<formalism::OpLt, T>>,
                                             Index<formalism::datalog::BinaryOperator<formalism::OpGe, T>>,
                                             Index<formalism::datalog::BinaryOperator<formalism::OpGt, T>>>;

    Variant value;

    Data() = default;
    Data(Variant value) : value(value) {}

    void clear() noexcept { tyr::clear(value); }

    auto cista_members() const noexcept { return std::tie(value); }
    auto identifying_members() const noexcept { return std::tie(value); }
};
}

#endif
