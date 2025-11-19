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

#ifndef TYR_COMMON_SPAN_HPP_
#define TYR_COMMON_SPAN_HPP_

#include "tyr/common/declarations.hpp"

#include <cstddef>
#include <iterator>
#include <span>

namespace tyr
{
template<typename T, typename Context>
class SpanProxy
{
private:
    const Context& m_context;
    std::span<const T> m_span;

public:
    // If T has a ProxyType, use it; otherwise the "proxy" is just T itself.
    using ProxyType = std::conditional_t<HasProxyType<T>, typename T::ProxyType, T>;

    template<class Container>
    explicit SpanProxy(const Context& context, const Container& container) : m_context(context), m_span(std::data(container), std::size(container))
    {
    }

    size_t size() const noexcept { return m_span.size(); }
    bool empty() const noexcept { return m_span.empty(); }

    ProxyType operator[](size_t i) const
    {
        if constexpr (HasProxyType<T>)
        {
            return ProxyType(m_context, m_span[i]);
        }
        else
        {
            return m_span[i];
        }
    }

    struct const_iterator
    {
        const Context* ctx;
        const T* ptr;

        using difference_type = std::ptrdiff_t;
        using value_type = ProxyType;
        using iterator_category = std::random_access_iterator_tag;
        using iterator_concept = std::random_access_iterator_tag;

        const_iterator() : ctx(nullptr), ptr(nullptr) {}
        const_iterator(const Context& ctx, const T* ptr) : ctx(&ctx), ptr(ptr) {}

        ProxyType operator*() const
        {
            if constexpr (HasProxyType<T>)
            {
                return ProxyType(*ctx, *ptr);
            }
            else
            {
                return *ptr;
            }
        }

        const_iterator& operator++()
        {
            ++ptr;
            return *this;
        }
        const_iterator operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const const_iterator& lhs, const const_iterator& rhs) noexcept { return lhs.ptr == rhs.ptr; }
        friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs) noexcept { return !(lhs == rhs); }
    };

    const_iterator begin() const { return const_iterator { m_context, m_span.data() }; }

    const_iterator end() const { return const_iterator { m_context, m_span.data() + m_span.size() }; }
};
}

#endif