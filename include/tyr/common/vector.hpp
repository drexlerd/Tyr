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

#ifndef TYR_COMMON_VECTOR_HPP_
#define TYR_COMMON_VECTOR_HPP_

#include "tyr/common/declarations.hpp"
#include "tyr/common/types.hpp"

#include <cista/containers/vector.h>
#include <cstddef>
#include <iterator>

namespace tyr
{

template<typename T, template<typename> typename Ptr, bool IndexPointers, typename TemplateSizeType, class Allocator, typename C>
class View<::cista::basic_vector<T, Ptr, IndexPointers, TemplateSizeType, Allocator>, C>
{
public:
    using Container = ::cista::basic_vector<T, Ptr, IndexPointers, TemplateSizeType, Allocator>;

    View(const Container& handle, const C& context) : m_context(&context), m_handle(&handle) {}

    size_t size() const noexcept { return get_data().size(); }
    bool empty() const noexcept { return get_data().empty(); }

    decltype(auto) operator[](size_t i) const
    {
        if constexpr (Viewable<T, C>)
        {
            return View<T, C>(get_data()[i], get_context());
        }
        else
        {
            return get_data()[i];
        }
    }

    decltype(auto) front() const
    {
        if constexpr (Viewable<T, C>)
        {
            return View<T, C>(get_data().front(), get_context());
        }
        else
        {
            return get_data().front();
        }
    }

    struct const_iterator
    {
        const C* ctx;
        const T* ptr;

        using difference_type = std::ptrdiff_t;
        using value_type = std::conditional_t<Viewable<T, C>, ::tyr::View<T, C>, T>;
        using iterator_category = std::random_access_iterator_tag;
        using iterator_concept = std::random_access_iterator_tag;

        const_iterator() : ctx(nullptr), ptr(nullptr) {}
        const_iterator(const T* ptr, const C& ctx) : ctx(&ctx), ptr(ptr) {}

        decltype(auto) operator*() const
        {
            if constexpr (Viewable<T, C>)
            {
                return View<T, C>(*ptr, *ctx);
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
        auto operator[](difference_type n) const
        {
            if constexpr (Viewable<T, C>)
                return View<T, C>(*(ptr + n), *ctx);
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

    const_iterator begin() const { return const_iterator { get_data().data(), get_context() }; }

    const_iterator end() const { return const_iterator { get_data().data() + get_data().size(), get_context() }; }

    const auto& get_data() const { return *m_handle; }
    const auto& get_context() const { return *m_context; }
    const auto& get_handle() const { return m_handle; }

private:
    const C* m_context;
    const Container* m_handle;
};

template<class T>
void set(size_t pos, const T& value, std::vector<T>& vec, const T& default_value)
{
    if (pos >= vec.size())
        vec.resize(pos + 1, default_value);
    vec[pos] = value;
}

}

#endif