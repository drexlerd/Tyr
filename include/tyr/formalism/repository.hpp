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

template<typename SymbolRepo, typename RelationRepo>
class Repository
{
private:
    const Repository* m_parent;
    SymbolRepo m_symbol_repository;
    RelationRepo m_relation_repository;
    size_t m_index;

    /**
     * SymbolRepository forwarding.
     */

    template<typename T>
        requires NonRelationBindingConcept<T>
    std::optional<View<Index<T>, Repository>> find_with_hash(const Data<T>& builder, size_t h) const noexcept
    {
        const Repository* current = this;
        while (current != nullptr)
        {
            if (auto index_or_nullopt = current->m_symbol_repository.find_local_with_hash(builder, h))
                return { View<Index<T>, Repository>(*index_or_nullopt, *current), false };

            current = current->m_parent;
        }

        return std::nullopt;
    }

    template<typename T>
        requires NonRelationBindingConcept<T>
    std::pair<View<Index<T>, Repository>, bool> get_or_create_with_hash(Data<T>& builder, size_t h)
    {
        const Repository* current = this;
        while (current != nullptr)
        {
            if (auto index_or_nullopt = current->m_symbol_repository.find_local_with_hash(builder, h))
                return { View<Index<T>, Repository>(*index_or_nullopt, *current), false };
            current = current->m_parent;
        }

        assert(!m_symbol_repository.template exists_parent_mutation<T>() && "Integrity error: Parent SymbolRepository modified after child branching!");

        const auto [index, success] = m_symbol_repository.get_or_create_local_with_hash(builder, h);
        return { View<Index<T>, Repository>(index, *this), success };
    }

    /**
     * RelationRepository forwarding
     */

    template<typename T>
    std::optional<View<Index<RelationBinding<T>>, Repository>> find_with_hash(const Data<RelationBinding<T>>& builder, size_t h) const noexcept
    {
        const auto g = builder.relation;

        const Repository* current = this;
        while (current != nullptr)
        {
            if (auto row_or_nullopt = current->m_relation_repository.find_local_with_hash(builder, h))
                return View<Index<RelationBinding<T>>, Repository>(Index<RelationBinding<T>> { g, *row_or_nullopt }, *current);

            current = current->m_parent;
        }

        return std::nullopt;
    }

    template<typename T>
    std::pair<View<Index<RelationBinding<T>>, Repository>, bool> get_or_create_with_hash(const Data<RelationBinding<T>>& builder, size_t h)
    {
        const auto g = builder.relation;

        const Repository* current = this;
        while (current != nullptr)
        {
            if (auto row_or_nullopt = current->m_relation_repository.find_local_with_hash(builder, h))
                return { View<Index<RelationBinding<T>>, Repository>(Index<RelationBinding<T>> { g, *row_or_nullopt }, *current), false };
            current = current->m_parent;
        }

        assert(!m_relation_repository.exists_parent_mutation(g) && "Integrity error: Parent RelationRepository modified after child branching!");

        const auto [row, success] = m_relation_repository.get_or_create_local_with_hash(builder, h);
        return { View<Index<RelationBinding<T>>, Repository>(Index<RelationBinding<T>> { g, row }, *this), success };
    }

public:
    Repository(size_t index, const Repository* parent = nullptr) :
        m_parent(parent),
        m_symbol_repository(m_parent ? &m_parent->m_symbol_repository : nullptr),
        m_relation_repository(m_parent ? &m_parent->m_relation_repository : nullptr),
        m_index(index)
    {
        clear();
    }
    Repository(const Repository& other) = delete;
    Repository& operator=(const Repository& other) = delete;
    Repository(Repository&& other) = delete;
    Repository& operator=(Repository&& other) = delete;

    const auto& get_index() const noexcept { return m_index; }

    void clear() noexcept
    {
        m_symbol_repository.clear();
        m_relation_repository.clear();
    }

    /**
     * SymbolRepository forwarding.
     */

    template<typename T>
        requires NonRelationBindingConcept<T>
    std::optional<View<Index<T>, Repository>> find(const Data<T>& builder) const noexcept
    {
        return find_with_hash(builder, SymbolRepo::hash(builder));
    }

    template<typename T>
        requires NonRelationBindingConcept<T>
    std::pair<View<Index<T>, Repository>, bool> get_or_create(Data<T>& builder)
    {
        return get_or_create_with_hash(builder, SymbolRepo::hash(builder));
    }

    template<typename T>
        requires NonRelationBindingConcept<T>
    const Data<T>& operator[](Index<T> index) const noexcept
    {
        const Repository* current = this;
        while (current != nullptr)
        {
            if (current->m_symbol_repository.is_local(index))
                return current->m_symbol_repository.at_local(index);

            current = current->m_parent;
        }
        assert(false && "Index not found in any repository layer.");
        __builtin_unreachable();
    }

    template<typename T>
        requires NonRelationBindingConcept<T>
    const Data<T>& front() const
    {
        const Repository* current = this;
        while (current->m_parent != nullptr)
        {
            if (current->m_symbol_repository.template parent_size<T>() == 0)
                break;
            current = current->m_parent;
        }

        return current->m_symbol_repository.template front_local<T>();
    }

    template<typename T>
        requires NonRelationBindingConcept<T>
    size_t size() const noexcept
    {
        return m_symbol_repository.template parent_size<T>() + m_symbol_repository.template local_size<T>();
    }

    template<typename T>
        requires NonRelationBindingConcept<T>
    const Repository& get_canonical_context(Index<T> index) const noexcept
    {
        const Repository* current = this;
        while (current != nullptr)
        {
            if (current->m_symbol_repository.is_local(index))
                return *current;

            assert(current->m_parent && "Index not found in any repository layer.");
            current = current->m_parent;
        }
        assert(false && "Index not found in any repository layer.");
        __builtin_unreachable();
    }

    /**
     * RelationRepository forwarding
     */

    template<typename T>
    std::optional<View<Index<RelationBinding<T>>, Repository>> find(const Data<RelationBinding<T>>& builder) const noexcept
    {
        return find_with_hash(builder, RelationRepo::hash(builder));
    }

    template<typename T>
    std::pair<View<Index<RelationBinding<T>>, Repository>, bool> get_or_create(const Data<RelationBinding<T>>& builder)
    {
        return get_or_create_with_hash(builder, RelationRepo::hash(builder));
    }

    template<typename T>
    auto operator[](Index<RelationBinding<T>> index) const noexcept
    {
        const Repository* current = this;
        while (current != nullptr)
        {
            if (current->m_relation_repository.is_local(index))
                return current->m_relation_repository.at_local(index);

            current = current->m_parent;
        }
        assert(false && "Index not found in any repository layer.");
        __builtin_unreachable();
    }

    template<typename T>
    auto front(Index<T> g) const noexcept
    {
        const Repository* current = this;
        while (current->m_parent != nullptr)
        {
            if (current->m_relation_repository.parent_size(g) == 0)
                break;
            current = current->m_parent;
        }

        return current->m_relation_repository.front_local(g);
    }

    template<typename T>
    size_t size(Index<T> g) const noexcept
    {
        return m_relation_repository.parent_size(g) + m_relation_repository.local_size(g);
    }

    template<typename T>
    const Repository& get_canonical_context(Index<RelationBinding<T>> index) const noexcept
    {
        const Repository* current = this;
        while (current != nullptr)
        {
            if (current->m_relation_repository.is_local(index))
                return *current;

            assert(current->m_parent && "Index not found in any repository layer.");
            current = current->m_parent;
        }
        assert(false && "Index not found in any repository layer.");
        __builtin_unreachable();
    }
};

}

#endif
