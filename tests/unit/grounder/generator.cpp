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

TEST(TyrTests, TyrGrounderGenerator)
{
    auto [program_index, repository] = create_example_problem();
    auto program = View<Index<Program>, Repository>(program_index, repository);

    std::cout << program << std::endl;

    // Once: Analyze variable domains to compress assignment sets
    auto domains = analysis::compute_variable_domains(program);

    // One: Allocate and create mutable sets of facts.
    auto fact_sets = grounder::FactSets<Repository>(program);

    // Once: Allocate reusable memory for AssignmentSets
    auto assignment_sets = grounder::AssignmentSets(program, domains);

    // Once: Insert facts into AssignmentSets
    assignment_sets.insert(fact_sets);

    // Once: Instantiate the static consistency graph for each rule
    auto static_consistency_graphs = std::vector<grounder::StaticConsistencyGraph<Repository>> {};

    for (uint_t i = 0; i < program.get_rules().size(); ++i)
    {
        const auto rule = program.get_rules()[i];
        const auto& parameter_domains = domains.rule_domains[i];

        static_consistency_graphs.emplace_back(grounder::StaticConsistencyGraph(rule.get_body(), parameter_domains, assignment_sets.static_sets));
    }

    // Once: Allocate reusable memory for kpkc
    auto consistency_graphs = std::vector<grounder::kpkc::DenseKPartiteGraph> {};
    auto kpkc_workspaces = std::vector<grounder::kpkc::Workspace> {};

    for (const auto& static_consistency_graph : static_consistency_graphs)
    {
        consistency_graphs.emplace_back(grounder::kpkc::allocate_dense_graph(static_consistency_graph));
        kpkc_workspaces.emplace_back(grounder::kpkc::allocate_workspace(static_consistency_graph));
    }

    // Per fact set: Remove inconsistent edges
    for (uint_t i = 0; i < program.get_rules().size(); ++i)
    {
        const auto& static_consistency_graph = static_consistency_graphs[i];
        auto& consistency_graph = consistency_graphs[i];
        auto& kpkc_workspace = kpkc_workspaces[i];

        grounder::kpkc::initialize_dense_graph_and_workspace(static_consistency_graph, assignment_sets, consistency_graph, kpkc_workspace);
    }

    // Once: Create a repository for each rule
    auto rule_repositories = std::vector<Repository> {};
    for (uint_t i = 0; i < program.get_rules().size(); ++i)
    {
        rule_repositories.emplace_back(Repository());
    }

    // Once: Create a scoped repository for each rule
    auto rule_scoped_repositories = std::vector<ScopedRepository<Repository>> {};
    for (uint_t i = 0; i < program.get_rules().size(); ++i)
    {
        rule_scoped_repositories.emplace_back(ScopedRepository(repository, rule_repositories[i]));
    }

    // Once: Create temporary bindings
    auto bindings = std::vector<IndexList<formalism::Object>>(program.get_rules().size());
    // Once: Create builders
    auto builders = std::vector<formalism::Builder>(program.get_rules().size());
    // Once: Create buffers
    auto buffers = std::vector<cista::Buffer>(program.get_rules().size());
    // Once: Create container for applicable ground rules
    auto ground_rules = std::vector<IndexList<formalism::GroundRule>>(program.get_rules().size());

    // Per fact set: Create workspaces that wrap all the data for grounding, then call ground
    // TODO: we can use onetbb parallel for here later.
    for (uint_t i = 0; i < program.get_rules().size(); ++i)
    {
        std::cout << "r: " << program.get_rules()[i] << std::endl;

        // Combine all the data dependencies into workspaces.
        auto immutable_workspace = grounder::ImmutableRuleWorkspace<Repository> { fact_sets,
                                                                                  assignment_sets,
                                                                                  program.get_rules()[i],
                                                                                  static_consistency_graphs[i],
                                                                                  consistency_graphs[i] };
        auto mutable_workspace = grounder::MutableRuleWorkspace<Repository> {
            rule_scoped_repositories[i], kpkc_workspaces[i], bindings[i], builders[i], buffers[i], ground_rules[i]
        };

        grounder::ground(immutable_workspace, mutable_workspace);
    }
}
}