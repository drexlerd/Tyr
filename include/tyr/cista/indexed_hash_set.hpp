/*
 * Copyright (C) 2023 Dominik Drexler and Simon Stahlberg
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

#ifndef TYR_CISTA_UNORDERED_INDEXED_HASH_SET_HPP_
#define TYR_CISTA_UNORDERED_INDEXED_HASH_SET_HPP_

#include "cista/serialization.h"
#include "tyr/cista/byte_buffer_segmented.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/observer_ptr.hpp"
#include "tyr/common/segmented_vector.hpp"

#include <functional>
#include <gtl/phmap.hpp>
#include <utility>
#include <vector>

namespace tyr::cista
{
template<typename T, typename H = Hash<ObserverPtr<const T>>, typename E = EqualTo<ObserverPtr<const T>>>
class IndexedHashSet
{
private:
    using IndexType = typename T::IndexType;

    // Persistent storage
    ByteBufferSegmented m_storage;

    // Deduplication
    gtl::flat_hash_set<ObserverPtr<const T>, H, E> m_set;

    // Randomized access
    SegmentedVector<const T*> m_vec;

public:
    explicit IndexedHashSet(size_t seg_size = 1024) : m_storage(seg_size), m_set(), m_vec() {}
    IndexedHashSet(const IndexedHashSet& other) = delete;
    IndexedHashSet& operator=(const IndexedHashSet& other) = delete;
    IndexedHashSet(IndexedHashSet&& other) = default;
    IndexedHashSet& operator=(IndexedHashSet&& other) = default;

    /**
     * Iterators
     */

    auto begin() const { return m_vec.begin(); }
    auto end() const { return m_vec.end(); }

    /**
     * Capacity
     */

    bool empty() const { return m_vec.empty(); }
    size_t size() const { return m_vec.size(); }

    /**
     * Modifiers
     */

    void clear()
    {
        m_storage.clear();
        m_set.clear();
        m_vec.clear();
    }

    template<::cista::mode Mode = ::cista::mode::NONE>
    auto insert(T& element, ::cista::buf<std::vector<uint8_t>>& buf)
    {
        // 1. Check if element already exists
        if (auto it = m_set.find(ObserverPtr<const T>(&element)); it != m_set.end())
        {
            return std::make_pair(it->get(), false);
        }

        // 2. Assign next index
        element.index = IndexType(m_set.size());

        // 3. Serialize
        buf.reset();
        ::cista::serialize<Mode>(buf, element);

        // 4. Write to storage
        auto begin = m_storage.write(buf.base(), buf.size());

        // 5. Insert to set
        auto [observer_ptr, success] = m_set.insert(::cista::deserialize<const T, Mode>(begin, begin + buf.size()));

        // 6. Insert to vec
        m_vec.push_back(observer_ptr->get());

        return std::make_pair(observer_ptr->get(), success);
    }

    /**
     * Lookup
     */

    size_t count(const T& key) const { return m_set.count(&key); }
    auto find(const T& key) const { return m_set.find(&key); }
    bool contains(const T& key) const { return m_set.contains(&key); }
    auto operator[](IndexType index) const { return m_vec[index.get()]; }
};

template<typename T, typename H = Hash<ObserverPtr<const T>>, typename E = EqualTo<ObserverPtr<const T>>>
using IndexedHashSetList = std::vector<IndexedHashSet<T, H, E>>;

}

#endif
