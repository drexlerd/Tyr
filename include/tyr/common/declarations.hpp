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

#ifndef TYR_COMMON_CONCEPTS_HPP_
#define TYR_COMMON_CONCEPTS_HPP_

#include "tyr/common/config.hpp"
#include "tyr/common/type_traits.hpp"
#include "tyr/common/types.hpp"

#include <boost/hana.hpp>
#include <cista/containers/string.h>
#include <cista/containers/variant.h>
#include <cista/containers/vector.h>
#include <concepts>
#include <functional>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace tyr
{

/**
 * General utility
 */

/// @brief Checks whether T is a floating point
template<typename T>
concept IsFloatingPoint = std::is_floating_point_v<T>;

/// @brief Checks whether T is a hana map
template<typename T>
concept IsHanaMap = std::same_as<typename boost::hana::tag_of<T>::type, boost::hana::map_tag>;

/// @brief Check whether T has a function that returns members that aims to identify the class.
template<typename T>
concept HasIdentifyingMembers = requires(const T a) {
    { a.identifying_members() };
};

template<typename T>
struct dependent_false : std::false_type
{
};

template<typename T>
concept HasValue = requires(const T& a) {
    { a.get_value() } -> std::same_as<uint_t>;
};

template<typename T>
concept HasGroup = requires(const T& a) {
    { a.get_group() } -> HasValue;
};

template<typename T>
concept HasTag = requires { typename T::Tag; };

/// @brief Check whether T is a flat type.
template<typename Tag>
concept IsFlatType = HasValue<Index<Tag>> && !HasGroup<Index<Tag>>;

/// @brief Check whether T is a group type.
template<typename Tag>
concept IsGroupType = HasValue<Index<Tag>> && HasGroup<Index<Tag>>;

/// @brief Check whether T is proxyable.
template<typename Tag, typename C>
concept IsProxyable = requires(Index<Tag> index, const C& context) { Proxy<Tag, C>(index, context); }
                      || requires(Data<Tag> data, const C& context) { Proxy<Tag, C>(data, context); };

/**
 * Forward declarations and type defs
 */

template<typename T>
struct Hash;

template<typename T>
struct EqualTo;

template<typename T>
class ObserverPtr;

template<typename T>
using UnorderedSet = std::unordered_set<T, Hash<T>, EqualTo<T>>;

template<typename T, typename V>
using UnorderedMap = std::unordered_map<T, V, Hash<T>, EqualTo<T>>;
}

#endif
