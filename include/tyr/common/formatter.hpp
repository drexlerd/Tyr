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
#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/common/index_mixins.hpp"
#include "tyr/common/uint_mixins.hpp"

#include <array>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <set>
#include <span>
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

/// --- operator<<

template<typename T, size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<T, N>& el);

template<typename Key, typename T, typename Compare, typename Allocator>
std::ostream& operator<<(std::ostream& os, const std::map<Key, T, Compare, Allocator>& el);

template<typename T1, typename T2>
std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2>& el);

template<typename Key, typename Compare, typename Allocator>
std::ostream& operator<<(std::ostream& os, const std::set<Key, Compare, Allocator>& el);

template<typename... Ts>
std::ostream& operator<<(std::ostream& os, const std::tuple<Ts...>& el);

template<typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<Key, T, Hash, KeyEqual, Allocator>& el);

template<typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& operator<<(std::ostream& os, const gtl::flat_hash_map<Key, T, Hash, KeyEqual, Allocator>& el);

template<typename Key, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& operator<<(std::ostream& os, const std::unordered_set<Key, Hash, KeyEqual, Allocator>& el);

template<typename Key, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& operator<<(std::ostream& os, const gtl::flat_hash_set<Key, Hash, KeyEqual, Allocator>& el);

template<typename T, typename Allocator>
std::ostream& operator<<(std::ostream& os, const std::vector<T, Allocator>& el);

template<typename... Ts>
    requires(sizeof...(Ts) > 0)
std::ostream& operator<<(std::ostream& os, const std::variant<Ts...>& el);

template<typename Derived>
std::ostream& operator<<(std::ostream& os, const IndexMixin<Derived>& el);

template<typename Derived, IndexConcept Group>
std::ostream& operator<<(std::ostream& os, const GroupIndexMixin<Derived, Group>& el);

template<typename Derived>
std::ostream& operator<<(std::ostream& os, const FixedUintMixin<Derived>& el);

template<std::unsigned_integral Block>
std::ostream& operator<<(std::ostream& os, const BitsetSpan<Block>& el);

inline std::ostream& operator<<(std::ostream& os, const std::monostate& el);

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::optional<T>& el);

template<typename T, std::size_t Extent>
std::ostream& operator<<(std::ostream& os, const std::span<T, Extent>& el);

// cista

template<typename T>
std::ostream& operator<<(std::ostream& os, const ::cista::optional<T>& el);

template<typename C, typename T>
std::ostream& operator<<(std::ostream& os, const View<::cista::optional<T>, C>& el);

template<typename... Ts>
    requires(sizeof...(Ts) > 0)
std::ostream& operator<<(std::ostream& os, const ::cista::offset::variant<Ts...>& el);

template<typename C, typename... Ts>
    requires(sizeof...(Ts) > 0)
std::ostream& operator<<(std::ostream& os, const View<::cista::offset::variant<Ts...>, C>& el);

template<typename T, template<typename> typename Ptr, bool IndexPointers, typename TemplateSizeType, class Allocator>
std::ostream& operator<<(std::ostream& os, const ::cista::basic_vector<T, Ptr, IndexPointers, TemplateSizeType, Allocator>& el);

template<typename C, typename T, template<typename> typename Ptr, bool IndexPointers, typename TemplateSizeType, class Allocator>
std::ostream& operator<<(std::ostream& os, const View<::cista::basic_vector<T, Ptr, IndexPointers, TemplateSizeType, Allocator>, C>& el);

/// --- print

template<typename T, size_t N>
std::ostream& print(std::ostream& os, const std::array<T, N>& el);

template<typename Key, typename T, typename Compare, typename Allocator>
std::ostream& print(std::ostream& os, const std::map<Key, T, Compare, Allocator>& el);

template<typename T1, typename T2>
std::ostream& print(std::ostream& os, const std::pair<T1, T2>& el);

template<typename Key, typename Compare, typename Allocator>
std::ostream& print(std::ostream& os, const std::set<Key, Compare, Allocator>& el);

template<typename... Ts>
std::ostream& print(std::ostream& os, const std::tuple<Ts...>& el);

template<typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& print(std::ostream& os, const std::unordered_map<Key, T, Hash, KeyEqual, Allocator>& el);

template<typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& print(std::ostream& os, const gtl::flat_hash_map<Key, T, Hash, KeyEqual, Allocator>& el);

template<typename Key, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& print(std::ostream& os, const std::unordered_set<Key, Hash, KeyEqual, Allocator>& el);

template<typename Key, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& print(std::ostream& os, const gtl::flat_hash_set<Key, Hash, KeyEqual, Allocator>& el);

template<typename T, typename Allocator>
std::ostream& print(std::ostream& os, const std::vector<T, Allocator>& el);

template<typename... Ts>
    requires(sizeof...(Ts) > 0)
std::ostream& print(std::ostream& os, const std::variant<Ts...>& el);

template<typename Derived>
std::ostream& print(std::ostream& os, const IndexMixin<Derived>& el);

template<typename Derived, IndexConcept Group>
std::ostream& print(std::ostream& os, const GroupIndexMixin<Derived, Group>& el);

template<typename Derived>
std::ostream& print(std::ostream& os, const FixedUintMixin<Derived>& el);

template<std::unsigned_integral Block>
std::ostream& print(std::ostream& os, const BitsetSpan<Block>& el);

inline std::ostream& print(std::ostream& os, const std::monostate& el);

template<typename T>
std::ostream& print(std::ostream& os, const std::optional<T>& el);

template<typename T, std::size_t Extent>
std::ostream& print(std::ostream& os, const std::span<T, Extent>& el);

// cista

template<typename T>
std::ostream& print(std::ostream& os, const ::cista::optional<T>& el);

template<typename C, typename T>
std::ostream& print(std::ostream& os, const View<::cista::optional<T>, C>& el);

template<typename... Ts>
    requires(sizeof...(Ts) > 0)
std::ostream& print(std::ostream& os, const ::cista::offset::variant<Ts...>& el);

template<typename C, typename... Ts>
    requires(sizeof...(Ts) > 0)
std::ostream& print(std::ostream& os, const View<::cista::offset::variant<Ts...>, C>& el);

template<typename T, template<typename> typename Ptr, bool IndexPointers, typename TemplateSizeType, class Allocator>
std::ostream& print(std::ostream& os, const ::cista::basic_vector<T, Ptr, IndexPointers, TemplateSizeType, Allocator>& el);

template<typename C, typename T, template<typename> typename Ptr, bool IndexPointers, typename TemplateSizeType, class Allocator>
std::ostream& print(std::ostream& os, const View<::cista::basic_vector<T, Ptr, IndexPointers, TemplateSizeType, Allocator>, C>& el);

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

template<std::ranges::input_range Range>
std::ostream& print_range(std::ostream& os, const Range& r, std::string_view open = "[", std::string_view close = "]")
{
    os << open;
    bool first = true;
    for (auto const& x : r)
    {
        if (!first)
            os << ", ";
        first = false;
        print(os, x);  // <-- calls your entire print stack, indentation-aware
    }
    os << close;
    return os;
}

/// --- operator<<

template<typename T, size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<T, N>& el)
{
    return print(os, el);
}

template<typename Key, typename T, typename Compare, typename Allocator>
std::ostream& operator<<(std::ostream& os, const std::map<Key, T, Compare, Allocator>& el)
{
    return print(os, el);
}

template<typename T1, typename T2>
std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2>& el)
{
    return print(os, el);
}

template<typename Key, typename Compare, typename Allocator>
std::ostream& operator<<(std::ostream& os, const std::set<Key, Compare, Allocator>& el)
{
    return print(os, el);
}

template<typename... Ts>
std::ostream& operator<<(std::ostream& os, const std::tuple<Ts...>& el)
{
    return print(os, el);
}

template<typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<Key, T, Hash, KeyEqual, Allocator>& el)
{
    return print(os, el);
}

template<typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& operator<<(std::ostream& os, const gtl::flat_hash_map<Key, T, Hash, KeyEqual, Allocator>& el)
{
    return print(os, el);
}

template<typename Key, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& operator<<(std::ostream& os, const std::unordered_set<Key, Hash, KeyEqual, Allocator>& el)
{
    return print(os, el);
}

template<typename Key, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& operator<<(std::ostream& os, const gtl::flat_hash_set<Key, Hash, KeyEqual, Allocator>& el)
{
    return print(os, el);
}

template<typename T, typename Allocator>
std::ostream& operator<<(std::ostream& os, const std::vector<T, Allocator>& el)
{
    return print(os, el);
}

template<typename... Ts>
    requires(sizeof...(Ts) > 0)
std::ostream& operator<<(std::ostream& os, const std::variant<Ts...>& el)
{
    return print(os, el);
}

template<typename Derived>
std::ostream& operator<<(std::ostream& os, const IndexMixin<Derived>& el)
{
    return print(os, el);
}

template<typename Derived, IndexConcept Group>
std::ostream& operator<<(std::ostream& os, const GroupIndexMixin<Derived, Group>& el)
{
    return print(os, el);
}

template<typename Derived>
std::ostream& operator<<(std::ostream& os, const FixedUintMixin<Derived>& el)
{
    return print(os, el);
}

template<std::unsigned_integral Block>
std::ostream& operator<<(std::ostream& os, const BitsetSpan<Block>& el)
{
    return print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const std::monostate& el) { return print(os, el); }

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::optional<T>& el)
{
    return print(os, el);
}

template<typename T, std::size_t Extent>
std::ostream& operator<<(std::ostream& os, const std::span<T, Extent>& el)
{
    return print(os, el);
}

// cista

template<typename T>
std::ostream& operator<<(std::ostream& os, const ::cista::optional<T>& el)
{
    return print(os, el);
}

template<typename C, typename T>
std::ostream& operator<<(std::ostream& os, const View<::cista::optional<T>, C>& el)
{
    return print(os, el);
}

template<typename... Ts>
    requires(sizeof...(Ts) > 0)
std::ostream& operator<<(std::ostream& os, const ::cista::offset::variant<Ts...>& el)
{
    return print(os, el);
}

template<typename C, typename... Ts>
    requires(sizeof...(Ts) > 0)
std::ostream& operator<<(std::ostream& os, const View<::cista::offset::variant<Ts...>, C>& el)
{
    return print(os, el);
}

template<typename T, template<typename> typename Ptr, bool IndexPointers, typename TemplateSizeType, class Allocator>
std::ostream& operator<<(std::ostream& os, const ::cista::basic_vector<T, Ptr, IndexPointers, TemplateSizeType, Allocator>& el)
{
    return print(os, el);
}

template<typename C, typename T, template<typename> typename Ptr, bool IndexPointers, typename TemplateSizeType, class Allocator>
std::ostream& operator<<(std::ostream& os, const View<::cista::basic_vector<T, Ptr, IndexPointers, TemplateSizeType, Allocator>, C>& el)
{
    return print(os, el);
}

/// --- print

template<typename T, size_t N>
std::ostream& print(std::ostream& os, const std::array<T, N>& el)
{
    return print_range(os, el, "<", ">");
}

template<typename Key, typename T, typename Compare, typename Allocator>
std::ostream& print(std::ostream& os, const std::map<Key, T, Compare, Allocator>& el)
{
    return print_range(os, el, "{", "}");
}

template<typename T1, typename T2>
std::ostream& print(std::ostream& os, const std::pair<T1, T2>& el)
{
    os << "<";
    print(os, el.first);
    os << ", ";
    print(os, el.second);
    os << ">";
    return os;
}

template<typename Key, typename Compare, typename Allocator>
std::ostream& print(std::ostream& os, const std::set<Key, Compare, Allocator>& el)
{
    return print_range(os, el, "{", "}");
}

template<typename... Ts>
std::ostream& print(std::ostream& os, const std::tuple<Ts...>& tuple)
{
    os << "<";
    if constexpr (sizeof...(Ts) > 0)
    {
        std::size_t n = 0;
        std::apply([&os, &n](const Ts&... args) { ((os << (n++ == 0 ? "" : ", "), print(os, args)), ...); }, tuple);
    }
    os << ">";
    return os;
}

template<typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& print(std::ostream& os, const std::unordered_map<Key, T, Hash, KeyEqual, Allocator>& el)
{
    return print_range(os, el, "{", "}");
}

template<typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& print(std::ostream& os, const gtl::flat_hash_map<Key, T, Hash, KeyEqual, Allocator>& el)
{
    return print_range(os, el, "{", "}");
}

template<typename Key, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& print(std::ostream& os, const std::unordered_set<Key, Hash, KeyEqual, Allocator>& el)
{
    return print_range(os, el, "{", "}");
}

template<typename Key, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& print(std::ostream& os, const gtl::flat_hash_set<Key, Hash, KeyEqual, Allocator>& el)
{
    return print_range(os, el, "{", "}");
}

template<typename T, typename Allocator>
std::ostream& print(std::ostream& os, const std::vector<T, Allocator>& el)
{
    return print_range(os, el, "[", "]");
}

template<typename... Ts>
    requires(sizeof...(Ts) > 0)
std::ostream& print(std::ostream& os, const std::variant<Ts...>& el)
{
    std::visit([&](auto&& arg) { print(os, arg); }, el);
    return os;
}

template<typename Derived>
std::ostream& print(std::ostream& os, const IndexMixin<Derived>& el)
{
    return print(os, el.get_value());
}

template<typename Derived, IndexConcept Group>
std::ostream& print(std::ostream& os, const GroupIndexMixin<Derived, Group>& el)
{
    os << "<";
    print(os, el.get_group());
    os << ": ";
    print(os, el.get_value());
    os << ">";
    return os;
}

template<typename Derived>
std::ostream& print(std::ostream& os, const FixedUintMixin<Derived>& el)
{
    return print(os, el.value());
}

template<std::unsigned_integral Block>
std::ostream& print(std::ostream& os, const BitsetSpan<Block>& el)
{
    os << "{";

    size_t pos = el.find_first();
    bool first = true;

    while (pos != BitsetSpan<Block>::npos)
    {
        if (!first)
            os << ", ";
        first = false;

        os << pos;
        pos = el.find_next(pos);
    }

    os << "}";
    return os;
}

inline std::ostream& print(std::ostream& os, const std::monostate& el)
{
    os << "monostate";
    return os;
}

template<typename T>
std::ostream& print(std::ostream& os, const std::optional<T>& el)
{
    if (el.has_value())
        print(os, el.value());
    else
        os << "<nullopt>";
    return os;
}

template<typename T, std::size_t Extent>
std::ostream& print(std::ostream& os, const std::span<T, Extent>& el)
{
    return print_range(os, el, "[", "]");
}

template<typename T>
std::ostream& print(std::ostream& os, const ::cista::optional<T>& el)
{
    if (el.has_value())
        print(os, el.value());
    else
        os << "<nullopt>";
    return os;
}

template<typename C, typename T>
std::ostream& print(std::ostream& os, const View<::cista::optional<T>, C>& el)
{
    if (el.has_value())
        print(os, el.value());
    else
        os << "<nullopt>";
    return os;
}

template<typename... Ts>
    requires(sizeof...(Ts) > 0)
std::ostream& print(std::ostream& os, const ::cista::offset::variant<Ts...>& el)
{
    std::visit([&](auto&& arg) { print(os, arg); }, el);
    return os;
}

template<typename C, typename... Ts>
    requires(sizeof...(Ts) > 0)
std::ostream& print(std::ostream& os, const View<::cista::offset::variant<Ts...>, C>& el)
{
    visit([&os](auto&& arg) { print(os, arg); }, el);
    return os;
}

template<typename T, template<typename> typename Ptr, bool IndexPointers, typename TemplateSizeType, class Allocator>
std::ostream& print(std::ostream& os, const ::cista::basic_vector<T, Ptr, IndexPointers, TemplateSizeType, Allocator>& el)
{
    return print_range(os, el, "[", "]");
}

template<typename C, typename T, template<typename> typename Ptr, bool IndexPointers, typename TemplateSizeType, class Allocator>
std::ostream& print(std::ostream& os, const View<::cista::basic_vector<T, Ptr, IndexPointers, TemplateSizeType, Allocator>, C>& el)
{
    return print_range(os, el, "[", "]");
}

}

#endif
