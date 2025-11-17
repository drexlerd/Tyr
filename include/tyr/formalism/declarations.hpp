/*
 * Copyright (C) 2023 Dominik Drexler
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TYR_FORMALISM_DECLARATIONS_HPP_
#define TYR_FORMALISM_DECLARATIONS_HPP_

#include <concepts>
#include <cstddef>
#include <ranges>
#include <string>
#include <string_view>

namespace tyr::datalog
{

template<typename T>
concept IsNumber = std::integral<std::remove_cvref_t<T>>;

template<typename T>
concept IsNumberRandomAccessRange = std::ranges::random_access_range<T> && IsNumber<std::ranges::range_reference_t<T>>;

template<typename T>
concept IsName = std::convertible_to<T, std::string_view>;

/// We will create a symbol table for variables
template<typename T>
concept IsVariable = requires(const T& a) {
    { a.get_index() } -> IsNumber;
    { a.get_name() } -> IsName;
};

template<typename T>
concept IsRelation = requires(const T& a) {
    { a.get_index() } -> IsNumber;
    { a.get_arity() } -> IsNumber;
};

template<typename T>
concept IsAtom = requires(const T& a) {
    { a.get_index() } -> IsNumber;
    { a.get_relation() } -> IsRelation;
    { a.get_arguments() } -> IsNumberRandomAccessRange;  /// positive numbers for constants, negative for variables
};

template<typename T>
concept IsAtomForwardRange = std::ranges::forward_range<T> && IsAtom<std::ranges::range_reference_t<T>>;

template<typename T>
concept IsFact = requires(const T& a) {
    { a.get_index() } -> IsNumber;
    { a.get_relation() } -> IsRelation;
    { a.get_arguments() } -> IsNumberRandomAccessRange;
};

template<typename T>
concept IsRule = requires(const T& a) {
    { a.get_index() } -> IsNumber;
    { a.get_head() } -> IsAtom;
    { a.get_static_body() } -> IsAtomForwardRange;
    { a.get_fluent_body() } -> IsAtomForwardRange;
};

}

#endif
