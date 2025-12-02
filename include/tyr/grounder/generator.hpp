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

#include "tyr/formalism/formalism.hpp"
#include "tyr/grounder/applicability.hpp"
#include "tyr/grounder/declarations.hpp"
#include "tyr/grounder/kpkc.hpp"
#include "tyr/grounder/workspace.hpp"

namespace tyr::grounder
{

template<formalism::FactKind T>
View<Index<formalism::GroundAtom<T>>, formalism::OverlayRepository<formalism::Repository>>
ground(View<Index<formalism::Atom<T>>, formalism::Repository> element,
       RuleExecutionContext& rule_execution_context,
       ThreadExecutionContext& thread_execution_context)
{
    // Fetch and clear
    auto& binding = thread_execution_context.binding;
    auto& builder = thread_execution_context.builder;
    auto& repository = rule_execution_context.repository;
    auto& buffer = builder.buffer;
    auto& atom = builder.template get_ground_atom<T>();
    atom.clear();

    auto& objects = atom.objects;

    // Fill data
    atom.predicate = element.get_predicate().get_index();
    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, formalism::ParameterIndex>)
                {
                    objects.push_back(binding[uint_t(arg)]);
                }
                else if constexpr (std::is_same_v<Alternative, View<Index<formalism::Object>, formalism::Repository>>)
                {
                    objects.push_back(arg.get_index());
                }
                else
                {
                    static_assert(dependent_false<Alternative>::value, "Missing case");
                }
            },
            term.get_variant());
    }

    // Canonicalize and Serialize
    canonicalize(atom);
    return repository.get_or_create(atom, buffer).first;
}

template<formalism::FactKind T>
View<Index<formalism::GroundLiteral<T>>, formalism::OverlayRepository<formalism::Repository>>
ground(View<Index<formalism::Literal<T>>, formalism::Repository> element,
       RuleExecutionContext& rule_execution_context,
       ThreadExecutionContext& thread_execution_context)
{
    // Fetch and clear
    auto& builder = thread_execution_context.builder;
    auto& repository = rule_execution_context.repository;
    auto& buffer = builder.buffer;
    auto& ground_literal = builder.template get_ground_literal<T>();
    ground_literal.clear();

    // Fill data
    ground_literal.polarity = element.get_polarity();
    ground_literal.atom = ground(element.get_atom(), rule_execution_context, thread_execution_context).get_index();

    // Canonicalize and Serialize
    canonicalize(ground_literal);
    return repository.get_or_create(ground_literal, buffer).first;
}

template<formalism::FactKind T>
View<Index<formalism::GroundFunctionTerm<T>>, formalism::OverlayRepository<formalism::Repository>>
ground(View<Index<formalism::FunctionTerm<T>>, formalism::Repository> element,
       RuleExecutionContext& rule_execution_context,
       ThreadExecutionContext& thread_execution_context)
{
    // Fetch and clear
    auto& binding = thread_execution_context.binding;
    auto& builder = thread_execution_context.builder;
    auto& repository = rule_execution_context.repository;
    auto& buffer = builder.buffer;
    auto& fterm = builder.template get_ground_fterm<T>();
    fterm.clear();

    auto& objects = fterm.objects;

    // Fill data
    fterm.function = element.get_function().get_index();
    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, formalism::ParameterIndex>)
                {
                    objects.push_back(binding[uint_t(arg)]);
                }
                else if constexpr (std::is_same_v<Alternative, View<Index<formalism::Object>, formalism::Repository>>)
                {
                    objects.push_back(arg.get_index());
                }
                else
                {
                    static_assert(dependent_false<Alternative>::value, "Missing case");
                }
            },
            term.get_variant());
    }

    // Canonicalize and Serialize
    canonicalize(fterm);
    return repository.get_or_create(fterm, buffer).first;
}

View<Data<formalism::GroundFunctionExpression>, formalism::OverlayRepository<formalism::Repository>>
ground(View<Data<formalism::FunctionExpression>, formalism::Repository> element,
       RuleExecutionContext& rule_execution_context,
       ThreadExecutionContext& thread_execution_context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
            {
                return View<Data<formalism::GroundFunctionExpression>, formalism::OverlayRepository<formalism::Repository>>(
                    Data<formalism::GroundFunctionExpression>(arg),
                    rule_execution_context.repository);
            }
            else if constexpr (std::is_same_v<Alternative,
                                              View<Data<formalism::ArithmeticOperator<Data<formalism::FunctionExpression>>>, formalism::Repository>>)
            {
                return View<Data<formalism::GroundFunctionExpression>, formalism::OverlayRepository<formalism::Repository>>(
                    Data<formalism::GroundFunctionExpression>(ground(arg, rule_execution_context, thread_execution_context).get_data()),
                    rule_execution_context.repository);
            }
            else
            {
                return View<Data<formalism::GroundFunctionExpression>, formalism::OverlayRepository<formalism::Repository>>(
                    Data<formalism::GroundFunctionExpression>(ground(arg, rule_execution_context, thread_execution_context).get_index()),
                    rule_execution_context.repository);
            }
        },
        element.get_variant());
}

template<formalism::OpKind O>
View<Index<formalism::UnaryOperator<O, Data<formalism::GroundFunctionExpression>>>, formalism::OverlayRepository<formalism::Repository>>
ground(View<Index<formalism::UnaryOperator<O, Data<formalism::FunctionExpression>>>, formalism::Repository> element,
       RuleExecutionContext& rule_execution_context,
       ThreadExecutionContext& thread_execution_context)
{
    // Fetch and clear
    auto& builder = thread_execution_context.builder;
    auto& repository = rule_execution_context.repository;
    auto& buffer = builder.buffer;
    auto& unary = builder.template get_ground_unary<O>();
    unary.clear();

    // Fill data
    unary.arg = ground(element.get_arg(), rule_execution_context, thread_execution_context).get_data();

    // Canonicalize and Serialize
    canonicalize(unary);
    return repository.get_or_create(unary, buffer).first;
}

template<formalism::OpKind O>
View<Index<formalism::BinaryOperator<O, Data<formalism::GroundFunctionExpression>>>, formalism::OverlayRepository<formalism::Repository>>
ground(View<Index<formalism::BinaryOperator<O, Data<formalism::FunctionExpression>>>, formalism::Repository> element,
       RuleExecutionContext& rule_execution_context,
       ThreadExecutionContext& thread_execution_context)
{
    // Fetch and clear
    auto& builder = thread_execution_context.builder;
    auto& repository = rule_execution_context.repository;
    auto& buffer = builder.buffer;
    auto& binary = builder.template get_ground_binary<O>();
    binary.clear();

    // Fill data
    binary.lhs = ground(element.get_lhs(), rule_execution_context, thread_execution_context).get_data();
    binary.rhs = ground(element.get_rhs(), rule_execution_context, thread_execution_context).get_data();

    // Canonicalize and Serialize
    canonicalize(binary);
    return repository.get_or_create(binary, buffer).first;
}

template<formalism::OpKind O>
View<Index<formalism::MultiOperator<O, Data<formalism::GroundFunctionExpression>>>, formalism::OverlayRepository<formalism::Repository>>
ground(View<Index<formalism::MultiOperator<O, Data<formalism::FunctionExpression>>>, formalism::Repository> element,
       RuleExecutionContext& rule_execution_context,
       ThreadExecutionContext& thread_execution_context)
{
    // Fetch and clear
    auto& builder = thread_execution_context.builder;
    auto& repository = rule_execution_context.repository;
    auto& buffer = builder.buffer;
    auto& multi = builder.template get_ground_multi<O>();
    multi.clear();

    auto& args = multi.args;

    // Fill data
    for (const auto arg : element.get_args())
        args.push_back(ground(arg, rule_execution_context, thread_execution_context).get_data());

    // Canonicalize and Serialize
    canonicalize(multi);
    return repository.get_or_create(multi, buffer).first;
}

View<Data<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>>, formalism::OverlayRepository<formalism::Repository>>
ground(View<Data<formalism::BooleanOperator<Data<formalism::FunctionExpression>>>, formalism::Repository> element,
       RuleExecutionContext& rule_execution_context,
       ThreadExecutionContext& thread_execution_context)
{
    return visit(
        [&](auto&& arg)
        {
            return View<Data<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>>, formalism::OverlayRepository<formalism::Repository>>(
                Data<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>>(
                    ground(arg, rule_execution_context, thread_execution_context).get_index()),
                rule_execution_context.repository);
        },
        element.get_variant());
}

View<Data<formalism::ArithmeticOperator<Data<formalism::GroundFunctionExpression>>>, formalism::OverlayRepository<formalism::Repository>>
ground(View<Data<formalism::ArithmeticOperator<Data<formalism::FunctionExpression>>>, formalism::Repository> element,
       RuleExecutionContext& rule_execution_context,
       ThreadExecutionContext& thread_execution_context)
{
    return visit(
        [&](auto&& arg)
        {
            return View<Data<formalism::ArithmeticOperator<Data<formalism::GroundFunctionExpression>>>, formalism::OverlayRepository<formalism::Repository>>(
                Data<formalism::ArithmeticOperator<Data<formalism::GroundFunctionExpression>>>(
                    ground(arg, rule_execution_context, thread_execution_context).get_index()),
                rule_execution_context.repository);
        },
        element.get_variant());
}

View<Index<formalism::GroundConjunctiveCondition>, formalism::OverlayRepository<formalism::Repository>>
ground(View<Index<formalism::ConjunctiveCondition>, formalism::Repository> element,
       RuleExecutionContext& rule_execution_context,
       ThreadExecutionContext& thread_execution_context)
{
    // Fetch and clear
    auto& builder = thread_execution_context.builder;
    auto& repository = rule_execution_context.repository;
    auto& buffer = builder.buffer;
    auto& conj_cond = builder.get_ground_conj_cond();
    conj_cond.clear();

    auto& static_literals = conj_cond.static_literals;
    auto& fluent_literals = conj_cond.fluent_literals;
    auto& numeric_constraints = conj_cond.numeric_constraints;

    // Fill data
    for (const auto literal : element.template get_literals<formalism::StaticTag>())
        static_literals.push_back(ground(literal, rule_execution_context, thread_execution_context).get_index());
    for (const auto literal : element.template get_literals<formalism::FluentTag>())
        fluent_literals.push_back(ground(literal, rule_execution_context, thread_execution_context).get_index());
    for (const auto numeric_constraint : element.get_numeric_constraints())
        numeric_constraints.push_back(ground(numeric_constraint, rule_execution_context, thread_execution_context).get_data());

    // Canonicalize and Serialize
    canonicalize(conj_cond);
    return repository.get_or_create(conj_cond, buffer).first;
}

View<Index<formalism::GroundRule>, formalism::OverlayRepository<formalism::Repository>> ground(View<Index<formalism::Rule>, formalism::Repository> element,
                                                                                               RuleExecutionContext& rule_execution_context,
                                                                                               ThreadExecutionContext& thread_execution_context)
{
    // Fetch and clear
    auto& builder = thread_execution_context.builder;
    auto& repository = rule_execution_context.repository;
    auto& buffer = builder.buffer;
    auto& rule = builder.get_ground_rule();
    rule.clear();

    auto& body = rule.body;
    auto& head = rule.head;

    // Fill data
    body = ground(element.get_body(), rule_execution_context, thread_execution_context).get_index();
    head = ground(element.get_head(), rule_execution_context, thread_execution_context).get_index();

    // Canonicalize and Serialize
    canonicalize(rule);
    return repository.get_or_create(rule, buffer).first;
}

void ground_nullary_case(const FactsExecutionContext& fact_execution_context,
                         RuleExecutionContext& rule_execution_context,
                         ThreadExecutionContext& thread_execution_context)
{
    thread_execution_context.binding.clear();

    auto ground_rule = ground(rule_execution_context.rule, rule_execution_context, thread_execution_context);

    if (is_applicable(ground_rule, fact_execution_context.fact_sets))
    {
        std::cout << ground_rule << std::endl;

        rule_execution_context.ground_rules.push_back(ground_rule);
    }
}

void ground_unary_case(const FactsExecutionContext& fact_execution_context,
                       RuleExecutionContext& rule_execution_context,
                       ThreadExecutionContext& thread_execution_context)
{
    thread_execution_context.binding.clear();

    for (const auto vertex_index : rule_execution_context.kpkc_workspace.consistent_vertices_vec)
    {
        const auto& vertex = rule_execution_context.static_consistency_graph.get_vertex(vertex_index);
        thread_execution_context.binding.push_back(vertex.get_object_index());

        auto ground_rule = ground(rule_execution_context.rule, rule_execution_context, thread_execution_context);

        if (is_applicable(ground_rule, fact_execution_context.fact_sets))
        {
            std::cout << ground_rule << std::endl;

            rule_execution_context.ground_rules.push_back(ground_rule);
        }
    }
}

void ground_general_case(const FactsExecutionContext& fact_execution_context,
                         RuleExecutionContext& rule_execution_context,
                         ThreadExecutionContext& thread_execution_context)
{
    kpkc::for_each_k_clique(rule_execution_context.consistency_graph,
                            rule_execution_context.kpkc_workspace,
                            [&](auto&& clique)
                            {
                                thread_execution_context.binding.clear();

                                for (const auto vertex_index : clique)
                                {
                                    const auto& vertex = rule_execution_context.static_consistency_graph.get_vertex(vertex_index);
                                    thread_execution_context.binding.push_back(Index<formalism::Object>(vertex.get_object_index()));
                                }

                                auto ground_rule = ground(rule_execution_context.rule, rule_execution_context, thread_execution_context);

                                if (is_applicable(ground_rule, fact_execution_context.fact_sets))
                                {
                                    std::cout << ground_rule << std::endl;

                                    rule_execution_context.ground_rules.push_back(ground_rule);
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

    rule_execution_context.ground_rules.clear();

    if (arity == 0)
        ground_nullary_case(fact_execution_context, rule_execution_context, thread_execution_context);
    else if (arity == 1)
        ground_unary_case(fact_execution_context, rule_execution_context, thread_execution_context);
    else
        ground_general_case(fact_execution_context, rule_execution_context, thread_execution_context);
}

}

#endif
