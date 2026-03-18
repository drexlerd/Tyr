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

#ifndef TYR_FORMALISM_DATALOG_SYMBOL_REPOSITORY_HPP_
#define TYR_FORMALISM_DATALOG_SYMBOL_REPOSITORY_HPP_

#include "tyr/buffer/declarations.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/common/types.hpp"
#include "tyr/formalism/basic_symbol_repository.hpp"
#include "tyr/formalism/declarations.hpp"

#include <cassert>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace tyr::formalism
{
template<typename... Ts>
class SymbolRepository
{
private:
    const SymbolRepository* m_parent;
    std::tuple<BasicSymbolRepository<Ts>...> m_repositories;

public:
    SymbolRepository(const SymbolRepository* parent = nullptr) :
        m_parent(parent),
        m_repositories(BasicSymbolRepository<Ts>(parent ? &std::get<BasicSymbolRepository<Ts>>(parent->m_repositories) : nullptr)...)
    {
    }

    SymbolRepository(const SymbolRepository&) = delete;
    SymbolRepository& operator=(const SymbolRepository&) = delete;
    SymbolRepository(SymbolRepository&&) = delete;
    SymbolRepository& operator=(SymbolRepository&&) = delete;

    template<typename T>
    BasicSymbolRepository<T>& get() noexcept
    {
        return std::get<BasicSymbolRepository<T>>(m_repositories);
    }

    template<typename T>
    const BasicSymbolRepository<T>& get() const noexcept
    {
        return std::get<BasicSymbolRepository<T>>(m_repositories);
    }

    void clear() noexcept
    {
        std::apply([](auto&... repos) { (repos.clear(), ...); }, m_repositories);
    }

    template<typename T>
    size_t hash(const Data<T>& builder) const
    {
        return get<T>().hash(builder);
    }

    template<typename T>
    auto find_with_hash(const Data<T>& builder, size_t h) const noexcept
    {
        return get<T>().find_with_hash(builder, h);
    }

    template<typename T>
    auto find(const Data<T>& builder) const noexcept
    {
        return get<T>().find(builder);
    }

    template<typename T>
    auto get_or_create_with_hash(Data<T>& builder, size_t h)
    {
        return get<T>().get_or_create_with_hash(builder, h);
    }

    template<typename T>
    auto get_or_create(Data<T>& builder)
    {
        return get<T>().get_or_create(builder);
    }

    template<typename T>
    const Data<T>& operator[](Index<T> index) const noexcept
    {
        return get<T>()[index];
    }

    template<typename T>
    const Data<T>& front() const
    {
        return get<T>().front();
    }

    template<typename T>
    size_t size() const noexcept
    {
        return get<T>().size();
    }

    template<typename T>
    const BasicSymbolRepository<T>& get_canonical_context(Index<T> index) const noexcept
    {
        return get<T>().get_canonical_context(index);
    }

    template<typename T>
    auto find_local_with_hash(const Data<T>& builder, size_t h) const noexcept
    {
        return get<T>().find_local_with_hash(builder, h);
    }

    template<typename T>
    auto find_local(const Data<T>& builder) const noexcept
    {
        return get<T>().find_local(builder);
    }

    template<typename T>
    auto get_or_create_local_with_hash(Data<T>& builder, size_t h)
    {
        return get<T>().get_or_create_local_with_hash(builder, h);
    }

    template<typename T>
    auto get_or_create_local(Data<T>& builder)
    {
        return get<T>().get_or_create_local(builder);
    }

    template<typename T>
    const Data<T>& at_local(Index<T> index) const noexcept
    {
        return get<T>().at_local(index);
    }

    template<typename T>
    const Data<T>& front_local() const
    {
        return get<T>().front_local();
    }

    template<typename T>
    size_t local_size() const noexcept
    {
        return get<T>().local_size();
    }

    template<typename T>
    size_t parent_size() const noexcept
    {
        return get<T>().parent_size();
    }

    template<typename T>
    bool is_local(Index<T> index) const noexcept
    {
        return get<T>().is_local(index);
    }
};

}

#endif
