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
#include <tyr/planning/planning.hpp>

using namespace tyr::buffer;
using namespace tyr::formalism;
using namespace tyr::planning;

namespace tyr::tests
{

TEST(TyrTests, TyrPlanningGroundTaskLayout)
{
    auto repository = Repository {};
    auto builder = Builder {};
    auto variable_ptr = builder.get_builder<FDRVariable<FluentTag>>();
    auto& variable = *variable_ptr;

    auto variables = IndexList<FDRVariable<FluentTag>> {};

    // high left aligned in first block
    variable.domain_size = 64;  // 6 bits
    canonicalize(variable);
    const auto variable_0 = repository.get_or_create(variable, builder.get_buffer()).first;
    variables.push_back(variable_0.get_index());

    // high + low
    variable.domain_size = 64;  // 6 bits
    canonicalize(variable);
    const auto variable_1 = repository.get_or_create(variable, builder.get_buffer()).first;
    variables.push_back(variable_1.get_index());

    // high right aligned
    variable.domain_size = 16;  // 4 bits
    canonicalize(variable);
    const auto variable_2 = repository.get_or_create(variable, builder.get_buffer()).first;
    variables.push_back(variable_2.get_index());

    // high left aligned after full block
    variable.domain_size = 32;  // 5 bits
    canonicalize(variable);
    const auto variable_3 = repository.get_or_create(variable, builder.get_buffer()).first;
    variables.push_back(variable_3.get_index());

    auto variables_layout = create_layouts<FluentTag, Repository, uint8_t>(make_view(variables, repository));

    EXPECT_EQ(variables_layout.total_blocks, 3);

    // Create a state
    auto state = std::vector<uint8_t>(variables_layout.total_blocks, 0);

    // Create a reference
    auto ref_fact_0 = VariableReference(variables_layout.layouts[0], state.data());

    EXPECT_EQ(FDRValue(ref_fact_0), FDRValue { 0 });

    ref_fact_0 = FDRValue { 2 };

    EXPECT_EQ(FDRValue(ref_fact_0), FDRValue { 2 });
}
}