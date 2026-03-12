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
#include <utility>
#include <vector>

namespace tyr
{

/// Stores fixed-length arrays as bit-packed unsigned integer codes.
/// The exposed value type is obtained via Encoder::decode.
template<std::unsigned_integral Block, typename Coder = bit::ForwardingBlockCoder<Block>, size_t FirstSegmentSize = 1>
    requires bit::BlockCoder<Coder, Block>
class BitPackedArrayPool
{
    static_assert(bit::is_power_of_two(FirstSegmentSize));

private:
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

    void resize_to_fit(size_t size)
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

public:
    template<typename Block_>
        requires std::same_as<std::remove_const_t<Block_>, Block>
    class BasicArrayView;

    using value_type = typename Coder::value_type;
    using reference_type = typename bit::int_reference<Block, Coder>;
    using ArrayView = BasicArrayView<Block>;
    using ConstArrayView = BasicArrayView<const Block>;

    explicit BitPackedArrayPool(size_t length, uint8_t width) : m_length(length), m_width(width), m_capacity(0), m_size(0) {}

    ArrayView operator[](size_t index) noexcept
    {
        const size_t seg_idx = get_segment_index(index);
        const size_t seg_pos = get_segment_pos(index);
        const size_t start_bit = seg_pos * m_width * m_length;

        auto* data = m_segments[seg_idx].data() + (start_bit >> block_shift);
        const uint8_t offset = static_cast<uint8_t>(start_bit & (digits - 1));

        return ArrayView(data, m_length, m_width, offset);
    }

    ConstArrayView operator[](size_t index) const noexcept
    {
        const size_t seg_idx = get_segment_index(index);
        const size_t seg_pos = get_segment_pos(index);
        const size_t start_bit = seg_pos * m_width * m_length;

        const auto* data = m_segments[seg_idx].data() + (start_bit >> block_shift);
        const uint8_t offset = static_cast<uint8_t>(start_bit & (digits - 1));

        return ConstArrayView(data, m_length, m_width, offset);
    }

    void push_back(std::span<const value_type> elements)
    {
        assert(elements.size() == m_length);

        const size_t index = m_size;
        resize_to_fit(m_size + 1);
        auto view = (*this)[index];
        for (size_t i = 0; i < m_length; ++i)
            view[i] = elements[i];

        ++m_size;
    }

    void clear() noexcept { m_size = 0; }

    size_t length() const noexcept { return m_length; }
    uint8_t width() const noexcept { return m_width; }
    size_t capacity() const noexcept { return m_capacity; }
    size_t size() const noexcept { return m_size; }

    template<typename Block_>
        requires std::same_as<std::remove_const_t<Block_>, Block>
    class BasicArrayView
    {
    public:
        template<typename Block__>
            requires std::same_as<std::remove_const_t<Block__>, Block>
        class BasicIterator;

        using iterator = BasicIterator<Block_>;
        using const_iterator = BasicIterator<const Block_>;

        BasicArrayView(Block_* data, size_t length, uint8_t width, uint8_t offset) : m_data(data), m_length(length), m_width(width), m_offset(offset) {}

        template<typename Block__>
            requires std::same_as<std::remove_const_t<Block__>, Block>
        class BasicIterator
        {
        public:
        private:
        };

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

        iterator begin() noexcept
            requires(!std::is_const_v<Block_>)
        {
        }
        iterator end() noexcept
            requires(!std::is_const_v<Block_>)
        {
        }
        const_iterator begin() const noexcept {}
        const_iterator end() const noexcept {}
        const_iterator cbegin() const noexcept {}
        const_iterator cend() const noexcept {}

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
