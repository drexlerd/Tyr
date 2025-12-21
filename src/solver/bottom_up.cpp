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
#include "tyr/common/chrono.hpp"
#include "tyr/common/declarations.hpp"
#include "tyr/common/types.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/grounder_datalog.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/grounder/execution_contexts.hpp"
#include "tyr/grounder/generator.hpp"

#include <oneapi/tbb/enumerable_thread_specific.h>
#include <oneapi/tbb/parallel_for.h>

namespace tyr::solver
{
static void solve_bottom_up_for_stratum(grounder::ProgramExecutionContext& program_execution_context, grounder::RuleSchedulerStratum& scheduler)
{
    scheduler.activate_all();

    while (true)
    {
        /**
         * Parallel evaluation.
         */

        const auto active_rules = scheduler.active_rules();
        scheduler.clear();

        const uint_t num_rules = active_rules.size();

        {
            auto stopwatch = StopwatchScope(program_execution_context.statistics.ground_seq_total_time);

            tbb::parallel_for(uint_t { 0 },
                              num_rules,
                              [&](uint_t j)
                              {
                                  const auto i = active_rules[j].get_index().get_value();
                                  auto& facts_execution_context = program_execution_context.facts_execution_context;
                                  auto& rule_execution_context = program_execution_context.rule_execution_contexts[i];
                                  auto& rule_stage_execution_context = program_execution_context.rule_stage_execution_contexts[i];
                                  auto& thread_execution_context = program_execution_context.thread_execution_contexts.local();

                                  {
                                      ///--- Initialization phase
                                      auto stopwatch = StopwatchScope(rule_execution_context.statistics.init_total_time);

                                      thread_execution_context.clear();
                                      rule_execution_context.clear();
                                      rule_execution_context.initialize(program_execution_context.facts_execution_context.assignment_sets);
                                  }

                                  {
                                      /// --- Grounding phase
                                      auto stopwatch = StopwatchScope(rule_execution_context.statistics.ground_total_time);
                                      ++rule_execution_context.statistics.num_executions;

                                      ground(facts_execution_context, rule_execution_context, rule_stage_execution_context, thread_execution_context);
                                  }
                              });
        }

        /**
         * Sequential merge.
         */

        /// --- Sequentially combine results into a temporary staging repository to prevent modying the program's repository

        {
            const auto stopwatch = StopwatchScope(program_execution_context.statistics.merge_seq_total_time);

            auto discovered_new_fact = bool { false };

            for (uint_t j = 0; j < num_rules; ++j)
            {
                const auto i = active_rules[j].get_index().get_value();

                const auto& rule_execution_context = program_execution_context.rule_execution_contexts[i];
                auto& rule_stage_execution_context = program_execution_context.rule_stage_execution_contexts[i];

                auto merge_context = formalism::MergeContext { program_execution_context.builder,
                                                               *program_execution_context.repository,
                                                               rule_stage_execution_context.merge_cache };

                for (const auto ground_head_index : rule_execution_context.ground_heads)
                {
                    /// --- Program

                    // Note: the head lives in the stage but its index is stored in rule because it is reset each iteration
                    const auto ground_head_rule = make_view(ground_head_index, *rule_stage_execution_context.repository);

                    // Merge it into the program
                    const auto ground_head_index_program = merge(ground_head_rule, merge_context).first;

                    // Insert new fact into fact sets and assigment sets
                    if (!program_execution_context.facts_execution_context.fact_sets.fluent_sets.predicate.contains(ground_head_index_program))
                    {
                        discovered_new_fact = true;

                        const auto ground_head_program = make_view(ground_head_index_program, *program_execution_context.repository);

                        scheduler.on_generate(ground_head_program.get_predicate().get_index());

                        program_execution_context.facts_execution_context.fact_sets.fluent_sets.predicate.insert(ground_head_program);
                        program_execution_context.facts_execution_context.assignment_sets.fluent_sets.predicate.insert(ground_head_program);
                    }
                }
            }

            if (!discovered_new_fact)
                break;  ///< Reached fixed point
        }
    }
}

void solve_bottom_up(grounder::ProgramExecutionContext& program_execution_context)
{
    for (auto& rule_stage_execution_context : program_execution_context.rule_stage_execution_contexts)
        rule_stage_execution_context.clear();

    for (auto& scheduler : program_execution_context.rule_scheduler_strata.data)
    {
        solve_bottom_up_for_stratum(program_execution_context, scheduler);
    }
}
}
