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
#include <oneapi/tbb/enumerable_thread_specific.h>
#include <oneapi/tbb/global_control.h>
#include <oneapi/tbb/parallel_for.h>
#include <tyr/analysis/analysis.hpp>
#include <tyr/grounder/grounder.hpp>

using namespace tyr::buffer;
using namespace tyr::formalism;

namespace tyr::tests
{

TEST(TyrTests, TyrGrounderGenerator)
{
    auto [program_index, repository] = create_example_problem();
    auto program = View<Index<Program>, Repository>(program_index, repository);

    std::cout << program << std::endl;

    /**
     * Preprocessing 1: Analyze variable domains
     */

    auto domains = analysis::compute_variable_domains(program);

    /**
     * Allocation 1: Execution contexts
     */

    // Per fact set
    auto facts_execution_context = grounder::FactsExecutionContext(program, domains);

    // Per rule
    auto rule_execution_contexts = std::vector<grounder::RuleExecutionContext> {};
    for (uint_t i = 0; i < program.get_rules().size(); ++i)
    {
        const auto rule = program.get_rules()[i];
        const auto& parameter_domains = domains.rule_domains[i];

        rule_execution_contexts.emplace_back(rule, parameter_domains, facts_execution_context.assignment_sets.get<formalism::StaticTag>(), repository);
    }

    // Per thread
    oneapi::tbb::enumerable_thread_specific<grounder::ThreadExecutionContext> thread_execution_contexts;

    /**
     * Initialization 1: Execution contexts
     */
    for (uint_t i = 0; i < program.get_rules().size(); ++i)
    {
        rule_execution_contexts[i].initialize(facts_execution_context.assignment_sets);
    }

    /**
     * Parallelization 1: Lock-free rule grounding
     */

    const uint_t num_rules = program.get_rules().size();

    // oneapi::tbb::global_control control(oneapi::tbb::global_control::max_allowed_parallelism, 1);

    tbb::parallel_for(uint_t { 0 },
                      num_rules,
                      [&](uint_t i)
                      {
                          auto& rule_execution_context = rule_execution_contexts[i];
                          auto& thread_execution_context = thread_execution_contexts.local();  // thread-local
                          thread_execution_context.clear();

                          grounder::ground(facts_execution_context, rule_execution_context, thread_execution_context);
                      });

    /**
     * Sequential merge.
     */

    auto builder = formalism::Builder();

    auto merge_cache = formalism::MergeCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository> {};
    auto merge_repository = formalism::Repository();
    auto merge_ground_rules = std::vector<View<Index<formalism::GroundRule>, formalism::Repository>> {};

    for (uint_t i = 0; i < program.get_rules().size(); ++i)
    {
        auto& rule_execution_context = rule_execution_contexts[i];

        for (const auto ground_rule : rule_execution_context.ground_rules)
        {
            merge_ground_rules.push_back(formalism::merge(ground_rule, builder, merge_repository, merge_cache));
        }
    }

    // Now everything is moved from an OverlayRepository to a top-level Repository and we can perform the final merge step
    auto global_cache = formalism::MergeCache<formalism::Repository, formalism::Repository> {};
    auto global_ground_rules = std::vector<View<Index<formalism::GroundRule>, formalism::Repository>> {};

    for (const auto ground_rule : merge_ground_rules)
    {
        const auto global_ground_rule = formalism::merge(ground_rule, builder, repository, global_cache);

        std::cout << global_ground_rule << std::endl;

        global_ground_rules.push_back(global_ground_rule);
    }
}
}