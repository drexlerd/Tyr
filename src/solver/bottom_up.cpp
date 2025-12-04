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

#include "tyr/solver/bottom_up.hpp"

#include "tyr/analysis/analysis.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/grounder/generator.hpp"
#include "tyr/grounder/workspace.hpp"

#include <oneapi/tbb/enumerable_thread_specific.h>
#include <oneapi/tbb/parallel_for.h>

namespace tyr::solver
{
static void solve_bottom_up_for_stratum(grounder::ProgramExecutionContext& program_execution_context,
                                        const analysis::RuleStratum& stratum,
                                        GroundAtomsPerPredicate& out_facts)
{
    std::vector<View<Index<formalism::Rule>, formalism::Repository>> expand(stratum.begin(), stratum.end());
    auto expand_next = UnorderedSet<View<Index<formalism::Rule>, formalism::Repository>> {};

    oneapi::tbb::enumerable_thread_specific<grounder::ThreadExecutionContext> thread_execution_contexts;

    while (true)
    {
        tbb::parallel_for(size_t { 0 },
                          expand.size(),
                          [&](size_t i)
                          {
                              i = expand[i].get_index().get_value();
                              auto& rule_execution_context = program_execution_context.rule_execution_contexts[i];
                              auto& thread_execution_context = thread_execution_contexts.local();
                              thread_execution_context.clear();

                              grounder::ground(program_execution_context.facts_execution_context, rule_execution_context, thread_execution_context);
                          });

        auto builder = formalism::Builder();

        auto merge_cache = formalism::MergeCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository> {};
        auto merge_repository = formalism::Repository();
        auto merge_ground_rules = std::vector<View<Index<formalism::GroundRule>, formalism::Repository>> {};

        for (const auto rule : expand)
        {
            const auto i = rule.get_index().get_value();
            auto& rule_execution_context = program_execution_context.rule_execution_contexts[i];

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
            const auto global_ground_rule = formalism::merge(ground_rule, builder, *program_execution_context.repository, global_cache);

            std::cout << global_ground_rule << std::endl;

            global_ground_rules.push_back(global_ground_rule);
        }

        break;  // just for testing
    }
}

void solve_bottom_up(grounder::ProgramExecutionContext& context, GroundAtomsPerPredicate& out_facts)
{
    for (const auto& stratum : context.strata.strata)
    {
        solve_bottom_up_for_stratum(context, stratum, out_facts);
    }
}
}
