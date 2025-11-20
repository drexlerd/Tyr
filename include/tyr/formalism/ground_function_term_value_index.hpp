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

#ifndef TYR_FORMALISM_GROUND_FUNCTION_TERM_VALUE_INDEX_HPP_
#define TYR_FORMALISM_GROUND_FUNCTION_TERM_VALUE_INDEX_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function_index.hpp"

namespace tyr::formalism
{
template<IsStaticOrFluentTag T>
struct GroundFunctionTermValueIndex
{
    template<IsContext C>
    using ProxyType = GroundFunctionTermValueProxy<T, C>;

    FunctionIndex<T> function_index {};
    uint_t value {};

    GroundFunctionTermValueIndex() = default;
    explicit GroundFunctionTermValueIndex(FunctionIndex<T> function_index, uint_t value) : function_index(function_index), value(value) {}

    friend bool operator==(const GroundFunctionTermValueIndex& lhs, const GroundFunctionTermValueIndex& rhs)
    {
        return EqualTo<uint_t> {}(lhs.value, rhs.value);
    }

    uint_t get() const noexcept { return value; }

    auto cista_members() const noexcept { return std::tie(value); }
    auto identifying_members() const noexcept { return std::tie(value); }
};

template<IsStaticOrFluentTag T>
using GroundFunctionTermValueIndexList = ::cista::offset::vector<GroundFunctionTermValueIndex<T>>;
}

#endif
