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

#include "tyr/grounder/generator.hpp"

#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/grounder_datalog.hpp"
#include "tyr/formalism/merge_datalog.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/grounder/applicability.hpp"
#include "tyr/grounder/declarations.hpp"
#include "tyr/grounder/execution_contexts.hpp"
#include "tyr/grounder/fact_sets.hpp"
#include "tyr/grounder/formatter.hpp"
#include "tyr/grounder/kpkc.hpp"

using namespace tyr::formalism;

namespace tyr::grounder
{

static auto create_nullary_ground_head_in_stage(View<Index<Atom<FluentTag>>, Repository> head, GrounderContext<Repository>& context)
{
    context.binding.clear();

    return ground_datalog(head, context);
}

static auto create_unary_ground_head_in_stage(uint_t vertex_index,
                                              const StaticConsistencyGraph<Repository, ConjunctiveCondition>& consistency_graph,
                                              View<Index<Atom<FluentTag>>, Repository> head,
                                              GrounderContext<Repository>& context)
{
    context.binding.clear();

    const auto& vertex = consistency_graph.get_vertex(vertex_index);
    assert(uint_t(vertex.get_parameter_index()) == 0);
    context.binding.push_back(vertex.get_object_index());

    return ground_datalog(head, context);
}

static auto create_general_ground_head_in_stage(const std::vector<uint_t>& clique,
                                                const StaticConsistencyGraph<Repository, ConjunctiveCondition>& consistency_graph,
                                                View<Index<Atom<FluentTag>>, Repository> head,
                                                GrounderContext<Repository>& context)
{
    context.binding.resize(clique.size());
    for (const auto vertex_index : clique)
    {
        const auto& vertex = consistency_graph.get_vertex(vertex_index);
        assert(uint_t(vertex.get_parameter_index()) < clique.size());
        context.binding[uint_t(vertex.get_parameter_index())] = vertex.get_object_index();
    }

    return ground_datalog(head, context);
}

void ground_nullary_case(const FactsExecutionContext& fact_execution_context,
                         RuleExecutionContext& rule_execution_context,
                         RuleStageExecutionContext& rule_stage_execution_context,
                         ThreadExecutionContext& thread_execution_context)
{
    auto ground_context_stage =
        GrounderContext { thread_execution_context.builder, *rule_stage_execution_context.repository, rule_stage_execution_context.binding };

    /// --- Rule stage
    const auto ground_head = create_nullary_ground_head_in_stage(rule_execution_context.rule.get_head(), ground_context_stage).first;

    if (rule_stage_execution_context.ground_heads.contains(ground_head))
        return;

    if (const auto it = rule_stage_execution_context.ground_heads_inapplicable.find(ground_head);
        it != rule_stage_execution_context.ground_heads_inapplicable.end())
    {
        const auto ground_rule = make_view(it->second, *rule_stage_execution_context.repository);

        if (is_applicable(ground_rule, fact_execution_context.fact_sets))
        {
            // std::cout << ground_rule << std::endl;

            rule_stage_execution_context.ground_heads.insert(ground_head);
            rule_execution_context.ground_heads.push_back(ground_head);
        }
    }
    else
    {
        /// --- Rule
        auto ground_context_rule =
            GrounderContext { thread_execution_context.builder, *rule_stage_execution_context.repository, rule_stage_execution_context.binding };

        const auto ground_rule_index = ground_datalog(rule_execution_context.rule, ground_context_rule).first;

        const auto ground_rule = make_view(ground_rule_index, *rule_stage_execution_context.repository);

        if (is_applicable(ground_rule, fact_execution_context.fact_sets))
        {
            // std::cout << ground_rule << std::endl;

            rule_stage_execution_context.ground_heads.insert(ground_head);
            rule_execution_context.ground_heads.push_back(ground_head);
        }
        else
        {
            rule_stage_execution_context.ground_heads_inapplicable.emplace(ground_head, ground_rule_index);
        }
    }
}

void ground_unary_case(const FactsExecutionContext& fact_execution_context,
                       RuleExecutionContext& rule_execution_context,
                       RuleStageExecutionContext& rule_stage_execution_context,
                       ThreadExecutionContext& thread_execution_context)
{
    auto ground_context_stage =
        GrounderContext { thread_execution_context.builder, *rule_stage_execution_context.repository, rule_stage_execution_context.binding };

    for (const auto vertex_index : rule_execution_context.kpkc_workspace.consistent_vertices_vec)
    {
        /// --- Rule stage
        const auto ground_head = create_unary_ground_head_in_stage(vertex_index,
                                                                   rule_execution_context.static_consistency_graph,
                                                                   rule_execution_context.rule.get_head(),
                                                                   ground_context_stage)
                                     .first;

        if (rule_stage_execution_context.ground_heads.contains(ground_head))
            continue;

        if (const auto it = rule_stage_execution_context.ground_heads_inapplicable.find(ground_head);
            it != rule_stage_execution_context.ground_heads_inapplicable.end())
        {
            const auto ground_rule = make_view(it->second, *rule_stage_execution_context.repository);

            if (is_applicable(ground_rule, fact_execution_context.fact_sets))
            {
                // std::cout << ground_rule << std::endl;

                rule_stage_execution_context.ground_heads.insert(ground_head);
                rule_execution_context.ground_heads.push_back(ground_head);
            }
        }
        else
        {
            /// --- Rule
            auto ground_context_rule =
                GrounderContext { thread_execution_context.builder, *rule_stage_execution_context.repository, rule_stage_execution_context.binding };

            const auto ground_rule_index = ground_datalog(rule_execution_context.rule, ground_context_rule).first;

            const auto ground_rule = make_view(ground_rule_index, *rule_stage_execution_context.repository);

            if (is_applicable(ground_rule, fact_execution_context.fact_sets))
            {
                // std::cout << ground_rule << std::endl;

                rule_stage_execution_context.ground_heads.insert(ground_head);
                rule_execution_context.ground_heads.push_back(ground_head);
            }
            else
            {
                rule_stage_execution_context.ground_heads_inapplicable.emplace(ground_head, ground_rule_index);
            }
        }
    }
}

void ground_general_case(const FactsExecutionContext& fact_execution_context,
                         RuleExecutionContext& rule_execution_context,
                         RuleStageExecutionContext& rule_stage_execution_context,
                         ThreadExecutionContext& thread_execution_context)
{
    auto ground_context_stage =
        GrounderContext { thread_execution_context.builder, *rule_stage_execution_context.repository, rule_stage_execution_context.binding };

    kpkc::for_each_k_clique(rule_execution_context.consistency_graph,
                            rule_execution_context.kpkc_workspace,
                            [&](auto&& clique)
                            {
                                /// --- Rule stage
                                const auto ground_head = create_general_ground_head_in_stage(clique,
                                                                                             rule_execution_context.static_consistency_graph,
                                                                                             rule_execution_context.rule.get_head(),
                                                                                             ground_context_stage)
                                                             .first;

                                if (rule_stage_execution_context.ground_heads.contains(ground_head))
                                    return;

                                if (const auto it = rule_stage_execution_context.ground_heads_inapplicable.find(ground_head);
                                    it != rule_stage_execution_context.ground_heads_inapplicable.end())
                                {
                                    const auto ground_rule = make_view(it->second, *rule_stage_execution_context.repository);

                                    if (is_applicable(ground_rule, fact_execution_context.fact_sets))
                                    {
                                        // std::cout << ground_rule << std::endl;

                                        rule_stage_execution_context.ground_heads.insert(ground_head);
                                        rule_execution_context.ground_heads.push_back(ground_head);
                                    }
                                }
                                else
                                {
                                    /// --- Rule
                                    auto ground_context_rule = GrounderContext { thread_execution_context.builder,
                                                                                 *rule_stage_execution_context.repository,
                                                                                 rule_stage_execution_context.binding };

                                    const auto ground_rule_index = ground_datalog(rule_execution_context.rule, ground_context_rule).first;

                                    const auto ground_rule = make_view(ground_rule_index, *rule_stage_execution_context.repository);

                                    if (is_applicable(ground_rule, fact_execution_context.fact_sets))
                                    {
                                        // std::cout << ground_rule << std::endl;

                                        rule_stage_execution_context.ground_heads.insert(ground_head);
                                        rule_execution_context.ground_heads.push_back(ground_head);
                                    }
                                    else
                                    {
                                        rule_stage_execution_context.ground_heads_inapplicable.emplace(ground_head, ground_rule_index);
                                    }
                                }
                            });
}

void ground(const FactsExecutionContext& fact_execution_context,
            RuleExecutionContext& rule_execution_context,
            RuleStageExecutionContext& rule_stage_execution_context,
            ThreadExecutionContext& thread_execution_context)
{
    if (!is_applicable(rule_execution_context.nullary_condition, fact_execution_context.fact_sets))
        return;

    const auto arity = rule_execution_context.rule.get_arity();

    if (arity == 0)
        ground_nullary_case(fact_execution_context, rule_execution_context, rule_stage_execution_context, thread_execution_context);
    else if (arity == 1)
        ground_unary_case(fact_execution_context, rule_execution_context, rule_stage_execution_context, thread_execution_context);
    else
        ground_general_case(fact_execution_context, rule_execution_context, rule_stage_execution_context, thread_execution_context);
}

}
