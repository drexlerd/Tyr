/*
 * Copyright (C) 2024 Dominik Drexler
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

#ifndef TYR_CISTA_BYTE_BUFFER_SEGMENTED_HPP_
#define TYR_CISTA_BYTE_BUFFER_SEGMENTED_HPP_

#include "tyr/common/bits.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace tyr::cista
{

class ByteBufferSegmented
{
private:
    size_t m_seg_size;

    std::vector<std::vector<uint8_t>> m_segments;

    size_t m_cur_seg;
    size_t m_cur_pos;

    size_t m_size;
    size_t m_capacity;

    void increase_capacity(size_t amount)
    {
        // 1) If current segment has space, we’re done.
        if (m_cur_seg < m_segments.size())
        {
            if (amount <= (m_segments[m_cur_seg].size() - m_cur_pos))
            {
                return;
            }
        }

        // 2) Try later segments, always starting at position 0 in them.
        for (size_t i = m_cur_seg + 1; i < m_segments.size(); ++i)
        {
            if (amount <= m_segments[i].size())
            {
                m_cur_seg = i;
                m_cur_pos = 0;
                return;
            }
        }

        // 3) No existing segment fits → allocate a new one.

        // Ensure that required bytes fit into a buffer.
        // Use doubling strategy to make future insertions cheaper.
        const auto need = std::bit_ceil(amount);
        m_seg_size = 2 * std::max(need, m_seg_size);

        m_segments.emplace_back(m_seg_size);

        m_capacity += m_seg_size;
        m_cur_seg = m_segments.size() - 1;
        m_cur_pos = 0;
    }

public:
    explicit ByteBufferSegmented(size_t seg_size = 1024) : m_seg_size(seg_size), m_segments(), m_cur_seg(-1), m_cur_pos(0), m_size(0), m_capacity(0)
    {
        assert(is_power_of_two(seg_size));

        // Reserve the initial segment.
        increase_capacity(seg_size);
    }

    /// @brief Write the data with alignment requirement.
    const uint8_t* write(const uint8_t* data, size_t amount, size_t align = 1)
    {
        assert(data);
        assert((align & (align - 1)) == 0 || align == 1);

        // Ensure that there is enough capacity for data written with correct alignment
        const auto worst_padding = (align > 1) ? (align - 1) : 0;
        auto required = amount + worst_padding;

        increase_capacity(required);

        // Ensure pos satisfies alignment.
        const auto old_pos = m_cur_pos;
        if (align > 1)
            m_cur_pos = (m_cur_pos + align - 1) & ~(align - 1);
        const auto padding = m_cur_pos - old_pos;

        assert(m_cur_pos % align == 0);

        const auto pos = &m_segments[m_cur_seg][m_cur_pos];

        memcpy(pos, data, amount);

        m_cur_pos += amount;
        m_size += amount + padding;

        return pos;
    }

    /// @brief Set the write head to the beginning.
    void clear()
    {
        m_cur_seg = 0;
        m_cur_pos = 0;
        m_size = 0;
    }

    size_t num_segments() const { return m_segments.size(); }
    size_t size() const { return m_size; }
    size_t capacity() const { return m_capacity; }
};
}

#endif
