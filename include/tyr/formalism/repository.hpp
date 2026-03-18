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

#ifndef TYR_FORMALISM_REPOSITORY_HPP_
#define TYR_FORMALISM_REPOSITORY_HPP_

#include "tyr/buffer/declarations.hpp"
#include "tyr/buffer/segmented_buffer.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/relation_repository.hpp"
#include "tyr/formalism/symbol_repository.hpp"

#include <cassert>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace tyr::formalism
{

template<typename SerializationRepo, typename RelationRepo>
class Repository
{
private:
    const Repository* m_parent;
    SerializationRepo m_symbol_repository;
    RelationRepo m_relation_repository;
    size_t m_num_objects;

public:
    Repository(size_t num_objects, const Repository* parent = nullptr) :
        m_parent(parent),
        m_symbol_repository(m_parent ? &m_parent->m_symbol_repository : nullptr),
        m_relation_repository(num_objects, m_parent ? &m_parent->m_relation_repository : nullptr),
        m_num_objects(num_objects)
    {
        clear();
    }
    Repository(const Repository& other) = delete;
    Repository& operator=(const Repository& other) = delete;
    Repository(Repository&& other) = delete;
    Repository& operator=(Repository&& other) = delete;

    auto get_num_objects() const noexcept { return m_num_objects; }

    /**
     * SymbolRepository forwarding.
     */

    template<typename T>
    std::optional<View<Index<T>, Repository>> find_with_hash(const Data<T>& builder, size_t h) const noexcept
    {
        if (auto index_or_nullopt = m_symbol_repository.find_local_with_hash(builder, h))
            return View<Index<T>, Repository>(*index_or_nullopt, *this);

        return m_parent ? m_parent->template find_with_hash<T>(builder, h) : std::nullopt;
    }

    template<typename T>
    std::optional<View<Index<T>, Repository>> find(const Data<T>& builder) const noexcept
    {
        return find_with_hash(builder, m_symbol_repository.hash(builder));
    }

    template<typename T>
    std::pair<View<Index<T>, Repository>, bool> get_or_create_with_hash(Data<T>& builder, size_t h)
    {
        if (auto index_or_nullopt = m_symbol_repository.find_local_with_hash(builder, h))
            return { View<Index<T>, Repository>(*index_or_nullopt, *this), false };

        if (m_parent)
            if (auto ptr = m_parent->template find_with_hash<T>(builder, h))
                return { *ptr, false };

        const auto [index, success] = m_symbol_repository.get_or_create_local_with_hash(builder, h);
        return { View<Index<T>, Repository>(index, *this), success };
    }

    template<typename T>
    std::pair<View<Index<T>, Repository>, bool> get_or_create(Data<T>& builder)
    {
        return get_or_create_with_hash(builder, m_symbol_repository.hash(builder));
    }

    template<typename T>
    const Data<T>& operator[](Index<T> index) const noexcept
    {
        if (!m_symbol_repository.is_local(index))
        {
            assert(m_parent);
            return (*m_parent)[index];
        }

        return m_symbol_repository.at_local(index);
    }

    template<typename T>
    const Data<T>& front() const
    {
        if (m_symbol_repository.template parent_size<T>() > 0)
        {
            assert(m_parent);
            return m_parent->template front<T>();
        }

        return m_symbol_repository.template front_local<T>();
    }

    template<typename T>
    size_t size() const noexcept
    {
        return m_symbol_repository.template parent_size<T>() + m_symbol_repository.template local_size<T>();
    }

    void clear() noexcept
    {
        m_symbol_repository.clear();
        m_relation_repository.clear();
    }

    template<typename T>
    const Repository& get_canonical_context(Index<T> index) const noexcept
    {
        if (!m_symbol_repository.is_local(index))
        {
            assert(m_parent && "Element not found in the repository chain.");
            return m_parent->get_canonical_context(index);
        }
        return *this;
    }

    /**
     * RelationRepository forwarding
     */

    template<IndexConcept I>
    std::optional<View<std::pair<I, Index<Binding>>, Repository>> find_with_hash(I g, const IndexList<Object>& builder, size_t h) const noexcept
    {
        const auto row_or_nullopt = m_relation_repository.find_local_with_hash(g, builder, h);
        if (row_or_nullopt)
            return View<std::pair<I, Index<Binding>>, Repository>(std::make_pair(g, *row_or_nullopt), *this);

        return m_parent ? m_parent->find_with_hash(g, builder, h) : std::nullopt;
    }

    template<IndexConcept I>
    std::optional<View<std::pair<I, Index<Binding>>, Repository>> find(I g, const IndexList<Object>& builder) const noexcept
    {
        const auto row_or_nullopt = m_relation_repository.find_local(g, builder);

        if (row_or_nullopt)
            return View<std::pair<I, Index<Binding>>, Repository>(std::make_pair(g, *row_or_nullopt), *this);

        return m_parent ? m_parent->find(g, builder) : std::nullopt;
    }

    template<IndexConcept I>
    std::pair<View<std::pair<I, Index<Binding>>, Repository>, bool> get_or_create(View<I, Repository> g, const IndexList<Object>& builder)
    {
        if (auto row_or_nullopt = m_relation_repository.find_local(g.get_index(), builder))
            return { View<std::pair<I, Index<Binding>>, Repository>(std::make_pair(g.get_index(), *row_or_nullopt), *this), false };

        if (m_parent)
            if (auto ptr = m_parent->find(g.get_index(), builder))
                return { *ptr, false };

        const auto [row, success] = m_relation_repository.get_or_create_local(g.get_index(), g.get_arity(), builder);

        return { View<std::pair<I, Index<Binding>>, Repository>(std::make_pair(g.get_index(), row), *this), success };
    }

    template<IndexConcept I>
    auto operator[](std::pair<I, Index<Binding>> index) const noexcept
    {
        if (m_relation_repository.is_local(index))
            return m_relation_repository.at_local(index);

        assert(m_parent);
        return (*m_parent)[index];
    }

    template<IndexConcept I>
    size_t size(I g) const noexcept
    {
        return m_relation_repository.parent_size(g) + m_relation_repository.local_size(g);
    }

    template<IndexConcept I>
    const Repository& get_canonical_context(std::pair<I, Index<Binding>> index) const noexcept
    {
        if (!m_relation_repository.is_local(index))
        {
            assert(m_parent && "Element not found in the repository chain.");
            return m_parent->get_canonical_context(index);
        }
        return *this;
    }
};

}

#endif
