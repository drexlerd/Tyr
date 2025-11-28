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

#ifndef TYR_COMMON_TYPES_HPP_
#define TYR_COMMON_TYPES_HPP_

#include "tyr/common/declarations.hpp"

#include <cista/containers/variant.h>
#include <cista/containers/vector.h>

namespace tyr
{

template<typename T>
struct Data
{
    static_assert(dependent_false<T>::value, "Data is not defined for type T.");
};

template<typename T>
using DataList = ::cista::offset::vector<Data<T>>;

template<typename T>
struct Index
{
    static_assert(dependent_false<T>::value, "Index is not defined for type T.");
};

template<typename T>
using IndexList = ::cista::offset::vector<Index<T>>;

template<typename T, typename Context>
struct View
{
    // View type is optional for recursing through data using the Context.
};

template<typename T, typename C>
concept IsViewable = requires(T type, const C& context) { View<T, C>(type, context); };

}

#endif