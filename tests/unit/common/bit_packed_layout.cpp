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
#include <tyr/common/bit_packed_layout.hpp>
#include <tyr/common/config.hpp>

namespace tyr::tests
{

TEST(TyrTests, TyrCommonBitPackedLayout)
{
    auto ranges = std::vector<uint8_t> { 64, 64, 16, 32 };

    auto variables_layout = create_bit_packed_array_layout<uint8_t>(ranges);

    EXPECT_EQ(variables_layout.total_blocks, 3);

    // Create a state
    auto state = std::vector<uint8_t>(variables_layout.total_blocks, 0);

    // Create a reference
    auto ref_fact_0 = BitPackedElementReference(variables_layout.layouts[0], state.data());

    EXPECT_EQ(uint8_t(ref_fact_0), uint8_t { 0 });

    ref_fact_0 = uint8_t { 2 };

    EXPECT_EQ(uint8_t(ref_fact_0), uint8_t { 2 });
}

TEST(TyrTests, TyrCommonBitPackedLayout2)
{
    auto ranges = std::vector<uint32_t> { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };

    auto variables_layout = create_bit_packed_array_layout<uint32_t>(ranges);

    EXPECT_EQ(variables_layout.total_blocks, 1);

    // Create a state
    auto state = std::vector<uint32_t>(variables_layout.total_blocks, 0);

    // Create a reference
    {
        auto ref = BitPackedElementReference(variables_layout.layouts[0], state.data());
        EXPECT_EQ(uint32_t(ref), uint32_t { 0 });
        ref = uint32_t { 1 };
        EXPECT_EQ(uint32_t(ref), uint32_t { 1 });
        ref = uint32_t { 0 };
        EXPECT_EQ(uint32_t(ref), uint32_t { 0 });
    }
    {
        auto ref = BitPackedElementReference(variables_layout.layouts[1], state.data());
        EXPECT_EQ(uint32_t(ref), uint32_t { 0 });
        ref = uint32_t { 1 };
        EXPECT_EQ(uint32_t(ref), uint32_t { 1 });
        ref = uint32_t { 0 };
        EXPECT_EQ(uint32_t(ref), uint32_t { 0 });
    }
    {
        auto ref = BitPackedElementReference(variables_layout.layouts[0], state.data());
        EXPECT_EQ(uint32_t(ref), uint32_t { 0 });
        ref = uint32_t { 1 };
        EXPECT_EQ(uint32_t(ref), uint32_t { 1 });
        ref = uint32_t { 0 };
        EXPECT_EQ(uint32_t(ref), uint32_t { 0 });
    }
    {
        auto ref = BitPackedElementReference(variables_layout.layouts[2], state.data());
        EXPECT_EQ(uint32_t(ref), uint32_t { 0 });
        ref = uint32_t { 1 };
        EXPECT_EQ(uint32_t(ref), uint32_t { 1 });
        ref = uint32_t { 0 };
        EXPECT_EQ(uint32_t(ref), uint32_t { 0 });
    }
}
}