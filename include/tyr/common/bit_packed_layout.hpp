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

#ifndef TYR_BIT_PACKED_LAYOUT_HPP_
#define TYR_BIT_PACKED_LAYOUT_HPP_

#include <bit>  // std::bit_width, std::countr_zero
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>

namespace tyr
{

template<std::unsigned_integral Block>
struct DataPortion
{
    size_t word_offset;  // 0 for base, 1 for base+1
    Block mask;          // mask in data word
    uint8_t rshift;      // tzcount(mask)
};

template<std::unsigned_integral Block>
struct ValuePortion
{
    Block mask;      // mask in value
    uint8_t rshift;  // tzcount(mask)
};

template<std::unsigned_integral Block>
struct PortionMap
{
    DataPortion<Block> data;
    ValuePortion<Block> value;
};

template<std::unsigned_integral Block>
struct BitPackedElementLayout
{
    size_t base_word_index;
    PortionMap<Block> high;
    PortionMap<Block> low;
};

template<std::unsigned_integral Block>
using BitPackedElementLayoutList = std::vector<BitPackedElementLayout<Block>>;

template<std::unsigned_integral Block>
struct VariableReference
{
    const BitPackedElementLayout<Block>* layout;
    Block* data;

    static void assert_portion_ok(const PortionMap<Block>& p) noexcept
    {
        // Allow absent portion (used e.g. when bits==0 or when variable fits in one block).
        if (p.data.mask == 0 || p.value.mask == 0)
        {
            assert(p.data.mask == 0);
            assert(p.value.mask == 0);
            return;
        }

        assert(std::popcount(p.data.mask) == std::popcount(p.value.mask));

        [[maybe_unused]] constexpr int W = std::numeric_limits<Block>::digits;
        assert(p.data.rshift < W);
        assert(p.value.rshift < W);

        assert(((p.data.mask >> p.data.rshift) & Block { 1 }) != 0);
        assert(((p.value.mask >> p.value.rshift) & Block { 1 }) != 0);
    }

    static void assert_layout_ok(const BitPackedElementLayout<Block>& l) noexcept
    {
        assert_portion_ok(l.high);
        assert_portion_ok(l.low);

        // high/low must not overlap in the value
        assert((l.high.value.mask & l.low.value.mask) == 0);

        // high/low must not overlap in the data words they target
        // (if they target different words, overlap is impossible)
        if (l.high.data.word_offset == l.low.data.word_offset)
        {
            assert((l.high.data.mask & l.low.data.mask) == 0);
        }

        // optional sanity: both portions are within [base, base+1] for your current scheme
        assert(l.high.data.word_offset <= 1);
        assert(l.low.data.word_offset <= 1);
    }

    static Block read_portion(const PortionMap<Block>& p, const Block* data, size_t base) noexcept
    {
        const Block bits = (data[base + p.data.word_offset] & p.data.mask) >> p.data.rshift;
        return bits << p.value.rshift;  // place into value region
    }

    static void write_portion(const PortionMap<Block>& p, Block* data, size_t base, Block v) noexcept
    {
        Block& w = data[base + p.data.word_offset];
        const Block bits = (v & p.value.mask) >> p.value.rshift;
        const Block field = (bits << p.data.rshift) & p.data.mask;
        w = (w & ~p.data.mask) | field;
    }

    VariableReference& operator=(Block value) noexcept
    {
        assert_layout_ok(*layout);

        const size_t base = layout->base_word_index;
        write_portion(layout->high, data, base, value);
        write_portion(layout->low, data, base, value);

        return *this;
    }

    explicit operator Block() const noexcept
    {
        assert_layout_ok(*layout);

        const size_t base = layout->base_word_index;

        return read_portion(layout->high, data, base) | read_portion(layout->low, data, base);
    }

    VariableReference(const BitPackedElementLayout<Block>& layout, Block* data) : layout(&layout), data(data) { assert_layout_ok(layout); }
};

template<std::unsigned_integral Block>
struct BitPackedArrayLayout
{
    BitPackedElementLayoutList<Block> layouts;
    size_t total_bits;
    size_t total_blocks;
};

template<std::unsigned_integral Block>
BitPackedArrayLayout<Block> create_bit_packed_array_layout(const std::vector<Block>& ranges)
{
    constexpr size_t W = std::numeric_limits<Block>::digits;

    auto mask_n_bits = [W](size_t n) -> Block
    {
        if (n == 0)
            return Block { 0 };
        if (n >= W)
            return ~Block { 0 };
        return (Block { 1 } << n) - 1;
    };

    BitPackedElementLayoutList<Block> layouts;

    size_t word_index = 0;  // index in Block[] (or Block[] if Block==Block)
    size_t bit_pos = 0;     // next free bit in current block [0, W)

    for (const auto& range : ranges)
    {
        assert(range >= 1);

        // bits needed to represent values in [0, domain_size-1]
        const size_t bits = (range <= 1) ? 0u : static_cast<size_t>(std::bit_width(static_cast<size_t>(range - 1)));

        BitPackedElementLayout<Block> L;
        L.base_word_index = word_index;

        // Default: "absent" portions (mask==0)
        L.high = PortionMap<Block> { DataPortion<Block> { 0u, 0u, 0u }, ValuePortion<Block> { 0u, 0u } };
        L.low = PortionMap<Block> { DataPortion<Block> { 0u, 0u, 0u }, ValuePortion<Block> { 0u, 0u } };

        if (bits == 0)
        {
            // domain_size==1 => no bits stored; do not advance cursor
            layouts.push_back(L);
            continue;
        }

        if (bit_pos + bits <= W)
        {
            // Entire variable fits in current block -> store in "low"
            const Block word_mask_b = mask_n_bits(bits) << bit_pos;
            const Block value_mask_b = mask_n_bits(bits) << 0;

            L.low.data = DataPortion<Block> { 0u, word_mask_b, static_cast<uint8_t>(bit_pos) };
            L.low.value = ValuePortion<Block> { value_mask_b, 0u };

            bit_pos += bits;
            if (bit_pos == W)
            {
                bit_pos = 0;
                ++word_index;
            }
        }
        else
        {
            // Split across current and next block
            const size_t bits_in_b0 = W - bit_pos;
            const size_t bits_in_b1 = bits - bits_in_b0;

            assert(bits_in_b0 > 0);
            assert(bits_in_b1 > 0);
            assert(bits_in_b1 <= W);

            // Low bits go into tail of block0
            const Block b0_mask_b = mask_n_bits(bits_in_b0) << bit_pos;
            const Block v_low_mask_b = mask_n_bits(bits_in_b0) << 0;

            L.low.data = DataPortion<Block> { 0u, b0_mask_b, static_cast<uint8_t>(bit_pos) };
            L.low.value = ValuePortion<Block> { v_low_mask_b, 0u };

            // High bits go into head of block1 (starting at bit 0)
            const Block b1_mask_b = mask_n_bits(bits_in_b1) << 0;
            const Block v_high_mask_b = mask_n_bits(bits_in_b1) << bits_in_b0;

            L.high.data = DataPortion<Block> {
                1u,
                b1_mask_b,
                static_cast<uint8_t>(std::countr_zero(b1_mask_b))  // = 0
            };
            L.high.value = ValuePortion<Block> { v_high_mask_b, static_cast<uint8_t>(bits_in_b0) };

            // Advance cursor into next block
            ++word_index;
            bit_pos = bits_in_b1;

            if (bit_pos == W)
            {
                bit_pos = 0;
                ++word_index;
            }
        }

        layouts.push_back(L);
    }

    const size_t total_blocks = word_index + (bit_pos != 0 ? 1 : 0);

    return BitPackedArrayLayout<Block> { layouts, word_index * W + bit_pos, total_blocks };
}

template<std::unsigned_integral Block>
struct BitReference
{
    Block* data;
    size_t bit;

    BitReference(size_t bit, Block* data) noexcept : data(data), bit(bit) {}

    static constexpr size_t bits_per_block = std::numeric_limits<Block>::digits;

    static constexpr size_t block_index(size_t bit) { return bit / bits_per_block; }
    static constexpr size_t bit_index(size_t bit) { return bit % bits_per_block; }

    BitReference& operator=(bool value)
    {
        Block& block = data[block_index(bit)];
        const Block mask = Block(1) << bit_index(bit);

        if (value)
            block |= mask;
        else
            block &= ~mask;

        return *this;
    }

    BitReference& operator=(const BitReference& other) noexcept { return *this = static_cast<bool>(other); }

    explicit operator bool() const noexcept { return ((data[block_index(bit)] >> bit_index(bit)) & Block(1)) != 0; }
};

template<std::unsigned_integral Block>
struct BitsetLayout
{
    size_t total_bits;
    size_t total_blocks;
};

template<std::unsigned_integral Block>
BitsetLayout<Block> create_bitset_layout(size_t num_bits)
{
    constexpr size_t W = std::numeric_limits<Block>::digits;
    const size_t num_blocks = (num_bits + W - 1) / W;
    return BitsetLayout<Block>(num_bits, num_blocks);
}
}

#endif
