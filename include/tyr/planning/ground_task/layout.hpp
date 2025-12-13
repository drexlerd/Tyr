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

#ifndef TYR_PLANNING_GROUND_TASK_LAYOUT_HPP_
#define TYR_PLANNING_GROUND_TASK_LAYOUT_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/planning/fdr_fact_data.hpp"
#include "tyr/formalism/planning/fdr_value.hpp"
#include "tyr/formalism/planning/fdr_variable_view.hpp"

#include <bit>  // std::bit_width, std::countr_zero
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>

namespace tyr::planning
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

template<formalism::FactKind T, std::unsigned_integral Block>
struct VariableLayout
{
    Index<formalism::FDRVariable<T>> variable;
    size_t base_word_index;
    PortionMap<Block> high;
    PortionMap<Block> low;
};

template<formalism::FactKind T, std::unsigned_integral Block>
using VariableLayoutList = std::vector<VariableLayout<T, Block>>;

template<formalism::FactKind T, std::unsigned_integral Block>
struct VariableReference
{
    const VariableLayout<T, Block>* layout;
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

        constexpr int W = std::numeric_limits<Block>::digits;
        assert(p.data.rshift < W);
        assert(p.value.rshift < W);

        assert(((p.data.mask >> p.data.rshift) & Block { 1 }) != 0);
        assert(((p.value.mask >> p.value.rshift) & Block { 1 }) != 0);
    }

    static void assert_layout_ok(const VariableLayout<T, Block>& l) noexcept
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

    VariableReference& operator=(Data<formalism::FDRFact<T>> fact) noexcept
    {
        assert_layout_ok(*layout);
        assert(fact.variable == layout->variable);

        const Block v = static_cast<Block>(fact.value);  // adapt if needed

        const size_t base = layout->base_word_index;
        write_portion(layout->high, data, base, v);
        write_portion(layout->low, data, base, v);

        return *this;
    }

    VariableReference& operator=(formalism::FDRValue value) noexcept
    {
        assert_layout_ok(*layout);

        const Block v = static_cast<Block>(uint_t(value));

        const size_t base = layout->base_word_index;
        write_portion(layout->high, data, base, v);
        write_portion(layout->low, data, base, v);

        return *this;
    }

    explicit operator Data<formalism::FDRFact<T>>() const noexcept
    {
        assert_layout_ok(*layout);

        const size_t base = layout->base_word_index;

        const Block v = read_portion(layout->high, data, base) | read_portion(layout->low, data, base);

        return Data<formalism::FDRFact<T>>(layout->variable, formalism::FDRValue(v));
    }

    VariableReference(const VariableLayout<T, Block>& layout, Block* data) : layout(&layout), data(data) { assert_layout_ok(layout); }
};

template<formalism::FactKind T, formalism::Context C, std::unsigned_integral Block>
std::pair<VariableLayoutList<T, Block>, size_t> create_layouts(View<IndexList<formalism::FDRVariable<T>>, C> variables)
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

    VariableLayoutList<T, Block> layouts;

    size_t word_index = 0;  // index in Block[] (or Block[] if Block==Block)
    size_t bit_pos = 0;     // next free bit in current block [0, W)

    for (const auto& variable : variables)
    {
        const auto index = variable.get_index();
        const size_t domain_size = static_cast<size_t>(variable.get_domain_size());
        assert(domain_size >= 1);

        // bits needed to represent values in [0, domain_size-1]
        const size_t bits = (domain_size <= 1) ? 0u : static_cast<size_t>(std::bit_width(domain_size - 1));

        VariableLayout<T, Block> L;
        L.variable = index;
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

    return { layouts, total_blocks };
}

}

#endif
