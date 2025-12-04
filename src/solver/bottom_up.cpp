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
static void solve_bottom_up_for_stratum(grounder::ProgramExecutionContext& program_execution_context, const analysis::RuleStratum& stratum)
{
    auto& builder = program_execution_context.builder;
    auto& stage_merge_cache = program_execution_context.stage_merge_cache;
    auto& stage_repository = *program_execution_context.stage_repository;
    auto& stage_merge_rules = program_execution_context.stage_merge_rules;
    auto& stage_merge_atoms = program_execution_context.stage_merge_atoms;
    auto& stage_to_program_merge_cache = program_execution_context.stage_to_program_merge_cache;
    auto& program_repository = *program_execution_context.repository;
    auto& program_merge_rules = program_execution_context.program_merge_rules;
    auto& program_merge_atoms = program_execution_context.program_merge_atoms;

    while (true)
    {
        /**
         * Parallel evaluation.
         */

        const uint_t num_rules = stratum.size();

        tbb::parallel_for(uint_t { 0 },
                          num_rules,
                          [&](uint_t j)
                          {
                              const auto i = stratum[j].get_index().get_value();

                              auto& facts_execution_context = program_execution_context.facts_execution_context;
                              auto& rule_execution_context = program_execution_context.rule_execution_contexts[i];
                              auto& thread_execution_context = program_execution_context.thread_execution_contexts.local();  // thread-local
                              thread_execution_context.clear();

                              ground(facts_execution_context, rule_execution_context, thread_execution_context);
                          });

        /**
         * Sequential merge.
         */

        /// --- Sequentially combine results into a temporary staging repository to prevent modying the program's repository

        stage_repository.clear();
        stage_merge_cache.clear();
        stage_merge_rules.clear();
        stage_merge_atoms.clear();

        for (uint_t j = 0; j < stratum.size(); ++j)
        {
            const auto i = stratum[j].get_index().get_value();

            const auto& rule_execution_context = program_execution_context.rule_execution_contexts[i];

            for (const auto rule : rule_execution_context.ground_rules)
            {
                const auto merge_rule = merge(rule, builder, stage_repository, stage_merge_cache);
                const auto merge_head = merge_rule.get_head();

                stage_merge_rules.insert(merge_rule);
                stage_merge_atoms.insert(merge_head);
            }
        }

        /// --- Copy the result into the program's repository

        stage_to_program_merge_cache.clear();
        program_merge_rules.clear();
        program_merge_atoms.clear();

        auto discovered_new_fact = bool { false };

        for (const auto rule : stage_merge_rules)
        {
            const auto merge_rule = merge(rule, builder, program_repository, stage_to_program_merge_cache);
            const auto merge_head = merge_rule.get_head();

            // Re-insert fact
            if (!program_execution_context.facts_execution_context.fact_sets.fluent_sets.predicate.contains(merge_head))
            {
                program_execution_context.facts_execution_context.fact_sets.fluent_sets.predicate.insert(merge_head);
                program_execution_context.facts_execution_context.assignment_sets.fluent_sets.predicate.insert(merge_head);

                program_merge_rules.insert(merge_rule);
                program_merge_atoms.insert(merge_head);

                discovered_new_fact = true;
            }
        }

        if (!discovered_new_fact)
            break;  ///< Reached fixed point

        /// --- Re-initialize RuleExecutionContext with new fact set.

        for (uint_t j = 0; j < stratum.size(); ++j)
        {
            const auto i = stratum[j].get_index().get_value();

            auto& rule_execution_context = program_execution_context.rule_execution_contexts[i];

            rule_execution_context.initialize(program_execution_context.facts_execution_context.assignment_sets);
        }
    }
}

void solve_bottom_up(grounder::ProgramExecutionContext& program_execution_context)
{
    for (const auto& stratum : program_execution_context.strata.strata)
    {
        solve_bottom_up_for_stratum(program_execution_context, stratum);
    }
}
}
