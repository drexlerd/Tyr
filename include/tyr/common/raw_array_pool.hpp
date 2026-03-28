/*
 * Copyright (C) 2025 Dominik Drexler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef TYR_COMMON_RAW_ARRAY_POOL_HPP_
#define TYR_COMMON_RAW_ARRAY_POOL_HPP_

#include "tyr/common/bit.hpp"

#include <bit>
#include <cassert>
#include <cstddef>
#include <type_traits>
#include <vector>

namespace tyr
{

template<typename T, size_t ArraysPerSegment = 1024>
    requires std::is_trivially_copyable_v<T>
class RawArrayPool
{
    static_assert(bit::is_power_of_two(ArraysPerSegment));

    static constexpr size_t seg_shift = std::countr_zero(ArraysPerSegment);
    static constexpr size_t seg_mask = ArraysPerSegment - 1;

private:
    void increase_capacity()
    {
        if (m_cur_seg < m_segments.size() && m_cur_pos + m_array_size <= m_segment_size)
            return;

        if (m_cur_seg + 1 < m_segments.size())
        {
            m_cur_seg = m_cur_seg + 1;
            m_cur_pos = 0;
            return;
        }

        m_segments.emplace_back(m_segment_size);

        m_cur_seg = m_segments.size() - 1;
        m_cur_pos = 0;
    }

public:
    explicit RawArrayPool(size_t array_size) : m_array_size(array_size), m_segment_size(ArraysPerSegment * array_size), m_cur_seg(0), m_cur_pos(0), m_size(0) {}

    T* allocate()
    {
        increase_capacity();

        T* result = &m_segments[m_cur_seg][m_cur_pos];

        m_cur_pos += m_array_size;
        ++m_size;

        return result;
    }

    const T* operator[](size_t array_index) const noexcept
    {
        assert(array_index < m_size);
        const size_t seg = array_index >> seg_shift;
        const size_t idx = array_index & seg_mask;
        return &m_segments[seg][idx * m_array_size];
    }

    T* operator[](size_t array_index) noexcept
    {
        assert(array_index < m_size);
        const size_t seg = array_index >> seg_shift;
        const size_t idx = array_index & seg_mask;
        return &m_segments[seg][idx * m_array_size];
    }

    void clear() noexcept
    {
        m_cur_seg = 0;
        m_cur_pos = 0;
        m_size = 0;
    }

    size_t size() const noexcept { return m_size; }
    size_t array_size() const noexcept { return m_array_size; }

private:
    std::vector<std::vector<T>> m_segments;

    size_t m_array_size;
    size_t m_segment_size;

    size_t m_cur_seg;
    size_t m_cur_pos;
    size_t m_size;
};

}  // namespace tyr

#endif