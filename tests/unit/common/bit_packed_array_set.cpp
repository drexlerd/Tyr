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
#include <tyr/common/bit_packed_array_set.hpp>
#include <tyr/common/config.hpp>

namespace tyr::tests
{

TEST(TyrTests, TyrCommonBitPackedArraySetOutOfRange)
{
    auto set = BitPackedArraySet<uint_t>(2, 2);

    // 4 requires width 3, which exceeds the limit of 2.
    EXPECT_THROW(set.insert(std::vector<uint_t>({ 1, 4 })), std::out_of_range);

    // 3 elements are too much for a pool that stores arrays of length 2.
    EXPECT_THROW(set.insert(std::vector<uint_t>({ 1, 1, 1 })), std::invalid_argument);
}

TEST(TyrTests, TyrCommonBitPackedArraySet)
{
    auto set = BitPackedArraySet<uint8_t>(2, 3);

    // Repeat 5 times on cleared pool.
    for (size_t i = 0; i < 5; ++i)
    {
        set.clear();

        // Write
        for (uint8_t a = 0; a < 4; ++a)
        {
            for (uint8_t b = 0; b < 4; ++b)
            {
                const size_t idx = a * 4 + b;

                const auto [i1, inserted1] = set.insert(std::vector<uint8_t>({ a, b }));
                EXPECT_EQ(i1, idx);
                EXPECT_TRUE(inserted1);
                const auto [i2, inserted2] = set.insert(std::vector<uint8_t>({ a, b }));
                EXPECT_FALSE(inserted2);
                EXPECT_EQ(i2, idx);
            }
        }

        // 16 arrays stored.
        EXPECT_EQ(set.size(), 16);
        EXPECT_EQ(set.segments().size(), 5);
        EXPECT_EQ(set.segments()[0].size(), 1);   /// capacity 1 array requires 6 bits   = 1  uint8_t
        EXPECT_EQ(set.segments()[1].size(), 2);   /// capacity 2 arrays require 12 bits  = 2  uint8_t
        EXPECT_EQ(set.segments()[2].size(), 3);   /// capacity 4 arrays require 24 bits  = 3  uint8_t
        EXPECT_EQ(set.segments()[3].size(), 6);   /// capacity 8 arrays require 48 bits  = 6  uint8_t
        EXPECT_EQ(set.segments()[4].size(), 12);  /// capacity 16 arrays require 96 bits = 12 uint8_t
        // 2^{5+1} - 1 = 31
        EXPECT_EQ(set.capacity(), 31);
    }
}

}
