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

#ifndef TYR_COMMON_TUPLE_HPP_
#define TYR_COMMON_TUPLE_HPP_

#include <cstddef>
#include <tuple>
#include <type_traits>

namespace tyr
{

template<typename T, std::size_t I, typename... Slots>
constexpr auto& get_container_impl(std::tuple<Slots...>& tuple)
{
    using Slot = std::tuple_element_t<I, std::tuple<Slots...>>;

    if constexpr (std::is_same_v<typename Slot::value_type, T>)
    {
        return std::get<I>(tuple).container;
    }
    else
    {
        static_assert(I + 1 < sizeof...(Slots), "Type T is not stored in this tuple");
        return get_container_impl<T, I + 1, Slots...>(tuple);
    }
}

template<typename T, typename... Slots>
constexpr auto& get_container(std::tuple<Slots...>& tuple)
{
    return get_container_impl<T, 0, Slots...>(tuple);
}

template<typename T, typename... Slots>
constexpr const auto& get_container(const std::tuple<Slots...>& tuple)
{
    return get_container_impl<T, 0, Slots...>(const_cast<std::tuple<Slots...>&>(tuple));
}

/**
 * std::get<I> with runtime I.
 */

template<size_t I>
struct visit_impl
{
    template<typename T, typename F>
    static void visit(T& tup, size_t idx, F function)
    {
        if (idx == I - 1)
        {
            function(std::get<I - 1>(tup));
        }
        else
            visit_impl<I - 1>::visit(tup, idx, function);
    }
};

template<>
struct visit_impl<0>
{
    template<typename T, typename F>
    static void visit(T& tup, size_t idx, F function)
    {
        if (idx == 0)
        {
            function(std::get<0>(tup));
        }
    }
};

template<typename F, typename... Ts>
void visit_at(std::tuple<Ts...> const& tup, size_t idx, F function)
{
    return visit_impl<sizeof...(Ts)>::visit(tup, idx, function);
}

template<typename F, typename... Ts>
void visit_at(std::tuple<Ts...>& tup, size_t idx, F function)
{
    return visit_impl<sizeof...(Ts)>::visit(tup, idx, function);
}
}

#endif