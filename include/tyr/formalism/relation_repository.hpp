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

#include "tyr/common/bit_packed_array_set.hpp"
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

    struct Slot
    {
        std::optional<BitPackedArraySet<uint_t, Coder<uint_t>>> container;
        size_t parent_size = 0;
    };

    template<typename T>
    struct Entry
    {
        using slot_type = UnorderedMap<Index<T>, Slot>;

        slot_type slots;
    };

    using RepositoryStorage = std::tuple<Entry<Ts>...>;

    const RelationRepository* m_parent;
    RepositoryStorage m_repository;
    size_t m_num_objects;

    template<typename T>
    auto& get_or_create_slot(Index<T> g)
    {
        auto& entry = std::get<Entry<T>>(m_repository);
        {
            const auto it = entry.slots.find(g);
            if (it != entry.slots.end())
                return it->second;  ///< slot already exists
        }

        const auto parent_size = m_parent ? m_parent->size(g) : size_t { 0 };
        const auto [it, inserted] = entry.slots.emplace(g, Slot { std::nullopt, parent_size });
        assert(inserted);
        return it->second;
    }

    auto& get_or_create_container(size_t arity, uint8_t width, Slot& slot)
    {
        if (!slot.container)
            slot.container.emplace(arity, width);
        return *slot.container;
    }

    template<typename T>
    void clear_entry(Entry<T>& entry) noexcept
    {
        for (auto& [g, slot] : entry.slots)
        {
            if (slot.container)
                slot.container->clear();

            slot.parent_size = m_parent ? m_parent->template size<T>(g) : size_t { 0 };
        }
    }

    void clear_entries() noexcept
    {
        std::apply([&](auto&... entry) { (clear_entry(entry), ...); }, m_repository);
    }

public:
    using ConstViewType = BasicBitPackedArrayView<const uint_t, Coder<uint_t>>;

    RelationRepository(size_t num_objects, const RelationRepository* parent = nullptr) : m_parent(parent), m_repository(), m_num_objects(num_objects)
    {
        clear_entries();
    }

    /**
     * Global methods.
     */

    template<typename T>
    std::optional<Index<Binding>> find_with_hash(Index<T> g, const IndexList<Object>& builder, size_t h) const noexcept
    {
        const auto& entry = std::get<Entry<T>>(m_repository);

        const auto& slots = entry.slots;
        const auto it = slots.find(g);
        if (it == slots.end())  ///< no slot exists here
            return m_parent ? m_parent->template find_with_hash<T>(g, builder, h) : std::nullopt;

        const auto& slot = it->second;
        if (!slot.container)  ///< no container exists here
            return m_parent ? m_parent->template find_with_hash<T>(g, builder, h) : std::nullopt;

        if (auto row_or_nullopt = slot.container->find_with_hash(builder, h))  /// container exists here
            return Index<Binding>(slot.parent_size + *row_or_nullopt);

        return m_parent ? m_parent->template find_with_hash<T>(g, builder, h) : std::nullopt;
    }

    template<typename T>
    std::optional<Index<Binding>> find(Index<T> g, const IndexList<Object>& builder) const noexcept
    {
        const auto& entry = std::get<Entry<T>>(m_repository);
        const auto& slots = entry.slots;

        const auto it = slots.find(g);
        if (it == slots.end())  ///< no slot exists here
            return m_parent ? m_parent->template find<T>(g, builder) : std::nullopt;

        const auto& slot = it->second;
        if (!slot.container)  ///< no container exists here
            return m_parent ? m_parent->template find<T>(g, builder) : std::nullopt;

        const auto h = slot.container->hash(builder);
        return find_with_hash<T>(g, builder, h);
    }

    template<typename T>
    std::pair<Index<Binding>, bool> get_or_create(Index<T> g, size_t arity, uint8_t width, const IndexList<Object>& builder)
    {
        auto& slot = get_or_create_slot(g);

        if (m_parent)
            if (auto ptr = m_parent->template find<T>(g, builder))
                return { *ptr, false };

        auto& container = get_or_create_container(arity, width, slot);
        const auto h = container.hash(builder);

        const auto [row, success] = container.insert_with_hash(h, builder);
        return { Index<Binding>(slot.parent_size + row), success };
    }

    template<typename T>
    ConstViewType operator[](std::pair<Index<T>, Index<Binding>> index) const noexcept
    {
        const auto& [g, row] = index;

        assert(g != Index<T>::max() && "Unassigned index.");
        assert(row != Index<Binding>::max() && "Unassigned index.");

        const auto& entry = std::get<Entry<T>>(m_repository);
        const auto it = entry.slots.find(g);

        if (it == entry.slots.end())
        {
            assert(m_parent);
            return (*m_parent)[index];
        }

        const auto& slot = it->second;
        const auto parent_size = slot.parent_size;

        if (row.value < parent_size)
        {
            assert(m_parent);
            return (*m_parent)[index];
        }

        assert(slot.container && "Missing local container for local row.");
        assert(row.value >= parent_size);

        return (*slot.container)[row.value - parent_size];
    }

    template<typename T>
    size_t size(Index<T> g) const noexcept
    {
        const auto& entry = std::get<Entry<T>>(m_repository);
        const auto& slots = entry.slots;

        const auto it = slots.find(g);
        if (it == slots.end())
            return m_parent ? m_parent->size(g) : 0;

        const size_t parent_size = it->second.parent_size;
        const size_t local_size = it->second.container ? it->second.container->size() : 0;

        return parent_size + local_size;
    }

    /// @brief Clear the repository but keep memory allocated.
    void clear() noexcept { clear_entries(); }

    template<typename T>
    const RelationRepository& get_canonical_context(std::pair<Index<T>, Index<Binding>> index) const noexcept
    {
        const auto& [g, row] = index;

        const auto& entry = std::get<Entry<T>>(m_repository);

        const auto it = entry.slots.find(g);
        if (it == entry.slots.end())
        {
            assert(m_parent && "Element not found in the repository chain.");
            return m_parent ? m_parent->get_canonical_context(index) : *this;
        }

        const auto& slot = it->second;
        if (row.value < slot.parent_size)
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
        const auto& entry = std::get<Entry<T>>(m_repository);
        const auto it = entry.slots.find(g);
        if (it == entry.slots.end())
            return std::nullopt;

        const auto& slot = it->second;
        if (!slot.container)
            return std::nullopt;

        if (auto row_or_nullopt = slot.container->find_with_hash(builder, h))
            return Index<Binding>(slot.parent_size + *row_or_nullopt);

        return std::nullopt;
    }

    template<typename T>
    std::optional<Index<Binding>> find_local(Index<T> g, const IndexList<Object>& builder) const noexcept
    {
        const auto& entry = std::get<Entry<T>>(m_repository);
        const auto it = entry.slots.find(g);
        if (it == entry.slots.end())
            return std::nullopt;

        const auto& slot = it->second;
        if (!slot.container)
            return std::nullopt;

        return find_local_with_hash<T>(g, builder, slot.container->hash(builder));
    }

    template<typename T>
    std::pair<Index<Binding>, bool> get_or_create_local(Index<T> g, size_t arity, uint8_t width, const IndexList<Object>& builder)
    {
        auto& slot = get_or_create_slot(g);
        auto& container = get_or_create_container(arity, width, slot);
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

        const auto& entry = std::get<Entry<T>>(m_repository);
        const auto it = entry.slots.find(g);

        assert(it != entry.slots.end());
        const auto& slot = it->second;
        assert(slot.container);
        assert(row.value >= slot.parent_size);

        return (*slot.container)[row.value - slot.parent_size];
    }

    template<typename T>
    size_t local_size(Index<T> g) const noexcept
    {
        const auto& entry = std::get<Entry<T>>(m_repository);
        const auto it = entry.slots.find(g);
        if (it == entry.slots.end() || !it->second.container)
            return 0;
        return it->second.container->size();
    }

    template<typename T>
    size_t parent_size(Index<T> g) const noexcept
    {
        const auto& entry = std::get<Entry<T>>(m_repository);
        const auto it = entry.slots.find(g);
        if (it == entry.slots.end())
            return m_parent ? m_parent->size(g) : 0;
        return it->second.parent_size;
    }

    template<typename T>
    bool is_local(std::pair<Index<T>, Index<Binding>> index) const noexcept
    {
        const auto& [g, row] = index;
        assert(g != Index<T>::max() && "Unassigned index.");
        assert(row != Index<Binding>::max() && "Unassigned index.");

        const auto& entry = std::get<Entry<T>>(m_repository);

        const auto it = entry.slots.find(g);
        if (it == entry.slots.end())
            return false;

        return uint_t(row) >= it->second.parent_size;
    }
};
}

#endif
