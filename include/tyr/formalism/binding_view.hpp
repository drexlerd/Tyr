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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TYR_FORMALISM_BINDING_VIEW_HPP_
#define TYR_FORMALISM_BINDING_VIEW_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/binding_index.hpp"
#include "tyr/formalism/datalog/rule_index.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function_index.hpp"
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/planning/action_index.hpp"
#include "tyr/formalism/planning/axiom_index.hpp"
#include "tyr/formalism/predicate_index.hpp"

namespace tyr
{
template<typename C>
class View<Index<formalism::Binding>, C>
{
private:
    const C* m_context;
    Index<formalism::Binding> m_handle;

public:
    View(Index<formalism::Binding> handle, const C& context) noexcept : m_context(&context), m_handle(handle) {}

    const auto& get_data() const noexcept { return get_repository(*m_context)[m_handle]; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

    auto get_index() const noexcept { return m_handle; }
    auto get_objects() const noexcept { return make_view(get_data().objects, *m_context); }

    auto identifying_members() const noexcept { return std::tie(m_handle, m_context->get_index()); }
};

template<typename Tag, typename C>
class View<formalism::RelationBindingIndex<Tag>, C>
{
private:
    const C* m_context;
    formalism::RelationBindingIndex<Tag> m_handle;

public:
    View(formalism::RelationBindingIndex<Tag> handle, const C& context) noexcept : m_context(&context), m_handle(handle) {}

    // This will return an ArrayView aleady
    auto get_data() const noexcept { return get_repository(*m_context)[m_handle]; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

    auto get_index() const noexcept { return m_handle; }
    auto get_relation() const noexcept { return make_view(m_handle.relation, *m_context); }
    auto get_objects() const noexcept { return make_view(get_data(), *m_context); }

    auto identifying_members() const noexcept { return std::tie(m_handle, m_context->get_index()); }
};

template<typename Tag, std::ranges::forward_range BindingRange, typename C>
    requires std::same_as<std::remove_cvref_t<std::ranges::range_reference_t<BindingRange>>, Index<formalism::Binding>>
class View<formalism::RelationBindingsForwardRange<Tag, BindingRange>, C>
{
public:
    using Container = formalism::RelationBindingsForwardRange<Tag, BindingRange>;
    using T = formalism::RelationBindingIndex<Tag>;
    using I1 = Index<Tag>;

    View(Container handle, const C& context) noexcept : m_context(&context), m_handle(handle) {}

    bool empty() const noexcept { return std::ranges::begin(get_data().rows) == std::ranges::end(get_data().rows); }

    size_t size() const noexcept
        requires std::ranges::sized_range<BindingRange>
    {
        return std::ranges::size(get_data().rows);
    }

    decltype(auto) front() const noexcept
    {
        auto it = std::ranges::begin(get_data().rows);
        assert(it != std::ranges::end(get_data().rows));
        if constexpr (ViewConcept<T, C>)
            return make_view(T { get_data().relation, *it }, get_context());
        else
            return T { get_data().relation, *it };
    }

    struct const_iterator
    {
        using BaseIt = std::ranges::iterator_t<const BindingRange>;

        const C* ctx;
        BaseIt it;
        I1 relation;

        using difference_type = std::ptrdiff_t;
        using value_type = std::conditional_t<ViewConcept<T, C>, ::tyr::View<T, C>, T>;
        using iterator_category = std::forward_iterator_tag;
        using iterator_concept = std::forward_iterator_tag;

        const_iterator() noexcept : ctx(nullptr), it() {}
        const_iterator(I1 relation, BaseIt it, const C& ctx) noexcept : ctx(&ctx), it(it), relation(relation) {}

        decltype(auto) operator*() const noexcept
        {
            if constexpr (ViewConcept<T, C>)
                return make_view(T { relation, *it }, *ctx);
            else
                return T { relation, *it };
        }

        const_iterator& operator++() noexcept
        {
            ++it;
            return *this;
        }

        const_iterator operator++(int) noexcept
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const const_iterator& lhs, const const_iterator& rhs) noexcept { return lhs.it == rhs.it; }

        friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs) noexcept { return !(lhs == rhs); }
    };

    const_iterator begin() const noexcept { return const_iterator { get_data().relation, std::ranges::begin(get_data().rows), get_context() }; }

    const_iterator end() const noexcept { return const_iterator { get_data().relation, std::ranges::end(get_data().rows), get_context() }; }

    const auto& get_data() const noexcept { return m_handle; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

private:
    const C* m_context;
    Container m_handle;
};

template<typename Tag, std::ranges::random_access_range BindingRange, typename C>
    requires std::ranges::sized_range<BindingRange>
             && std::same_as<std::remove_cvref_t<std::ranges::range_reference_t<BindingRange>>, Index<formalism::Binding>>
class View<formalism::RelationBindingsRandomAccessRange<Tag, BindingRange>, C>
{
public:
    using Container = formalism::RelationBindingsRandomAccessRange<Tag, BindingRange>;
    using T = formalism::RelationBindingIndex<Tag>;
    using I1 = Index<Tag>;

    View(Container handle, const C& context) noexcept : m_context(&context), m_handle(handle) {}

    bool empty() const noexcept { return std::ranges::begin(get_data().rows) == std::ranges::end(get_data().rows); }

    size_t size() const noexcept { return std::ranges::size(get_data().rows); }

    decltype(auto) front() const noexcept
    {
        auto it = std::ranges::begin(get_data().rows);
        assert(it != std::ranges::end(get_data().rows));
        if constexpr (ViewConcept<T, C>)
            return make_view(T { get_data().relation, *it }, get_context());
        else
            return T { get_data().relation, *it };
    }

    decltype(auto) back() const noexcept
    {
        assert(!empty());
        auto it = std::ranges::begin(get_data().rows) + (std::ranges::ssize(get_data().rows) - 1);
        if constexpr (ViewConcept<T, C>)
            return make_view(T { get_data().relation, *it }, get_context());
        else
            return T { get_data().relation, *it };
    }

    decltype(auto) operator[](size_t i) const noexcept
    {
        auto it = std::ranges::begin(get_data().rows) + static_cast<std::ptrdiff_t>(i);
        if constexpr (ViewConcept<T, C>)
            return make_view(T { get_data().relation, *it }, get_context());
        else
            return T { get_data().relation, *it };
    }

    struct const_iterator
    {
        using BaseIt = std::ranges::iterator_t<const BindingRange>;

        const C* ctx;
        BaseIt it;
        I1 relation;

        using difference_type = std::ptrdiff_t;
        using value_type = std::conditional_t<ViewConcept<T, C>, ::tyr::View<T, C>, T>;
        using iterator_category = std::random_access_iterator_tag;
        using iterator_concept = std::random_access_iterator_tag;

        const_iterator() noexcept : ctx(nullptr), it() {}
        const_iterator(I1 relation, BaseIt it, const C& ctx) noexcept : ctx(&ctx), it(it), relation(relation) {}

        decltype(auto) operator*() const noexcept
        {
            if constexpr (ViewConcept<T, C>)
                return make_view(T { relation, *it }, *ctx);
            else
                return T { relation, *it };
        }

        const_iterator& operator++() noexcept
        {
            ++it;
            return *this;
        }

        const_iterator operator++(int) noexcept
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        const_iterator& operator--() noexcept
        {
            --it;
            return *this;
        }

        const_iterator operator--(int) noexcept
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        const_iterator& operator+=(difference_type n) noexcept
        {
            it += n;
            return *this;
        }

        const_iterator& operator-=(difference_type n) noexcept
        {
            it -= n;
            return *this;
        }

        friend const_iterator operator+(const const_iterator& it, difference_type n) noexcept
        {
            auto tmp = it;
            tmp += n;
            return tmp;
        }

        friend const_iterator operator+(difference_type n, const const_iterator& it) noexcept
        {
            auto tmp = it;
            tmp += n;
            return tmp;
        }

        friend const_iterator operator-(const const_iterator& it, difference_type n) noexcept
        {
            auto tmp = it;
            tmp -= n;
            return tmp;
        }

        friend difference_type operator-(const const_iterator& lhs, const const_iterator& rhs) noexcept { return lhs.it - rhs.it; }

        decltype(auto) operator[](difference_type n) const noexcept
        {
            if constexpr (ViewConcept<T, C>)
                return make_view(T { relation, it[n] }, *ctx);
            else
                return T { relation, it[n] };
        }

        friend bool operator==(const const_iterator& lhs, const const_iterator& rhs) noexcept { return lhs.it == rhs.it; }
        friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs) noexcept { return !(lhs == rhs); }
        friend bool operator<(const const_iterator& lhs, const const_iterator& rhs) noexcept { return lhs.it < rhs.it; }
        friend bool operator>(const const_iterator& lhs, const const_iterator& rhs) noexcept { return rhs < lhs; }
        friend bool operator<=(const const_iterator& lhs, const const_iterator& rhs) noexcept { return !(rhs < lhs); }
        friend bool operator>=(const const_iterator& lhs, const const_iterator& rhs) noexcept { return !(lhs < rhs); }
    };

    const_iterator begin() const noexcept { return const_iterator { get_data().relation, std::ranges::begin(get_data().rows), get_context() }; }

    const_iterator end() const noexcept { return const_iterator { get_data().relation, std::ranges::end(get_data().rows), get_context() }; }

    const auto& get_data() const noexcept { return m_handle; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

private:
    const C* m_context;
    Container m_handle;
};

}

#endif
