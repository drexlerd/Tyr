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

#ifndef TYR_COMMON_SEGMENTED_ARRAY_POOL_HPP_
#define TYR_COMMON_SEGMENTED_ARRAY_POOL_HPP_

#include <bit>
#include <cassert>
#include <cstddef>
#include <tyr/common/bits.hpp>
#include <vector>

namespace tyr
{

template<typename T, size_t ArraysPerSegment = 1024>
class SegmentedArrayPool
{
    static_assert(is_power_of_two(ArraysPerSegment));

    static constexpr size_t seg_shift = std::countr_zero(ArraysPerSegment);
    static constexpr size_t seg_mask = ArraysPerSegment - 1;

private:
    void increase_capacity()
    {
        // 1) If current segment has space, we’re done.
        if (m_cur_seg < m_segments.size() && m_cur_pos + m_array_size <= m_segment_size)
        {
            return;
        }

        // 2) Next segment exists -> jump there
        if (m_cur_seg + 1 < m_segments.size())
        {
            m_cur_seg = m_cur_seg + 1;
            m_cur_pos = 0;
            return;
        }

        // 3) No existing segment fits -> allocate a new one.

        m_segments.emplace_back(m_segment_size);

        m_cur_seg = m_segments.size() - 1;
        m_cur_pos = 0;
    }

public:
    explicit SegmentedArrayPool(size_t array_size) :
        m_array_size(array_size),
        m_segment_size(ArraysPerSegment * array_size),
        m_cur_seg(0),
        m_cur_pos(0),
        m_num_arrays(0)
    {
    }

    T* allocate()
    {
        increase_capacity();

        T* result = &m_segments[m_cur_seg][m_cur_pos];

        m_cur_pos += m_array_size;
        ++m_num_arrays;

        return result;
    }

    const T* operator[](size_t array_index) const noexcept
    {
        assert(array_index < m_num_arrays);
        const size_t seg = array_index >> seg_shift;
        const size_t idx = array_index & seg_mask;
        return &m_segments[seg][idx * m_array_size];
    }

    T* operator[](size_t array_index) noexcept
    {
        assert(array_index < m_num_arrays);
        const size_t seg = array_index >> seg_shift;
        const size_t idx = array_index & seg_mask;
        return &m_segments[seg][idx * m_array_size];
    }

    void clear() noexcept
    {
        m_cur_seg = 0;
        m_cur_pos = 0;
        m_num_arrays = 0;
    }

private:
    std::vector<std::vector<T>> m_segments;

    size_t m_array_size;
    size_t m_segment_size;

    size_t m_cur_seg;
    size_t m_cur_pos;
    size_t m_num_arrays;
};

}

#endif
