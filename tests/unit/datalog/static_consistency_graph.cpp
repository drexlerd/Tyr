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
#include <tyr/datalog/datalog.hpp>
#include <tyr/formalism/formalism.hpp>

namespace a = tyr::analysis;
namespace b = tyr::buffer;
namespace f = tyr::formalism;
namespace g = tyr::grounder;

namespace tyr::tests
{

TEST(TyrTests, TyrDatalogStaticConsistencyGraph)
{
    auto [program, repository] = create_example_problem();

    // Analyze variable domains to compress assignment sets
    auto domains = a::compute_variable_domains(program);

    // Allocate
    auto assignment_sets = g::AssignmentSets(program, domains);

    // Reset
    assignment_sets.static_sets.predicate.reset();
    assignment_sets.fluent_sets.predicate.reset();
    assignment_sets.static_sets.function.reset();
    assignment_sets.fluent_sets.function.reset();

    // Insert for a given set of facts
    assignment_sets.static_sets.predicate.insert(program.get_atoms<f::StaticTag>());
    assignment_sets.fluent_sets.predicate.insert(program.get_atoms<f::FluentTag>());
    assignment_sets.static_sets.function.insert(program.get_fterm_values<f::StaticTag>());
    assignment_sets.fluent_sets.function.insert(program.get_fterm_values<f::FluentTag>());

    for (uint_t i = 0; i < program.get_rules().size(); ++i)
    {
        const auto rule = program.get_rules()[i];
        const auto& parameter_domains = domains.rule_domains[i];

        auto graph = datalog::StaticConsistencyGraph(rule.get_body(), parameter_domains, 0, rule.get_arity(), assignment_sets.static_sets);

        std::cout << graph << std::endl;
    }
}
}