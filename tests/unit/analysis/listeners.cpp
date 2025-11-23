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

#include "../utils.hpp"

#include <gtest/gtest.h>
#include <tyr/tyr.hpp>

using namespace tyr::cista;
using namespace tyr::formalism;

namespace tyr::tests
{

TEST(TyrTests, TyrAnalysisListeners)
{
    auto [program_index, repository] = create_example_problem();
    auto program = Proxy<Program, Repository>(program_index, repository);

    auto rule_strata = analysis::compute_rule_stratification(program);
    auto listeners = analysis::compute_listeners(rule_strata, repository);

    for (const auto& listeners_in_stratum : listeners.positive_listeners_per_stratum)
    {
        std::cout << to_string(listeners_in_stratum) << std::endl;
    }
}
}