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
#include "tyr/buffer/indexed_hash_set.hpp"
#include "tyr/buffer/segmented_buffer.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/indexed_hash_set.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/common/types.hpp"
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
    template<typename T, bool Trivial = uses_trivial_storage_v<T>>
    struct Slot;

    template<typename T>
    struct Slot<T, true>
    {
        ::tyr::IndexedHashSet<T> container;
        size_t parent_size = 0;

        Slot(buffer::Buffer&, buffer::SegmentedBuffer&) : container(), parent_size(0) {}
    };

    template<typename T>
    struct Slot<T, false>
    {
        buffer::IndexedHashSet<T> container;
        size_t parent_size = 0;

        Slot(buffer::Buffer& buffer, buffer::SegmentedBuffer& arena) : container(buffer, arena), parent_size(0) {}
    };

    template<typename T>
    struct Entry
    {
        using slot_type = Slot<T>;
        slot_type slot;

        Entry(buffer::Buffer& buffer, buffer::SegmentedBuffer& arena) : slot(buffer, arena) {}
    };

    using RepositoryStorage = std::tuple<Entry<Ts>...>;

    const SymbolRepository* m_parent;
    buffer::SegmentedBuffer m_arena;
    buffer::Buffer m_buffer;
    RepositoryStorage m_repository;

    /**
     * Clear
     */

    template<typename T>
    void clear_entry(Entry<T>& entry) noexcept
    {
        entry.slot.container.clear();
        entry.slot.parent_size = m_parent ? m_parent->template size<T>() : size_t { 0 };
    }

    void clear_entries() noexcept
    {
        std::apply([&](auto&... entry) { (clear_entry(entry), ...); }, m_repository);
    }

public:
    SymbolRepository(const SymbolRepository* parent = nullptr) : m_parent(parent), m_arena(), m_buffer(), m_repository(Entry<Ts>(m_buffer, m_arena)...)
    {
        clear_entries();
    }
    SymbolRepository(const SymbolRepository& other) = delete;
    SymbolRepository& operator=(const SymbolRepository& other) = delete;
    SymbolRepository(SymbolRepository&& other) = delete;
    SymbolRepository& operator=(SymbolRepository&& other) = delete;

    /**
     * Common methods
     */

    /// @brief Clear the repository but keep memory allocated.
    void clear() noexcept
    {
        m_arena.clear();
        clear_entries();
    }

    template<typename T>
    size_t hash(const Data<T>& builder) const
    {
        const auto& entry = std::get<Entry<T>>(m_repository);
        return entry.slot.container.hash(builder);
    }

    /**
     * Global methods
     */

    template<typename T>
    std::optional<View<Index<T>, SymbolRepository>> find_with_hash(const Data<T>& builder, size_t h) const noexcept
    {
        const auto& entry = std::get<Entry<T>>(m_repository);
        const auto& slot = entry.slot;
        const auto& container = slot.container;
        assert(h == container.hash(builder) && "The given hash does not match container internal's hash.");

        if (auto index_or_nullopt = container.find_with_hash(builder, h))
            return View<Index<T>, SymbolRepository>(slot.parent_size + *index_or_nullopt, *this);

        return m_parent ? m_parent->template find_with_hash<T>(builder, h) : std::nullopt;
    }

    template<typename T>
    std::optional<View<Index<T>, SymbolRepository>> find(const Data<T>& builder) const noexcept
    {
        return find_with_hash<T>(builder, hash(builder));
    }

    template<typename T>
    std::pair<View<Index<T>, SymbolRepository>, bool> get_or_create_with_hash(Data<T>& builder, size_t h)
    {
        auto& entry = std::get<Entry<T>>(m_repository);
        auto& slot = entry.slot;
        auto& container = slot.container;

        if (m_parent)
            if (auto view_or_nullopt = m_parent->template find_with_hash<T>(builder, h))
                return { *view_or_nullopt, false };

        // Manually assign index to continue indexing.
        builder.index.value = slot.parent_size + container.size();

        const auto [index, success] = container.insert_with_hash(h, builder);

        return { View<Index<T>, SymbolRepository>(slot.parent_size + index, *this), success };
    }

    template<typename T>
    std::pair<View<Index<T>, SymbolRepository>, bool> get_or_create(Data<T>& builder)
    {
        return get_or_create_with_hash(builder, hash(builder));
    }

    /// @brief Access the element with the given index.
    template<typename T>
    const Data<T>& operator[](Index<T> index) const noexcept
    {
        assert(index != Index<T>::max() && "Unassigned index.");

        const auto& entry = std::get<Entry<T>>(m_repository);
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
    const Data<T>& front() const
    {
        const auto& entry = std::get<Entry<T>>(m_repository);
        const auto& slot = entry.slot;

        if (slot.parent_size > 0)
        {
            assert(m_parent);
            return m_parent->template front<T>();  // recurse to root-most non-empty
        }

        return slot.container.front();
    }

    /// @brief Get the number of stored elements.
    template<typename T>
    size_t size() const noexcept
    {
        const auto& entry = std::get<Entry<T>>(m_repository);
        const auto& slot = entry.slot;

        return slot.parent_size + slot.container.size();
    }

    template<typename T>
    const SymbolRepository& get_canonical_context(Index<T> index) const noexcept
    {
        const auto& entry = std::get<Entry<T>>(m_repository);
        const auto& slot = entry.slot;

        if (index.value < slot.parent_size)
        {
            assert(m_parent && "Element not found in the repository chain.");
            return m_parent->get_canonical_context(index);
        }

        return *this;
    }

    /**
     * Local methods
     */

    template<typename T>
    std::optional<Index<T>> find_local_with_hash(const Data<T>& builder, size_t h) const noexcept
    {
        const auto& entry = std::get<Entry<T>>(m_repository);
        const auto& slot = entry.slot;
        const auto& container = slot.container;
        assert(h == container.hash(builder));

        if (auto index_or_nullopt = container.find_with_hash(builder, h))
            return Index<T>(slot.parent_size + uint_t(*index_or_nullopt));

        return std::nullopt;
    }

    template<typename T>
    std::optional<Index<T>> find_local(const Data<T>& builder) const noexcept
    {
        const auto& entry = std::get<Entry<T>>(m_repository);
        const auto& container = entry.slot.container;
        return find_local_with_hash<T>(builder, container.hash(builder));
    }

    template<typename T>
    std::pair<Index<T>, bool> get_or_create_local_with_hash(Data<T>& builder, size_t h)
    {
        auto& entry = std::get<Entry<T>>(m_repository);
        auto& slot = entry.slot;
        auto& container = slot.container;

        if (auto index_or_nullopt = container.find_with_hash(builder, h))
            return { Index<T>(slot.parent_size + uint_t(*index_or_nullopt)), false };

        builder.index.value = slot.parent_size + container.size();

        const auto [index, success] = container.insert_with_hash(h, builder);
        return { Index<T>(slot.parent_size + uint_t(index)), success };
    }

    template<typename T>
    std::pair<Index<T>, bool> get_or_create_local(Data<T>& builder)
    {
        return get_or_create_local_with_hash(builder, hash(builder));
    }

    template<typename T>
    const Data<T>& at_local(Index<T> index) const noexcept
    {
        const auto& entry = std::get<Entry<T>>(m_repository);
        const auto parent_size = entry.slot.parent_size;

        assert(index.value >= parent_size);
        return entry.slot.container[Index<T>(index.value - parent_size)];
    }

    template<typename T>
    const Data<T>& front_local() const
    {
        const auto& entry = std::get<Entry<T>>(m_repository);
        return entry.slot.container.front();
    }

    template<typename T>
    size_t local_size() const noexcept
    {
        const auto& entry = std::get<Entry<T>>(m_repository);
        return entry.slot.container.size();
    }

    template<typename T>
    size_t parent_size() const noexcept
    {
        const auto& entry = std::get<Entry<T>>(m_repository);
        return entry.slot.parent_size;
    }

    template<typename T>
    bool is_local(Index<T> index) const noexcept
    {
        assert(index != Index<T>::max() && "Unassigned index.");
        const auto& entry = std::get<Entry<T>>(m_repository);
        return uint_t(index) >= entry.slot.parent_size;
    }
};

}

#endif
