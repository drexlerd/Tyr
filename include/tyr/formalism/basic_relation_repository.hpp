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

#ifndef TYR_FORMALISM_BASIC_RELATION_REPOSITORY_HPP_
#define TYR_FORMALISM_BASIC_RELATION_REPOSITORY_HPP_

#include "tyr/common/block_array_set.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/common/types.hpp"
#include "tyr/formalism/binding_data.hpp"
#include "tyr/formalism/binding_index.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/object_index.hpp"

#include <cassert>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace tyr::formalism
{

template<typename T>
class BasicRelationRepository
{
private:
    template<std::unsigned_integral Block>
    struct Coder
    {
        using value_type = Index<Object>;

        static constexpr value_type decode(Block block) noexcept { return value_type(block); }
        static constexpr Block encode(value_type value) noexcept { return static_cast<Block>((static_cast<uint_t>(value))); }
    };

    static constexpr uint_t kInvalid = std::numeric_limits<uint_t>::max();

    struct Slot
    {
        Index<T> g;
        BlockArraySet<uint_t, Coder<uint_t>> container;
        size_t parent_size = 0;

        Slot(Index<T> g, size_t arity, size_t parent_size) : g(g), container(arity), parent_size(parent_size) {}
    };

    const BasicRelationRepository* m_parent;

    std::vector<uint_t> m_forward;
    std::vector<Slot> m_slots;

    Slot& get_or_create_slot(Index<T> g, size_t arity)
    {
        const auto gi = uint_t(g);

        if (gi >= m_forward.size())
            m_forward.resize(gi + 1, kInvalid);

        if (m_forward[gi] == kInvalid)
        {
            const auto parent_size = m_parent ? m_parent->size(g) : size_t { 0 };
            m_forward[gi] = static_cast<uint_t>(m_slots.size());
            m_slots.emplace_back(g, arity, parent_size);
        }

        return m_slots[m_forward[gi]];
    }

    void clear_slots() noexcept
    {
        for (auto& slot : m_slots)
        {
            slot.container.clear();
            slot.parent_size = m_parent ? m_parent->size(slot.g) : size_t { 0 };
        }
    }

    const Slot* find_slot(Index<T> g) const noexcept
    {
        const auto gi = uint_t(g);

        if (gi >= m_forward.size())
            return nullptr;

        const auto si = m_forward[gi];
        if (si == kInvalid)
            return nullptr;

        return &m_slots[si];
    }

    Slot* find_slot(Index<T> g) noexcept
    {
        const auto gi = uint_t(g);

        if (gi >= m_forward.size())
            return nullptr;

        const auto si = m_forward[gi];
        if (si == kInvalid)
            return nullptr;

        return &m_slots[si];
    }

public:
    using container_type = BlockArraySet<uint_t, Coder<uint_t>>;
    using ConstViewType = typename container_type::ConstArrayView;

    BasicRelationRepository(const BasicRelationRepository* parent = nullptr) : m_parent(parent), m_forward(), m_slots() { clear_slots(); }
    BasicRelationRepository(const BasicRelationRepository& other) = delete;
    BasicRelationRepository& operator=(const BasicRelationRepository& other) = delete;
    BasicRelationRepository(BasicRelationRepository&& other) = default;
    BasicRelationRepository& operator=(BasicRelationRepository&& other) = default;

    /**
     * Common methods.
     */

    /// @brief Clear the repository but keep memory allocated.
    void clear() noexcept { clear_slots(); }

    /**
     * Local methods
     */

    static size_t hash(const Data<RelationBinding<T>>& builder) noexcept { return BlockArraySet<uint_t, Coder<uint_t>>::hash(builder.objects); }

    std::optional<Index<Row>> find_local_with_hash(const Data<RelationBinding<T>>& builder, size_t h) const noexcept
    {
        const auto g = builder.relation;

        const auto* slot = find_slot(g);
        if (!slot)
            return std::nullopt;

        if (auto row_or_nullopt = slot->container.find_with_hash(builder.objects, h))
            return Index<Row>(slot->parent_size + *row_or_nullopt);

        return std::nullopt;
    }

    std::optional<Index<Row>> find_local(const Data<RelationBinding<T>>& builder) const noexcept
    {
        return find_local_with_hash(builder, BasicRelationRepository::hash(builder));
    }

    std::pair<Index<Row>, bool> get_or_create_local_with_hash(const Data<RelationBinding<T>>& builder, size_t h)
    {
        const auto g = builder.relation;

        auto& slot = get_or_create_slot(g, builder.objects.size());
        auto& container = slot.container;

        if (auto row_or_nullopt = container.find_with_hash(builder.objects, h))
            return { Index<Row>(slot.parent_size + *row_or_nullopt), false };

        const auto [row, success] = container.insert_with_hash(h, builder.objects);
        return { Index<Row>(slot.parent_size + row), success };
    }

    std::pair<Index<Row>, bool> get_or_create_local(const Data<RelationBinding<T>>& builder)
    {
        return get_or_create_local_with_hash(builder, BasicRelationRepository::hash(builder));
    }

    ConstViewType at_local(Index<RelationBinding<T>> index) const noexcept
    {
        const auto& [g, row] = index;

        const auto* slot = find_slot(g);
        assert(slot);
        assert(row.value >= slot->parent_size);

        return slot->container[row.value - slot->parent_size];
    }

    ConstViewType front_local(Index<T> g) const noexcept
    {
        const auto* slot = find_slot(g);
        assert(slot);
        return slot->container.front();
    }

    size_t local_size(Index<T> g) const noexcept
    {
        const auto* slot = find_slot(g);
        return slot ? slot->container.size() : 0;
    }

    size_t size(Index<T> g) const noexcept
    {
        const auto* slot = find_slot(g);
        if (!slot)
            return m_parent ? m_parent->size(g) : 0;

        return slot->parent_size + slot->container.size();
    }

    size_t parent_size(Index<T> g) const noexcept
    {
        const auto* slot = find_slot(g);
        return slot ? slot->parent_size : (m_parent ? m_parent->size(g) : 0);
    }

    bool is_local(Index<RelationBinding<T>> index) const noexcept
    {
        const auto& [g, row] = index;
        assert(g != Index<T>::max() && "Unassigned index.");
        assert(row != Index<Row>::max() && "Unassigned index.");

        const auto* slot = find_slot(g);
        if (!slot)
            return false;

        return uint_t(row) >= slot->parent_size;
    }

    bool exists_parent_mutation(Index<T> g) const noexcept
    {
        if (!m_parent)
            return false;

        const auto* slot = find_slot(g);
        if (!slot)
            return false;

        return m_parent->size(g) > slot->parent_size;
    }
};
}

#endif
