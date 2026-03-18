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

#ifndef TYR_FORMALISM_RELATION_REPOSITORY_HPP_
#define TYR_FORMALISM_RELATION_REPOSITORY_HPP_

#include "tyr/common/tuple.hpp"
#include "tyr/common/types.hpp"
#include "tyr/formalism/basic_relation_repository.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/object_index.hpp"

#include <cassert>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace tyr::formalism
{
template<typename... Ts>
class RelationRepository
{
private:
    const RelationRepository* m_parent;
    std::tuple<BasicRelationRepository<Ts>...> m_repositories;
    size_t m_num_objects;

public:
    using container_type = typename BasicRelationRepository<std::tuple_element_t<0, std::tuple<Ts...>>>::container_type;
    using ConstViewType = typename container_type::ConstArrayView;

    RelationRepository(size_t num_objects, const RelationRepository* parent = nullptr) :
        m_parent(parent),
        m_repositories(BasicRelationRepository<Ts>(num_objects, parent ? &std::get<BasicRelationRepository<Ts>>(parent->m_repositories) : nullptr)...),
        m_num_objects(num_objects)
    {
    }

    RelationRepository(const RelationRepository&) = delete;
    RelationRepository& operator=(const RelationRepository&) = delete;
    RelationRepository(RelationRepository&&) = delete;
    RelationRepository& operator=(RelationRepository&&) = delete;

    template<typename T>
    BasicRelationRepository<T>& get() noexcept
    {
        return std::get<BasicRelationRepository<T>>(m_repositories);
    }

    template<typename T>
    const BasicRelationRepository<T>& get() const noexcept
    {
        return std::get<BasicRelationRepository<T>>(m_repositories);
    }

    void clear() noexcept
    {
        std::apply([](auto&... repos) { (repos.clear(), ...); }, m_repositories);
    }

    template<typename T>
    auto find_with_hash(Index<T> g, const IndexList<Object>& builder, size_t h) const noexcept
    {
        return get<T>().find_with_hash(g, builder, h);
    }

    template<typename T>
    auto find(Index<T> g, const IndexList<Object>& builder) const noexcept
    {
        return get<T>().find(g, builder);
    }

    template<typename T>
    auto get_or_create(Index<T> g, size_t arity, const IndexList<Object>& builder)
    {
        return get<T>().get_or_create(g, arity, builder);
    }

    template<typename T>
    auto operator[](std::pair<Index<T>, Index<Binding>> index) const noexcept
    {
        return get<T>()[index];
    }

    template<typename T>
    size_t size(Index<T> g) const noexcept
    {
        return get<T>().size(g);
    }

    template<typename T>
    const BasicRelationRepository<T>& get_canonical_context(std::pair<Index<T>, Index<Binding>> index) const noexcept
    {
        return get<T>().get_canonical_context(index);
    }

    template<typename T>
    auto find_local_with_hash(Index<T> g, const IndexList<Object>& builder, size_t h) const noexcept
    {
        return get<T>().find_local_with_hash(g, builder, h);
    }

    template<typename T>
    auto find_local(Index<T> g, const IndexList<Object>& builder) const noexcept
    {
        return get<T>().find_local(g, builder);
    }

    template<typename T>
    auto get_or_create_local(Index<T> g, size_t arity, const IndexList<Object>& builder)
    {
        return get<T>().get_or_create_local(g, arity, builder);
    }

    template<typename T>
    auto at_local(std::pair<Index<T>, Index<Binding>> index) const noexcept
    {
        return get<T>().at_local(index);
    }

    template<typename T>
    size_t local_size(Index<T> g) const noexcept
    {
        return get<T>().local_size(g);
    }

    template<typename T>
    size_t parent_size(Index<T> g) const noexcept
    {
        return get<T>().parent_size(g);
    }

    template<typename T>
    bool is_local(std::pair<Index<T>, Index<Binding>> index) const noexcept
    {
        return get<T>().is_local(index);
    }
};
}

#endif
