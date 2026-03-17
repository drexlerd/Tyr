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

#ifndef TYR_PLANNING_GROUND_TASK_MATCH_TREE_REPOSITORY_HPP_
#define TYR_PLANNING_GROUND_TASK_MATCH_TREE_REPOSITORY_HPP_

// Include specialization headers first
#include "tyr/planning/ground_task/match_tree/nodes/atom_data.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/atom_index.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/atom_view.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/constraint_data.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/constraint_index.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/constraint_view.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/generator_data.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/generator_index.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/generator_view.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/node_data.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/node_view.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/variable_data.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/variable_index.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/variable_view.hpp"
//
#include "tyr/buffer/declarations.hpp"
#include "tyr/buffer/indexed_hash_set.hpp"
#include "tyr/buffer/segmented_buffer.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/common/types.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/ground_action_index.hpp"
#include "tyr/formalism/planning/ground_axiom_index.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/planning/ground_task/match_tree/declarations.hpp"

#include <cassert>
#include <optional>
#include <tuple>
#include <utility>

namespace tyr::planning::match_tree
{

template<typename Tag>
class Repository
{
private:
    template<typename T>
    struct Entry
    {
        using value_type = T;
        using container_type = buffer::IndexedHashSet<T>;

        container_type container;

        Entry(buffer::Buffer& buffer, buffer::SegmentedBuffer& arena) : container(buffer, arena) {}
    };

    using RepositoryStorage =
        std::tuple<Entry<AtomSelectorNode<Tag>>, Entry<VariableSelectorNode<Tag>>, Entry<NumericConstraintSelectorNode<Tag>>, Entry<ElementGeneratorNode<Tag>>>;

    const formalism::planning::Repository& m_formalism_repository;
    buffer::SegmentedBuffer m_arena;
    buffer::Buffer m_buffer;
    RepositoryStorage m_repository;

public:
    explicit Repository(const formalism::planning::Repository& formalism_repository) :
        m_formalism_repository(formalism_repository),
        m_arena(),
        m_buffer(),
        m_repository(Entry<AtomSelectorNode<Tag>>(m_buffer, m_arena),
                     Entry<VariableSelectorNode<Tag>>(m_buffer, m_arena),
                     Entry<NumericConstraintSelectorNode<Tag>>(m_buffer, m_arena),
                     Entry<ElementGeneratorNode<Tag>>(m_buffer, m_arena))
    {
    }
    Repository(const Repository& other) = delete;
    Repository& operator=(const Repository& other) = delete;
    Repository(Repository&& other) = delete;
    Repository& operator=(Repository&& other) = delete;

    const formalism::planning::Repository& get_formalism_repository() const noexcept { return m_formalism_repository; }

    template<typename T>
    std::optional<Index<T>> find(const Data<T>& builder) const noexcept
    {
        const auto& indexed_hash_set = std::get<Entry<T>>(m_repository).container;

        if (const auto index_or_nullopt = indexed_hash_set.find(builder))
            return *index_or_nullopt;

        return std::nullopt;
    }

    template<typename T>
    std::pair<Index<T>, bool> get_or_create(Data<T>& builder)
    {
        auto& indexed_hash_set = std::get<Entry<T>>(m_repository).container;

        builder.index.value = indexed_hash_set.size();

        const auto [index, success] = indexed_hash_set.insert(builder);

        return std::make_pair(index, success);
    }

    /// @brief Access the element with the given index.
    template<typename T>
    const Data<T>& operator[](Index<T> index) const noexcept
    {
        assert(index != Index<T>::max() && "Unassigned index.");

        const auto& repository = std::get<Entry<T>>(m_repository).container;

        return repository[index];
    }

    template<typename T>
    const Data<T>& front() const
    {
        const auto& repository = std::get<Entry<T>>(m_repository).container;

        return repository.front();
    }

    /// @brief Get the number of stored elements.
    template<typename T>
    size_t size() const noexcept
    {
        const auto& repository = std::get<Entry<T>>(m_repository).container;

        return repository.size();
    }

    /// @brief Clear the repository but keep memory allocated.
    void clear() noexcept
    {
        std::apply([](auto&... slots) { (slots.container.clear(), ...); }, m_repository);
    }
};

static_assert(RepositoryConcept<Repository<formalism::planning::GroundAction>>);

static_assert(Context<Repository<formalism::planning::GroundAction>>);

}

#endif
