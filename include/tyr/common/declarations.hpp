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

/**
 * Index, data, and proxy concepts
 */

/// @brief Check whether T is any index type.
template<typename T>
concept IsIndexType = requires(const T& a) {
    // Required traits
    typename IndexTraits<T>::DataType;
    // Required functions
    { a.get_value() } -> std::same_as<uint_t>;
};

/// @brief Check whether T is a group index type.
template<typename T>
concept IsGroupIndexType = requires(const T& a) {
    // Required traits
    typename IndexTraits<T>::DataType;
    // Required functions
    { a.get_group() } -> IsIndexType;
    { a.get_value() } -> std::same_as<uint_t>;
};

/// @brief Check whether T is a flat index type.
template<typename T>
concept IsFlatIndexType = IsIndexType<T> && !IsGroupIndexType<T>;

template<typename T, typename C>
concept IndexTypeHasProxy = requires { typename IndexTraits<T>::template ProxyType<C>; };

/// @brief Check whether T is a data type for an arbitrary context C.
template<typename T, typename C>
concept IsDataType = requires {
    // Required traits
    typename DataTraits<T>::IndexType;
    typename DataTraits<T>::template ProxyType<C>;
};

template<typename T>
concept IsFlatDataType = requires {
    typename DataTraits<T>::IndexType;
    requires IsFlatIndexType<typename DataTraits<T>::IndexType>;
};

template<typename T>
concept IsGroupDataType = requires {
    typename DataTraits<T>::IndexType;
    requires IsGroupIndexType<typename DataTraits<T>::IndexType>;
};

/// @brief Check whether T is a proxy type.
template<typename T>
concept IsProxyType = requires {
    // Required traits
    typename ProxyTraits<T>::IndexType;
    typename ProxyTraits<T>::DataType;
};

template<typename T>
concept IsFlatProxyType = requires {
    typename ProxyTraits<T>::IndexType;
    requires IsFlatIndexType<typename ProxyTraits<T>::IndexType>;
};

template<typename T>
concept IsGroupProxyType = requires {
    typename ProxyTraits<T>::IndexType;
    requires IsGroupIndexType<typename ProxyTraits<T>::IndexType>;
};

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
