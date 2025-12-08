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

#ifndef TYR_COMMON_FORMATTER_HPP_
#define TYR_COMMON_FORMATTER_HPP_

#include "tyr/common/declarations.hpp"
#include "tyr/common/index_mixins.hpp"
#include "tyr/common/uint_mixins.hpp"

#include <array>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <sstream>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace tyr
{

/**
 * Forward declarations
 */

template<typename T, size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<T, N>& arr);

template<typename Key, typename T, typename Compare, typename Allocator>
std::ostream& operator<<(std::ostream& os, const std::map<Key, T, Compare, Allocator>& map);

template<typename T1, typename T2>
std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2>& pair);

template<typename Key, typename Compare, typename Allocator>
std::ostream& operator<<(std::ostream& os, const std::set<Key, Compare, Allocator>& set);

template<typename... Ts>
std::ostream& operator<<(std::ostream& os, const std::tuple<Ts...>& tuple);

template<typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<Key, T, Hash, KeyEqual, Allocator>& map);

template<typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& operator<<(std::ostream& os, const gtl::flat_hash_map<Key, T, Hash, KeyEqual, Allocator>& map);

template<typename Key, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& operator<<(std::ostream& os, const std::unordered_set<Key, Hash, KeyEqual, Allocator>& set);

template<typename Key, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& operator<<(std::ostream& os, const gtl::flat_hash_set<Key, Hash, KeyEqual, Allocator>& set);

template<typename T, typename Allocator>
std::ostream& operator<<(std::ostream& os, const std::vector<T, Allocator>& vec);

template<typename... Ts>
    requires(sizeof...(Ts) > 0)
std::ostream& operator<<(std::ostream& os, const std::variant<Ts...>& variant);

template<typename Derived>
std::ostream& operator<<(std::ostream& os, const IndexMixin<Derived>& mixin);

template<typename Derived>
std::ostream& operator<<(std::ostream& os, const FixedUintMixin<Derived>& mixin);

template<typename... Ts>
    requires(sizeof...(Ts) > 0)
std::ostream& print(std::ostream& os, const ::cista::offset::variant<Ts...>& el);

template<typename C, typename... Ts>
    requires(sizeof...(Ts) > 0)
std::ostream& print(std::ostream& os, const View<::cista::offset::variant<Ts...>, C>& el);

template<typename T, template<typename> typename Ptr, bool IndexPointers, typename TemplateSizeType, class Allocator>
std::ostream& print(std::ostream& os, const ::cista::basic_vector<T, Ptr, IndexPointers, TemplateSizeType, Allocator>& vec);

template<typename C, typename T, template<typename> typename Ptr, bool IndexPointers, typename TemplateSizeType, class Allocator>
std::ostream& print(std::ostream& os, const View<::cista::basic_vector<T, Ptr, IndexPointers, TemplateSizeType, Allocator>, C>& vec);

/**
 * ADL-enabled stream helper: finds operator<< in the type's namespace
 */

template<class T>
std::ostream& print(std::ostream& os, const T& t)
{
    return os << t;
}

/**
 * Helpers to materialize strings
 */

template<typename T>
std::string to_string(const T& element)
{
    std::stringstream ss;

    if constexpr (OptionalLike<T>)
    {
        if (element.has_value())
            print(ss, *element);
        else
            ss << "<nullopt>";
    }
    else if constexpr (PointerLike<T>)
    {
        if (element)
            print(ss, *element);
        else
            ss << "<nullptr>";
    }
    else
    {
        print(ss, element);
    }

    return ss.str();
}

template<std::ranges::input_range Range>
std::vector<std::string> to_strings(const Range& range)
{
    auto result = std::vector<std::string> {};
    if constexpr (std::ranges::sized_range<Range>)
        result.reserve(std::ranges::size(range));
    for (const auto& element : range)
        result.push_back(to_string(element));
    return result;
}

/**
 * Definitions
 */

template<typename T, size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<T, N>& arr)
{
    fmt::print(os, "<{}>", fmt::join(to_strings(arr), ", "));
    return os;
}

template<typename Key, typename T, typename Compare, typename Allocator>
std::ostream& operator<<(std::ostream& os, const std::map<Key, T, Compare, Allocator>& map)
{
    fmt::print(os, "{{{}}}", fmt::join(to_strings(map), ", "));
    return os;
}

template<typename T1, typename T2>
std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2>& pair)
{
    fmt::print(os, "<{},{}>", to_string(pair.first), to_string(pair.second));
    return os;
}

template<typename Key, typename Compare, typename Allocator>
std::ostream& operator<<(std::ostream& os, const std::set<Key, Compare, Allocator>& set)
{
    fmt::print(os, "{{{}}}", fmt::join(to_strings(set), ", "));
    return os;
}

template<typename... Ts>
std::ostream& operator<<(std::ostream& os, const std::tuple<Ts...>& tuple)
{
    os << "<";
    if constexpr (sizeof...(Ts) > 0)
    {
        std::size_t n = 0;
        std::apply([&os, &n](const Ts&... args) { ((os << (n++ == 0 ? "" : ", ") << to_string(args)), ...); }, tuple);
    }
    os << ">";
    return os;
}

template<typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<Key, T, Hash, KeyEqual, Allocator>& map)
{
    fmt::print(os, "{{{}}}", fmt::join(to_strings(map), ", "));
    return os;
}

template<typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& operator<<(std::ostream& os, const gtl::flat_hash_map<Key, T, Hash, KeyEqual, Allocator>& map)
{
    fmt::print(os, "{{{}}}", fmt::join(to_strings(map), ", "));
    return os;
}

template<typename Key, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& operator<<(std::ostream& os, const std::unordered_set<Key, Hash, KeyEqual, Allocator>& set)
{
    fmt::print(os, "{{{}}}", fmt::join(to_strings(set), ", "));
    return os;
}

template<typename Key, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& operator<<(std::ostream& os, const gtl::flat_hash_set<Key, Hash, KeyEqual, Allocator>& set)
{
    fmt::print(os, "{{{}}}", fmt::join(to_strings(set), ", "));
    return os;
}

template<typename T, typename Allocator>
std::ostream& operator<<(std::ostream& os, const std::vector<T, Allocator>& vec)
{
    fmt::print(os, "[{}]", fmt::join(to_strings(vec), ", "));
    return os;
}

template<typename... Ts>
    requires(sizeof...(Ts) > 0)
std::ostream& operator<<(std::ostream& os, const std::variant<Ts...>& variant)
{
    std::visit([&](auto&& arg) { os << to_string(arg); }, variant);
    return os;
}

template<typename Derived>
std::ostream& operator<<(std::ostream& os, const IndexMixin<Derived>& mixin)
{
    os << to_string(mixin.get_value());
    return os;
}

template<typename Derived>
std::ostream& operator<<(std::ostream& os, const FixedUintMixin<Derived>& mixin)
{
    os << to_string(mixin.value());
    return os;
}

template<typename... Ts>
    requires(sizeof...(Ts) > 0)
std::ostream& print(std::ostream& os, const ::cista::offset::variant<Ts...>& variant)
{
    std::visit([&](auto&& arg) { os << to_string(arg); }, variant);
    return os;
}

template<typename C, typename... Ts>
    requires(sizeof...(Ts) > 0)
std::ostream& print(std::ostream& os, const View<::cista::offset::variant<Ts...>, C>& el)
{
    visit([&os](auto&& arg) { os << to_string(arg); }, el);
    return os;
}

template<typename T, template<typename> typename Ptr, bool IndexPointers, typename TemplateSizeType, class Allocator>
std::ostream& print(std::ostream& os, const ::cista::basic_vector<T, Ptr, IndexPointers, TemplateSizeType, Allocator>& vec)
{
    fmt::print(os, "[{}]", fmt::join(to_strings(vec), ", "));
    return os;
}

template<typename C, typename T, template<typename> typename Ptr, bool IndexPointers, typename TemplateSizeType, class Allocator>
std::ostream& print(std::ostream& os, const View<::cista::basic_vector<T, Ptr, IndexPointers, TemplateSizeType, Allocator>, C>& vec)
{
    fmt::print(os, "[{}]", fmt::join(to_strings(vec), ", "));
    return os;
}

}

#endif
