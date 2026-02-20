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

#ifndef TYR_COMMON_DYNAMIC_BITSET_HPP_
#define TYR_COMMON_DYNAMIC_BITSET_HPP_

#include <boost/dynamic_bitset.hpp>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <limits>
#include <span>

namespace tyr
{
inline bool test(size_t pos, const boost::dynamic_bitset<>& bitset) noexcept
{
    if (pos >= bitset.size())
        return false;
    return bitset.test(pos);
}

inline void set(size_t pos, bool value, boost::dynamic_bitset<>& bitset)
{
    if (pos >= bitset.size())
        bitset.resize(pos + 1, false);
    bitset[pos] = value;
}

template<std::unsigned_integral Block>
class BitsetSpan
{
private:
    template<std::unsigned_integral>
    friend class BitsetSpan;

public:
    using U = std::remove_const_t<Block>;

    static constexpr size_t Digits = std::numeric_limits<U>::digits;
    static constexpr size_t BlockShift = std::countr_zero(Digits);
    static constexpr size_t BlockMask = Digits - 1;

    static constexpr size_t block_index(size_t pos) noexcept { return pos >> BlockShift; }
    static constexpr size_t block_pos(size_t pos) noexcept { return pos & BlockMask; }
    static constexpr size_t num_blocks(size_t num_bits) noexcept { return (num_bits + Digits - 1) >> BlockShift; }

    static constexpr size_t npos = std::numeric_limits<size_t>::max();

    static constexpr U full_mask() noexcept { return ~U { 0 }; }

    static constexpr U last_mask(size_t num_bits) noexcept
    {
        const size_t r = num_bits % Digits;
        return (r == 0) ? full_mask() : (U { 1 } << r) - U { 1 };
    }

public:
    BitsetSpan(Block* data, size_t num_bits) noexcept : m_data(data), m_num_bits(num_bits) {}

    /**
     * Helpers
     */

    void clear_trailing_bits() noexcept
        requires(!std::is_const_v<Block>)
    {
        const size_t n = num_blocks(m_num_bits);
        if (n == 0)
            return;
        m_data[n - 1] &= last_mask(m_num_bits);
    }

    bool trailing_bits_zero() const noexcept
    {
        const size_t n = num_blocks(m_num_bits);
        if (n == 0)
            return true;

        const U mask = last_mask(m_num_bits);
        return (m_data[n - 1] & ~mask) == U { 0 };
    }

    /**
     * Accessors
     */

    bool test(size_t pos) const noexcept
    {
        assert(pos < m_num_bits);

        return (m_data[block_index(pos)] & (U { 1 } << block_pos(pos))) != U { 0 };
    }

    size_t count() const noexcept
    {
        assert(trailing_bits_zero());

        const size_t n = num_blocks(m_num_bits);
        size_t cnt = 0;

        for (size_t i = 0; i < n; ++i)
            cnt += std::popcount(m_data[i]);

        return cnt;
    }

    size_t count_zeros() const noexcept
    {
        assert(trailing_bits_zero());

        return m_num_bits - count();
    }

    bool any() const noexcept
    {
        assert(trailing_bits_zero());

        const size_t n = num_blocks(m_num_bits);
        for (size_t i = 0; i < n; ++i)
            if (m_data[i] != U { 0 })
                return true;
        return false;
    }

    /**
     * Modifiers
     */

    void set(size_t pos) noexcept
        requires(!std::is_const_v<Block>)
    {
        assert(pos < m_num_bits);

        m_data[block_index(pos)] |= (U { 1 } << block_pos(pos));
    }

    void set() noexcept
        requires(!std::is_const_v<Block>)
    {
        const size_t n = num_blocks(m_num_bits);
        std::fill(m_data, m_data + n, full_mask());

        clear_trailing_bits();
    }

    void reset(size_t pos) noexcept
        requires(!std::is_const_v<Block>)
    {
        assert(pos < m_num_bits);

        m_data[block_index(pos)] &= (~(U { 1 } << block_pos(pos)));
    }

    void reset() noexcept
        requires(!std::is_const_v<Block>)
    {
        const size_t n = num_blocks(m_num_bits);
        std::fill(m_data, m_data + n, U { 0 });
    }

    /**
     * Iterators
     */

    size_t find_first() const noexcept
    {
        assert(trailing_bits_zero());

        const size_t n = num_blocks(m_num_bits);
        for (size_t i = 0; i < n; ++i)
        {
            U w = m_data[i];
            if (w == U { 0 })
                continue;

            const size_t bit = i * Digits + std::countr_zero(w);
            return bit < m_num_bits ? bit : npos;
        }

        return npos;
    }

    size_t find_next(size_t pos) const noexcept
    {
        assert(trailing_bits_zero());

        ++pos;
        if (pos >= m_num_bits)
            return npos;

        size_t i = block_index(pos);
        U w = m_data[i] & (~U { 0 } << block_pos(pos));

        const size_t n = num_blocks(m_num_bits);
        for (;;)
        {
            if (w != U { 0 })
            {
                const size_t bit = i * Digits + std::countr_zero(w);
                return bit < m_num_bits ? bit : npos;
            }

            if (++i == n)
                return npos;

            w = m_data[i];
        }
    }

    size_t find_first_zero() const noexcept
    {
        const size_t n = num_blocks(m_num_bits);
        for (size_t i = 0; i < n; ++i)
        {
            U w = ~m_data[i];

            if (i == n - 1)
                w &= last_mask(m_num_bits);

            if (w == U { 0 })
                continue;

            const size_t bit = i * Digits + std::countr_zero(w);
            return bit < m_num_bits ? bit : npos;
        }

        return npos;
    }

    size_t find_next_zero(size_t pos) const noexcept
    {
        ++pos;
        if (pos >= m_num_bits)
            return npos;

        size_t i = block_index(pos);
        U w = ~m_data[i] & (~U { 0 } << block_pos(pos));

        const size_t n = num_blocks(m_num_bits);
        for (;;)
        {
            if (i == n - 1)
                w &= last_mask(m_num_bits);

            if (w != U { 0 })
            {
                const size_t bit = i * Digits + std::countr_zero(w);
                return bit < m_num_bits ? bit : npos;
            }

            if (++i == n)
                return npos;

            w = ~m_data[i];
        }
    }

    /**
     * Operators
     */

    template<std::unsigned_integral OtherBlock>
        requires(std::same_as<std::remove_const_t<OtherBlock>, U>)
    BitsetSpan& copy_from(const BitsetSpan<OtherBlock>& other) noexcept
    {
        assert(m_num_bits == other.m_num_bits);
        assert(trailing_bits_zero());
        assert(other.trailing_bits_zero());

        const size_t n = num_blocks(m_num_bits);
        for (size_t i = 0; i < n; ++i)
            m_data[i] = other.m_data[i];

        return *this;
    }

    template<std::unsigned_integral OtherBlock>
        requires(std::same_as<std::remove_const_t<OtherBlock>, U>)
    BitsetSpan& diff_from(const BitsetSpan<OtherBlock>& other) noexcept
    {
        assert(m_num_bits == other.m_num_bits);
        assert(trailing_bits_zero());
        assert(other.trailing_bits_zero());

        const size_t n = num_blocks(m_num_bits);
        for (size_t i = 0; i < n; ++i)
            m_data[i] = other.m_data[i] & ~m_data[i];

        return *this;
    }

    template<std::unsigned_integral OtherBlock>
        requires(std::same_as<std::remove_const_t<OtherBlock>, U>)
    BitsetSpan& operator&=(const BitsetSpan<OtherBlock>& other) noexcept
    {
        assert(m_num_bits == other.m_num_bits);
        assert(trailing_bits_zero());
        assert(other.trailing_bits_zero());

        const size_t n = num_blocks(m_num_bits);
        for (size_t i = 0; i < n; ++i)
            m_data[i] &= other.m_data[i];

        return *this;
    }

    template<std::unsigned_integral OtherBlock>
        requires(std::same_as<std::remove_const_t<OtherBlock>, U>)
    BitsetSpan& operator|=(const BitsetSpan<OtherBlock>& other) noexcept
    {
        assert(m_num_bits == other.m_num_bits);
        assert(trailing_bits_zero());
        assert(other.trailing_bits_zero());

        const size_t n = num_blocks(m_num_bits);
        for (size_t i = 0; i < n; ++i)
            m_data[i] |= other.m_data[i];

        return *this;
    }

    template<std::unsigned_integral OtherBlock>
        requires(std::same_as<std::remove_const_t<OtherBlock>, U>)
    BitsetSpan& operator-=(const BitsetSpan<OtherBlock>& other) noexcept
    {
        assert(m_num_bits == other.m_num_bits);
        assert(trailing_bits_zero());
        assert(other.trailing_bits_zero());

        const size_t n = num_blocks(m_num_bits);
        for (size_t i = 0; i < n; ++i)
            m_data[i] &= ~other.m_data[i];

        return *this;
    }

    /**
     * Getters
     */

    std::span<const U> blocks() const noexcept { return { m_data, num_blocks(m_num_bits) }; }
    size_t num_bits() const noexcept { return m_num_bits; }

private:
    Block* m_data;
    size_t m_num_bits;
};

template<std::unsigned_integral B1, std::unsigned_integral B2>
    requires(std::same_as<std::remove_const_t<B1>, std::remove_const_t<B2>>)
constexpr bool operator==(const BitsetSpan<B1>& lhs, const BitsetSpan<B2>& rhs) noexcept
{
    assert(lhs.trailing_bits_zero());
    assert(rhs.trailing_bits_zero());

    if (lhs.num_bits() != rhs.num_bits())
        return false;

    const size_t n = BitsetSpan<std::remove_const_t<B1>>::num_blocks(lhs.num_bits());
    // Need access to raw blocks: use blocks() but ensure both spans are same length.
    auto lb = lhs.blocks();
    auto rb = rhs.blocks();
    for (size_t i = 0; i < n; ++i)
        if (lb[i] != rb[i])
            return false;

    return true;
}

template<std::unsigned_integral B1, std::unsigned_integral B2>
    requires(std::same_as<std::remove_const_t<B1>, std::remove_const_t<B2>>)
constexpr bool operator!=(const BitsetSpan<B1>& lhs, const BitsetSpan<B2>& rhs) noexcept
{
    return !(lhs == rhs);
}

}

#endif
