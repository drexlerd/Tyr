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

#ifndef TYR_COMMON_BIT_PACKED_ARRAY_POOL_HPP_
#define TYR_COMMON_BIT_PACKED_ARRAY_POOL_HPP_

#include "tyr/common/bit.hpp"

#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>
#include <utility>
#include <vector>

namespace tyr
{

/// Stores fixed-length arrays as bit-packed unsigned integer codes with stable references.
/// Values are encoded and decoded via Coder.
template<std::unsigned_integral Block, typename Coder = bit::ForwardingBlockCoder<Block>, size_t FirstSegmentSize = 1>
    requires bit::BlockCoder<Coder, Block>
class BitPackedArrayPool
{
    static_assert(bit::is_power_of_two(FirstSegmentSize));

public:
    /**
     * Type aliases
     */

    template<typename Block_>
        requires std::same_as<std::remove_const_t<Block_>, Block>
    class BasicArrayView;

    using value_type = typename Coder::value_type;
    using reference_type = typename bit::int_reference<Block, Coder>;
    using ArrayView = BasicArrayView<Block>;
    using ConstArrayView = BasicArrayView<const Block>;

    /**
     * Compile-time properties
     */

private:
    /**
     * Type aliases
     */

    /**
     * Compile-time properties
     */

    static constexpr std::size_t digits = std::numeric_limits<Block>::digits;
    static constexpr size_t block_shift = std::countr_zero(digits);

    static constexpr size_t seg_shift = std::countr_zero(FirstSegmentSize);
    static constexpr size_t seg_mask = FirstSegmentSize - 1;

    static size_t get_segment_index(size_t index) noexcept { return std::bit_width((index >> seg_shift) + 1) - 1; }
    static size_t get_segment_pos(size_t index) noexcept
    {
        const size_t q = index >> seg_shift;
        const size_t r = index & seg_mask;
        const size_t seg_idx = get_segment_index(index);
        return ((q - ((size_t { 1 } << seg_idx) - 1)) << seg_shift) + r;
    }

    static constexpr size_t blocks_for_bits(size_t bits) noexcept { return (bits + digits - 1) >> block_shift; }

    void reserve(size_t size)
    {
        if (size == 0 || size <= m_capacity)
            return;

        const size_t last_segment = get_segment_index(size - 1);
        const size_t first_new_segment = m_segments.size();

        m_segments.resize(last_segment + 1);

        const size_t bits_per_array = m_length * static_cast<size_t>(m_width);

        for (size_t seg = first_new_segment; seg <= last_segment; ++seg)
        {
            const size_t arrays_in_segment = FirstSegmentSize << seg;  // geometric growth
            assert(bit::is_power_of_two(arrays_in_segment));

            const size_t bits_in_segment = arrays_in_segment * bits_per_array;
            const size_t blocks_in_segment = blocks_for_bits(bits_in_segment);

            m_segments[seg].resize(blocks_in_segment, Block { 0 });
            m_capacity += arrays_in_segment;
        }
    }

    void ensure_fits(std::span<const value_type> elements) const
    {
        if (elements.size() != m_length)
            throw std::invalid_argument("BitPackedArrayPool: wrong number of elements.");
    }

public:
    /**
     * Iterator declarations
     */

    template<typename Pool>
    class BasicIterator;

    using iterator = BasicIterator<BitPackedArrayPool>;
    using const_iterator = BasicIterator<const BitPackedArrayPool>;

    /**
     * Constructors
     */

    explicit BitPackedArrayPool(size_t length, uint8_t width) : m_length(length), m_width(width), m_capacity(0), m_size(0) {}

    /**
     * Iterator definitions
     */

    template<typename Pool>
    class BasicIterator
    {
    private:
        using pool_type = Pool;
        using pool_pointer = Pool*;

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = ConstArrayView;
        using reference = std::conditional_t<std::is_const_v<pool_type>, ConstArrayView, ArrayView>;
        using iterator_category = std::bidirectional_iterator_tag;
        using iterator_concept = std::bidirectional_iterator_tag;

        BasicIterator() : m_pool(nullptr), m_pos(0) {}
        BasicIterator(pool_type& pool, size_t pos) : m_pool(&pool), m_pos(pos) {}

        reference operator*() const { return (*m_pool)[m_pos]; }

        BasicIterator& operator++()
        {
            ++m_pos;
            return *this;
        }

        BasicIterator operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        BasicIterator& operator--()
        {
            --m_pos;
            return *this;
        }

        BasicIterator operator--(int)
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        friend bool operator==(const BasicIterator&, const BasicIterator&) = default;

    private:
        pool_pointer m_pool;
        size_t m_pos;
    };

    /**
     * Accessors
     */

    ArrayView operator[](size_t index) noexcept
    {
        assert(index < m_size);

        const size_t seg_idx = get_segment_index(index);
        const size_t seg_pos = get_segment_pos(index);
        const size_t start_bit = seg_pos * m_width * m_length;

        auto* data = m_segments[seg_idx].data() + (start_bit >> block_shift);
        const uint8_t offset = static_cast<uint8_t>(start_bit & (digits - 1));

        return ArrayView(data, m_length, m_width, offset);
    }

    ConstArrayView operator[](size_t index) const noexcept
    {
        assert(index < m_size);

        const size_t seg_idx = get_segment_index(index);
        const size_t seg_pos = get_segment_pos(index);
        const size_t start_bit = seg_pos * m_width * m_length;

        const auto* data = m_segments[seg_idx].data() + (start_bit >> block_shift);
        const uint8_t offset = static_cast<uint8_t>(start_bit & (digits - 1));

        return ConstArrayView(data, m_length, m_width, offset);
    }

    /**
     * Iterators
     */

    iterator begin() noexcept { return iterator(*this, 0); }

    iterator end() noexcept { return iterator(*this, size()); }

    const_iterator begin() const noexcept { return const_iterator(*this, 0); }

    const_iterator end() const noexcept { return const_iterator(*this, size()); }

    const_iterator cbegin() const noexcept { return const_iterator(*this, 0); }

    const_iterator cend() const noexcept { return const_iterator(*this, size()); }

    /**
     * Modifiers
     */

    void push_back(std::span<const value_type> elements)
    {
        ensure_fits(elements);

        const size_t index = m_size;
        reserve(m_size + 1);
        ++m_size;
        auto view = (*this)[index];
        for (size_t i = 0; i < m_length; ++i)
            view[i] = elements[i];
    }

    void clear() noexcept { m_size = 0; }

    /**
     * Capacity
     */

    size_t length() const noexcept { return m_length; }
    uint8_t width() const noexcept { return m_width; }
    size_t capacity() const noexcept { return m_capacity; }
    size_t size() const noexcept { return m_size; }
    bool empty() const noexcept { return m_size == 0; }
    const auto& segments() const noexcept { return m_segments; }

    /**
     * BasicArrayView
     */

    template<typename Block_>
        requires std::same_as<std::remove_const_t<Block_>, Block>
    class BasicArrayView
    {
    private:
        void ensure_fits(std::span<const value_type> elements) const
        {
            if (elements.size() != m_length)
                throw std::invalid_argument("BasicArrayView: wrong number of elements.");
        }

    public:
        /**
         * Compile-time properties
         */

        /**
         * Iterator declarations
         */

        template<typename View>
        class BasicIterator;

        using iterator = BasicIterator<BasicArrayView<Block_>>;
        using const_iterator = BasicIterator<const BasicArrayView<Block_>>;

        /**
         * Constructors
         */

        BasicArrayView(Block_* data, size_t length, uint8_t width, uint8_t offset) : m_data(data), m_length(length), m_width(width), m_offset(offset) {}

        BasicArrayView& operator=(std::span<const value_type> elements)
        {
            assert(elements.size() == m_length);

            ensure_fits(elements);

            for (size_t i = 0; i < m_length; ++i)
                (*this)[i] = Coder::encode(elements[i]);

            return *this;
        }

        /**
         * Operators
         */

        friend bool operator==(const BasicArrayView& lhs, const BasicArrayView& rhs)
        {
            if (lhs.size() != rhs.size())
                return false;

            for (size_t i = 0; i < lhs.size(); ++i)
                if (lhs[i] != rhs[i])
                    return false;

            return true;
        }

        friend bool operator==(const BasicArrayView& lhs, std::span<const value_type> rhs)
        {
            if (lhs.size() != rhs.size())
                return false;

            for (size_t i = 0; i < lhs.size(); ++i)
                if (lhs[i] != rhs[i])
                    return false;

            return true;
        }

        friend bool operator==(std::span<const value_type> lhs, const BasicArrayView& rhs) { return rhs == lhs; }

        /**
         * Iterator definitions
         */

        template<typename View>
        class BasicIterator
        {
        private:
            using view_type = View;
            using view_pointer = View*;

        public:
            using difference_type = std::ptrdiff_t;
            using value_type = typename Coder::value_type;
            using reference = std::conditional_t<std::is_const_v<view_type>, value_type, reference_type>;
            using iterator_category = std::bidirectional_iterator_tag;
            using iterator_concept = std::bidirectional_iterator_tag;

            BasicIterator() : m_view(nullptr), m_pos(0) {}
            BasicIterator(view_type& view, size_t pos) : m_view(&view), m_pos(pos) {}

            reference operator*() const { return (*m_view)[m_pos]; }

            BasicIterator& operator++()
            {
                ++m_pos;
                return *this;
            }

            BasicIterator operator++(int)
            {
                auto tmp = *this;
                ++(*this);
                return tmp;
            }

            BasicIterator& operator--()
            {
                --m_pos;
                return *this;
            }

            BasicIterator operator--(int)
            {
                auto tmp = *this;
                --(*this);
                return tmp;
            }

            friend bool operator==(const BasicIterator&, const BasicIterator&) = default;

        private:
            view_pointer m_view;
            size_t m_pos;
        };

        /**
         * Accessors
         */

        reference_type operator[](size_t pos) noexcept
            requires(!std::is_const_v<Block_>)
        {
            assert(pos < m_length);

            const size_t bit_index = static_cast<size_t>(m_offset) + pos * m_width;
            auto* word = m_data + (bit_index >> block_shift);
            const uint8_t offset = static_cast<uint8_t>(bit_index & (digits - 1));

            return reference_type(word, offset, m_width);
        }

        value_type operator[](size_t pos) const noexcept
        {
            assert(pos < m_length);

            const size_t bit_index = static_cast<size_t>(m_offset) + pos * m_width;
            const auto* word = m_data + (bit_index >> block_shift);
            const uint8_t offset = static_cast<uint8_t>(bit_index & (digits - 1));

            return Coder::decode(bit::read_int<Block>(word, offset, m_width));
        }

        /**
         * Iterators
         */

        iterator begin() noexcept
            requires(!std::is_const_v<Block_>)
        {
            return iterator(*this, 0);
        }
        iterator end() noexcept
            requires(!std::is_const_v<Block_>)
        {
            return iterator(*this, size());
        }
        const_iterator begin() const noexcept { return const_iterator(*this, 0); }
        const_iterator end() const noexcept { return const_iterator(*this, size()); }
        const_iterator cbegin() const noexcept { return const_iterator(*this, 0); }
        const_iterator cend() const noexcept { return const_iterator(*this, size()); }

        /**
         * Capacity
         */

        size_t size() const noexcept { return m_length; }
        uint8_t width() const noexcept { return m_width; }

    private:
        Block_* m_data;
        size_t m_length;
        uint8_t m_width;
        uint8_t m_offset;
    };

private:
    // Segments grow geometrically, i.e., FirstSegmentSize, 2*FirstSegmentSize, 4*FirstSegmentSize, ...
    std::vector<std::vector<Block>> m_segments;

    size_t m_length;
    uint8_t m_width;

    size_t m_capacity;
    size_t m_size;
};
}

#endif
