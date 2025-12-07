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

namespace tyr::tests
{

TEST(TyrTests, TyrGrounderStaticConsistencyGraph)
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

    for (uint_t i = 0; i < program.get_rules().size(); ++i)
    {
        const auto rule = program.get_rules()[i];
        const auto& parameter_domains = domains.rule_domains[i];

        auto graph = grounder::StaticConsistencyGraph(rule.get_body(), parameter_domains, assignment_sets.static_sets);

        std::cout << graph << std::endl;
    }
}
}