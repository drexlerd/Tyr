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

#ifndef TYR_COMMON_VARIANT_HPP_
#define TYR_COMMON_VARIANT_HPP_

#include "tyr/common/declarations.hpp"

#include <cista/containers/variant.h>
#include <cstddef>
#include <iterator>
#include <span>
#include <variant>

namespace tyr
{

template<typename Variant, typename Context>
class VariantProxy
{
private:
    const Context* m_context;
    Variant m_value;

public:
    VariantProxy(Variant value, const Context& context) : m_context(&context), m_value(value) {}

    const Variant& index_variant() const noexcept { return m_value; }
    const Context& context() const noexcept { return *m_context; }

    template<typename T>
    bool is() const noexcept
    {
        return std::holds_alternative<T>(index_variant());
    }

    template<typename T>
    auto get() const
    {
        if constexpr (IsProxyable<T, Context>)
        {
            return ProxyType<T, Context>(std::get<T>(index_variant()), context());
        }
        else
        {
            return std::get<T>(index_variant());
        }
    }

    template<typename F>
    decltype(auto) apply(F&& f) const
    {
        return std::visit(
            [&](auto arg) -> decltype(auto)
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (IsProxyable<T, Context>)
                {
                    return std::forward<F>(f)(ProxyType<T, Context>(arg, context()));
                }
                else
                {
                    return std::forward<F>(f)(arg);
                }
            },
            index_variant());
    }
};

template<typename Visitor, typename Variant, typename Context>
constexpr auto visit(Visitor&& vis, tyr::VariantProxy<Variant, Context>&& v)
{
    return v.apply(std::forward<Visitor>(vis));
}

template<typename Visitor, typename Variant, typename Context>
constexpr auto visit(Visitor&& vis, const tyr::VariantProxy<Variant, Context>& v)
{
    return v.apply(vis);
}

}
#endif
