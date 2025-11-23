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

#ifndef TYR_COMMON_CONCEPTS_HPP_
#define TYR_COMMON_CONCEPTS_HPP_

#include "tyr/common/config.hpp"
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

/**
 * We distinguish between Tag, instantiations of Data<Tag>, Index<Tag>, Proxy<Tag>, and other data types.
 * - Lightweight types might not define an Index<Tag> specialization,
 *   in which case we would like to store Data<Tag> directly inplace.
 * - Data<Tag> stores either Index<Tag2> or Data<Tag2> or other data types.
 * - Index<Tag> is just 1 or 2 integers to reference Data<Tag>.
 * - Proxy<Tag> is just an Index<Tag> and a Context to recurse through the underlying Data.
 */

template<typename T>
concept HasTag = requires { typename T::Tag; };

template<typename T, bool HasTag>
struct UnderlyingTagOrTImpl_Helper
{
    using Type = T;
};

template<typename T>
struct UnderlyingTagOrTImpl_Helper<T, true>
{
    using Type = typename T::Tag;
};

/// @brief Retrieve the Tag of T if available, allowing us to analyze more details, and otherwise, use T.
template<typename T>
using UnderlyingTagOrT = typename UnderlyingTagOrTImpl_Helper<T, HasTag<T>>::Type;

/// @brief The concept succeeds if there is an Index specialization associated with T.
/// Data will be deduplicated and referenced via Index.
template<typename T>
concept IsIndexStorage = HasTag<Index<UnderlyingTagOrT<T>>>;

/// @brief The concept succeeds if there is a Data but no Index specialization associated with T.
/// Data will be stored inline, and hence, should be cheap.
template<typename T>
concept IsDataStorage = !IsIndexStorage<UnderlyingTagOrT<T>> && HasTag<Data<UnderlyingTagOrT<T>>>;

/* Extract type to store data. */

template<typename T, typename = void>
struct StorageTypeImpl
{
    using Type = T;
};

template<typename T>
    requires IsIndexStorage<T>
struct StorageTypeImpl<T>
{
    using Type = Data<T>;
};

template<typename T>
    requires(!IsIndexStorage<T> && IsDataStorage<T>)
struct StorageTypeImpl<T>
{
    using Type = Data<T>;
};

template<typename T>
using StorageType = typename StorageTypeImpl<UnderlyingTagOrT<T>>::Type;

/* Extract type to reference data. */

template<typename T, typename = void>
struct ReferenceTypeImpl
{
    using Type = T;
};

template<typename T>
    requires IsIndexStorage<T>
struct ReferenceTypeImpl<T>
{
    using Type = Index<T>;
};

template<typename T>
    requires(!IsIndexStorage<T> && IsDataStorage<T>)
struct ReferenceTypeImpl<T>
{
    using Type = Data<T>;
};

template<typename T>
using ReferenceType = typename ReferenceTypeImpl<UnderlyingTagOrT<T>>::Type;

/* Extract type to proxy data. */

template<typename T, typename C, typename = void>
struct ProxyTypeImpl
{
    using Type = T;
};

template<typename T, typename C>
    requires IsIndexStorage<T>
struct ProxyTypeImpl<T, C>
{
    using Type = Proxy<T, C>;
};

template<typename T, typename C>
    requires(!IsIndexStorage<T> && IsDataStorage<T>)
struct ProxyTypeImpl<T, C>
{
    using Type = Proxy<T, C>;
};

template<typename T, typename C>
using ProxyType = typename ProxyTypeImpl<UnderlyingTagOrT<T>, C>::Type;

/* Define requirements on Index. */

template<typename T>
concept HasValue = requires(const T& a) {
    { a.get_value() } -> std::same_as<uint_t>;
};

template<typename T>
concept HasGroup = requires(const T& a) {
    { a.get_group() } -> HasValue;
};

/// @brief Check whether T is a flat type.
template<typename T>
concept IsFlatType = HasValue<Index<UnderlyingTagOrT<T>>> && !HasGroup<Index<UnderlyingTagOrT<T>>>;

/// @brief Check whether T is a group type.
template<typename T>
concept IsGroupType = HasValue<Index<UnderlyingTagOrT<T>>> && HasGroup<Index<UnderlyingTagOrT<T>>>;

/// @brief Check whether T is proxyable.
template<typename T, typename C>
concept IsProxyable = requires(Index<UnderlyingTagOrT<T>> index, const C& context) { Proxy<UnderlyingTagOrT<T>, C>(index, context); }
                      || requires(Data<UnderlyingTagOrT<T>> data, const C& context) { Proxy<UnderlyingTagOrT<T>, C>(data, context); };

/**
 * Forward declarations and type defs
 */

template<typename T>
struct Hash;

template<typename T>
struct EqualTo;

template<typename T>
class ObserverPtr;

template<typename Derived>
struct FlatIndexMixin;

template<typename Derived, HasValue Group>
struct GroupIndexMixin;

template<typename T>
using UnorderedSet = std::unordered_set<T, Hash<T>, EqualTo<T>>;

template<typename T, typename V>
using UnorderedMap = std::unordered_map<T, V, Hash<T>, EqualTo<T>>;
}

#endif
