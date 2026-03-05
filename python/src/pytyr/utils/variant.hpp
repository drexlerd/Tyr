/*
    nanobind/stl/variant.h: type caster for ::cista::offset::variant<...>

    Copyright (c) 2022 Yoshiki Matsuda and Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include <nanobind/nanobind.h>
#include <nanobind/stl/variant.h>
#include <type_traits>
#include <tyr/common/variant.hpp>

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

// Adapted from nanobind/stl/variant.h
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

// Taken from nanobind/stl/variant.h
template<typename... Ts>
struct type_caster<::cista::offset::variant<Ts...>> : private variant_caster_storage<variant_is_defaultable<Ts...>, Ts...>
{
    // We don't use NB_TYPE_CASTER so that we can customize the cast operators
    // to use `variant_caster_storage`, in order to support variants that are
    // not default-constructible.
    using Value = ::cista::offset::variant<Ts...>;
    static constexpr auto Name = union_name(make_caster<Ts>::Name...);

    template<typename T>
    using Cast = movable_cast_t<T>;
    template<typename T>
    static constexpr bool can_cast()
    {
        return true;
    }
    explicit operator Value*() { return &this->get(); }
    explicit operator Value&() { return (Value&) this->get(); }
    explicit operator Value&&() { return (Value&&) this->get(); }

    template<typename T>
    bool try_variant(const handle& src, uint8_t flags, cleanup_list* cleanup)
    {
        using CasterT = make_caster<T>;

        CasterT caster;

        if (!caster.from_python(src, flags_for_local_caster<T>(flags), cleanup) || !caster.template can_cast<T>())
            return false;

        this->store(caster.operator cast_t<T>());

        return true;
    }

    bool from_python(handle src, uint8_t flags, cleanup_list* cleanup) noexcept
    {
        if (flags & (uint8_t) cast_flags::convert)
        {
            if ((try_variant<Ts>(src, flags & ~(uint8_t) cast_flags::convert, cleanup) || ...))
            {
                return true;
            }
        }
        return (try_variant<Ts>(src, flags, cleanup) || ...);
    }

    template<typename T>
    static handle from_cpp(T* value, rv_policy policy, cleanup_list* cleanup)
    {
        if (!value)
            return none().release();
        return from_cpp(*value, policy, cleanup);
    }

    template<typename T>
    static handle from_cpp(T&& value, rv_policy policy, cleanup_list* cleanup) noexcept
    {
        return std::visit([&](auto&& v) { return make_caster<decltype(v)>::from_cpp(std::forward<decltype(v)>(v), policy, cleanup); }, std::forward<T>(value));
    }
};

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)
