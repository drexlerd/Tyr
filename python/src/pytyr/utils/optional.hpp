/*
    nanobind/stl/optional.h: type caster for std::optional<...>

    Copyright (c) 2022 Yoshiki Matsuda and Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include <nanobind/nanobind.h>
#include <type_traits>
#include <tyr/common/optional.hpp>

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

template<typename C, typename T>
struct type_caster<::tyr::View<::cista::optional<T>, C>>
{
    using ViewT = ::tyr::View<::cista::optional<T>, C>;
    using Entry = std::conditional_t<::tyr::ViewConcept<T, C>, ::tyr::View<T, C>, T>;
    using Caster = make_caster<Entry>;

    NB_TYPE_CASTER(ViewT, optional_name(Caster::Name))

    // Views are not constructible from Python
    bool from_python(handle, uint8_t, cleanup_list*) noexcept { return false; }

    template<typename U>
    static handle from_cpp(U&& value, rv_policy policy, cleanup_list* cleanup) noexcept
    {
        if (!value)  // uses View::operator bool()
            return none().release();

        return Caster::from_cpp(value.value(), policy, cleanup);
    }
};

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)
