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

#include "tyr/common/block_array_set.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/common/types.hpp"
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
    template<std::unsigned_integral Block>
    struct Coder
    {
        using value_type = Index<Object>;

        static constexpr value_type decode(Block block) noexcept { return value_type(block); }
        static constexpr Block encode(value_type value) noexcept { return static_cast<Block>((static_cast<uint_t>(value))); }
    };

    template<typename T>
    struct Slot
    {
        Index<T> g;
        BlockArraySet<uint_t, Coder<uint_t>> container;
        size_t parent_size = 0;

        Slot(Index<T> g, BlockArraySet<uint_t, Coder<uint_t>> container, size_t parent_size) : g(g), container(std::move(container)), parent_size(parent_size)
        {
        }
    };

    static constexpr uint_t kInvalid = std::numeric_limits<uint_t>::max();

    template<typename T>
    struct Entry
    {
        std::vector<uint_t> forward;

        using slot_type = std::vector<Slot<T>>;

        slot_type slots;
    };

    using RepositoryStorage = std::tuple<Entry<Ts>...>;

    const RelationRepository* m_parent;
    RepositoryStorage m_repository;
    size_t m_num_objects;

    template<typename T>
    auto& get_or_create_slot(Index<T> g, size_t arity)
    {
        const auto parent_size = m_parent ? m_parent->size(g) : size_t { 0 };
        auto& entry = std::get<Entry<T>>(m_repository);
        const auto gi = uint_t(g);

        if (gi >= entry.forward.size())
            entry.forward.resize(gi + 1, kInvalid);

        if (entry.forward[gi] == kInvalid)
        {
            entry.forward[gi] = static_cast<uint_t>(entry.slots.size());
            entry.slots.emplace_back(g, BlockArraySet<uint_t, Coder<uint_t>>(arity), parent_size);
        }

        return entry.slots[entry.forward[gi]];
    }

    template<typename T>
    void clear_entry(Entry<T>& entry) noexcept
    {
        for (auto& slot : entry.slots)
        {
            slot.container.clear();
            slot.parent_size = m_parent ? m_parent->size(slot.g) : size_t { 0 };
        }
    }

    void clear_entries() noexcept
    {
        std::apply([&](auto&... entry) { (clear_entry(entry), ...); }, m_repository);
    }

    template<typename T>
    const Slot<T>* find_slot(Index<T> g) const noexcept
    {
        const auto& entry = std::get<Entry<T>>(m_repository);
        const auto gi = uint_t(g);

        if (gi >= entry.forward.size())
            return nullptr;

        const auto si = entry.forward[gi];
        if (si == kInvalid)
            return nullptr;

        return &entry.slots[si];
    }

    template<typename T>
    Slot<T>* find_slot(Index<T> g) noexcept
    {
        auto& entry = std::get<Entry<T>>(m_repository);
        const auto gi = uint_t(g);

        if (gi >= entry.forward.size())
            return nullptr;

        const auto si = entry.forward[gi];
        if (si == kInvalid)
            return nullptr;

        return &entry.slots[si];
    }

public:
    using container_type = BlockArraySet<uint_t, Coder<uint_t>>;
    using ConstViewType = typename container_type::ConstArrayView;

    RelationRepository(size_t num_objects, const RelationRepository* parent = nullptr) : m_parent(parent), m_repository(), m_num_objects(num_objects)
    {
        clear_entries();
    }
    RelationRepository(const RelationRepository& other) = delete;
    RelationRepository& operator=(const RelationRepository& other) = delete;
    RelationRepository(RelationRepository&& other) = delete;
    RelationRepository& operator=(RelationRepository&& other) = delete;

    /**
     * Common methods.
     */

    /// @brief Clear the repository but keep memory allocated.
    void clear() noexcept { clear_entries(); }

    /**
     * Global methods.
     */

    template<typename T>
    std::optional<View<std::pair<Index<T>, Index<Binding>>, RelationRepository>>
    find_with_hash(Index<T> g, const IndexList<Object>& builder, size_t h) const noexcept
    {
        const auto* slot = find_slot(g);
        if (!slot)
            return m_parent ? m_parent->template find_with_hash<T>(g, builder, h) : std::nullopt;

        if (auto row_or_nullopt = slot->container.find_with_hash(builder, h))
            return View<std::pair<Index<T>, Index<Binding>>, RelationRepository>(std::make_pair(g, Index<Binding>(slot->parent_size + *row_or_nullopt)), *this);

        return m_parent ? m_parent->template find_with_hash<T>(g, builder, h) : std::nullopt;
    }

    template<typename T>
    std::optional<View<std::pair<Index<T>, Index<Binding>>, RelationRepository>> find(Index<T> g, const IndexList<Object>& builder) const noexcept
    {
        const auto* slot = find_slot(g);

        if (!slot)
            return m_parent ? m_parent->template find<T>(g, builder) : std::nullopt;

        const auto h = slot->container.hash(builder);
        if (auto row_or_nullopt = slot->container.find_with_hash(builder, h))
            return View<std::pair<Index<T>, Index<Binding>>, RelationRepository>(std::make_pair(g, Index<Binding>(slot->parent_size + *row_or_nullopt)), *this);

        return m_parent ? m_parent->template find_with_hash<T>(g, builder, h) : std::nullopt;
    }

    template<typename T>
    std::pair<View<std::pair<Index<T>, Index<Binding>>, RelationRepository>, bool> get_or_create(Index<T> g, size_t arity, const IndexList<Object>& builder)
    {
        auto& slot = get_or_create_slot(g, arity);

        if (m_parent)
            if (auto ptr = m_parent->template find<T>(g, builder))
                return { *ptr, false };

        auto& container = slot.container;
        const auto h = container.hash(builder);

        const auto [row, success] = container.insert_with_hash(h, builder);
        return { View<std::pair<Index<T>, Index<Binding>>, RelationRepository>(std::make_pair(g, Index<Binding>(slot.parent_size + row)), *this), success };
    }

    template<typename T>
    ConstViewType operator[](std::pair<Index<T>, Index<Binding>> index) const noexcept
    {
        const auto& [g, row] = index;

        assert(g != Index<T>::max() && "Unassigned index.");
        assert(row != Index<Binding>::max() && "Unassigned index.");

        const auto* slot = find_slot(g);
        if (!slot)
        {
            assert(m_parent);
            return (*m_parent)[index];
        }

        if (row.value < slot->parent_size)
        {
            assert(m_parent);
            return (*m_parent)[index];
        }

        return slot->container[row.value - slot->parent_size];
    }

    template<typename T>
    size_t size(Index<T> g) const noexcept
    {
        const auto* slot = find_slot(g);
        if (!slot)
            return m_parent ? m_parent->size(g) : 0;

        return slot->parent_size + slot->container.size();
    }

    template<typename T>
    const RelationRepository& get_canonical_context(std::pair<Index<T>, Index<Binding>> index) const noexcept
    {
        const auto& [g, row] = index;

        const auto* slot = find_slot(g);
        if (!slot)
        {
            assert(m_parent && "Element not found in the repository chain.");
            return m_parent ? m_parent->get_canonical_context(index) : *this;
        }

        if (row.value < slot->parent_size)
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
    std::optional<Index<Binding>> find_local_with_hash(Index<T> g, const IndexList<Object>& builder, size_t h) const noexcept
    {
        const auto* slot = find_slot(g);
        if (!slot)
            return std::nullopt;

        if (auto row_or_nullopt = slot->container.find_with_hash(builder, h))
            return Index<Binding>(slot->parent_size + *row_or_nullopt);

        return std::nullopt;
    }

    template<typename T>
    std::optional<Index<Binding>> find_local(Index<T> g, const IndexList<Object>& builder) const noexcept
    {
        const auto* slot = find_slot(g);
        if (!slot)
            return std::nullopt;

        const auto h = slot->container.hash(builder);
        if (auto row_or_nullopt = slot->container.find_with_hash(builder, h))
            return Index<Binding>(slot->parent_size + *row_or_nullopt);

        return std::nullopt;
    }

    template<typename T>
    std::pair<Index<Binding>, bool> get_or_create_local(Index<T> g, size_t arity, const IndexList<Object>& builder)
    {
        auto& slot = get_or_create_slot(g, arity);
        auto& container = slot.container;
        const auto h = container.hash(builder);

        if (auto row_or_nullopt = container.find_with_hash(builder, h))
            return { Index<Binding>(slot.parent_size + *row_or_nullopt), false };

        const auto [row, success] = container.insert_with_hash(h, builder);
        return { Index<Binding>(slot.parent_size + row), success };
    }

    template<typename T>
    ConstViewType at_local(std::pair<Index<T>, Index<Binding>> index) const noexcept
    {
        const auto& [g, row] = index;

        const auto* slot = find_slot(g);
        assert(slot);
        assert(row.value >= slot->parent_size);

        return slot->container[row.value - slot->parent_size];
    }

    template<typename T>
    size_t local_size(Index<T> g) const noexcept
    {
        const auto* slot = find_slot(g);
        return slot ? slot->container.size() : 0;
    }

    template<typename T>
    size_t parent_size(Index<T> g) const noexcept
    {
        const auto* slot = find_slot(g);
        return slot ? slot->parent_size : (m_parent ? m_parent->size(g) : 0);
    }

    template<typename T>
    bool is_local(std::pair<Index<T>, Index<Binding>> index) const noexcept
    {
        const auto& [g, row] = index;
        assert(g != Index<T>::max() && "Unassigned index.");
        assert(row != Index<Binding>::max() && "Unassigned index.");

        const auto* slot = find_slot(g);
        if (!slot)
            return false;

        return uint_t(row) >= slot->parent_size;
    }
};
}

#endif
