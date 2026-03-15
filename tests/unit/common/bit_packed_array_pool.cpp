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

#include <gtest/gtest.h>
#include <tyr/common/bit_packed_array_pool.hpp>
#include <tyr/common/config.hpp>

namespace tyr::tests
{

TEST(TyrTests, TyrCommonBitPackedArrayPoolOutOfRange)
{
    auto pool = BitPackedArrayPool<uint_t, bit::ForwardingBlockCoder<uint_t>, 1>(2, 2);

    // 4 requires width 3, which exceeds the limit of 2.
    EXPECT_THROW(pool.push_back(std::vector<uint_t>({ 1, 4 })), std::out_of_range);

    // 3 elements are too much for a pool that stores arrays of length 2.
    EXPECT_THROW(pool.push_back(std::vector<uint_t>({ 1, 1, 1 })), std::invalid_argument);
}

TEST(TyrTests, TyrCommonBitPackedArrayPool)
{
    auto pool = BitPackedArrayPool<uint8_t, bit::ForwardingBlockCoder<uint8_t>, 1>(2, 3);

    // Repeat 5 times on cleared pool.
    for (size_t i = 0; i < 5; ++i)
    {
        pool.clear();

        // Write
        for (uint8_t a = 0; a < 4; ++a)
            for (uint8_t b = 0; b < 4; ++b)
                pool.push_back(std::vector<uint8_t>({ a, b }));

        // 16 arrays stored.
        EXPECT_EQ(pool.size(), 16);
        EXPECT_EQ(pool.segments().size(), 5);
        EXPECT_EQ(pool.segments()[0].size(), 1);   /// capacity 1 array requires 6 bits   = 1  uint8_t
        EXPECT_EQ(pool.segments()[1].size(), 2);   /// capacity 2 arrays require 12 bits  = 2  uint8_t
        EXPECT_EQ(pool.segments()[2].size(), 3);   /// capacity 4 arrays require 24 bits  = 3  uint8_t
        EXPECT_EQ(pool.segments()[3].size(), 6);   /// capacity 8 arrays require 48 bits  = 6  uint8_t
        EXPECT_EQ(pool.segments()[4].size(), 12);  /// capacity 16 arrays require 96 bits = 12 uint8_t
        // 2^{5+1} - 1 = 31
        EXPECT_EQ(pool.capacity(), 31);

        // Read
        for (uint8_t a = 0; a < 4; ++a)
        {
            for (uint8_t b = 0; b < 4; ++b)
            {
                const size_t idx = a * 4 + b;
                EXPECT_EQ(pool[idx], (std::vector<uint8_t> { a, b }));
            }
        }

        // Re-write
        for (uint8_t a = 0; a < 4; ++a)
        {
            for (uint8_t b = 0; b < 4; ++b)
            {
                const size_t idx = (pool.size() - 1) - (a * 4 + b);
                pool[idx] = std::vector<uint8_t>({ a, b });
            }
        }

        // Re-read
        for (uint8_t a = 0; a < 4; ++a)
        {
            for (uint8_t b = 0; b < 4; ++b)
            {
                const size_t idx = (pool.size() - 1) - (a * 4 + b);
                EXPECT_EQ(pool[idx], (std::vector<uint8_t> { a, b }));
            }
        }
    }
}

}
