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

#ifndef TYR_COMMON_DECLARATIONS_HPP_
#define TYR_COMMON_DECLARATIONS_HPP_

#include "tyr/common/config.hpp"

#include <cista/containers/optional.h>
#include <cista/containers/string.h>
#include <cista/containers/variant.h>
#include <cista/containers/vector.h>
#include <concepts>
#include <functional>
#include <gtl/phmap.hpp>
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

/// @brief Check whether T has a function that returns members that aims to identify the class.
template<typename T>
concept Identifiable = requires(const T a) {
    { a.identifying_members() };
};

template<class T>
concept PointerLike = requires(T t) {
    { static_cast<bool>(t) } -> std::convertible_to<bool>;
    { *t };
};

template<class T>
concept OptionalLike = requires(T t) {
    { t.has_value() } -> std::convertible_to<bool>;
    { *t };
};

template<typename T>
struct dependent_false : std::false_type
{
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
using UnorderedSet = gtl::flat_hash_set<T, Hash<T>, EqualTo<T>>;

template<typename T, typename V>
using UnorderedMap = gtl::flat_hash_map<T, V, Hash<T>, EqualTo<T>>;
}

#endif
