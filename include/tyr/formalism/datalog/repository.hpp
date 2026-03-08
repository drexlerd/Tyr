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

#ifndef TYR_FORMALISM_DATALOG_REPOSITORY_HPP_
#define TYR_FORMALISM_DATALOG_REPOSITORY_HPP_

// Include specialization headers first
#include "tyr/formalism/datalog/datas.hpp"
#include "tyr/formalism/datalog/indices.hpp"
//
#include "tyr/buffer/declarations.hpp"
#include "tyr/buffer/indexed_hash_set.hpp"
#include "tyr/buffer/segmented_buffer.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/datalog/declarations.hpp"

#include <cassert>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace tyr::formalism::datalog
{

class Repository
{
private:
    template<typename T>
    struct Slot
    {
        buffer::IndexedHashSet<T> container {};
        size_t parent_size = 0;
    };

    template<typename T>
    struct RepositoryEntry;

    template<typename T>
        requires(IndexConcept<Index<T>> && !GroupIndexConcept<Index<T>>)
    struct RepositoryEntry<T>
    {
        using value_type = T;
        using slot_type = Slot<T>;

        slot_type slot;
    };

    template<typename T>
        requires(GroupIndexConcept<Index<T>>)
    struct RepositoryEntry<T>
    {
        using value_type = T;
        using slot_type = UnorderedMap<typename Index<T>::GroupType, Slot<T>>;

        slot_type slot;
    };

    using RepositoryStorage = std::tuple<RepositoryEntry<Variable>,
                                         RepositoryEntry<Object>,
                                         RepositoryEntry<Binding>,
                                         RepositoryEntry<Predicate<StaticTag>>,
                                         RepositoryEntry<Predicate<FluentTag>>,
                                         RepositoryEntry<Atom<StaticTag>>,
                                         RepositoryEntry<Atom<FluentTag>>,
                                         RepositoryEntry<GroundAtom<StaticTag>>,
                                         RepositoryEntry<GroundAtom<FluentTag>>,
                                         RepositoryEntry<Literal<StaticTag>>,
                                         RepositoryEntry<Literal<FluentTag>>,
                                         RepositoryEntry<GroundLiteral<StaticTag>>,
                                         RepositoryEntry<GroundLiteral<FluentTag>>,
                                         RepositoryEntry<Function<StaticTag>>,
                                         RepositoryEntry<Function<FluentTag>>,
                                         RepositoryEntry<FunctionTerm<StaticTag>>,
                                         RepositoryEntry<FunctionTerm<FluentTag>>,
                                         RepositoryEntry<GroundFunctionTerm<StaticTag>>,
                                         RepositoryEntry<GroundFunctionTerm<FluentTag>>,
                                         RepositoryEntry<GroundFunctionTermValue<StaticTag>>,
                                         RepositoryEntry<GroundFunctionTermValue<FluentTag>>,
                                         RepositoryEntry<UnaryOperator<OpSub, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpAdd, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpSub, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpMul, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpDiv, Data<FunctionExpression>>>,
                                         RepositoryEntry<MultiOperator<OpAdd, Data<FunctionExpression>>>,
                                         RepositoryEntry<MultiOperator<OpMul, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpEq, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpNe, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpLe, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpLt, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpGe, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpGt, Data<FunctionExpression>>>,
                                         RepositoryEntry<UnaryOperator<OpSub, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpAdd, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpSub, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpMul, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpDiv, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<MultiOperator<OpAdd, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<MultiOperator<OpMul, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpEq, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpNe, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpLe, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpLt, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpGe, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpGt, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<ConjunctiveCondition>,
                                         RepositoryEntry<Rule>,
                                         RepositoryEntry<GroundConjunctiveCondition>,
                                         RepositoryEntry<GroundRule>,
                                         RepositoryEntry<Program>>;

    const Repository* m_parent;
    RepositoryStorage m_repository;
    buffer::SegmentedBuffer m_arena;

    template<typename T>
        requires(GroupIndexConcept<Index<T>>)
    auto& get_or_create_slot(Index<T> index)
    {
        const auto g = index.group;

        auto& entry = std::get<RepositoryEntry<T>>(m_repository);
        const auto it = entry.slot.find(g);
        if (it != entry.slot.end())
            return it->second;  ///< slot already exists

        auto& slot = entry.slot[g];  ///< create a new slot and initialize it
        const auto parent_size = m_parent ? m_parent->size(index) : size_t { 0 };
        slot.parent_size = parent_size;

        return slot;
    }

    /**
     * Clear
     */

    template<typename T>
        requires(IndexConcept<Index<T>> && !GroupIndexConcept<Index<T>>)
    void clear_entry(RepositoryEntry<T>& entry) noexcept
    {
        entry.slot.container.clear();
        entry.slot.parent_size = m_parent ? m_parent->template size<T>() : size_t { 0 };
    }

    template<typename T>
        requires(GroupIndexConcept<Index<T>>)
    void clear_entry(RepositoryEntry<T>& entry) noexcept
    {
        for (auto& [g, slot] : entry.slot)
        {
            slot.container.clear();
            auto idx = Index<T> {};
            idx.group = g;
            slot.parent_size = m_parent ? m_parent->template size<T>(idx) : size_t { 0 };
        }
    }

    void clear_entries() noexcept
    {
        std::apply([&](auto&... entry) { (clear_entry(entry), ...); }, m_repository);
    }

public:
    Repository(const Repository* parent = nullptr) : m_parent(parent), m_repository(), m_arena() { clear_entries(); }

    template<typename T>
        requires(IndexConcept<Index<T>> && !GroupIndexConcept<Index<T>>)
    std::optional<View<Index<T>, Repository>> find_with_hash(const Data<T>& builder, size_t h) const noexcept
    {
        const auto& entry = std::get<RepositoryEntry<T>>(m_repository);
        const auto& container = entry.slot.container;
        assert(h == container.hash(builder) && "The given hash does not match container internal's hash.");

        if (auto ptr = container.find_with_hash(builder, h))
            return View<Index<T>, Repository>(ptr->index, *this);

        return m_parent ? m_parent->template find_with_hash<T>(builder, h) : std::nullopt;
    }

    template<typename T>
        requires(IndexConcept<Index<T>> && !GroupIndexConcept<Index<T>>)
    std::optional<View<Index<T>, Repository>> find(const Data<T>& builder) const noexcept
    {
        const auto& entry = std::get<RepositoryEntry<T>>(m_repository);
        const auto& container = entry.slot.container;
        const auto h = container.hash(builder);

        return find_with_hash<T>(builder, h);
    }

    template<typename T>
        requires(GroupIndexConcept<Index<T>>)
    std::optional<View<Index<T>, Repository>> find_with_hash(const Data<T>& builder, size_t h) const noexcept
    {
        const auto& entry = std::get<RepositoryEntry<T>>(m_repository);
        const auto g = builder.index.group;
        const auto& slot = entry.slot;

        const auto it = slot.find(g);
        if (it == slot.end())
        {
            assert(!m_parent || !std::get<RepositoryEntry<T>>(m_parent->m_repository).slot.contains(g));
            return std::nullopt;
        }

        const auto& container = it->second.container;
        if (auto ptr = container.find_with_hash(builder, h))
            return View<Index<T>, Repository>(ptr->index, *this);

        return m_parent ? m_parent->template find_with_hash<T>(builder, h) : std::nullopt;
    }

    template<typename T>
        requires(GroupIndexConcept<Index<T>>)
    std::optional<View<Index<T>, Repository>> find(const Data<T>& builder) const noexcept
    {
        const auto& entry = std::get<RepositoryEntry<T>>(m_repository);
        const auto g = builder.index.group;
        const auto& slot = entry.slot;

        const auto it = slot.find(g);
        if (it == slot.end())
        {
            assert((!m_parent || !std::get<RepositoryEntry<T>>(m_parent->m_repository).slot.contains(g)) && "Child doesn't have slot while parent does.");
            return std::nullopt;
        }

        const auto& container = it->second.container;
        const auto h = container.hash(builder);

        return find_with_hash<T>(builder, h);
    }

    template<typename T>
        requires(IndexConcept<Index<T>> && !GroupIndexConcept<Index<T>>)
    std::pair<View<Index<T>, Repository>, bool> get_or_create(Data<T>& builder, buffer::Buffer& buf)
    {
        auto& entry = std::get<RepositoryEntry<T>>(m_repository);
        auto& slot = entry.slot;
        auto& container = slot.container;
        const auto h = container.hash(builder);

        if (m_parent)
            if (auto ptr = m_parent->template find_with_hash<T>(builder, h))
                return { *ptr, false };

        // Manually assign index to continue indexing.
        builder.index.value = slot.parent_size + container.size();

        const auto [ptr, success] = container.insert_with_hash(h, builder, buf, m_arena);

        return { View<Index<T>, Repository>(ptr->index, *this), success };
    }

    template<typename T>
        requires(GroupIndexConcept<Index<T>>)
    std::pair<View<Index<T>, Repository>, bool> get_or_create(Data<T>& builder, buffer::Buffer& buf)
    {
        auto& slot = get_or_create_slot(builder.index);
        auto& container = slot.container;
        const auto h = container.hash(builder);

        if (m_parent)
            if (auto ptr = m_parent->template find_with_hash<T>(builder, h))
                return { *ptr, false };

        // Manually assign index to continue indexing.
        builder.index.value = slot.parent_size + container.size();

        const auto [ptr, success] = container.insert_with_hash(h, builder, buf, m_arena);
        return { View<Index<T>, Repository>(ptr->index, *this), success };
    }

    /// @brief Access the element with the given index.
    template<typename T>
        requires(IndexConcept<Index<T>> && !GroupIndexConcept<Index<T>>)
    const Data<T>& operator[](Index<T> index) const noexcept
    {
        assert(index != Index<T>::max() && "Unassigned index.");

        const auto& entry = std::get<RepositoryEntry<T>>(m_repository);
        const auto& slot = entry.slot;
        const auto parent_size = slot.parent_size;

        // In parent range -> delegate
        if (index.value < parent_size)
        {
            assert(m_parent);
            return (*m_parent)[index];
        }

        // Local range -> shift down
        index.value -= parent_size;

        return slot.container[index];
    }

    template<typename T>
        requires(GroupIndexConcept<Index<T>>)
    const Data<T>& operator[](Index<T> index) const noexcept
    {
        assert(index != Index<T>::max() && "Unassigned index.");

        const auto& entry = std::get<RepositoryEntry<T>>(m_repository);
        const auto g = index.group;
        const auto& slot = entry.slot.at(g);
        const auto parent_size = slot.parent_size;

        // In parent range -> delegate
        if (index.value < parent_size)
        {
            assert(m_parent);
            return (*m_parent)[index];
        }

        // Local range -> shift down
        index.value -= parent_size;

        return slot.container[index];
    }

    template<typename T>
        requires(IndexConcept<Index<T>> && !GroupIndexConcept<Index<T>>)
    const Data<T>& front() const
    {
        const auto& entry = std::get<RepositoryEntry<T>>(m_repository);
        const auto& slot = entry.slot;

        if (slot.parent_size > 0)
        {
            assert(m_parent);
            return m_parent->template front<T>();  // recurse to root-most non-empty
        }

        return slot.container.front();
    }

    template<typename T>
        requires(GroupIndexConcept<Index<T>>)
    const Data<T>& front(Index<T> index) const
    {
        const auto& entry = std::get<RepositoryEntry<T>>(m_repository);
        const auto g = index.group;
        const auto& slot = entry.slot.at(g);

        const auto parent_size = slot.parent_size;
        if (parent_size > 0)
        {
            assert(m_parent);
            return m_parent->template front<T>(index);  // same group
        }

        return slot.container.front();
    }

    /// @brief Get the number of stored elements.
    template<typename T>
        requires(IndexConcept<Index<T>> && !GroupIndexConcept<Index<T>>)
    size_t size() const noexcept
    {
        const auto& entry = std::get<RepositoryEntry<T>>(m_repository);
        const auto& slot = entry.slot;

        return slot.parent_size + slot.container.size();
    }

    template<typename T>
        requires(GroupIndexConcept<Index<T>>)
    size_t size(Index<T> index) const noexcept
    {
        const auto& entry = std::get<RepositoryEntry<T>>(m_repository);
        const auto g = index.group;
        const auto& slot = entry.slot;

        const auto it = slot.find(g);
        if (it == slot.end())
            return 0;

        const size_t parent_size = it->second.parent_size;
        const size_t local_size = it->second.container.size();

        return parent_size + local_size;
    }

    /// @brief Clear the repository but keep memory allocated.
    void clear() noexcept
    {
        m_arena.clear();
        clear_entries();
    }

    // Get the canonical repository which is repository whose container stores the data of the element corresponding to the given index.
    template<typename T>
        requires(IndexConcept<Index<T>> && !GroupIndexConcept<Index<T>>)
    const Repository& get_canonical_context(Index<T> index) const noexcept
    {
        const auto& entry = std::get<RepositoryEntry<T>>(m_repository);

        if (index.value < entry.slot.parent_size)
        {
            assert(m_parent && "Element not found in the repository chain.");
            return m_parent->get_canonical_context(index);
        }

        return *this;
    }

    template<typename T>
        requires GroupIndexConcept<Index<T>>
    const Repository& get_canonical_context(Index<T> index) const noexcept
    {
        const auto& entry = std::get<RepositoryEntry<T>>(m_repository);
        const auto g = index.group;

        const auto it = entry.slot.find(g);
        if (it == entry.slot.end())
        {
            assert(m_parent && "Element not found in the repository chain.");
            return m_parent->get_canonical_context(index);
        }

        const auto& slot = it->second;
        if (index.value < slot.parent_size)
        {
            assert(m_parent && "Element not found in the repository chain.");
            return m_parent->get_canonical_context(index);
        }

        return *this;
    }
};

static_assert(RepositoryConcept<Repository>);
static_assert(Context<Repository>);

}

#endif
