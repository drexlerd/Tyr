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
#include <tyr/analysis/analysis.hpp>
#include <tyr/grounder/grounder.hpp>

using namespace tyr::cista;
using namespace tyr::formalism;

namespace tyr::tests
{

TEST(TyrTests, TyrGrounderAssignmentSets)
{
    auto [program_index, repository] = create_example_problem();
    auto program = Proxy<Program, Repository>(program_index, repository);

    auto domains = analysis::compute_variable_list_per_predicate(program);

    auto static_assignment_set = grounder::AssignmentSets<formalism::StaticTag>(program, domains);
}
}