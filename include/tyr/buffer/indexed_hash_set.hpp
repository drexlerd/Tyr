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

#ifndef TYR_BUFFER_INDEXED_HASH_SET_HPP_
#define TYR_BUFFER_INDEXED_HASH_SET_HPP_

#include "cista/serialization.h"
#include "tyr/buffer/declarations.hpp"
#include "tyr/buffer/segmented_buffer.hpp"
#include "tyr/common/config.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/segmented_vector.hpp"
#include "tyr/common/types.hpp"

#include <functional>
#include <gtl/phmap.hpp>
#include <optional>
#include <utility>
#include <vector>

namespace tyr::buffer
{
template<typename Tag, typename H = Hash<Data<Tag>>, typename E = EqualTo<Data<Tag>>>
class IndexedHashSet
{
private:
    struct IndexableHash;
    struct IndexableEqualTo;

    using VectorType = std::vector<const Data<Tag>*>;

public:
    IndexedHashSet() = default;
    IndexedHashSet(::cista::buf<std::vector<uint8_t>>& buf, SegmentedBuffer& arena) :
        m_storage(std::make_unique<VectorType>()),
        m_set(0, IndexableHash(*m_storage), IndexableEqualTo(*m_storage)),
        m_buf(&buf),
        m_arena(&arena)
    {
    }
    IndexedHashSet(const IndexedHashSet& other) = delete;
    IndexedHashSet& operator=(const IndexedHashSet& other) = delete;
    IndexedHashSet(IndexedHashSet&& other) = default;
    IndexedHashSet& operator=(IndexedHashSet&& other) = default;

    /**
     * Capacity
     */

    bool empty() const noexcept { return m_storage->empty(); }
    size_t size() const noexcept { return m_storage->size(); }

    /**
     * Modifiers
     */

    void clear()
    {
        m_set.clear();
        m_storage->clear();
    }

    size_t hash(const Data<Tag>& element) const noexcept { return m_set.hash(element); }

    std::optional<Index<Tag>> find_with_hash(const Data<Tag>& element, size_t h) const noexcept
    {
        assert(is_canonical(element) && "The given element is not canonical. Did you forget to call canonicalize?");
        assert(h == hash(element) && "The given hash does not match container internal's hash.");

        const auto it = m_set.find(element, h);
        if (it != m_set.end())
            return *it;

        return std::nullopt;
    }

    std::optional<Index<Tag>> find(const Data<Tag>& element) const noexcept
    {
        assert(is_canonical(element));

        return find_with_hash(element, hash(element));
    }

    template<::cista::mode Mode = CISTA_MODE>
    std::pair<Index<Tag>, bool> insert_with_hash(size_t h, const Data<Tag>& element)
    {
        assert(is_canonical(element) && "The given element is not canonical. Did you forget to call canonicalize?");
        assert(h == hash(element) && "The given hash does not match container internal's hash.");

        // 1. Check if element already exists

        auto it = m_set.find(element, h);
        if (it != m_set.end())
            return std::make_pair(*it, false);

        // 2. Serialize
        m_buf->reset();
        ::cista::serialize<Mode>(*m_buf, element);

        // 3. Write to storage
        auto begin = m_arena->write(m_buf->base(), m_buf->size(), alignof(Data<Tag>));

        const auto index = Index<Tag>(static_cast<uint_t>(m_storage->size()));

        // 4. Insert into vec
        const auto serialized_element = ::cista::deserialize<const Data<Tag>, Mode>(begin, begin + m_buf->size());
        m_storage->push_back(serialized_element);

        // 5. Insert into set
        [[maybe_unused]] auto [it2, inserted] = m_set.emplace_with_hash(h, index);
        assert(inserted);

        return std::make_pair(index, true);
    }

    // const T* always points to a valid instantiation of the class.
    // We return const T* here to avoid bugs when using structured bindings.
    template<::cista::mode Mode = CISTA_MODE>
    std::pair<Index<Tag>, bool> insert(const Data<Tag>& element)
    {
        assert(is_canonical(element));

        return insert_with_hash<Mode>(hash(element), element);
    }

    /**
     * Lookup
     */

    const Data<Tag>& operator[](Index<Tag> index) const noexcept
    {
        assert(index.get_value() < m_storage->size());
        return *(*m_storage)[index.get_value()];
    }

    const Data<Tag>& front() const
    {
        assert(!m_storage->empty());
        return *m_storage->front();
    }

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

        size_t operator()(Index<Tag> el) const noexcept { return m_hash(*(*m_storage)[uint_t(el)]); }
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

        bool operator()(Index<Tag> lhs, Index<Tag> rhs) const noexcept { return m_equal_to(*(*m_storage)[uint_t(lhs)], *(*m_storage)[uint_t(rhs)]); }
        bool operator()(const Data<Tag>& lhs, Index<Tag> rhs) const noexcept { return m_equal_to(lhs, *(*m_storage)[uint_t(rhs)]); }
        bool operator()(Index<Tag> lhs, const Data<Tag>& rhs) const noexcept { return m_equal_to(*(*m_storage)[uint_t(lhs)], rhs); }
        bool operator()(const Data<Tag>& lhs, const Data<Tag>& rhs) const noexcept { return m_equal_to(lhs, rhs); }
    };

    std::unique_ptr<VectorType> m_storage;
    gtl::flat_hash_set<Index<Tag>, IndexableHash, IndexableEqualTo> m_set;

    ::cista::buf<std::vector<uint8_t>>* m_buf;
    SegmentedBuffer* m_arena;
};

}

#endif
