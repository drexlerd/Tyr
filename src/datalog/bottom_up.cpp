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

#include "tyr/common/chrono.hpp"              // for StopwatchScope
#include "tyr/common/comparators.hpp"         // for operator!=, opera...
#include "tyr/common/config.hpp"              // for uint_t
#include "tyr/common/equal_to.hpp"            // for EqualTo
#include "tyr/common/formatter.hpp"           // for operator<<
#include "tyr/common/hash.hpp"                // for Hash
#include "tyr/common/types.hpp"               // for View
#include "tyr/common/vector.hpp"              // for View
#include "tyr/datalog/applicability.hpp"      // for is_ap...
#include "tyr/datalog/assignment_sets.hpp"    // for AssignmentSets
#include "tyr/datalog/consistency_graph.hpp"  // for Vertex
#include "tyr/datalog/declarations.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/datalog/formatter.hpp"
#include "tyr/datalog/kpkc.hpp"            // for for_e...
#include "tyr/datalog/kpkc_data.hpp"       // for Works...
#include "tyr/datalog/rule_scheduler.hpp"  // for RuleSchedulerStratum
#include "tyr/datalog/workspaces/facts.hpp"
#include "tyr/datalog/workspaces/program.hpp"
#include "tyr/datalog/workspaces/rule.hpp"
#include "tyr/datalog/workspaces/rule_delta.hpp"
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
#include <memory>     // for __sha...
#include <oneapi/tbb/parallel_for_each.h>
#include <tuple>    // for opera...
#include <utility>  // for pair
#include <vector>   // for vector

namespace f = tyr::formalism;
namespace fd = tyr::formalism::datalog;

namespace tyr::datalog
{

template<f::FactKind T, fd::Context C_SRC, fd::Context C_DST>
bool is_valid_binding(View<Index<fd::Literal<T>>, C_SRC> element, const FactSets& fact_sets, fd::GrounderContext<C_DST>& context)
{
    return fact_sets.template get<T>().predicate.contains(ground(element.get_atom(), context).first) == element.get_polarity();
}

template<f::FactKind T, fd::Context C_SRC, fd::Context C_DST>
bool is_valid_binding(View<IndexList<fd::Literal<T>>, C_SRC> elements, const FactSets& fact_sets, fd::GrounderContext<C_DST>& context)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_valid_binding(arg, fact_sets, context); });
}

template<fd::Context C_SRC, fd::Context C_DST>
bool is_valid_binding(View<DataList<fd::Literal<f::FluentTag>>, C_SRC> elements, const FactSets& fact_sets, fd::GrounderContext<C_DST>& context)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_valid_binding(arg, fact_sets, context); });
}

template<fd::Context C_SRC, fd::Context C_DST>
bool is_valid_binding(View<Data<fd::BooleanOperator<Data<fd::FunctionExpression>>>, C_SRC> element,
                      const FactSets& fact_sets,
                      fd::GrounderContext<C_DST>& context)
{
    return evaluate(make_view(ground(element, context), context.destination), fact_sets);
}

template<fd::Context C_SRC, fd::Context C_DST>
bool is_valid_binding(View<DataList<fd::BooleanOperator<Data<fd::FunctionExpression>>>, C_SRC> elements,
                      const FactSets& fact_sets,
                      fd::GrounderContext<C_DST>& context)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_valid_binding(arg, fact_sets, context); });
}

template<fd::Context C_SRC, fd::Context C_DST>
bool is_valid_binding(View<Index<fd::ConjunctiveCondition>, C_SRC> element, const FactSets& fact_sets, fd::GrounderContext<C_DST>& context)
{
    return is_valid_binding(element.template get_literals<f::StaticTag>(), fact_sets, context)     //
           && is_valid_binding(element.template get_literals<f::FluentTag>(), fact_sets, context)  //
           && is_valid_binding(element.get_numeric_constraints(), fact_sets, context);
}

static auto create_nullary_ground_head_in_delta(View<Index<fd::Atom<f::FluentTag>>, fd::Repository> head, fd::GrounderContext<fd::Repository>& context)
{
    context.binding.clear();

    return ground(head, context);
}

static auto create_unary_ground_head_in_delta(uint_t vertex_index,
                                              const StaticConsistencyGraph& consistency_graph,
                                              View<Index<fd::Atom<f::FluentTag>>, fd::Repository> head,
                                              fd::GrounderContext<fd::Repository>& context)
{
    context.binding.clear();

    const auto& vertex = consistency_graph.get_vertex(vertex_index);
    assert(uint_t(vertex.get_parameter_index()) == 0);
    context.binding.push_back(vertex.get_object_index());

    return ground(head, context);
}

static auto create_general_ground_head_in_delta(const std::vector<uint_t>& clique,
                                                const StaticConsistencyGraph& consistency_graph,
                                                View<Index<fd::Atom<f::FluentTag>>, fd::Repository> head,
                                                fd::GrounderContext<fd::Repository>& context)
{
    context.binding.resize(clique.size());
    for (const auto vertex_index : clique)
    {
        const auto& vertex = consistency_graph.get_vertex(vertex_index);
        assert(uint_t(vertex.get_parameter_index()) < clique.size());
        context.binding[uint_t(vertex.get_parameter_index())] = vertex.get_object_index();
    }

    return ground(head, context);
}

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP>
struct GenerateContext
{
    Index<fd::Rule> rule;
    ProgramWorkspace& ws;
    const ConstProgramWorkspace& cws;
    AnnotationPolicies<OrAP, AndAP>& aps;

    /// Workspaces
    RuleIterationWorkspace& ws_rule;
    const ConstRuleWorkspace& cws_rule;
    RuleDeltaWorkspace& ws_rule_delta;
    RulePersistentWorkspace& ws_rule_persistent;
    WorkerWorkspace& ws_worker;

    /// Annotations
    AndAP& and_ap;
    AndAnnotationsMap& and_annot;
    HeadToWitness& delta_head_to_witness;

    // Derivatives
    FactSets fact_sets;
    fd::GrounderContext<fd::Repository> ground_context_delta;
    fd::GrounderContext<f::OverlayRepository<fd::Repository>> ground_context_iteration;
    fd::GrounderContext<f::OverlayRepository<fd::Repository>> ground_context_persistent;

    GenerateContext(Index<fd::Rule> rule, ProgramWorkspace& ws, const ConstProgramWorkspace& cws, AnnotationPolicies<OrAP, AndAP>& aps) :
        rule(rule),
        ws(ws),
        cws(cws),
        aps(aps),
        ws_rule(ws.rules[uint_t(rule)]),
        cws_rule(cws.rules[uint_t(rule)]),
        ws_rule_delta(ws.rule_deltas[uint_t(rule)]),
        ws_rule_persistent(ws.rule_persistents[uint_t(rule)]),
        ws_worker(ws.worker.local()),
        and_ap(aps.and_aps[uint_t(rule)]),
        and_annot(aps.and_annots[uint_t(rule)]),
        delta_head_to_witness(aps.delta_head_to_witness[uint_t(rule)]),
        fact_sets(FactSets(cws.facts.fact_sets, ws.facts.fact_sets)),
        ground_context_delta(fd::GrounderContext { ws_worker.builder, *ws_rule_delta.repository, ws_rule_delta.binding }),
        ground_context_iteration(fd::GrounderContext { ws_worker.builder, ws_rule.overlay_repository, ws_rule_delta.binding }),
        ground_context_persistent(fd::GrounderContext { ws_worker.builder, ws_rule_persistent.overlay_repository, ws_rule_delta.binding })
    {
        ws_worker.clear();
        ws_rule.clear();
        ws_rule.initialize(cws_rule.static_consistency_graph, AssignmentSets { cws.facts.assignment_sets, ws.facts.assignment_sets });
    }
};

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP>
void generate_nullary_case(GenerateContext<OrAP, AndAP>& gc)
{
    const auto head_index = create_nullary_ground_head_in_delta(gc.cws_rule.get_rule().get_head(), gc.ground_context_delta).first;

    const auto exists = gc.ws_rule_delta.heads.contains(head_index);

    if (!exists || AndAP::ShouldAnnotate)
    {
        // Note: we never go through the consistency graph, and hence, have to check validity on the entire rule body.
        if (is_valid_binding(gc.cws_rule.get_rule().get_body(), gc.fact_sets, gc.ground_context_iteration))
        {
            if (!exists)
            {
                gc.ws_rule_delta.heads.insert(head_index);
                gc.ws_rule.heads.push_back(head_index);
            }

            gc.and_ap.update_annotation(gc.cws_rule.rule,
                                        gc.cws_rule.witness_condition,
                                        head_index,
                                        gc.aps.or_annot,
                                        gc.and_annot,
                                        gc.delta_head_to_witness,
                                        gc.ground_context_iteration,
                                        gc.ground_context_delta,
                                        gc.ground_context_persistent);
        }
    }
}

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP>
void generate_unary_case(GenerateContext<OrAP, AndAP>& gc)
{
    for (const auto vertex_index : gc.ws_rule.kpkc_workspace.consistent_vertices_vec)
    {
        const auto head_index =
            create_unary_ground_head_in_delta(vertex_index, gc.cws_rule.static_consistency_graph, gc.cws_rule.get_rule().get_head(), gc.ground_context_delta)
                .first;

        const auto exists = gc.ws_rule_delta.heads.contains(head_index);

        if (!exists || AndAP::ShouldAnnotate)
        {
            if (is_valid_binding(gc.cws_rule.get_unary_conflicting_overapproximation_condition(), gc.fact_sets, gc.ground_context_iteration))
            {
                // Ensure that ground rule is truly applicable
                assert(
                    is_applicable(make_view(ground(gc.cws_rule.get_rule(), gc.ground_context_iteration).first, gc.ws_rule.overlay_repository), gc.fact_sets));

                if (!exists)
                {
                    gc.ws_rule_delta.heads.insert(head_index);
                    gc.ws_rule.heads.push_back(head_index);
                }

                gc.and_ap.update_annotation(gc.cws_rule.rule,
                                            gc.cws_rule.witness_condition,
                                            head_index,
                                            gc.aps.or_annot,
                                            gc.and_annot,
                                            gc.delta_head_to_witness,
                                            gc.ground_context_iteration,
                                            gc.ground_context_delta,
                                            gc.ground_context_persistent);
            }
        }
    }
}

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP>
void generate_general_case(GenerateContext<OrAP, AndAP>& gc)
{
    kpkc::for_each_k_clique(
        gc.ws_rule.consistency_graph,
        gc.ws_rule.kpkc_workspace,
        [&](auto&& clique)
        {
            const auto head_index =
                create_general_ground_head_in_delta(clique, gc.cws_rule.static_consistency_graph, gc.cws_rule.get_rule().get_head(), gc.ground_context_delta)
                    .first;

            const auto exists = gc.ws_rule_delta.heads.contains(head_index);

            if (!exists || AndAP::ShouldAnnotate)
            {
                if (is_valid_binding(gc.cws_rule.get_binary_conflicting_overapproximation_condition(), gc.fact_sets, gc.ground_context_iteration))
                {
                    // Ensure that ground rule is truly applicable
                    assert(is_applicable(make_view(ground(gc.cws_rule.get_rule(), gc.ground_context_iteration).first, gc.ws_rule.overlay_repository),
                                         gc.fact_sets));

                    if (!exists)
                    {
                        gc.ws_rule_delta.heads.insert(head_index);
                        gc.ws_rule.heads.push_back(head_index);
                    }

                    gc.and_ap.update_annotation(gc.cws_rule.rule,
                                                gc.cws_rule.witness_condition,
                                                head_index,
                                                gc.aps.or_annot,
                                                gc.and_annot,
                                                gc.delta_head_to_witness,
                                                gc.ground_context_iteration,
                                                gc.ground_context_delta,
                                                gc.ground_context_persistent);
                }
            }
        });
}

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP>
void generate(GenerateContext<OrAP, AndAP>& gc)
{
    if (!is_applicable(gc.cws_rule.get_nullary_condition(), gc.fact_sets))
        return;

    const auto arity = gc.cws_rule.get_rule().get_arity();

    if (arity == 0)
        generate_nullary_case(gc);
    else if (arity == 1)
        generate_unary_case(gc);
    else
        generate_general_case(gc);
}

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
void solve_bottom_up_for_stratum(StratumExecutionContext<OrAP, AndAP, TP> ctx)
{
    ctx.scheduler.activate_all();

    ctx.ctx.ws.cost_buckets.clear();

    while (true)
    {
        // std::cout << "Cost: " << ws.cost_buckets.current_cost() << std::endl;

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
            auto stopwatch = StopwatchScope(ctx.ctx.ws.statistics.ground_seq_total_time);

            oneapi::tbb::parallel_for_each(active_rules.begin(),
                                           active_rules.end(),
                                           [&](auto&& rule_index)
                                           {
                                               const auto i = uint_t(rule_index);

                                               auto stopwatch = StopwatchScope(ctx.ctx.ws.rules[i].statistics.ground_total_time);
                                               ++ctx.ctx.ws.rules[i].statistics.num_executions;

                                               auto generate_context = GenerateContext(rule_index, ctx.ctx.ws, ctx.ctx.cws, ctx.ctx.aps);

                                               generate(generate_context);
                                           });
        }

        /**
         * Sequential merge.
         */

        /// --- Sequentially combine results into a temporary staging repository to prevent modying the program's repository

        {
            const auto stopwatch = StopwatchScope(ctx.ctx.ws.statistics.merge_seq_total_time);

            // Clear current bucket to avoid duplicate handling
            ctx.ctx.ws.cost_buckets.clear_current();

            for (const auto rule_index : active_rules)
            {
                const auto i = uint_t(rule_index);

                auto merge_context = fd::MergeContext { ctx.ctx.ws.datalog_builder, ctx.ctx.ws.repository, ctx.ctx.ws.rule_deltas[i].merge_cache };

                for (const auto delta_head : ctx.ctx.ws.rules[i].heads)
                {
                    // Merge head from delta into the program
                    const auto program_head = fd::merge_d2d(make_view(delta_head, *ctx.ctx.ws.rule_deltas[i].repository), merge_context).first;

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
                }
            }
        }

        ctx.scheduler.on_finish_iteration();
    }
}

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
void solve_bottom_up(ProgramExecutionContext<OrAP, AndAP, TP>& ctx)
{
    ctx.on_solve_bottom_up();

    for (auto stratum_ctx : ctx.get_stratum_execution_contexts())
        solve_bottom_up_for_stratum(stratum_ctx);
}

template void solve_bottom_up(ProgramExecutionContext<NoOrAnnotationPolicy, NoAndAnnotationPolicy, NoTerminationPolicy>& ctx);

template void solve_bottom_up(ProgramExecutionContext<OrAnnotationPolicy, AndAnnotationPolicy<SumAggregation>, NoTerminationPolicy>& ctx);

template void solve_bottom_up(ProgramExecutionContext<OrAnnotationPolicy, AndAnnotationPolicy<SumAggregation>, TerminationPolicy<SumAggregation>>& ctx);

template void solve_bottom_up(ProgramExecutionContext<OrAnnotationPolicy, AndAnnotationPolicy<MaxAggregation>, NoTerminationPolicy>& ctx);

template void solve_bottom_up(ProgramExecutionContext<OrAnnotationPolicy, AndAnnotationPolicy<MaxAggregation>, TerminationPolicy<MaxAggregation>>& ctx);

}
