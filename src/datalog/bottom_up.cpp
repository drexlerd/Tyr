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

#include "tyr/datalog/bottom_up.hpp"

#include "tyr/common/chrono.hpp"            // for StopwatchScope
#include "tyr/common/comparators.hpp"       // for operator!=, opera...
#include "tyr/common/config.hpp"            // for uint_t
#include "tyr/common/formatter.hpp"         // for operator<<
#include "tyr/common/types.hpp"             // for make_view, IndexList
#include "tyr/common/vector.hpp"            // for View
#include "tyr/datalog/assignment_sets.hpp"  // for AssignmentSets
#include "tyr/datalog/fact_sets.hpp"        // for FactSets, Predica...
#include "tyr/datalog/generator.hpp"        // for ground
#include "tyr/datalog/rule_scheduler.hpp"   // for RuleSchedulerStratum
#include "tyr/datalog/workspaces/program.hpp"
#include "tyr/formalism/datalog/merge.hpp"  // for merge, MergeContext
#include "tyr/formalism/datalog/views.hpp"

#include <cista/containers/hash_storage.h>          // for operator!=
#include <gtl/phmap.hpp>                            // for operator!=
#include <memory>                                   // for __shared_ptr_access
#include <oneapi/tbb/enumerable_thread_specific.h>  // for enumerable_thread...
#include <oneapi/tbb/parallel_for_each.h>
#include <utility>  // for pair
#include <vector>   // for vector

namespace tyr::datalog
{

static void solve_bottom_up_for_stratum(datalog::RuleSchedulerStratum& scheduler, datalog::ProgramWorkspace& ws, const datalog::ConstProgramWorkspace& cws)
{
    scheduler.activate_all();

    while (true)
    {
        /**
         * Parallel evaluation.
         */

        const auto active_rules = scheduler.get_active_rules();
        scheduler.on_start_iteration();

        {
            auto stopwatch = StopwatchScope(ws.statistics.ground_seq_total_time);

            oneapi::tbb::parallel_for_each(active_rules.begin(),
                                           active_rules.end(),
                                           [&](auto&& rule_index)
                                           {
                                               const auto i = uint_t(rule_index);
                                               auto& facts = ws.facts;
                                               const auto& const_facts = cws.facts;

                                               const auto assignment_sets = AssignmentSets { const_facts.assignment_sets, facts.assignment_sets };
                                               auto& rule_ws = ws.rules[i];
                                               const auto& const_rule_ws = cws.rules[i];
                                               auto& rule_delta_ws = ws.rule_deltas[i];
                                               auto& worker_ws = ws.worker.local();

                                               {
                                                   ///--- Initialization phase
                                                   auto stopwatch = StopwatchScope(rule_ws.statistics.init_total_time);

                                                   worker_ws.clear();
                                                   rule_ws.clear();
                                                   rule_ws.initialize(const_rule_ws.static_consistency_graph, assignment_sets);
                                               }

                                               {
                                                   /// --- Grounding phase
                                                   auto stopwatch = StopwatchScope(rule_ws.statistics.ground_total_time);
                                                   ++rule_ws.statistics.num_executions;

                                                   ground(facts, const_facts, rule_ws, const_rule_ws, rule_delta_ws, worker_ws);
                                               }
                                           });
        }

        /**
         * Sequential merge.
         */

        /// --- Sequentially combine results into a temporary staging repository to prevent modying the program's repository

        {
            const auto stopwatch = StopwatchScope(ws.statistics.merge_seq_total_time);

            auto discovered_new_fact = bool { false };

            auto& fluent_predicate_fact_sets = ws.facts.fact_sets.predicate;
            auto& fluent_predicate_assignment_sets = ws.facts.assignment_sets.predicate;

            for (const auto rule_index : active_rules)
            {
                const auto i = uint_t(rule_index);

                const auto& rule_ws = ws.rules[i];
                auto& rule_delta_ws = ws.rule_deltas[i];

                auto merge_context = formalism::datalog::MergeContext { ws.datalog_builder, ws.repository, rule_delta_ws.merge_cache };

                for (const auto ground_head_index : rule_ws.ground_heads)
                {
                    /// --- Program

                    // Note: the head lives in the stage but its index is stored in rule because it is reset each iteration
                    const auto ground_head_rule = make_view(ground_head_index, *rule_delta_ws.repository);

                    // Merge it into the program
                    const auto ground_head_index_program = formalism::datalog::merge_d2d(ground_head_rule, merge_context).first;

                    // Insert new fact into fact sets and assigment sets
                    if (!fluent_predicate_fact_sets.contains(ground_head_index_program))
                    {
                        discovered_new_fact = true;

                        const auto ground_head_program = make_view(ground_head_index_program, ws.repository);

                        scheduler.on_generate(ground_head_program.get_predicate().get_index());

                        fluent_predicate_fact_sets.insert(ground_head_program);
                        fluent_predicate_assignment_sets.insert(ground_head_program);

                        // TODO: here we could annotate the fact with the cost since we are basically running breadth-first

                        ++ws.statistics.num_merges_inserted;
                    }
                    else
                    {
                        ++ws.statistics.num_merges_discarded;
                    }
                }
            }

            if (!discovered_new_fact)
                break;  ///< Reached fixed point
        }

        scheduler.on_finish_iteration();
    }
}

void solve_bottom_up(datalog::ProgramWorkspace& ws, const datalog::ConstProgramWorkspace& cws)
{
    for (auto& rule_delta_ws : ws.rule_deltas)
        rule_delta_ws.clear();

    for (auto& scheduler : ws.rule_scheduler_strata.data)
    {
        solve_bottom_up_for_stratum(scheduler, ws, cws);
    }
}
}
