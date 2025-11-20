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

#include <boost/hana.hpp>
#include <cista/containers/string.h>
#include <cista/containers/variant.h>
#include <cista/containers/vector.h>
#include <concepts>
#include <functional>
#include <tuple>
#include <type_traits>
#include <vector>

namespace tyr
{

template<typename T>
concept IsFloatingPoint = std::is_floating_point_v<T>;

template<typename T>
concept HasIdentifyingMembers = requires(const T a) {
    { a.identifying_members() };
};

template<typename T, typename C>
concept HasProxyType = requires { typename T::template ProxyType<C>; };

template<typename P, typename C, typename Elem>
concept IsProxyFor = std::constructible_from<P, const C&, const Elem&>;

template<typename T>
concept IsHanaMap = std::same_as<typename boost::hana::tag_of<T>::type, boost::hana::map_tag>;

}

#endif
