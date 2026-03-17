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

#ifndef TYR_COMMON_BIT_PACKED_ARRAY_SET_HPP_
#define TYR_COMMON_BIT_PACKED_ARRAY_SET_HPP_

#include "tyr/common/bit_packed_array_pool.hpp"
#include "tyr/common/declarations.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"

#include <algorithm>
#include <concepts>
#include <optional>
#include <ranges>

namespace tyr
{
template<std::unsigned_integral Block, typename Coder = bit::ForwardingBlockCoder<Block>, size_t FirstSegmentSize = 16>
class BitPackedArraySet
{
private:
    using pool_type = BitPackedArrayPool<Block, Coder, FirstSegmentSize>;

public:
    using value_type = typename pool_type::value_type;
    using index_type = uint_t;
    using ConstArrayView = typename pool_type::ConstArrayView;

private:
    std::unique_ptr<pool_type> m_pool;

    class IndexableHash
    {
    private:
        const pool_type* pool = nullptr;

        template<std::ranges::input_range Range>
        size_t hash_range(const Range& el) const noexcept
        {
            size_t aggregated_hash = 0;
            for (const auto& item : el)
                hash_combine(aggregated_hash, item);
            return aggregated_hash;
        }

    public:
        using is_transparent = void;

        IndexableHash() noexcept = default;
        explicit IndexableHash(const pool_type& pool_) noexcept : pool(&pool_) {}

        size_t operator()(index_type idx) const noexcept { return hash_range((*pool)[idx]); }

        size_t operator()(std::span<const value_type> values) const noexcept { return hash_range(values); }
    };

    class IndexableEqualTo
    {
    private:
        const pool_type* pool = nullptr;

        template<std::ranges::input_range Range1, std::ranges::input_range Range2>
        bool equal_ranges(const Range1& lhs, const Range2& rhs) const noexcept
        {
            return std::ranges::equal(lhs, rhs);
        }

    public:
        using is_transparent = void;

        IndexableEqualTo() noexcept = default;
        explicit IndexableEqualTo(const pool_type& pool_) noexcept : pool(&pool_) {}

        bool operator()(index_type lhs, index_type rhs) const noexcept { return equal_ranges((*pool)[lhs], (*pool)[rhs]); }

        bool operator()(std::span<const value_type> lhs, index_type rhs) const noexcept { return equal_ranges((*pool)[rhs], lhs); }

        bool operator()(index_type lhs, std::span<const value_type> rhs) const noexcept { return equal_ranges((*pool)[lhs], rhs); }
    };

    gtl::flat_hash_set<index_type, IndexableHash, IndexableEqualTo> m_set;

public:
    BitPackedArraySet(size_t length, uint8_t width) :
        m_pool(std::make_unique<pool_type>(length, width)),
        m_set(0, IndexableHash(*m_pool), IndexableEqualTo(*m_pool))
    {
    }

    void clear() noexcept
    {
        m_set.clear();
        m_pool->clear();
    }

    size_t hash(std::span<const value_type> elements) const noexcept { return m_set.hash(elements); }

    std::optional<index_type> find_with_hash(std::span<const value_type> elements, size_t h) const
    {
        const auto it = m_set.find(elements, h);
        if (it != m_set.end())
            return *it;

        return std::nullopt;
    }

    std::optional<index_type> find(std::span<const value_type> elements) const { return find_with_hash(elements, hash(elements)); }

    std::pair<index_type, bool> insert_with_hash(size_t h, std::span<const value_type> elements)
    {
        if (const auto it = m_set.find(elements, h); it != m_set.end())
            return { *it, false };

        const auto index = static_cast<index_type>(m_pool->size());
        m_pool->push_back(elements);

        [[maybe_unused]] const auto [it, inserted] = m_set.emplace_with_hash(h, index);
        assert(inserted);

        return { index, true };
    }

    std::pair<index_type, bool> insert(std::span<const value_type> elements) { return insert_with_hash(hash(elements), elements); }

    bool contains(std::span<const value_type> elements) const { return m_set.contains(elements); }

    ConstArrayView operator[](index_type index) const { return std::as_const(*m_pool)[index]; }

    size_t size() const noexcept { return m_pool->size(); }
    size_t capacity() const noexcept { return m_pool->capacity(); }
    bool empty() const noexcept { return m_pool->empty(); }
    size_t length() const noexcept { return m_pool->length(); }
    uint8_t width() const noexcept { return m_pool->width(); }
    const auto& segments() const noexcept { return m_pool->segments(); }
};
}

#endif
