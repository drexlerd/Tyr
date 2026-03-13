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

#ifndef TYR_FORMALISM_DATALOG_RELATION_TABLE_REPOSITORY_HPP_
#define TYR_FORMALISM_DATALOG_RELATION_TABLE_REPOSITORY_HPP_

// Include specialization headers first
#include "tyr/formalism/datalog/datas.hpp"
#include "tyr/formalism/datalog/indices.hpp"
//
#include "tyr/common/bit_packed_array_set.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/datalog/binding_view.hpp"
#include "tyr/formalism/datalog/declarations.hpp"

#include <cassert>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace tyr::formalism::datalog
{
class RelationTableRepository
{
private:
    template<std::unsigned_integral Block>
    struct Coder
    {
        using value_type = Index<Object>;

        static constexpr value_type decode(Block block) noexcept { return value_type(block); }
        static constexpr Block encode(value_type value) noexcept { return static_cast<Block>((static_cast<uint_t>(value))); }
    };

    using ConstViewType = BasicBitPackedArrayView<const uint_t, Coder<uint_t>>;

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

    using RepositoryStorage = std::tuple<Entry<Predicate<StaticTag>>, Entry<Predicate<FluentTag>>, Entry<Function<StaticTag>>, Entry<Function<FluentTag>>>;

    const RelationTableRepository* m_parent;
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

    template<typename T>
    auto& get_or_create_container(Index<T> g, Slot& slot)
    {
        if (!slot.container)
        {
            const auto view = make_view(g, *this);
            const auto arity = view.get_arity();
            const auto width = std::max<uint8_t>(1, std::bit_width(m_num_objects));
            slot.container.emplace(arity, width);
        }
        return *slot.container;
    }

    template<typename T>
    void clear_entry(Entry<T>& entry) noexcept
    {
        // First, ensure we have slots for all parent relations of this type.
        if (m_parent)
        {
            const auto& parent_entry = std::get<Entry<T>>(m_parent->m_repository);
            for (const auto& [g, parent_slot] : parent_entry.slots)
            {
                auto [it, inserted] = entry.slots.try_emplace(g);
                it->second.parent_size = m_parent->template size<T>(g);

                if (it->second.container)
                    it->second.container->clear();
            }
        }

        // Then refresh all local slots, including those not present in parent.
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
    RelationTableRepository(size_t num_objects, const RelationTableRepository* parent = nullptr) : m_parent(parent), m_repository(), m_num_objects(num_objects)
    {
        clear_entries();
    }

    template<typename T>
    std::optional<View<Index<Binding2>, RelationTableRepository>> find_with_hash(Index<T> g, const IndexList<Object>& builder, size_t h) const noexcept
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
            return View<Index<Binding2>, RelationTableRepository>(Index<Binding2>(slot.parent_size + *row_or_nullopt), *this);

        return m_parent ? m_parent->template find_with_hash<T>(g, builder, h) : std::nullopt;
    }

    template<typename T>
    std::optional<View<Index<Binding2>, RelationTableRepository>> find(Index<T> g, const IndexList<Object>& builder) const noexcept
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
    std::pair<View<Index<Binding2>, RelationTableRepository>, bool> get_or_create(Index<T> g, const IndexList<Object>& builder)
    {
        auto& slot = get_or_create_slot(g);
        // Note: this creates the local container even if the element was found in the parent.
        auto& container = get_or_create_container(g, slot);
        const auto h = container.hash(builder);

        if (m_parent)
            if (auto ptr = m_parent->template find_with_hash<T>(g, builder, h))
                return { *ptr, false };

        const auto [row, success] = container.insert_with_hash(h, builder);
        return { View<Index<Binding2>, RelationTableRepository>(Index<Binding2>(slot.parent_size + row), *this), success };
    }

    template<typename T>
    ConstViewType operator[](std::pair<Index<T>, Index<Binding2>> index) const noexcept
    {
        const auto& [g, row] = index;

        assert(g != Index<T>::max() && "Unassigned index.");
        assert(row != Index<Binding2>::max() && "Unassigned index.");

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
    const RelationTableRepository& get_canonical_context(std::pair<Index<T>, Index<Binding2>> index) const noexcept
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
};
}

#endif
