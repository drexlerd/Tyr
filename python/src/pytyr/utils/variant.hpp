/*
    nanobind/stl/variant.h: type caster for std::variant<...>

    Copyright (c) 2022 Yoshiki Matsuda and Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include <nanobind/nanobind.h>
#include <type_traits>
#include <tyr/common/variant.hpp>

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

template<typename C, typename... Ts>
struct type_caster<::tyr::View<::cista::offset::variant<Ts...>, C>>
{
    using ViewT = ::tyr::View<::cista::offset::variant<Ts...>, C>;

    // Optional nice typing: Union[...]
    static constexpr auto Name = union_name(make_caster<std::conditional_t<::tyr::ViewConcept<Ts, C>, ::tyr::View<Ts, C>, Ts>>::Name...);

    template<typename T>
    using Cast = movable_cast_t<T>;

    template<typename T>
    static constexpr bool can_cast()
    {
        return true;
    }

    // No Python -> C++ conversion
    bool from_python(handle, uint8_t, cleanup_list*) noexcept { return false; }

    template<typename U>
    static handle from_cpp(U&& v, rv_policy policy, cleanup_list* cleanup) noexcept
    {
        // Use your View::apply so ViewConcept alternatives become views
        return v.apply(
            [&](auto&& arg) -> handle
            {
                using A = std::decay_t<decltype(arg)>;
                return make_caster<A>::from_cpp(std::forward<decltype(arg)>(arg), policy, cleanup);
            });
    }
};

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)
