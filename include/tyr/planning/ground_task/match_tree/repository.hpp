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

// Include specialization headers first
#include "tyr/planning/ground_task/match_tree/datas.hpp"
#include "tyr/planning/ground_task/match_tree/indices.hpp"
//
#include "tyr/buffer/declarations.hpp"
#include "tyr/buffer/indexed_hash_set.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/planning/ground_task/match_tree/declarations.hpp"

#include <cassert>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace tyr::planning::match_tree
{

template<formalism::Context C, typename Tag>
class Repository
{
private:
    template<typename T>
    struct RepositoryEntry
    {
        using value_type = T;
        using container_type = buffer::IndexedHashSet<T>;

        container_type container;
    };

    using RepositoryStorage = std::tuple<RepositoryEntry<PlaceholderNode<Tag>>,
                                         RepositoryEntry<InverseAtomSelectorNode<Tag>>,
                                         RepositoryEntry<InverseFactSelectorNode<Tag>>,
                                         RepositoryEntry<InverseNumericConstraintSelectorNode<Tag>>,
                                         RepositoryEntry<InverseElementGeneratorNode<Tag>>,
                                         RepositoryEntry<InverseNode<Tag>>,
                                         RepositoryEntry<AtomSelectorNode<Tag>>,
                                         RepositoryEntry<FactSelectorNode<Tag>>,
                                         RepositoryEntry<NumericConstraintSelectorNode<Tag>>,
                                         RepositoryEntry<ElementGeneratorNode<Tag>>,
                                         RepositoryEntry<Node<Tag>>>;

    RepositoryStorage m_repository;

    const C& m_formalism_repository;

public:
    explicit Repository(const C& formalism_repository) : m_formalism_repository(formalism_repository) {}

    const C& get_formalism_repository() const noexcept { return m_formalism_repository; }

    template<typename T>
    std::optional<Index<T>> find(const Data<T>& builder) const noexcept
    {
        const auto& indexed_hash_set = get_container<T>(m_repository);

        if (const auto ptr = indexed_hash_set.find(builder))
            return ptr->index;

        return std::nullopt;
    }

    template<typename T, bool AssignIndex = true>
    std::pair<Index<T>, bool> get_or_create(Data<T>& builder, buffer::Buffer& buf)
    {
        auto& indexed_hash_set = get_container<T>(m_repository);

        if constexpr (AssignIndex)
            builder.index.value = indexed_hash_set.size();

        const auto [ptr, success] = indexed_hash_set.insert(builder, buf);

        return std::make_pair(ptr->index, success);
    }

    /// @brief Access the element with the given index.
    template<typename T>
    const Data<T>& operator[](Index<T> index) const noexcept
    {
        assert(index != Index<T>::max() && "Unassigned index.");

        const auto& repository = get_container<T>(m_repository);

        return repository[index];
    }

    template<typename T>
    const Data<T>& front() const
    {
        const auto& repository = get_container<T>(m_repository);

        return repository.front();
    }

    /// @brief Get the number of stored elements.
    template<typename T>
    size_t size() const noexcept
    {
        const auto& repository = get_container<T>(m_repository);

        return repository.size();
    }

    /// @brief Clear the repository but keep memory allocated.
    void clear() noexcept
    {
        std::apply([](auto&... slots) { (slots.container.clear(), ...); }, m_repository);
    }
};

// static_assert(RepositoryConcept<Repository>);

// static_assert(Context<Repository>);

}

#endif
