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

#ifndef TYR_COMMON_VARIANT_HPP_
#define TYR_COMMON_VARIANT_HPP_

#include "tyr/common/declarations.hpp"
#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"

#include <cista/containers/variant.h>

namespace tyr
{

template<typename C, typename... T>
class View<::cista::offset::variant<T...>, C>
{
public:
    using Variant = ::cista::offset::variant<T...>;

    View(const Variant& handle, const C& context) noexcept : m_context(&context), m_handle(&handle) {}

    const Variant& index_variant() const noexcept { return *m_handle; }

    template<typename U>
    bool is() const noexcept
    {
        return std::holds_alternative<U>(index_variant());
    }

    template<typename U>
    decltype(auto) get() const noexcept
    {
        if constexpr (ViewConcept<U, C>)
            return make_view(std::get<U>(index_variant()), get_context());
        else
            return std::get<U>(index_variant());
    }

    template<typename F>
    decltype(auto) apply(F&& f) const noexcept
    {
        return std::visit(
            [&](auto&& arg) -> decltype(auto)
            {
                using U = std::decay_t<decltype(arg)>;

                if constexpr (ViewConcept<U, C>)
                    return std::forward<F>(f)(make_view(arg, get_context()));
                else
                    return std::forward<F>(f)(arg);
            },
            index_variant());
    }

    const auto& get_data() const noexcept { return *m_handle; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

private:
    const C* m_context;
    const Variant* m_handle;
};

template<typename Visitor, typename C, typename... T>
constexpr auto visit(Visitor&& vis, View<::cista::offset::variant<T...>, C>&& v) noexcept
{
    return v.apply(std::forward<Visitor>(vis));
}

template<typename Visitor, typename C, typename... T>
constexpr auto visit(Visitor&& vis, const View<::cista::offset::variant<T...>, C>& v) noexcept
{
    return v.apply(vis);
}
}
#endif
