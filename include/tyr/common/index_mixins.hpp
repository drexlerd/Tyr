/*
 * Copyright (C) 2023 Dominik Drexler and Simon Stahlberg
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

#include <tuple>

namespace tyr
{

template<typename Derived>
struct FlatIndexMixin
{
    uint_t value {};

    FlatIndexMixin() = default;
    explicit FlatIndexMixin(uint_t value) : value(value) {}

    uint_t get_value() const noexcept { return value; }

    friend bool operator==(const Derived& lhs, const Derived& rhs) noexcept { return EqualTo<uint_t> {}(lhs.value, rhs.value); }
    friend bool operator<(const Derived& lhs, const Derived& rhs) noexcept { return lhs.value < rhs.value; }

    auto cista_members() const noexcept { return std::tie(value); }
    auto identifying_members() const noexcept { return std::tie(value); }
};

template<typename Derived, IsIndexType Group>
struct GroupIndexMixin
{
    Group group {};
    uint_t value {};

    GroupIndexMixin() = default;
    GroupIndexMixin(Group group, uint_t value) : group(group), value(value) {}

    uint_t get_value() const noexcept { return value; }
    Group get_group() const noexcept { return group; }

    friend bool operator==(const Derived& lhs, const Derived& rhs) noexcept
    {
        return EqualTo<Group> {}(lhs.group, rhs.group) && EqualTo<uint_t> {}(lhs.value, rhs.value);
    }
    friend bool operator<(const Derived& lhs, const Derived& rhs) noexcept
    {
        if (lhs.group == rhs.group)
        {
            return lhs.value < rhs.value;
        }
        return lhs.group < rhs.group;
    }

    auto cista_members() const noexcept { return std::tie(group, value); }
    auto identifying_members() const noexcept { return std::tie(group, value); }
};

}

#endif
