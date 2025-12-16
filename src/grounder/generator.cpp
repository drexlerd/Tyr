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

static auto create_empty_binding_in_stage(MergeContext<OverlayRepository<Repository>, Repository>& context)
{
    auto binding_ptr = context.builder.get_builder<Binding>();
    auto& binding = *binding_ptr;
    binding.clear();

    canonicalize(binding);
    return context.destination.get_or_create(binding, context.builder.get_buffer()).first;
}

static auto create_unary_binding_in_stage(uint_t vertex_index,
                                          const StaticConsistencyGraph<Repository, ConjunctiveCondition>& consistency_graph,
                                          MergeContext<OverlayRepository<Repository>, Repository>& context)
{
    auto binding_ptr = context.builder.get_builder<Binding>();
    auto& binding = *binding_ptr;
    binding.clear();

    const auto& vertex = consistency_graph.get_vertex(vertex_index);
    assert(uint_t(vertex.get_parameter_index()) == 0);
    binding.objects.push_back(vertex.get_object_index());

    canonicalize(binding);
    return context.destination.get_or_create(binding, context.builder.get_buffer()).first;
}

static auto create_general_binding_in_stage(const std::vector<uint_t>& clique,
                                            const StaticConsistencyGraph<Repository, ConjunctiveCondition>& consistency_graph,
                                            MergeContext<OverlayRepository<Repository>, Repository>& context)
{
    auto binding_ptr = context.builder.get_builder<Binding>();
    auto& binding = *binding_ptr;
    binding.clear();

    binding.objects.resize(clique.size());
    for (const auto vertex_index : clique)
    {
        const auto& vertex = consistency_graph.get_vertex(vertex_index);
        assert(uint_t(vertex.get_parameter_index()) < clique.size());
        binding.objects[uint_t(vertex.get_parameter_index())] = vertex.get_object_index();
    }

    canonicalize(binding);
    return context.destination.get_or_create(binding, context.builder.get_buffer()).first;
}

void ground_nullary_case(const FactsExecutionContext& fact_execution_context,
                         RuleExecutionContext& rule_execution_context,
                         RuleStageExecutionContext& rule_stage_execution_context,
                         ThreadExecutionContext& thread_execution_context)
{
    auto merge_context_rule = MergeContext<Repository, OverlayRepository<Repository>> { thread_execution_context.builder,
                                                                                        rule_execution_context.overlay_repository,
                                                                                        rule_execution_context.merge_cache };

    auto merge_context_stage = MergeContext<OverlayRepository<Repository>, Repository> { thread_execution_context.builder,
                                                                                         *rule_stage_execution_context.repository,
                                                                                         rule_stage_execution_context.merge_cache };

    /// --- Rule stage
    const auto binding_stage = create_empty_binding_in_stage(merge_context_stage);

    if (!rule_stage_execution_context.bindings.contains(binding_stage))
    {
        rule_stage_execution_context.bindings.insert(binding_stage);

        /// --- Rule stage -> Rule
        const auto binding_rule = merge(binding_stage, merge_context_rule);

        auto ground_context_rule = GrounderContext<Repository, OverlayRepository<Repository>> { thread_execution_context.builder,
                                                                                                rule_execution_context.overlay_repository,
                                                                                                binding_rule,
                                                                                                rule_execution_context.grounder_cache };

        /// --- Rule
        auto ground_rule = ground_datalog(rule_execution_context.rule, ground_context_rule);

        if (is_applicable(ground_rule, fact_execution_context.fact_sets))
        {
            // std::cout << ground_rule << std::endl;

            rule_execution_context.bindings.push_back(binding_stage);
        }
    }
}

void ground_unary_case(const FactsExecutionContext& fact_execution_context,
                       RuleExecutionContext& rule_execution_context,
                       RuleStageExecutionContext& rule_stage_execution_context,
                       ThreadExecutionContext& thread_execution_context)
{
    auto merge_context_rule = MergeContext<Repository, OverlayRepository<Repository>> { thread_execution_context.builder,
                                                                                        rule_execution_context.overlay_repository,
                                                                                        rule_execution_context.merge_cache };

    auto merge_context_stage = MergeContext<OverlayRepository<Repository>, Repository> { thread_execution_context.builder,
                                                                                         *rule_stage_execution_context.repository,
                                                                                         rule_stage_execution_context.merge_cache };

    for (const auto vertex_index : rule_execution_context.kpkc_workspace.consistent_vertices_vec)
    {
        /// --- Rule stage
        const auto binding_stage = create_unary_binding_in_stage(vertex_index, rule_execution_context.static_consistency_graph, merge_context_stage);

        if (!rule_stage_execution_context.bindings.contains(binding_stage))
        {
            rule_stage_execution_context.bindings.insert(binding_stage);

            /// --- Rule stage -> Rule
            const auto binding_rule = merge(binding_stage, merge_context_rule);

            auto ground_context_rule = GrounderContext<Repository, OverlayRepository<Repository>> { thread_execution_context.builder,
                                                                                                    rule_execution_context.overlay_repository,
                                                                                                    binding_rule,
                                                                                                    rule_execution_context.grounder_cache };

            /// --- Rule
            auto ground_rule = ground_datalog(rule_execution_context.rule, ground_context_rule);

            if (is_applicable(ground_rule, fact_execution_context.fact_sets))
            {
                // std::cout << ground_rule << std::endl;

                rule_execution_context.bindings.push_back(binding_stage);
            }
        }
    }
}

void ground_general_case(const FactsExecutionContext& fact_execution_context,
                         RuleExecutionContext& rule_execution_context,
                         RuleStageExecutionContext& rule_stage_execution_context,
                         ThreadExecutionContext& thread_execution_context)
{
    auto merge_context_rule = MergeContext<Repository, OverlayRepository<Repository>> { thread_execution_context.builder,
                                                                                        rule_execution_context.overlay_repository,
                                                                                        rule_execution_context.merge_cache };

    auto merge_context_stage = MergeContext<OverlayRepository<Repository>, Repository> { thread_execution_context.builder,
                                                                                         *rule_stage_execution_context.repository,
                                                                                         rule_stage_execution_context.merge_cache };

    kpkc::for_each_k_clique(rule_execution_context.consistency_graph,
                            rule_execution_context.kpkc_workspace,
                            [&](auto&& clique)
                            {
                                /// --- Rule stage
                                const auto binding_stage =
                                    create_general_binding_in_stage(clique, rule_execution_context.static_consistency_graph, merge_context_stage);

                                if (!rule_stage_execution_context.bindings.contains(binding_stage))
                                {
                                    rule_stage_execution_context.bindings.insert(binding_stage);

                                    /// --- Rule stage -> Rule
                                    const auto binding_rule = merge(binding_stage, merge_context_rule);

                                    auto ground_context_rule =
                                        GrounderContext<Repository, OverlayRepository<Repository>> { thread_execution_context.builder,
                                                                                                     rule_execution_context.overlay_repository,
                                                                                                     binding_rule,
                                                                                                     rule_execution_context.grounder_cache };

                                    /// --- Rule
                                    auto ground_rule = ground_datalog(rule_execution_context.rule, ground_context_rule);

                                    if (is_applicable(ground_rule, fact_execution_context.fact_sets))
                                    {
                                        // std::cout << ground_rule << std::endl;

                                        rule_execution_context.bindings.push_back(binding_stage);
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
