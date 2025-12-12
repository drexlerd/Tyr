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

#ifndef TYR_GROUNDER_GENERATOR_HPP_
#define TYR_GROUNDER_GENERATOR_HPP_

#include "tyr/grounder/generator.hpp"

#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/ground.hpp"
#include "tyr/formalism/merge.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/grounder/applicability.hpp"
#include "tyr/grounder/declarations.hpp"
#include "tyr/grounder/execution_contexts.hpp"
#include "tyr/grounder/fact_sets.hpp"
#include "tyr/grounder/facts_view.hpp"
#include "tyr/grounder/formatter.hpp"
#include "tyr/grounder/kpkc.hpp"

namespace tyr::grounder
{

void ground_nullary_case(const FactsExecutionContext& fact_execution_context,
                         RuleExecutionContext& rule_execution_context,
                         ThreadExecutionContext& thread_execution_context)
{
    thread_execution_context.binding.clear();
    const auto binding = make_view(thread_execution_context.binding, rule_execution_context.repository);

    const auto fact_sets_adapter = FactsView(fact_execution_context.fact_sets);

    auto ground_rule = formalism::ground(rule_execution_context.rule, binding, thread_execution_context.builder, rule_execution_context.repository);

    const auto merge_binding = formalism::merge(ground_rule.get_binding(),
                                                thread_execution_context.builder,
                                                *rule_execution_context.stage_repository,
                                                rule_execution_context.stage_merge_cache);

    if (!rule_execution_context.all_bindings.contains(merge_binding))
    {
        rule_execution_context.all_bindings.insert(merge_binding);

        if (is_applicable(ground_rule, fact_sets_adapter))
        {
            // std::cout << ground_rule << std::endl;

            rule_execution_context.bindings.push_back(merge_binding);
        }
    }
}

void ground_unary_case(const FactsExecutionContext& fact_execution_context,
                       RuleExecutionContext& rule_execution_context,
                       ThreadExecutionContext& thread_execution_context)
{
    const auto fact_sets_adapter = FactsView(fact_execution_context.fact_sets);

    for (const auto vertex_index : rule_execution_context.kpkc_workspace.consistent_vertices_vec)
    {
        thread_execution_context.binding.clear();
        const auto& vertex = rule_execution_context.static_consistency_graph.get_vertex(vertex_index);

        assert(uint_t(vertex.get_parameter_index()) == 0);

        thread_execution_context.binding.push_back(vertex.get_object_index());

        const auto binding = make_view(thread_execution_context.binding, rule_execution_context.repository);

        auto ground_rule = formalism::ground(rule_execution_context.rule, binding, thread_execution_context.builder, rule_execution_context.repository);

        const auto merge_binding = formalism::merge(ground_rule.get_binding(),
                                                    thread_execution_context.builder,
                                                    *rule_execution_context.stage_repository,
                                                    rule_execution_context.stage_merge_cache);

        if (!rule_execution_context.all_bindings.contains(merge_binding))
        {
            rule_execution_context.all_bindings.insert(merge_binding);

            if (is_applicable(ground_rule, fact_sets_adapter))
            {
                // std::cout << ground_rule << std::endl;

                rule_execution_context.bindings.push_back(merge_binding);
            }
        }
    }
}

void ground_general_case(const FactsExecutionContext& fact_execution_context,
                         RuleExecutionContext& rule_execution_context,
                         ThreadExecutionContext& thread_execution_context)
{
    const auto fact_sets_adapter = FactsView(fact_execution_context.fact_sets);

    kpkc::for_each_k_clique(
        rule_execution_context.consistency_graph,
        rule_execution_context.kpkc_workspace,
        [&](auto&& clique)
        {
            thread_execution_context.binding.resize(clique.size());
            for (const auto vertex_index : clique)
            {
                const auto& vertex = rule_execution_context.static_consistency_graph.get_vertex(vertex_index);

                assert(uint_t(vertex.get_parameter_index()) < clique.size());

                thread_execution_context.binding[uint_t(vertex.get_parameter_index())] = vertex.get_object_index();
            }

            const auto binding = make_view(thread_execution_context.binding, rule_execution_context.repository);

            auto ground_rule = formalism::ground(rule_execution_context.rule, binding, thread_execution_context.builder, rule_execution_context.repository);

            const auto merge_binding = formalism::merge(ground_rule.get_binding(),
                                                        thread_execution_context.builder,
                                                        *rule_execution_context.stage_repository,
                                                        rule_execution_context.stage_merge_cache);

            if (!rule_execution_context.all_bindings.contains(merge_binding))
            {
                rule_execution_context.all_bindings.insert(merge_binding);

                if (is_applicable(ground_rule, fact_sets_adapter))
                {
                    // std::cout << ground_rule << std::endl;

                    rule_execution_context.bindings.push_back(merge_binding);
                }
            }
        });
}

void ground(const FactsExecutionContext& fact_execution_context, RuleExecutionContext& rule_execution_context, ThreadExecutionContext& thread_execution_context)
{
    const auto rule = rule_execution_context.rule;
    const auto& fact_sets = fact_execution_context.fact_sets;

    if (!nullary_conditions_hold(rule.get_body(), fact_sets))
        return;

    const auto arity = rule.get_body().get_arity();

    if (arity == 0)
        ground_nullary_case(fact_execution_context, rule_execution_context, thread_execution_context);
    else if (arity == 1)
        ground_unary_case(fact_execution_context, rule_execution_context, thread_execution_context);
    else
        ground_general_case(fact_execution_context, rule_execution_context, thread_execution_context);
}

}

#endif
