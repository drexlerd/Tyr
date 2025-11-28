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
 *<
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TYR_COMMON_INDEX_MIXIN_HPP_
#define TYR_COMMON_INDEX_MIXIN_HPP_

#include "tyr/common/config.hpp"
#include "tyr/common/declarations.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/types.hpp"

#include <limits>
#include <tuple>

namespace tyr
{

template<typename Derived>
struct IndexMixin
{
    uint_t value {};

    static constexpr uint_t MAX = std::numeric_limits<uint_t>::max();

    IndexMixin() noexcept : value(MAX) {}
    explicit IndexMixin(uint_t value) noexcept : value(value) {}

    static constexpr Derived max() noexcept { return Derived(MAX); }

    // ----------------------------------------------------
    // Comparisons
    // ----------------------------------------------------

    friend constexpr bool operator==(const IndexMixin& lhs, const IndexMixin& rhs) noexcept { return EqualTo<uint_t> {}(lhs.value, rhs.value); }
    friend constexpr bool operator!=(const IndexMixin& lhs, const IndexMixin& rhs) noexcept { return !(lhs == rhs); }
    friend constexpr bool operator<=(const IndexMixin& lhs, const IndexMixin& rhs) noexcept { return lhs.value <= rhs.value; }
    friend constexpr bool operator<(const IndexMixin& lhs, const IndexMixin& rhs) noexcept { return lhs.value < rhs.value; }
    friend constexpr bool operator>=(const IndexMixin& lhs, const IndexMixin& rhs) noexcept { return lhs.value >= rhs.value; }
    friend constexpr bool operator>(const IndexMixin& lhs, const IndexMixin& rhs) noexcept { return lhs.value > rhs.value; }

    explicit constexpr operator uint_t() const noexcept { return value; }

    uint_t get_value() const noexcept { return value; }

    auto cista_members() const noexcept { return std::tie(value); }
    auto identifying_members() const noexcept { return std::tie(value); }
};

}

#endif
