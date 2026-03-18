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
 *<
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TYR_COMMON_INDEXED_HASH_SET_HPP_
#define TYR_COMMON_INDEXED_HASH_SET_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/index_mixins.hpp"
#include "tyr/common/segmented_vector.hpp"
#include "tyr/common/types.hpp"

#include <concepts>
#include <gtl/phmap.hpp>
#include <memory>
#include <optional>
#include <vector>

namespace tyr
{

template<typename Tag, typename H = Hash<Data<Tag>>, typename E = EqualTo<Data<Tag>>, size_t FirstSegmentSize = 32>
class IndexedHashSet
{
    static_assert(bit::is_power_of_two(FirstSegmentSize));
    static_assert(std::is_trivially_copyable_v<Data<Tag>>);
    static_assert(std::is_default_constructible_v<Data<Tag>>);

private:
    class IndexableHash;
    class IndexableEqualTo;

    using VectorType = SegmentedVector<Data<Tag>, FirstSegmentSize>;

public:
    IndexedHashSet() : m_storage(std::make_unique<VectorType>()), m_set(0, IndexableHash(*m_storage), IndexableEqualTo(*m_storage)) {}
    IndexedHashSet(const IndexedHashSet& other) = delete;
    IndexedHashSet& operator=(const IndexedHashSet& other) = delete;
    IndexedHashSet(IndexedHashSet&& other) = default;
    IndexedHashSet& operator=(IndexedHashSet&& other) = default;

    void clear() noexcept
    {
        m_set.clear();
        m_storage->clear();
    }

    size_t hash(const Data<Tag>& element) const noexcept { return m_set.hash(element); }

    std::optional<Index<Tag>> find_with_hash(const Data<Tag>& value, size_t h) const
    {
        if (auto it = m_set.find(value, h); it != m_set.end())
            return *it;

        return std::nullopt;
    }

    std::optional<Index<Tag>> find(const Data<Tag>& value) const { return find_with_hash(value, hash(value)); }

    std::pair<Index<Tag>, bool> insert_with_hash(size_t h, const Data<Tag>& value)
    {
        if (auto it = m_set.find(value, h); it != m_set.end())
            return { *it, false };

        Index<Tag> idx(static_cast<uint_t>(m_storage->size()));
        m_storage->push_back(value);
        m_set.emplace_with_hash(h, idx);
        return { idx, true };
    }

    std::pair<Index<Tag>, bool> insert(const Data<Tag>& value) { return insert_with_hash(hash(value), value); }

    const Data<Tag>& operator[](Index<Tag> idx) const noexcept { return (*m_storage)[uint_t(idx)]; }

    const Data<Tag>& front() const noexcept { return m_storage->front(); }

    std::size_t size() const noexcept { return m_storage->size(); }

private:
    class IndexableHash
    {
    private:
        const VectorType* m_storage;
        H m_hash;

    public:
        using is_transparent = void;

        IndexableHash() noexcept : m_storage(nullptr) {}
        explicit IndexableHash(const VectorType& storage) noexcept : m_storage(&storage) {}

        size_t operator()(Index<Tag> el) const noexcept { return m_hash((*m_storage)[uint_t(el)]); }
        size_t operator()(const Data<Tag>& el) const noexcept { return m_hash(el); }
    };

    class IndexableEqualTo
    {
    private:
        const VectorType* m_storage;
        E m_equal_to;

    public:
        using is_transparent = void;

        IndexableEqualTo() noexcept : m_storage(nullptr), m_equal_to() {}
        explicit IndexableEqualTo(const VectorType& storage) noexcept : m_storage(&storage), m_equal_to() {}

        bool operator()(Index<Tag> lhs, Index<Tag> rhs) const noexcept { return m_equal_to((*m_storage)[uint_t(lhs)], (*m_storage)[uint_t(rhs)]); }
        bool operator()(const Data<Tag>& lhs, Index<Tag> rhs) const noexcept { return m_equal_to(lhs, (*m_storage)[uint_t(rhs)]); }
        bool operator()(Index<Tag> lhs, const Data<Tag>& rhs) const noexcept { return m_equal_to((*m_storage)[uint_t(lhs)], rhs); }
        bool operator()(const Data<Tag>& lhs, const Data<Tag>& rhs) const noexcept { return m_equal_to(lhs, rhs); }
    };

    std::unique_ptr<VectorType> m_storage;
    gtl::flat_hash_set<Index<Tag>, IndexableHash, IndexableEqualTo> m_set;
};
}

#endif