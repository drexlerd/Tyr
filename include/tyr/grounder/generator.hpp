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

namespace tyr::grounder
{

template<formalism::FactKind T, formalism::Context C>
View<Index<formalism::GroundAtom<T>>, formalism::OverlayRepository<C>> ground(View<Index<formalism::Atom<T>>, C> element, MutableRuleWorkspace<C>& workspace)
{
    // Fetch and clear
    auto& binding = workspace.binding;
    auto& builder = workspace.builder;
    auto& repository = workspace.repository;
    auto& buffer = workspace.buffer;
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
                else if constexpr (std::is_same_v<Alternative, View<Index<formalism::Object>, C>>)
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

template<formalism::FactKind T, formalism::Context C>
View<Index<formalism::GroundLiteral<T>>, formalism::OverlayRepository<C>> ground(View<Index<formalism::Literal<T>>, C> element,
                                                                                 MutableRuleWorkspace<C>& workspace)
{
    // Fetch and clear
    auto& builder = workspace.builder;
    auto& repository = workspace.repository;
    auto& buffer = workspace.buffer;
    auto& ground_literal = builder.template get_ground_literal<T>();
    ground_literal.clear();

    // Fill data
    ground_literal.polarity = element.get_polarity();
    ground_literal.atom = ground(element.get_atom(), workspace).get_index();

    // Canonicalize and Serialize
    canonicalize(ground_literal);
    return repository.get_or_create(ground_literal, buffer).first;
}

template<formalism::FactKind T, formalism::Context C>
View<Index<formalism::GroundFunctionTerm<T>>, formalism::OverlayRepository<C>> ground(View<Index<formalism::FunctionTerm<T>>, C> element,
                                                                                      MutableRuleWorkspace<C>& workspace)
{
    // Fetch and clear
    auto& binding = workspace.binding;
    auto& builder = workspace.builder;
    auto& repository = workspace.repository;
    auto& buffer = workspace.buffer;
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
                else if constexpr (std::is_same_v<Alternative, View<Index<formalism::Object>, C>>)
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

template<formalism::Context C>
View<Data<formalism::GroundFunctionExpression>, formalism::OverlayRepository<C>> ground(View<Data<formalism::FunctionExpression>, C> element,
                                                                                        MutableRuleWorkspace<C>& workspace)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
            {
                return View<Data<formalism::GroundFunctionExpression>, formalism::OverlayRepository<C>>(Data<formalism::GroundFunctionExpression>(arg),
                                                                                                        workspace.repository);
            }
            else if constexpr (std::is_same_v<Alternative, View<Index<formalism::FunctionTerm<formalism::StaticTag>>, formalism::Repository>>)
            {
                return View<Data<formalism::GroundFunctionExpression>, formalism::OverlayRepository<C>>(
                    Data<formalism::GroundFunctionExpression>(ground(arg, workspace).get_index()),
                    workspace.repository);
            }
            else if constexpr (std::is_same_v<Alternative, View<Index<formalism::FunctionTerm<formalism::FluentTag>>, formalism::Repository>>)
            {
                return View<Data<formalism::GroundFunctionExpression>, formalism::OverlayRepository<C>>(
                    Data<formalism::GroundFunctionExpression>(ground(arg, workspace).get_index()),
                    workspace.repository);
            }
            else
            {
                return View<Data<formalism::GroundFunctionExpression>, formalism::OverlayRepository<C>>(
                    Data<formalism::GroundFunctionExpression>(ground(arg, workspace).get_index()),
                    workspace.repository);
            }
        },
        element.get_variant());
}

template<formalism::OpKind O, formalism::Context C>
View<Index<formalism::UnaryOperator<O, Data<formalism::GroundFunctionExpression>>>, formalism::OverlayRepository<C>>
ground(View<Index<formalism::UnaryOperator<O, Data<formalism::FunctionExpression>>>, C> element, MutableRuleWorkspace<C>& workspace)
{
    // Fetch and clear
    auto& builder = workspace.builder;
    auto& repository = workspace.repository;
    auto& buffer = workspace.buffer;
    auto& unary = builder.template get_ground_unary<O>();
    unary.clear();

    // Fill data
    unary.arg = ground(element.get_arg(), workspace).get_data();

    // Canonicalize and Serialize
    canonicalize(unary);
    return repository.get_or_create(unary, buffer).first;
}

template<formalism::OpKind O, formalism::Context C>
View<Index<formalism::BinaryOperator<O, Data<formalism::GroundFunctionExpression>>>, formalism::OverlayRepository<C>>
ground(View<Index<formalism::BinaryOperator<O, Data<formalism::FunctionExpression>>>, C> element, MutableRuleWorkspace<C>& workspace)
{
    // Fetch and clear
    auto& builder = workspace.builder;
    auto& repository = workspace.repository;
    auto& buffer = workspace.buffer;
    auto& binary = builder.template get_ground_binary<O>();
    binary.clear();

    // Fill data
    binary.lhs = ground(element.get_lhs(), workspace).get_data();
    binary.rhs = ground(element.get_rhs(), workspace).get_data();

    // Canonicalize and Serialize
    canonicalize(binary);
    return repository.get_or_create(binary, buffer).first;
}

template<formalism::OpKind O, formalism::Context C>
View<Index<formalism::MultiOperator<O, Data<formalism::GroundFunctionExpression>>>, formalism::OverlayRepository<C>>
ground(View<Index<formalism::MultiOperator<O, Data<formalism::FunctionExpression>>>, C> element, MutableRuleWorkspace<C>& workspace)
{
    // Fetch and clear
    auto& builder = workspace.builder;
    auto& repository = workspace.repository;
    auto& buffer = workspace.buffer;
    auto& multi = builder.template get_ground_multi<O>();
    multi.clear();

    auto& args = multi.args;

    // Fill data
    for (const auto arg : element.get_args())
        args.push_back(ground(arg, workspace).get_data());

    // Canonicalize and Serialize
    canonicalize(multi);
    return repository.get_or_create(multi, buffer).first;
}

template<formalism::Context C>
View<Data<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>>, formalism::OverlayRepository<C>>
ground(View<Data<formalism::BooleanOperator<Data<formalism::FunctionExpression>>>, C> element, MutableRuleWorkspace<C>& workspace)
{
    return visit(
        [&](auto&& arg)
        {
            return View<Data<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>>, formalism::OverlayRepository<C>>(
                Data<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>>(ground(arg, workspace).get_index()),
                workspace.repository);
        },
        element.get_variant());
}

template<formalism::Context C>
View<Data<formalism::ArithmeticOperator<Data<formalism::GroundFunctionExpression>>>, formalism::OverlayRepository<C>>
ground(View<Data<formalism::ArithmeticOperator<Data<formalism::FunctionExpression>>>, C> element, MutableRuleWorkspace<C>& workspace)
{
    return visit(
        [&](auto&& arg)
        {
            return View<Data<formalism::ArithmeticOperator<Data<formalism::GroundFunctionExpression>>>, formalism::OverlayRepository<C>>(
                Data<formalism::ArithmeticOperator<Data<formalism::GroundFunctionExpression>>>(ground(arg, workspace).get_index()),
                workspace.repository);
        },
        element.get_variant());
}

template<formalism::Context C>
View<Index<formalism::GroundConjunctiveCondition>, formalism::OverlayRepository<C>> ground(View<Index<formalism::ConjunctiveCondition>, C> element,
                                                                                           MutableRuleWorkspace<C>& workspace)
{
    // Fetch and clear
    auto& binding = workspace.binding;
    auto& builder = workspace.builder;
    auto& repository = workspace.repository;
    auto& buffer = workspace.buffer;
    auto& conj_cond = builder.get_ground_conj_cond();
    conj_cond.clear();

    auto& static_literals = conj_cond.static_literals;
    auto& fluent_literals = conj_cond.fluent_literals;
    auto& numeric_constraints = conj_cond.numeric_constraints;

    // Fill data
    for (const auto literal : element.template get_literals<formalism::StaticTag>())
        static_literals.push_back(ground(literal, workspace).get_index());
    for (const auto literal : element.template get_literals<formalism::FluentTag>())
        fluent_literals.push_back(ground(literal, workspace).get_index());
    for (const auto numeric_constraint : element.get_numeric_constraints())
        numeric_constraints.push_back(ground(numeric_constraint, workspace).get_data());

    // Canonicalize and Serialize
    canonicalize(conj_cond);
    return repository.get_or_create(conj_cond, buffer).first;
}

template<formalism::Context C>
View<Index<formalism::GroundRule>, formalism::OverlayRepository<C>> ground(View<Index<formalism::Rule>, C> element, MutableRuleWorkspace<C>& workspace)
{
    // Fetch and clear
    auto& builder = workspace.builder;
    auto& repository = workspace.repository;
    auto& buffer = workspace.buffer;
    auto& rule = builder.get_ground_rule();
    rule.clear();

    auto& body = rule.body;
    auto& head = rule.head;

    // Fill data
    body = ground(element.get_body(), workspace).get_index();
    head = ground(element.get_head(), workspace).get_index();

    // Canonicalize and Serialize
    canonicalize(rule);
    return repository.get_or_create(rule, buffer).first;
}

template<formalism::Context C>
void ground_nullary_case(const ImmutableRuleWorkspace<C>& immutable_workspace, MutableRuleWorkspace<C>& mutable_workspace)
{
    mutable_workspace.binding.clear();

    auto ground_rule = ground(immutable_workspace.rule, mutable_workspace);

    if (is_applicable(ground_rule, immutable_workspace.fact_sets))
    {
        std::cout << ground_rule << std::endl;

        mutable_workspace.ground_rules.push_back(ground_rule.get_index());
    }
}

template<formalism::Context C>
void ground_unary_case(const ImmutableRuleWorkspace<C>& immutable_workspace, MutableRuleWorkspace<C>& mutable_workspace)
{
    mutable_workspace.binding.clear();

    for (const auto vertex_index : mutable_workspace.kpkc_workspace.consistent_vertices_vec)
    {
        const auto& vertex = immutable_workspace.static_consistency_graph.get_vertex(vertex_index);
        mutable_workspace.binding.push_back(vertex.get_object_index());

        auto ground_rule = ground(immutable_workspace.rule, mutable_workspace);

        if (is_applicable(ground_rule, immutable_workspace.fact_sets))
        {
            std::cout << ground_rule << std::endl;

            mutable_workspace.ground_rules.push_back(ground_rule.get_index());
        }
    }
}

template<formalism::Context C>
void ground_general_case(const ImmutableRuleWorkspace<C>& immutable_workspace, MutableRuleWorkspace<C>& mutable_workspace)
{
    kpkc::for_each_k_clique(immutable_workspace.consistency_graph,
                            mutable_workspace.kpkc_workspace,
                            [&](auto&& clique)
                            {
                                mutable_workspace.binding.clear();

                                for (const auto vertex_index : clique)
                                {
                                    const auto& vertex = immutable_workspace.static_consistency_graph.get_vertex(vertex_index);
                                    mutable_workspace.binding.push_back(Index<formalism::Object>(vertex.get_object_index()));
                                }

                                auto ground_rule = ground(immutable_workspace.rule, mutable_workspace);

                                if (is_applicable(ground_rule, immutable_workspace.fact_sets))
                                {
                                    std::cout << ground_rule << std::endl;

                                    mutable_workspace.ground_rules.push_back(ground_rule.get_index());
                                }
                            });
}

template<formalism::Context C>
void ground(const ImmutableRuleWorkspace<C>& immutable_workspace, MutableRuleWorkspace<C>& mutable_workspace)
{
    const auto rule = immutable_workspace.rule;
    const auto& fact_sets = immutable_workspace.fact_sets;

    if (!nullary_conditions_hold(rule.get_body(), fact_sets))
        return;

    const auto arity = rule.get_body().get_arity();

    mutable_workspace.ground_rules.clear();

    if (arity == 0)
        ground_nullary_case(immutable_workspace, mutable_workspace);
    else if (arity == 1)
        ground_unary_case(immutable_workspace, mutable_workspace);
    else
        ground_general_case(immutable_workspace, mutable_workspace);
}

}

#endif
