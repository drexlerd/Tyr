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
#include <tyr/formalism/formalism.hpp>
#include <tyr/grounder/grounder.hpp>

using namespace tyr::buffer;
using namespace tyr::formalism;
using namespace tyr::grounder;

namespace tyr::tests
{

TEST(TyrTests, TyrGrounderAssignmentHash)
{
    /*
    Predicate: location/2
    Vertex Assignment: [0/39] rank: 41
    Edge Assignment: [0/39, 1/15] rank: 1929
    Vertex Assignment: [1/15] rank: 43
    Predicate: location/2
    Vertex Assignment: [0/39] rank: 41
    Edge Assignment: [0/39, 1/14] rank: 1929
    Vertex Assignment: [1/14] rank: 43
    */
    auto num_objects = size_t(41);
    auto o_3 = Index<Object> { 3 };
    auto o_4 = Index<Object> { 4 };
    auto o_14 = Index<Object> { 14 };
    auto o_15 = Index<Object> { 15 };
    auto o_39 = Index<Object> { 39 };
    auto o_40 = Index<Object> { 40 };
    auto p_0 = ParameterIndex { 0 };
    auto p_1 = ParameterIndex { 1 };
    auto p_2 = ParameterIndex { 2 };
    // [[39], [3, 4], [40]]
    auto domains = analysis::DomainListList { { o_39 }, { o_3, o_4 }, { o_40 } };
    auto hash = PerfectAssignmentHash(domains, num_objects);

    // Both are mapped to the same index. We now have assertions that guard against this unintended behavior, resulting from illformed variable domain analysis.
    EXPECT_EQ(hash.get_rank<true>(EdgeAssignment(p_0, o_39, p_1, o_15)), hash.get_rank<true>(EdgeAssignment(p_0, o_39, p_1, o_14)));
}

TEST(TyrTests, TyrGrounderAssignmentSets)
{
    auto [program, repository] = create_example_problem();

    // Analyze variable domains to compress assignment sets
    auto domains = analysis::compute_variable_domains(program);

    // Allocate
    auto assignment_sets = grounder::AssignmentSets(program, domains);

    // Reset
    assignment_sets.static_sets.predicate.reset();
    assignment_sets.fluent_sets.predicate.reset();
    assignment_sets.static_sets.function.reset();
    assignment_sets.fluent_sets.function.reset();

    // Insert for a given set of facts
    assignment_sets.static_sets.predicate.insert(program.get_atoms<formalism::StaticTag>());
    assignment_sets.fluent_sets.predicate.insert(program.get_atoms<formalism::FluentTag>());
    assignment_sets.static_sets.function.insert(program.get_fterm_values<formalism::StaticTag>());
    assignment_sets.fluent_sets.function.insert(program.get_fterm_values<formalism::FluentTag>());
}
}