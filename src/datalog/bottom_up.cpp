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

template<AndAnnotationPolicyConcept AndAP>
struct GenerateContext
{
    /// Workspaces
    const FactsWorkspace& facts_ws;
    const ConstFactsWorkspace& const_facts_ws;
    RuleWorkspace& rule_ws;
    const ConstRuleWorkspace& const_rule_ws;
    RuleDeltaWorkspace& rule_delta_ws;
    WorkerWorkspace& worker_ws;

    /// Annotations
    AndAP& and_ap;
    const OrAnnotationsList& or_annot;
    AndAnnotationsMap& and_annot;
    HeadToWitness& head_to_witness;

    /// Derivatives
    FactSets fact_sets;
    fd::GrounderContext<fd::Repository> ground_context_delta;
    fd::GrounderContext<f::OverlayRepository<fd::Repository>> ground_context_rule;
};

template<AndAnnotationPolicyConcept AndAP>
void generate_nullary_case(GenerateContext<AndAP>& gc)
{
    const auto head = create_nullary_ground_head_in_delta(gc.const_rule_ws.get_rule().get_head(), gc.ground_context_delta).first;

    const auto exists = gc.rule_delta_ws.heads.contains(head);

    if (!exists || AndAP::ShouldAnnotate)
    {
        // Note: we never go through the consistency graph, and hence, have to check validity on the entire rule body.
        if (is_valid_binding(gc.const_rule_ws.get_rule().get_body(), gc.fact_sets, gc.ground_context_rule))
        {
            if (!exists)
            {
                gc.rule_delta_ws.heads.insert(head);
                gc.rule_ws.heads.push_back(head);
            }

            gc.and_ap.annotate(gc.const_rule_ws.rule,
                               gc.const_rule_ws.fluent_rule,
                               gc.ground_context_delta.binding,
                               head,
                               gc.or_annot,
                               gc.and_annot,
                               gc.head_to_witness,
                               gc.ground_context_rule,
                               gc.ground_context_delta);
        }
    }
}

template<AndAnnotationPolicyConcept AndAP>
void generate_unary_case(GenerateContext<AndAP>& gc)
{
    for (const auto vertex_index : gc.rule_ws.kpkc_workspace.consistent_vertices_vec)
    {
        const auto head = create_unary_ground_head_in_delta(vertex_index,
                                                            gc.const_rule_ws.static_consistency_graph,
                                                            gc.const_rule_ws.get_rule().get_head(),
                                                            gc.ground_context_delta)
                              .first;

        const auto exists = gc.rule_delta_ws.heads.contains(head);

        if (!exists || AndAP::ShouldAnnotate)
        {
            if (is_valid_binding(gc.const_rule_ws.get_unary_conflicting_overapproximation_condition(), gc.fact_sets, gc.ground_context_rule))
            {
                // Ensure that ground rule is truly applicable
                assert(
                    is_applicable(make_view(ground(gc.const_rule_ws.get_rule(), gc.ground_context_rule).first, gc.rule_ws.overlay_repository), gc.fact_sets));

                if (!exists)
                {
                    gc.rule_delta_ws.heads.insert(head);
                    gc.rule_ws.heads.push_back(head);
                }

                gc.and_ap.annotate(gc.const_rule_ws.rule,
                                   gc.const_rule_ws.fluent_rule,
                                   gc.ground_context_delta.binding,
                                   head,
                                   gc.or_annot,
                                   gc.and_annot,
                                   gc.head_to_witness,
                                   gc.ground_context_rule,
                                   gc.ground_context_delta);

                // TODO: must also store witness for head with minimal cost to make sequential update cheap
            }
        }
    }
}

template<AndAnnotationPolicyConcept AndAP>
void generate_general_case(GenerateContext<AndAP>& gc)
{
    kpkc::for_each_k_clique(
        gc.rule_ws.consistency_graph,
        gc.rule_ws.kpkc_workspace,
        [&](auto&& clique)
        {
            const auto head = create_general_ground_head_in_delta(clique,
                                                                  gc.const_rule_ws.static_consistency_graph,
                                                                  gc.const_rule_ws.get_rule().get_head(),
                                                                  gc.ground_context_delta)
                                  .first;

            const auto exists = gc.rule_delta_ws.heads.contains(head);

            if (!exists || AndAP::ShouldAnnotate)
            {
                if (is_valid_binding(gc.const_rule_ws.get_binary_conflicting_overapproximation_condition(), gc.fact_sets, gc.ground_context_rule))
                {
                    // Ensure that ground rule is truly applicable
                    assert(is_applicable(make_view(ground(gc.const_rule_ws.get_rule(), gc.ground_context_rule).first, gc.rule_ws.overlay_repository),
                                         gc.fact_sets));

                    if (!exists)
                    {
                        gc.rule_delta_ws.heads.insert(head);
                        gc.rule_ws.heads.push_back(head);
                    }

                    gc.and_ap.annotate(gc.const_rule_ws.rule,
                                       gc.const_rule_ws.fluent_rule,
                                       gc.ground_context_delta.binding,
                                       head,
                                       gc.or_annot,
                                       gc.and_annot,
                                       gc.head_to_witness,
                                       gc.ground_context_rule,
                                       gc.ground_context_delta);
                }
            }
        });
}

template<AndAnnotationPolicyConcept AndAP>
void generate(GenerateContext<AndAP>& gc)
{
    if (!is_applicable(gc.const_rule_ws.get_nullary_condition(), gc.fact_sets))
        return;

    const auto arity = gc.const_rule_ws.get_rule().get_arity();

    if (arity == 0)
        generate_nullary_case(gc);
    else if (arity == 1)
        generate_unary_case(gc);
    else
        generate_general_case(gc);
}

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicy TP>
void solve_bottom_up_for_stratum(RuleSchedulerStratum& scheduler,
                                 ProgramWorkspace& ws,
                                 const ConstProgramWorkspace& cws,
                                 AnnotationPolicies<OrAP, AndAP>& aps,
                                 TP& tp)
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

            oneapi::tbb::parallel_for_each(
                active_rules.begin(),
                active_rules.end(),
                [&](auto&& rule_index)
                {
                    const auto i = uint_t(rule_index);
                    auto& facts_ws = ws.facts;
                    const auto& const_facts_ws = cws.facts;
                    auto& rule_ws = ws.rules[i];
                    const auto& const_rule_ws = cws.rules[i];
                    auto& rule_delta_ws = ws.rule_deltas[i];
                    auto& worker_ws = ws.worker.local();
                    auto& and_ap = aps.and_aps[i];
                    const auto& or_annot = aps.or_annot;
                    auto& and_annot = aps.and_annots[i];
                    auto& head_to_witness = aps.head_to_witnesss[i];

                    {
                        ///--- Initialization phase
                        auto stopwatch = StopwatchScope(rule_ws.statistics.init_total_time);

                        worker_ws.clear();
                        rule_ws.clear();
                        rule_ws.initialize(const_rule_ws.static_consistency_graph, AssignmentSets { const_facts_ws.assignment_sets, facts_ws.assignment_sets });
                    }

                    {
                        /// --- Grounding phase
                        auto stopwatch = StopwatchScope(rule_ws.statistics.ground_total_time);
                        ++rule_ws.statistics.num_executions;

                        auto generate_context =
                            GenerateContext { facts_ws,
                                              const_facts_ws,
                                              rule_ws,
                                              const_rule_ws,
                                              rule_delta_ws,
                                              worker_ws,
                                              and_ap,
                                              or_annot,
                                              and_annot,
                                              head_to_witness,
                                              FactSets(const_facts_ws.fact_sets, facts_ws.fact_sets),
                                              fd::GrounderContext { worker_ws.builder, *rule_delta_ws.repository, rule_delta_ws.binding },
                                              fd::GrounderContext { worker_ws.builder, rule_ws.overlay_repository, rule_delta_ws.binding } };

                        generate(generate_context);
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

                auto& or_ap = aps.or_ap;
                auto& or_annot = aps.or_annot;
                const auto& and_annot = aps.and_annots[i];
                const auto& head_to_witness = aps.head_to_witnesss[i];

                auto merge_context = fd::MergeContext { ws.datalog_builder, ws.repository, rule_delta_ws.merge_cache };

                for (const auto ground_head_index : rule_ws.heads)
                {
                    /// --- Program

                    // Note: the head lives in the stage but its index is stored in rule because it is reset each iteration
                    const auto ground_head_rule = make_view(ground_head_index, *rule_delta_ws.repository);

                    // Merge it into the program
                    const auto ground_head_index_program = fd::merge_d2d(ground_head_rule, merge_context).first;

                    // Insert new fact into fact sets and assigment sets
                    if (!fluent_predicate_fact_sets.contains(ground_head_index_program))
                    {
                        discovered_new_fact = true;

                        const auto ground_head_program = make_view(ground_head_index_program, ws.repository);

                        scheduler.on_generate(ground_head_program.get_predicate().get_index());

                        fluent_predicate_fact_sets.insert(ground_head_program);
                        fluent_predicate_assignment_sets.insert(ground_head_program);

                        ++ws.statistics.num_merges_inserted;

                        // TODO: we have to take the minimum among all rules that achieve the head
                        // We should store in the rule delta the witness that achieves the head with minimal cost
                        or_ap.annotate(rule_index, ground_head_index_program, or_annot, and_annot, head_to_witness);

                        if (tp.achieve(ground_head_index_program))
                            return;
                    }
                    else
                    {
                        ++ws.statistics.num_merges_discarded;
                    }
                }
            }

            if (!discovered_new_fact)
                return;  ///< Reached fixed point
        }

        scheduler.on_finish_iteration();
    }
}

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicy TP>
void solve_bottom_up(ProgramWorkspace& ws, const ConstProgramWorkspace& cws, AnnotationPolicies<OrAP, AndAP>& aps, TP& tp)
{
    for (auto& rule_delta_ws : ws.rule_deltas)
        rule_delta_ws.clear();
    aps.clear();
    tp.clear();

    for (auto& scheduler : ws.schedulers.data)
        solve_bottom_up_for_stratum(scheduler, ws, cws, aps, tp);
}

template void solve_bottom_up(ProgramWorkspace& ws,
                              const ConstProgramWorkspace& cws,
                              AnnotationPolicies<NoOrAnnotationPolicy, NoAndAnnotationPolicy>& aps,
                              NoTerminationPolicy& tp);

template void solve_bottom_up(ProgramWorkspace& ws,
                              const ConstProgramWorkspace& cws,
                              AnnotationPolicies<OrAnnotationPolicy, AndAnnotationPolicy<SumAggregation>>& aps,
                              NoTerminationPolicy& tp);

}
