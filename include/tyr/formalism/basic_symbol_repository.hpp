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

#ifndef TYR_FORMALISM_DATALOG_BASIC_SYMBOL_REPOSITORY_HPP_
#define TYR_FORMALISM_DATALOG_BASIC_SYMBOL_REPOSITORY_HPP_

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

template<typename T>
class BasicSymbolRepository
{
private:
    template<typename U, bool Trivial = uses_trivial_storage_v<U>>
    struct Slot;

    template<typename U>
    struct Slot<U, true>
    {
        ::tyr::IndexedHashSet<U> container;
        size_t parent_size = 0;

        Slot(buffer::Buffer&, buffer::SegmentedBuffer&) : container(), parent_size(0) {}
    };

    template<typename U>
    struct Slot<U, false>
    {
        buffer::IndexedHashSet<U> container;
        size_t parent_size = 0;

        Slot(buffer::Buffer& buffer, buffer::SegmentedBuffer& arena) : container(buffer, arena), parent_size(0) {}
    };

    const BasicSymbolRepository* m_parent;
    std::unique_ptr<buffer::SegmentedBuffer> m_arena;
    std::unique_ptr<buffer::Buffer> m_buffer;
    Slot<T> m_slot;

    void clear_slot() noexcept
    {
        m_slot.container.clear();
        m_slot.parent_size = m_parent ? m_parent->size() : size_t { 0 };
    }

public:
    BasicSymbolRepository(const BasicSymbolRepository* parent = nullptr) :
        m_parent(parent),
        m_arena(std::make_unique<buffer::SegmentedBuffer>()),
        m_buffer(std::make_unique<buffer::Buffer>()),
        m_slot(*m_buffer, *m_arena)
    {
        clear_slot();
    }

    BasicSymbolRepository(const BasicSymbolRepository&) = delete;
    BasicSymbolRepository& operator=(const BasicSymbolRepository&) = delete;
    BasicSymbolRepository(BasicSymbolRepository&&) noexcept = default;
    BasicSymbolRepository& operator=(BasicSymbolRepository&&) noexcept = default;

    void clear() noexcept
    {
        m_arena->clear();
        clear_slot();
    }

    size_t hash(const Data<T>& builder) const { return m_slot.container.hash(builder); }

    /**
     * Local methods
     */

    std::optional<Index<T>> find_local_with_hash(const Data<T>& builder, size_t h) const noexcept
    {
        const auto& container = m_slot.container;
        assert(h == container.hash(builder));

        if (auto index_or_nullopt = container.find_with_hash(builder, h))
            return Index<T>(m_slot.parent_size + uint_t(*index_or_nullopt));

        return std::nullopt;
    }

    std::optional<Index<T>> find_local(const Data<T>& builder) const noexcept { return find_local_with_hash(builder, hash(builder)); }

    std::pair<Index<T>, bool> get_or_create_local_with_hash(Data<T>& builder, size_t h)
    {
        auto& container = m_slot.container;

        if (auto index_or_nullopt = container.find_with_hash(builder, h))
            return { Index<T>(m_slot.parent_size + uint_t(*index_or_nullopt)), false };

        builder.index.value = m_slot.parent_size + container.size();

        const auto [index, success] = container.insert_with_hash(h, builder);
        return { Index<T>(m_slot.parent_size + uint_t(index)), success };
    }

    std::pair<Index<T>, bool> get_or_create_local(Data<T>& builder) { return get_or_create_local_with_hash(builder, hash(builder)); }

    const Data<T>& at_local(Index<T> index) const noexcept
    {
        const auto parent_size = m_slot.parent_size;

        assert(index.value >= parent_size);
        return m_slot.container[Index<T>(index.value - parent_size)];
    }

    const Data<T>& front_local() const { return m_slot.container.front(); }

    size_t local_size() const noexcept { return m_slot.container.size(); }

    size_t size() const noexcept { return m_slot.parent_size + m_slot.container.size(); }

    size_t parent_size() const noexcept { return m_slot.parent_size; }

    bool is_local(Index<T> index) const noexcept
    {
        assert(index != Index<T>::max() && "Unassigned index.");
        return uint_t(index) >= m_slot.parent_size;
    }
};
}

#endif
