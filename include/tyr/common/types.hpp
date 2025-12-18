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

#include <cista/containers/optional.h>
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

template<typename T, typename C>
struct View
{
    // View type is optional for recursing through data using the C.

    // DEBUG guard: blow up if the *primary* template is instantiated for Domain
    // static_assert(!(std::is_same_v<T, Index<formalism::Domain>>),
    //              "Primary View<Index<formalism::Domain>, formalism::Repository> "
    //              "is being instantiated here. You are missing #include <tyr/formalism/views.hpp> "
    //              "before using this combination.");
};

template<typename T, typename C>
concept ViewConcept = requires(T type, const C& context) {
    // Constructor
    View<T, C>(type, context);
    // Method to retrieve the underlying Data.
    { View<T, C>(type, context).get_data() };
    // Method to retrieve the underlying context.
    { View<T, C>(type, context).get_context() };
    // Method to retrieve the underlying lightweight handle or data.
    { View<T, C>(type, context).get_handle() } -> std::same_as<const T&>;
};

template<typename T>
concept Indexable = requires(T a) {
    { a.get_value() } -> std::same_as<uint_t>;
};

/// @brief Helper to create a view
template<typename T, typename C>
auto make_view(const T& element, const C& context) noexcept
{
    return View<T, C>(element, context);
}

template<typename T, typename C>
auto make_view(T&& element, C&& context) noexcept
{
    using TT = std::remove_cvref_t<T>;
    using CC = std::remove_cvref_t<C>;
    return View<TT, CC>(std::forward<T>(element), std::forward<C>(context));
}

}

#endif