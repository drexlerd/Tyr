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
    const Variant* m_value;

public:
    using VariantType = Variant;

    VariantProxy(const Context& context, const Variant& value) : m_context(&context), m_value(&value) {}

    const Variant& index_variant() const noexcept { return *m_value; }
    const Context& context() const noexcept { return *m_context; }

    template<typename T>
    bool is() const noexcept
    {
        return std::holds_alternative<T>(index_variant());
    }

    template<typename T>
    auto get() const
    {
        using Proxy = typename T::ProxyType;
        return Proxy(context(), std::get<T>(index_variant()));
    }

    /// Apply a callable to the *proxy* corresponding to the held index.
    /// The callable F must accept any of the proxy types.
    template<typename F>
    decltype(auto) visit(F&& f) const
    {
        return std::visit(
            [&](auto index) -> decltype(auto)
            {
                using Index = std::decay_t<decltype(index)>;
                using Proxy = typename Index::ProxyType;
                return std::forward<F>(f)(Proxy(context(), index));
            },
            index_variant());
    }
};
}

#endif
