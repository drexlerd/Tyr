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
public:
    using Reference = ReferenceType<T>;
    using Proxy = ProxyType<T, Context>;

private:
    const Context* m_context;
    std::span<const Reference> m_span;

public:
    template<std::ranges::random_access_range Container>
        requires std::convertible_to<std::ranges::range_value_t<const Container>, Reference>
    explicit SpanProxy(const Container& container, const Context& context) : m_context(&context), m_span(std::data(container), std::size(container))
    {
    }

    size_t size() const noexcept { return m_span.size(); }
    bool empty() const noexcept { return m_span.empty(); }

    Proxy operator[](size_t i) const
    {
        if constexpr (IsProxyable<T, Context>)
        {
            return Proxy(m_span[i], *m_context);
        }
        else
        {
            return m_span[i];
        }
    }

    struct const_iterator
    {
        const Context* ctx;
        const Reference* ptr;

        using difference_type = std::ptrdiff_t;
        using value_type = Proxy;
        using iterator_category = std::random_access_iterator_tag;
        using iterator_concept = std::random_access_iterator_tag;

        const_iterator() : ctx(nullptr), ptr(nullptr) {}
        const_iterator(const Reference* ptr, const Context& ctx) : ctx(&ctx), ptr(ptr) {}

        Proxy operator*() const
        {
            if constexpr (IsProxyable<T, Context>)
            {
                return Proxy(*ptr, *ctx);
            }
            else
            {
                return *ptr;
            }
        }

        // ++
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

        // --
        const_iterator& operator--()
        {
            --ptr;
            return *this;
        }
        const_iterator operator--(int)
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        // += / -=
        const_iterator& operator+=(difference_type n)
        {
            ptr += n;
            return *this;
        }
        const_iterator& operator-=(difference_type n)
        {
            ptr -= n;
            return *this;
        }

        // + / -
        friend const_iterator operator+(const_iterator it, difference_type n)
        {
            it += n;
            return it;
        }

        friend const_iterator operator+(difference_type n, const_iterator it)
        {
            it += n;
            return it;
        }

        friend const_iterator operator-(const_iterator it, difference_type n)
        {
            it -= n;
            return it;
        }

        // iterator - iterator
        friend difference_type operator-(const_iterator lhs, const_iterator rhs) { return lhs.ptr - rhs.ptr; }

        // []
        Proxy operator[](difference_type n) const
        {
            if constexpr (IsProxyable<T, Context>)
                return Proxy(*(ptr + n), *ctx);
            else
                return *(ptr + n);
        }

        // comparisons
        friend bool operator==(const const_iterator& lhs, const const_iterator& rhs) noexcept { return lhs.ptr == rhs.ptr; }

        friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs) noexcept { return !(lhs == rhs); }

        friend bool operator<(const const_iterator& lhs, const const_iterator& rhs) noexcept { return lhs.ptr < rhs.ptr; }

        friend bool operator>(const const_iterator& lhs, const const_iterator& rhs) noexcept { return rhs < lhs; }

        friend bool operator<=(const const_iterator& lhs, const const_iterator& rhs) noexcept { return !(rhs < lhs); }

        friend bool operator>=(const const_iterator& lhs, const const_iterator& rhs) noexcept { return !(lhs < rhs); }
    };

    const_iterator begin() const { return const_iterator { m_span.data(), *m_context }; }

    const_iterator end() const { return const_iterator { m_span.data() + m_span.size(), *m_context }; }
};
}

#endif