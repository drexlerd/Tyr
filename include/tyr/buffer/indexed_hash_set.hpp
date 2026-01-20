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
#include "tyr/common/observer_ptr.hpp"
#include "tyr/common/segmented_vector.hpp"
#include "tyr/common/types.hpp"

#include <functional>
#include <gtl/phmap.hpp>
#include <utility>
#include <vector>

namespace tyr::buffer
{
template<typename Tag, typename H, typename E>
class IndexedHashSet
{
private:
    // Persistent storage
    SegmentedBuffer m_storage;

    // Deduplication
    gtl::flat_hash_set<ObserverPtr<const Data<Tag>>, H, E> m_set;

    // Randomized access
    std::vector<const Data<Tag>*> m_vec;

public:
    explicit IndexedHashSet(size_t seg_size = 1024) : m_storage(seg_size), m_set(), m_vec() {}
    IndexedHashSet(const IndexedHashSet& other) = delete;
    IndexedHashSet& operator=(const IndexedHashSet& other) = delete;
    IndexedHashSet(IndexedHashSet&& other) = default;
    IndexedHashSet& operator=(IndexedHashSet&& other) = default;

    /**
     * Iterators
     */

    auto begin() const noexcept { return m_vec.begin(); }
    auto end() const noexcept { return m_vec.end(); }

    /**
     * Capacity
     */

    bool empty() const noexcept { return m_vec.empty(); }
    size_t size() const noexcept { return m_vec.size(); }

    /**
     * Modifiers
     */

    void clear()
    {
        m_storage.clear();
        m_set.clear();
        m_vec.clear();
    }

    const Data<Tag>* find(const Data<Tag>& element) const noexcept
    {
        assert(is_canonical(element));

        if (auto it = m_set.find(ObserverPtr<const Data<Tag>>(&element)); it != m_set.end())
            return it->get();

        return nullptr;
    }

    // const T* always points to a valid instantiation of the class.
    // We return const T* here to avoid bugs when using structured bindings.
    template<::cista::mode Mode = CISTA_MODE>
    std::pair<const Data<Tag>*, bool> insert(const Data<Tag>& element, ::cista::buf<std::vector<uint8_t>>& buf)
    {
        assert(is_canonical(element));

        // 1. Check if element already exists
        if (auto it = m_set.find(ObserverPtr<const Data<Tag>>(&element)); it != m_set.end())
            return std::make_pair(it->get(), false);

        // 2. Serialize
        buf.reset();
        ::cista::serialize<Mode>(buf, element);

        // 3. Write to storage
        auto begin = m_storage.write(buf.base(), buf.size(), alignof(Data<Tag>));

        // 4. Insert to set
        auto [observer_ptr, success] = m_set.insert(::cista::deserialize<const Data<Tag>, Mode>(begin, begin + buf.size()));

        // 5. Insert to vec
        m_vec.push_back(observer_ptr->get());

        return std::make_pair(observer_ptr->get(), success);
    }

    /**
     * Lookup
     */

    const Data<Tag>& operator[](Index<Tag> index) const noexcept
    {
        assert(index.get_value() < m_vec.size());
        return *m_vec[index.get_value()];
    }

    const Data<Tag>& front() const
    {
        assert(!m_vec.empty());
        return *m_vec.front();
    }
};

}

#endif
