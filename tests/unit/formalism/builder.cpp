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

#include "tyr/formalism/formalism.hpp"

#include <gtest/gtest.h>

using namespace tyr::buffer;
using namespace tyr::formalism;

namespace tyr::tests
{

TEST(TyrTests, TyrFormalismBuilder)
{
    auto builder = Builder();

    {
        auto numeric_effect_ptr = builder.template get_builder<GroundNumericEffect<OpIncrease, AuxiliaryTag>>();
        auto& numeric_effect = *numeric_effect_ptr;
        numeric_effect.clear();

        numeric_effect.fexpr.value = 0.5;

        EXPECT_EQ(numeric_effect.fexpr, Data<GroundFunctionExpression>(0.5));
    }

    {
        auto numeric_effect_ptr = builder.template get_builder<GroundNumericEffect<OpIncrease, AuxiliaryTag>>();
        auto& numeric_effect = *numeric_effect_ptr;
        numeric_effect.clear();

        numeric_effect.fexpr.value = 1.5;

        EXPECT_EQ(numeric_effect.fexpr, Data<GroundFunctionExpression>(1.5));
    }
}
}