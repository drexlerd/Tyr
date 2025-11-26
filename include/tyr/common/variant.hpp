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

#include <cista/containers/variant.h>

namespace tyr
{

template<typename Context, typename... T>
class Proxy<::cista::offset::variant<T...>, Context>
{
public:
    using Variant = ::cista::offset::variant<T...>;

    Proxy(const Variant& data, const Context& context) : m_context(&context), m_data(&data) {}

    const Variant& index_variant() const noexcept { return *m_data; }
    const Context& context() const noexcept { return *m_context; }

    template<typename U>
    bool is() const noexcept
    {
        return std::holds_alternative<U>(index_variant());
    }

    template<typename U>
    auto get() const
    {
        if constexpr (IsProxyable<U, Context>)
        {
            return Proxy<U, Context>(std::get<U>(index_variant()), context());
        }
        else
        {
            return std::get<U>(index_variant());
        }
    }

    template<typename F>
    decltype(auto) apply(F&& f) const
    {
        return std::visit(
            [&](auto&& arg) -> decltype(auto)
            {
                using U = std::decay_t<decltype(arg)>;

                if constexpr (IsProxyable<U, Context>)
                {
                    return std::forward<F>(f)(Proxy<U, Context>(arg, context()));
                }
                else
                {
                    return std::forward<F>(f)(arg);
                }
            },
            index_variant());
    }

private:
    const Context* m_context;
    const Variant* m_data;
};

template<typename Visitor, typename Context, typename... T>
constexpr auto visit(Visitor&& vis, Proxy<::cista::offset::variant<T...>, Context>&& v)
{
    return v.apply(std::forward<Visitor>(vis));
}

template<typename Visitor, typename Context, typename... T>
constexpr auto visit(Visitor&& vis, const Proxy<::cista::offset::variant<T...>, Context>& v)
{
    return v.apply(vis);
}
}
#endif
