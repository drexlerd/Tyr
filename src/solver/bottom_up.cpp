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
#include "tyr/formalism/ground.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/grounder/execution_contexts.hpp"
#include "tyr/grounder/generator.hpp"

#include <oneapi/tbb/enumerable_thread_specific.h>
#include <oneapi/tbb/parallel_for.h>

namespace tyr::solver
{
static void solve_bottom_up_for_stratum(grounder::ProgramExecutionContext& program_execution_context, const analysis::RuleStratum& stratum)
{
    while (true)
    {
        /**
         * Parallel evaluation.
         */

        const uint_t num_rules = stratum.size();

        auto start_ground_seq = std::chrono::steady_clock::now();

        tbb::parallel_for(uint_t { 0 },
                          num_rules,
                          [&](uint_t j)
                          {
                              const auto i = stratum[j].get_index().get_value();
                              auto& facts_execution_context = program_execution_context.facts_execution_context;
                              auto& rule_execution_context = program_execution_context.rule_execution_contexts[i];
                              auto& rule_stage_execution_context = program_execution_context.rule_stage_execution_contexts[i];
                              auto& thread_execution_context = program_execution_context.thread_execution_contexts.local();

                              {
                                  ///--- Initialization phase
                                  auto start_init = std::chrono::steady_clock::now();

                                  thread_execution_context.clear();
                                  rule_execution_context.clear();
                                  rule_execution_context.initialize(program_execution_context.facts_execution_context.assignment_sets);

                                  auto end_init = std::chrono::steady_clock::now();
                                  rule_execution_context.statistics.init_total_time +=
                                      std::chrono::duration_cast<std::chrono::nanoseconds>(end_init - start_init);
                              }

                              {
                                  /// --- Grounding phase
                                  auto start_ground = std::chrono::steady_clock::now();

                                  ground(facts_execution_context, rule_execution_context, rule_stage_execution_context, thread_execution_context);

                                  auto end_ground = std::chrono::steady_clock::now();
                                  rule_execution_context.statistics.ground_total_time +=
                                      std::chrono::duration_cast<std::chrono::nanoseconds>(end_ground - start_ground);

                                  ++rule_execution_context.statistics.num_executions;
                              }
                          });

        auto end_ground_seq = std::chrono::steady_clock::now();
        program_execution_context.statistics.ground_seq_total_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end_ground_seq - start_ground_seq);

        /**
         * Sequential merge.
         */

        /// --- Sequentially combine results into a temporary staging repository to prevent modying the program's repository

        auto start_merge_seq = std::chrono::steady_clock::now();

        auto discovered_new_fact = bool { false };

        for (uint_t j = 0; j < stratum.size(); ++j)
        {
            const auto i = stratum[j].get_index().get_value();

            const auto& rule_execution_context = program_execution_context.rule_execution_contexts[i];

            for (const auto binding : rule_execution_context.bindings)
            {
                const auto merge_binding = merge(binding, program_execution_context.builder, *program_execution_context.repository);

                /// --- Insert (rule, binding) pair
                program_execution_context.program_results_execution_context.rule_binding_pairs.emplace(rule_execution_context.rule, merge_binding);

                const auto ground_head = formalism::ground(rule_execution_context.rule.get_head(),
                                                           merge_binding.get_objects(),
                                                           program_execution_context.builder,
                                                           *program_execution_context.repository);

                // Insert new fact into fact sets and assigment sets
                if (!program_execution_context.facts_execution_context.fact_sets.fluent_sets.predicate.contains(ground_head))
                    discovered_new_fact = true;

                program_execution_context.facts_execution_context.fact_sets.fluent_sets.predicate.insert(ground_head);
                program_execution_context.facts_execution_context.assignment_sets.fluent_sets.predicate.insert(ground_head);
            }
        }

        auto end_merge_seq = std::chrono::steady_clock::now();
        program_execution_context.statistics.merge_seq_total_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end_merge_seq - start_merge_seq);

        if (!discovered_new_fact)
            break;  ///< Reached fixed point
    }
}

void solve_bottom_up(grounder::ProgramExecutionContext& program_execution_context)
{
    program_execution_context.program_results_execution_context.clear();
    for (auto& rule_stage_execution_context : program_execution_context.rule_stage_execution_contexts)
        rule_stage_execution_context.clear();

    for (const auto& stratum : program_execution_context.strata.strata)
    {
        solve_bottom_up_for_stratum(program_execution_context, stratum);
    }
}
}
