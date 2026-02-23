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

static void create_general_binding(std::span<const kpkc::Vertex> clique, const StaticConsistencyGraph& consistency_graph, IndexList<f::Object>& binding)
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
    auto wrctx = rctx.get_rule_worker_execution_context();

    const auto& in = wrctx.in();
    auto& out = wrctx.out();

    create_nullary_binding(out.ground_context_solve().binding);

    // Note: we never go through the consistency graph, and hence, have to check validity on the entire rule body.
    if (is_applicable(in.cws_rule().get_nullary_condition(), in.fact_sets())
        && is_valid_binding(in.cws_rule().get_rule().get_body(), in.fact_sets(), out.const_ground_context_program()))
    {
        const auto program_head_index = fd::ground(in.cws_rule().get_rule().get_head(), out.ground_context_iteration()).first;
        const auto worker_head_index = fd::ground(in.cws_rule().get_rule().get_head(), out.ground_context_solve()).first;

        out.heads().insert(worker_head_index);

        in.and_ap().update_annotation(program_head_index,
                                      worker_head_index,
                                      in.cost_buckets().current_cost(),
                                      in.program_repository(),
                                      in.cws_rule().get_rule(),
                                      in.cws_rule().get_witness_condition(),
                                      in.or_annot(),
                                      out.head_to_witness(),
                                      out.ground_context_solve());
    }
}

[[maybe_unused]] static bool ensure_applicability(View<Index<fd::Rule>, fd::Repository> rule, fd::GrounderContext& context, const FactSets& fact_sets)
{
    const auto ground_rule = make_view(ground(rule, context).first, context.destination);

    const auto applicable = is_applicable(ground_rule, fact_sets);

    if (!applicable)
    {
        std::cout << "Delta-KPKC generated false positive." << std::endl;
        std::cout << rule << std::endl;
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
void process_clique(RuleWorkerExecutionContext<OrAP, AndAP, TP>& wrctx, std::span<const kpkc::Vertex> clique)
{
    const auto& in = wrctx.in();
    auto& out = wrctx.out();

    create_general_binding(clique, in.cws_rule().static_consistency_graph, out.ground_context_solve().binding);

    assert(ensure_novel_binding(out.ground_context_solve().binding, out.seen_bindings_dbg()));

    const auto program_head_index = fd::ground(in.cws_rule().get_rule().get_head(), out.ground_context_iteration()).first;
    if (in.fact_sets().fluent_sets.predicate.contains(program_head_index))
        return;  ///< optimal cost proven

    auto applicability_check = out.applicability_check_pool().get_or_allocate(in.cws_rule().get_nullary_condition(),
                                                                              in.cws_rule().get_conflicting_overapproximation_condition(),
                                                                              in.fact_sets(),
                                                                              out.const_ground_context_program());

    if (!applicability_check->is_statically_applicable())
        return;

    // IMPORTANT: A binding can fail the nullary part (e.g., arm-empty) even though the clique already exists.
    // Later, nullary may become true without any new kPKC edges/vertices, so delta-kPKC will NOT re-enumerate this binding.
    // Therefore we must store it as pending (keyed by binding) and recheck in the next fact envelope.
    if (applicability_check->is_dynamically_applicable(in.fact_sets(), out.const_ground_context_program()))
    {
        assert(ensure_applicability(in.cws_rule().get_rule(), out.ground_context_iteration(), in.fact_sets()));

        // std::cout << rctx.cws_rule.rule << " " << rctx.out.ground_context_solve().binding << std::endl;

        const auto worker_head_index = fd::ground(in.cws_rule().get_rule().get_head(), out.ground_context_solve()).first;

        // std::cout << make_view(ground(rctx.cws_rule.get_rule(), rctx.ground_context_iter).first, rctx.ground_context_iter.destination)
        //           << std::endl;

        out.heads().insert(worker_head_index);

        in.and_ap().update_annotation(program_head_index,
                                      worker_head_index,
                                      in.cost_buckets().current_cost(),
                                      in.program_repository(),
                                      in.cws_rule().get_rule(),
                                      in.cws_rule().get_witness_condition(),
                                      in.or_annot(),
                                      out.head_to_witness(),
                                      out.ground_context_solve());
    }
    else
    {
        out.pending_rules().emplace(fd::ground(out.ground_context_solve().binding, out.ground_context_solve()).first, std::move(applicability_check));
    }
}

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
void generate_general_case(RuleExecutionContext<OrAP, AndAP, TP>& rctx)
{
    const auto& kpkc_algorithm = rctx.ws_rule.common.kpkc;
    auto& kpkc_cliques = rctx.ws_rule.common.kpkc_cliques;
    auto& kpkc_workspace = rctx.ws_rule.common.kpkc_workspace;

    kpkc_algorithm.for_each_new_k_clique(kpkc_cliques, kpkc_workspace);
    rctx.ws_rule.common.statistics.num_bindings += kpkc_cliques.size();

    constexpr size_t PAR_THRESHOLD = 1024;
    constexpr size_t GRAIN = 256;

    const int arena_conc = oneapi::tbb::this_task_arena::max_concurrency();

    const bool do_parallel_inner = (kpkc_cliques.size() >= PAR_THRESHOLD) && (arena_conc >= 2);

    {
        if (do_parallel_inner)
        {
            oneapi::tbb::parallel_for(oneapi::tbb::blocked_range<size_t>(0, kpkc_cliques.size(), GRAIN),
                                      [&](const oneapi::tbb::blocked_range<size_t>& r)
                                      {
                                          auto wrctx = rctx.get_rule_worker_execution_context();

                                          for (size_t i = r.begin(); i != r.end(); ++i)
                                              process_clique(wrctx, kpkc_cliques[i]);
                                      });
        }
        else
        {
            auto wrctx = rctx.get_rule_worker_execution_context();

            for (size_t i = 0; i < kpkc_cliques.size(); ++i)
                process_clique(wrctx, kpkc_cliques[i]);
        }
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
void process_pending(RuleExecutionContext<OrAP, AndAP, TP>& rctx)
{
    for (auto& worker : rctx.ws_rule.worker)
    {
        auto wrctx = RuleWorkerExecutionContext(rctx, worker);

        const auto& in = wrctx.in();
        auto& out = wrctx.out();

        for (auto it = out.pending_rules().begin(); it != out.pending_rules().end();)
        {
            out.ground_context_solve().binding = make_view(it->first, out.ground_context_solve().destination).get_objects().get_data();

            assert(out.ground_context_solve().binding == out.ground_context_iteration().binding);
            const auto program_head_index = fd::ground(in.cws_rule().get_rule().get_head(), out.ground_context_iteration()).first;

            if (in.fact_sets().fluent_sets.predicate.contains(program_head_index))  ///< optimal cost proven
            {
                it = out.pending_rules().erase(it);
            }
            else if (it->second->is_dynamically_applicable(in.fact_sets(), out.const_ground_context_program()))
            {
                assert(ensure_applicability(in.cws_rule().get_rule(), out.ground_context_iteration(), in.fact_sets()));

                const auto worker_head_index = fd::ground(in.cws_rule().get_rule().get_head(), out.ground_context_solve()).first;

                out.heads().insert(worker_head_index);

                in.and_ap().update_annotation(program_head_index,
                                              worker_head_index,
                                              in.cost_buckets().current_cost(),
                                              in.program_repository(),
                                              in.cws_rule().get_rule(),
                                              in.cws_rule().get_witness_condition(),
                                              in.or_annot(),
                                              out.head_to_witness(),
                                              out.ground_context_solve());

                it = out.pending_rules().erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
void solve_bottom_up_for_stratum(StratumExecutionContext<OrAP, AndAP, TP>& ctx)
{
    auto& scheduler = ctx.scheduler;
    auto& facts = ctx.ctx.ws.facts;
    auto& cost_buckets = ctx.ctx.ws.cost_buckets;
    auto& ws = ctx.ctx.ws;
    auto& tp = ctx.ctx.ws.tp;

    scheduler.activate_all();

    cost_buckets.clear();

    while (true)
    {
        // std::cout << "Cost: " << cost_buckets.current_cost() << std::endl;

        // Check whether min cost for goal was proven.
        if (tp.check())
        {
            return;
        }

        // std::cout << std::endl;
        // std::cout << "=======================================================================" << std::endl;
        // std::cout << "Facts: " << std::endl;
        // for (const auto& set : facts.fact_sets.predicate.get_sets())
        // {
        //     std::cout << set.get_facts() << std::endl;
        // }
        // std::cout << "Delta facts: " << std::endl;
        // for (const auto& set : facts.delta_fact_sets.predicate.get_sets())
        // {
        //     std::cout << set.get_facts() << std::endl;
        // }
        // std::cout << std::endl;

        scheduler.on_start_iteration();

        const auto& active_rules = scheduler.get_active_rules();

        /**
         * Parallel process pending applicability checks and generate ground witnesses.
         */

        {
            const auto program_stopwatch = StopwatchScope(ws.statistics.parallel_time);

            oneapi::tbb::parallel_for_each(active_rules.begin(),
                                           active_rules.end(),
                                           [&](auto&& rule_index)
                                           {
                                               // std::cout << make_view(rule_index, ws.repository) << std::endl;

                                               auto rctx = ctx.get_rule_execution_context(rule_index);

                                               const auto total_time = StopwatchScope(rctx.ws_rule.common.statistics.total_time);
                                               ++rctx.ws_rule.common.statistics.num_executions;

                                               rctx.clear_iteration();  ///< Clear iteration before process_pending/generate

                                               {
                                                   const auto initialize_time = StopwatchScope(rctx.ws_rule.common.statistics.initialize_time);

                                                   rctx.initialize();  ///< Initialize before process_pending/generate
                                               }

                                               {
                                                   const auto process_pending_time = StopwatchScope(rctx.ws_rule.common.statistics.process_pending_time);

                                                   process_pending(rctx);
                                               }

                                               {
                                                   const auto process_generate_time = StopwatchScope(rctx.ws_rule.common.statistics.process_generate_time);

                                                   generate(rctx);
                                               }
                                           });
        }

        // Clear delta facts
        facts.delta_fact_sets.reset();

        // Clear current bucket to avoid duplicate handling
        cost_buckets.clear_current();

        /**
         * Sequential merge results from workers into program
         */

        {
            for (const auto rule_index : active_rules)
            {
                const auto i = uint_t(rule_index);
                auto merge_context = fd::MergeContext { ws.datalog_builder, ws.repository };
                const auto& ws_rule = ws.rules[i];

                for (const auto& worker : ws_rule->worker)
                {
                    for (const auto worker_head : worker.iteration.heads)
                    {
                        // Merge head from delta into the program
                        const auto program_head = fd::merge_d2d(make_view(worker_head, worker.solve.stage_repository), merge_context).first;

                        // std::cout << make_view(program_head, ws.repository) << std::endl;

                        // Update annotation
                        const auto cost_update = ctx.ctx.ws.or_ap.update_annotation(program_head,
                                                                                    worker_head,
                                                                                    ctx.ctx.ws.or_annot,
                                                                                    worker.iteration.head_to_witness,
                                                                                    ctx.ctx.ws.head_to_witness);

                        cost_buckets.update(cost_update, program_head);
                    }
                }
            }

            if (!cost_buckets.advance_to_next_nonempty())
                return;  // Terminate if no-nonempty bucket was found.

            // Insert next bucket heads into fact and assignment sets + trigger scheduler.
            for (const auto head_index : cost_buckets.get_current_bucket())
            {
                if (!facts.fact_sets.predicate.contains(head_index))
                {
                    const auto head = make_view(head_index, ws.repository);

                    // Notify scheduler
                    scheduler.on_generate(head.get_predicate().get_index());

                    // Notify termination policy
                    tp.achieve(head_index);

                    // Update fact sets
                    facts.fact_sets.predicate.insert(head);
                    facts.assignment_sets.predicate.insert(head);
                    facts.delta_fact_sets.predicate.insert(head);
                }
            }
        }

        scheduler.on_finish_iteration();
    }
}

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
void solve_bottom_up(ProgramExecutionContext<OrAP, AndAP, TP>& ctx)
{
    const auto program_stopwatch = StopwatchScope(ctx.ws.statistics.total_time);
    ++ctx.ws.statistics.num_executions;

    for (auto stratum_ctx : ctx.get_stratum_execution_contexts())
    {
        solve_bottom_up_for_stratum(stratum_ctx);
    }
}

template void solve_bottom_up(ProgramExecutionContext<NoOrAnnotationPolicy, NoAndAnnotationPolicy, NoTerminationPolicy>& ctx);
template void solve_bottom_up(ProgramExecutionContext<OrAnnotationPolicy, AndAnnotationPolicy<SumAggregation>, NoTerminationPolicy>& ctx);
template void solve_bottom_up(ProgramExecutionContext<OrAnnotationPolicy, AndAnnotationPolicy<SumAggregation>, TerminationPolicy<SumAggregation>>& ctx);
template void solve_bottom_up(ProgramExecutionContext<OrAnnotationPolicy, AndAnnotationPolicy<MaxAggregation>, NoTerminationPolicy>& ctx);
template void solve_bottom_up(ProgramExecutionContext<OrAnnotationPolicy, AndAnnotationPolicy<MaxAggregation>, TerminationPolicy<MaxAggregation>>& ctx);
}
