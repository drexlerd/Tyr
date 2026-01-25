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

#include "tyr/common/chrono.hpp"       // for StopwatchScope
#include "tyr/common/comparators.hpp"  // for operator!=, opera...
#include "tyr/common/config.hpp"       // for uint_t
#include "tyr/common/equal_to.hpp"     // for EqualTo
#include "tyr/common/formatter.hpp"
#include "tyr/common/hash.hpp"                // for Hash
#include "tyr/common/types.hpp"               // for View
#include "tyr/common/vector.hpp"              // for View
#include "tyr/datalog/applicability.hpp"      // for is_ap...
#include "tyr/datalog/assignment_sets.hpp"    // for AssignmentSets
#include "tyr/datalog/consistency_graph.hpp"  // for Vertex
#include "tyr/datalog/declarations.hpp"
#include "tyr/datalog/delta_kpkc.hpp"  // for Works...
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/datalog/formatter.hpp"
#include "tyr/datalog/rule_scheduler.hpp"  // for RuleSchedulerStratum
#include "tyr/datalog/workspaces/facts.hpp"
#include "tyr/datalog/workspaces/program.hpp"
#include "tyr/datalog/workspaces/rule.hpp"
#include "tyr/datalog/workspaces/worker.hpp"
#include "tyr/formalism/datalog/conjunctive_condition_view.hpp"  // for View
#include "tyr/formalism/datalog/declarations.hpp"                // for Context
#include "tyr/formalism/datalog/formatter.hpp"                   // for opera...
#include "tyr/formalism/datalog/grounder.hpp"                    // for Groun...
#include "tyr/formalism/datalog/merge.hpp"                       // for merge, MergeContext
#include "tyr/formalism/datalog/repository.hpp"                  // for Repos...
#include "tyr/formalism/datalog/views.hpp"

#include <algorithm>  // for all_of
#include <assert.h>   // for assert
#include <boost/dynamic_bitset.hpp>
#include <memory>  // for __sha...
#include <oneapi/tbb/parallel_for_each.h>
#include <tuple>    // for opera...
#include <utility>  // for pair
#include <vector>   // for vector

namespace f = tyr::formalism;
namespace fd = tyr::formalism::datalog;

namespace tyr::datalog
{

static void create_nullary_binding(IndexList<f::Object>& binding) { binding.clear(); }

static void create_general_binding(const std::vector<kpkc::Vertex>& clique, const StaticConsistencyGraph& consistency_graph, IndexList<f::Object>& binding)
{
    binding.resize(clique.size());
    for (const auto v : clique)
    {
        const auto& vertex = consistency_graph.get_vertex(v.index);
        assert(uint_t(vertex.get_parameter_index()) < clique.size());
        binding[uint_t(vertex.get_parameter_index())] = vertex.get_object_index();
    }
}

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
void generate_nullary_case(RuleExecutionContext<OrAP, AndAP, TP>& rctx)
{
    auto ground_context_iter = rctx.get_ground_context_iter();
    auto ground_context_solve = rctx.get_ground_context_solve();
    auto ground_context_program = rctx.get_ground_context_program();
    auto fact_sets = rctx.get_fact_sets();

    create_nullary_binding(ground_context_solve.binding);

    // Note: we never go through the consistency graph, and hence, have to check validity on the entire rule body.
    if (is_applicable(rctx.cws_rule.get_nullary_condition(), fact_sets)
        && is_valid_binding(rctx.cws_rule.get_rule().get_body(), fact_sets, ground_context_program))
    {
        const auto program_head_index = fd::ground(rctx.cws_rule.get_rule().get_head(), ground_context_iter).first;
        const auto delta_head_index = fd::ground(rctx.cws_rule.get_rule().get_head(), ground_context_solve).first;

        rctx.ws_rule_iter.heads.insert(delta_head_index);

        rctx.and_ap.update_annotation(rctx.ctx.ctx.ws.cost_buckets.current_cost(),
                                      rctx.cws_rule.rule,
                                      rctx.cws_rule.witness_condition,
                                      program_head_index,
                                      delta_head_index,
                                      rctx.ctx.ctx.aps.or_annot,
                                      rctx.and_annot,
                                      rctx.delta_head_to_witness,
                                      ground_context_solve,
                                      rctx.ctx.ctx.ws.repository);
    }
}

[[maybe_unused]] static bool
ensure_applicability(View<Index<fd::Rule>, fd::Repository> rule, fd::GrounderContext<f::OverlayRepository<fd::Repository>>& context, const FactSets& fact_sets)
{
    const auto ground_rule = make_view(ground(rule, context).first, context.destination);

    const auto applicable = is_applicable(ground_rule, fact_sets);

    if (!applicable)
    {
        std::cout << "Delta-KPKC generated false positive." << std::endl;
        std::cout << "Rule:" << std::endl;
        std::cout << rule << std::endl;
        std::cout << "GroundRule:" << std::endl;
        std::cout << ground_rule << std::endl;
    }

    return applicable;
}

[[maybe_unused]] static bool ensure_novel_binding(const IndexList<f::Object>& binding, UnorderedSet<IndexList<f::Object>>& set)
{
    const auto inserted = set.insert(binding).second;

    if (!inserted)
    {
        std::cout << "Delta-KPKC generated duplicate binding." << std::endl;
    }

    return inserted;
}

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
void generate_general_case(RuleExecutionContext<OrAP, AndAP, TP>& rctx)
{
    // std::cout << std::endl << std::endl << rctx.cws_rule.get_rule() << std::endl;

    // auto generated = uint_t(0);
    //  auto rules = std::vector<View<Index<fd::GroundRule>, f::OverlayRepository<fd::Repository>>> {};

    auto ground_context_iter = rctx.get_ground_context_iter();
    auto ground_context_solve = rctx.get_ground_context_solve();
    auto ground_context_program = rctx.get_ground_context_program();
    auto fact_sets = rctx.get_fact_sets();

    rctx.ws_rule_iter.kpkc.for_each_new_k_clique(
        [&](auto&& clique)
        {
            const auto stopwatch = StopwatchScope(rctx.ws_rule_iter.statistics.gen_time);

            create_general_binding(clique, rctx.cws_rule.static_consistency_graph, ground_context_solve.binding);

            assert(ensure_novel_binding(ground_context_solve.binding, rctx.ws_rule_solve.seen_bindings_dbg));

            const auto program_head_index = fd::ground(rctx.cws_rule.get_rule().get_head(), ground_context_iter).first;
            if (rctx.ctx.ctx.ws.facts.fact_sets.predicate.contains(program_head_index))
                return;  ///< optimal cost proven

            ++rctx.ws_rule_iter.statistics.num_bindings;

            auto applicability_check = rctx.ws_rule_solve.applicability_check_pool.get_or_allocate(rctx.cws_rule.get_nullary_condition(),
                                                                                                   rctx.cws_rule.get_conflicting_overapproximation_condition(),
                                                                                                   fact_sets,
                                                                                                   ground_context_program);

            if (!applicability_check->is_statically_applicable())
                return;

            //++generated;

            // IMPORTANT: A binding can fail the nullary part (e.g., arm-empty) even though the clique already exists.
            // Later, nullary may become true without any new kPKC edges/vertices, so delta-kPKC will NOT re-enumerate this binding.
            // Therefore we must store it as pending (keyed by binding) and recheck in the next fact envelope.
            if (applicability_check->is_dynamically_applicable(fact_sets, ground_context_program))
            {
                assert(ensure_applicability(rctx.cws_rule.get_rule(), ground_context_iter, fact_sets));

                // std::cout << rctx.cws_rule.rule << " " << rctx.ground_context_solve.binding << std::endl;

                const auto delta_head_index = fd::ground(rctx.cws_rule.get_rule().get_head(), ground_context_solve).first;

                // rules.push_back(make_view(ground(rctx.cws_rule.get_rule(), rctx.ground_context_iter).first, rctx.ground_context_iter.destination));

                // std::cout << make_view(ground(rctx.cws_rule.get_rule(), rctx.ground_context_iter).first, rctx.ground_context_iter.destination)
                //           << std::endl;

                rctx.ws_rule_iter.heads.insert(delta_head_index);

                rctx.and_ap.update_annotation(rctx.ctx.ctx.ws.cost_buckets.current_cost(),
                                              rctx.cws_rule.rule,
                                              rctx.cws_rule.witness_condition,
                                              program_head_index,
                                              delta_head_index,
                                              rctx.ctx.ctx.aps.or_annot,
                                              rctx.and_annot,
                                              rctx.delta_head_to_witness,
                                              ground_context_solve,
                                              rctx.ctx.ctx.ws.repository);
            }
            else
            {
                rctx.ws_rule_solve.pending_rules.emplace(fd::ground(ground_context_solve.binding, ground_context_solve).first, std::move(applicability_check));
            }
        },
        rctx.ws_rule_iter.kpkc_workspace);

    // if (generated > 100)
    //{
    //  std::cout << "Generated: " << generated << std::endl;
    //  std::cout << "Rule:" << std::endl;
    //  std::cout << rctx.cws_rule.get_rule() << std::endl;
    //  std::cout << "Witness condition:" << std::endl;
    //  std::cout << rctx.cws_rule.get_witness_condition() << std::endl;
    //  std::cout << "Nullary condition:" << std::endl;
    //  std::cout << rctx.cws_rule.get_nullary_condition() << std::endl;
    //  std::cout << "Overapproximation condition:" << std::endl;
    //  std::cout << rctx.cws_rule.get_conflicting_overapproximation_condition() << std::endl;
    //  std::cout << std::endl;
    //    for (const auto& r : rules)
    //{
    //        std::cout << r << std::endl;
    //    }
    //}

    // std::cout << "Num pending rules before: " << rctx.ws_rule_solve.pending_rules.size() << std::endl;

    {
        const auto stopwatch = StopwatchScope(rctx.ws_rule_iter.statistics.pending_time);

        for (auto it = rctx.ws_rule_solve.pending_rules.begin(); it != rctx.ws_rule_solve.pending_rules.end();)
        {
            ground_context_solve.binding = make_view(it->first, ground_context_solve.destination).get_objects().get_data();

            // Fast path
            assert(ground_context_solve.binding == ground_context_iter.binding);
            const auto program_head_index = fd::ground(rctx.cws_rule.get_rule().get_head(), ground_context_iter).first;

            if (rctx.ctx.ctx.ws.facts.fact_sets.predicate.contains(program_head_index))  ///< optimal cost proven
            {
                it = rctx.ws_rule_solve.pending_rules.erase(it);
            }
            else if (it->second->is_dynamically_applicable(fact_sets, ground_context_program))
            {
                assert(ensure_applicability(rctx.cws_rule.get_rule(), ground_context_iter, fact_sets));

                // std::cout << rctx.cws_rule.rule << " " << rctx.ground_context_solve.binding << std::endl;

                const auto delta_head_index = fd::ground(rctx.cws_rule.get_rule().get_head(), ground_context_solve).first;

                rctx.ws_rule_iter.heads.insert(delta_head_index);

                rctx.and_ap.update_annotation(rctx.ctx.ctx.ws.cost_buckets.current_cost(),
                                              rctx.cws_rule.rule,
                                              rctx.cws_rule.witness_condition,
                                              program_head_index,
                                              delta_head_index,
                                              rctx.ctx.ctx.aps.or_annot,
                                              rctx.and_annot,
                                              rctx.delta_head_to_witness,
                                              ground_context_solve,
                                              rctx.ctx.ctx.ws.repository);

                it = rctx.ws_rule_solve.pending_rules.erase(it);
            }
            else
            {
                ++it;
            }
        }

        // std::cout << "Num pending rules after: " << rctx.ws_rule_solve.pending_rules.size() << std::endl;
    }
}

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
void generate(RuleExecutionContext<OrAP, AndAP, TP>& rctx)
{
    const auto arity = rctx.cws_rule.get_rule().get_arity();

    if (arity == 0)
        generate_nullary_case(rctx);
    else
        generate_general_case(rctx);
}

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
void solve_bottom_up_for_stratum(StratumExecutionContext<OrAP, AndAP, TP>& ctx)
{
    ctx.scheduler.activate_all();

    ctx.ctx.ws.cost_buckets.clear();

    while (true)
    {
        // std::cout << "Cost: " << ctx.ctx.ws.cost_buckets.current_cost() << std::endl;

        // Check whether min cost for goal was proven.
        if (ctx.ctx.tp.check())
        {
            return;
        }

        /**
         * Parallel evaluation.
         */

        const auto active_rules = ctx.scheduler.get_active_rules();
        ctx.scheduler.on_start_iteration();

        {
            const auto program_stopwatch = StopwatchScope(ctx.ctx.ws.statistics.parallel_time);

            oneapi::tbb::parallel_for_each(active_rules.begin(),
                                           active_rules.end(),
                                           [&](auto&& rule_index)
                                           {
                                               const auto i = uint_t(rule_index);

                                               const auto rule_stopwatch = StopwatchScope(ctx.ctx.ws.rules_iter[i].statistics.parallel_time);
                                               ++ctx.ctx.ws.rules_iter[i].statistics.num_executions;

                                               // std::cout << make_view(rule_index, ctx.ctx.ws.repository) << std::endl;

                                               auto rctx = ctx.get_rule_execution_context(rule_index);

                                               generate(rctx);

                                               // std::cout << std::endl << std::endl;
                                           });
        }

        /**
         * Sequential merge.
         */

        /// --- Sequentially combine results into a temporary staging repository to prevent modying the program's repository

        {
            // Clear current bucket to avoid duplicate handling
            ctx.ctx.ws.cost_buckets.clear_current();

            for (const auto rule_index : active_rules)
            {
                const auto i = uint_t(rule_index);

                auto merge_context = fd::MergeContext { ctx.ctx.ws.datalog_builder, ctx.ctx.ws.repository };

                for (const auto delta_head : ctx.ctx.ws.rules_iter[i].heads)
                {
                    // Merge head from delta into the program
                    const auto program_head = fd::merge_d2d(make_view(delta_head, *ctx.ctx.ws.rules_solve[i].repository), merge_context).first;

                    // Update annotation
                    const auto cost_update = ctx.ctx.aps.or_ap.update_annotation(program_head,
                                                                                 delta_head,
                                                                                 ctx.ctx.aps.or_annot,
                                                                                 ctx.ctx.aps.and_annots[i],
                                                                                 ctx.ctx.aps.delta_head_to_witness[i],
                                                                                 ctx.ctx.aps.program_head_to_witness);

                    ctx.ctx.ws.cost_buckets.update(cost_update, program_head);
                }
            }

            if (!ctx.ctx.ws.cost_buckets.advance_to_next_nonempty())
                return;  // Terminate if no-nonempty bucket was found.

            // Insert next bucket heads into fact and assignment sets + trigger scheduler.
            for (const auto head_index : ctx.ctx.ws.cost_buckets.get_current_bucket())
            {
                if (!ctx.ctx.ws.facts.fact_sets.predicate.contains(head_index))
                {
                    const auto head = make_view(head_index, ctx.ctx.ws.repository);

                    // Notify scheduler
                    ctx.scheduler.on_generate(head.get_predicate().get_index());

                    // Notify termination policy
                    ctx.ctx.tp.achieve(head_index);

                    // Update fact sets
                    ctx.ctx.ws.facts.fact_sets.predicate.insert(head);
                    ctx.ctx.ws.facts.assignment_sets.predicate.insert(head);

                    // std::cout << "Discovered: " << head << std::endl;
                }
            }
        }

        ctx.scheduler.on_finish_iteration();
    }
}

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
void solve_bottom_up(ProgramExecutionContext<OrAP, AndAP, TP>& ctx)
{
    // std::cout << "solve_bottom_up" << std::endl;

    const auto program_stopwatch = StopwatchScope(ctx.ws.statistics.total_time);
    ++ctx.ws.statistics.num_executions;

    for (auto stratum_ctx : ctx.get_stratum_execution_contexts())
        solve_bottom_up_for_stratum(stratum_ctx);
}

template void solve_bottom_up(ProgramExecutionContext<NoOrAnnotationPolicy, NoAndAnnotationPolicy, NoTerminationPolicy>& ctx);

template void solve_bottom_up(ProgramExecutionContext<OrAnnotationPolicy, AndAnnotationPolicy<SumAggregation>, NoTerminationPolicy>& ctx);

template void solve_bottom_up(ProgramExecutionContext<OrAnnotationPolicy, AndAnnotationPolicy<SumAggregation>, TerminationPolicy<SumAggregation>>& ctx);

template void solve_bottom_up(ProgramExecutionContext<OrAnnotationPolicy, AndAnnotationPolicy<MaxAggregation>, NoTerminationPolicy>& ctx);

template void solve_bottom_up(ProgramExecutionContext<OrAnnotationPolicy, AndAnnotationPolicy<MaxAggregation>, TerminationPolicy<MaxAggregation>>& ctx);
}
