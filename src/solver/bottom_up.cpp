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
    while (true)
    {
        /**
         * Parallel evaluation.
         */

        const uint_t num_rules = program_execution_context.program.get_rules().size();

        tbb::parallel_for(uint_t { 0 },
                          num_rules,
                          [&](uint_t i)
                          {
                              auto& facts_execution_context = program_execution_context.facts_execution_context;
                              auto& rule_execution_context = program_execution_context.rule_execution_contexts[i];
                              auto& thread_execution_context = program_execution_context.thread_execution_contexts.local();  // thread-local
                              thread_execution_context.clear();

                              ground(facts_execution_context, rule_execution_context, thread_execution_context);
                          });

        /**
         * Sequential merge.
         */

        /// --- Sequentially combine results into a temporary top-level repository to prevent modying the program's repository
        auto& builder = program_execution_context.builder;
        auto& global_merge_cache = program_execution_context.global_merge_cache;
        auto& merge_repository = *program_execution_context.merge_repository;
        auto& tmp_merge_rules = program_execution_context.tmp_merge_rules;
        merge_repository.clear();
        global_merge_cache.clear();
        tmp_merge_rules.clear();

        for (const auto& rule_execution_context : program_execution_context.rule_execution_contexts)
            for (const auto rule : rule_execution_context.ground_rules)
                tmp_merge_rules.insert(merge(rule, builder, merge_repository, global_merge_cache));

        /// --- Copy the result into the program's repository
        auto& merge_cache = program_execution_context.merge_cache;
        auto& merge_rules = program_execution_context.merge_rules;
        merge_cache.clear();
        merge_rules.clear();

        for (const auto rule : tmp_merge_rules)
            merge_rules.insert(merge(rule, builder, *program_execution_context.repository, merge_cache));
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
