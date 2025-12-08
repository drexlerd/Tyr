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

#ifndef TYR_FORMALISM_GROUND_HPP_
#define TYR_FORMALISM_GROUND_HPP_

#include "tyr/analysis/domains.hpp"
#include "tyr/common/itertools.hpp"
#include "tyr/formalism/builder.hpp"
#include "tyr/formalism/canonicalization.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/views.hpp"

namespace tyr::formalism
{
template<FactKind T, Context C_SRC, Context C_DST>
View<Index<GroundAtom<T>>, C_DST> ground(View<Index<Atom<T>>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    // Fetch and clear
    auto atom_ptr = builder.template get_builder<GroundAtom<T>>();
    auto& atom = *atom_ptr;
    atom.clear();

    // Fill data
    atom.predicate = element.get_predicate().get_index();
    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                    atom.objects.push_back(binding[uint_t(arg)].get_index());
                else if constexpr (std::is_same_v<Alternative, View<Index<Object>, C_SRC>>)
                    atom.objects.push_back(arg.get_index());
                else
                    static_assert(dependent_false<Alternative>::value, "Missing case");
            },
            term.get_variant());
    }

    // Canonicalize and Serialize
    canonicalize(atom);
    return destination.get_or_create(atom, builder.get_buffer()).first;
}

template<FactKind T, Context C_SRC, Context C_DST>
View<Index<GroundLiteral<T>>, C_DST>
ground(View<Index<Literal<T>>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    // Fetch and clear
    auto ground_literal_ptr = builder.template get_builder<GroundLiteral<T>>();
    auto& ground_literal = *ground_literal_ptr;
    ground_literal.clear();

    // Fill data
    ground_literal.polarity = element.get_polarity();
    ground_literal.atom = ground(element.get_atom(), binding, builder, destination).get_index();

    // Canonicalize and Serialize
    canonicalize(ground_literal);
    return destination.get_or_create(ground_literal, builder.get_buffer()).first;
}

template<FactKind T, Context C_SRC, Context C_DST>
View<Index<GroundFunctionTerm<T>>, C_DST>
ground(View<Index<FunctionTerm<T>>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    // Fetch and clear
    auto fterm_ptr = builder.template get_builder<GroundFunctionTerm<T>>();
    auto& fterm = *fterm_ptr;
    fterm.clear();

    // Fill data
    fterm.function = element.get_function().get_index();
    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                    fterm.objects.push_back(binding[uint_t(arg)].get_index());
                else if constexpr (std::is_same_v<Alternative, View<Index<Object>, C_SRC>>)
                    fterm.objects.push_back(arg.get_index());
                else
                    static_assert(dependent_false<Alternative>::value, "Missing case");
            },
            term.get_variant());
    }

    // Canonicalize and Serialize
    canonicalize(fterm);
    return destination.get_or_create(fterm, builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST>
View<Data<GroundFunctionExpression>, C_DST>
ground(View<Data<FunctionExpression>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return View<Data<GroundFunctionExpression>, C_DST>(Data<GroundFunctionExpression>(arg), destination);
            else if constexpr (std::is_same_v<Alternative, View<Data<ArithmeticOperator<Data<FunctionExpression>>>, C_SRC>>)
                return View<Data<GroundFunctionExpression>, C_DST>(Data<GroundFunctionExpression>(ground(arg, binding, builder, destination).get_data()),
                                                                   destination);
            else
                return View<Data<GroundFunctionExpression>, C_DST>(Data<GroundFunctionExpression>(ground(arg, binding, builder, destination).get_index()),
                                                                   destination);
        },
        element.get_variant());
}

template<OpKind O, Context C_SRC, Context C_DST>
View<Index<UnaryOperator<O, Data<GroundFunctionExpression>>>, C_DST>
ground(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    // Fetch and clear
    auto unary_ptr = builder.template get_builder<UnaryOperator<O, Data<GroundFunctionExpression>>>();
    auto& unary = *unary_ptr;
    unary.clear();

    // Fill data
    unary.arg = ground(element.get_arg(), binding, builder, destination).get_data();

    // Canonicalize and Serialize
    canonicalize(unary);
    return destination.get_or_create(unary, builder.get_buffer()).first;
}

template<OpKind O, Context C_SRC, Context C_DST>
View<Index<BinaryOperator<O, Data<GroundFunctionExpression>>>, C_DST>
ground(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    // Fetch and clear
    auto binary_ptr = builder.template get_builder<BinaryOperator<O, Data<GroundFunctionExpression>>>();
    auto& binary = *binary_ptr;
    binary.clear();

    // Fill data
    binary.lhs = ground(element.get_lhs(), binding, builder, destination).get_data();
    binary.rhs = ground(element.get_rhs(), binding, builder, destination).get_data();

    // Canonicalize and Serialize
    canonicalize(binary);
    return destination.get_or_create(binary, builder.get_buffer()).first;
}

template<OpKind O, Context C_SRC, Context C_DST>
View<Index<MultiOperator<O, Data<GroundFunctionExpression>>>, C_DST>
ground(View<Index<MultiOperator<O, Data<FunctionExpression>>>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    // Fetch and clear
    auto multi_ptr = builder.template get_builder<MultiOperator<O, Data<GroundFunctionExpression>>>();
    auto& multi = *multi_ptr;
    multi.clear();

    // Fill data
    for (const auto arg : element.get_args())
        multi.args.push_back(ground(arg, binding, builder, destination).get_data());

    // Canonicalize and Serialize
    canonicalize(multi);
    return destination.get_or_create(multi, builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST>
View<Data<BooleanOperator<Data<GroundFunctionExpression>>>, C_DST>
ground(View<Data<BooleanOperator<Data<FunctionExpression>>>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    return visit(
        [&](auto&& arg)
        {
            return View<Data<BooleanOperator<Data<GroundFunctionExpression>>>, C_DST>(
                Data<BooleanOperator<Data<GroundFunctionExpression>>>(ground(arg, binding, builder, destination).get_index()),
                destination);
        },
        element.get_variant());
}

template<Context C_SRC, Context C_DST>
View<Data<ArithmeticOperator<Data<GroundFunctionExpression>>>, C_DST>
ground(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    return visit(
        [&](auto&& arg)
        {
            return View<Data<ArithmeticOperator<Data<GroundFunctionExpression>>>, C_DST>(
                Data<ArithmeticOperator<Data<GroundFunctionExpression>>>(ground(arg, binding, builder, destination).get_index()),
                destination);
        },
        element.get_variant());
}

template<Context C_SRC, Context C_DST>
View<Index<GroundConjunctiveCondition>, C_DST>
ground(View<Index<ConjunctiveCondition>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    // Fetch and clear
    auto conj_cond_ptr = builder.get_builder<GroundConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    // Fill data
    for (const auto literal : element.template get_literals<StaticTag>())
        conj_cond.static_literals.push_back(ground(literal, binding, builder, destination).get_index());
    for (const auto literal : element.template get_literals<FluentTag>())
        conj_cond.fluent_literals.push_back(ground(literal, binding, builder, destination).get_index());
    for (const auto literal : element.template get_literals<DerivedTag>())
        conj_cond.derived_literals.push_back(ground(literal, binding, builder, destination).get_index());
    for (const auto numeric_constraint : element.get_numeric_constraints())
        conj_cond.numeric_constraints.push_back(ground(numeric_constraint, binding, builder, destination).get_data());

    // Canonicalize and Serialize
    canonicalize(conj_cond);
    return destination.get_or_create(conj_cond, builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST>
View<Index<GroundRule>, C_DST> ground(View<Index<Rule>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    // Fetch and clear
    auto rule_ptr = builder.get_builder<GroundRule>();
    auto& rule = *rule_ptr;
    rule.clear();

    // Fill data
    rule.body = ground(element.get_body(), binding, builder, destination).get_index();
    rule.head = ground(element.get_head(), binding, builder, destination).get_index();

    // Canonicalize and Serialize
    canonicalize(rule);
    return destination.get_or_create(rule, builder.get_buffer()).first;
}

template<NumericEffectOpKind Op, FactKind T, Context C_SRC, Context C_DST>
View<Index<GroundNumericEffect<Op, T>>, C_DST>
ground(View<Index<NumericEffect<Op, T>>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    // Fetch and clear
    auto numeric_effect_ptr = builder.template get_builder<GroundNumericEffect<Op, T>>();
    auto& numeric_effect = *numeric_effect_ptr;
    numeric_effect.clear();

    // Fill data
    numeric_effect.fterm = ground(element.get_fterm(), binding, builder, destination).get_index();
    numeric_effect.fexpr = ground(element.get_fexpr(), binding, builder, destination).get_data();

    // Canonicalize and Serialize
    canonicalize(numeric_effect);
    return destination.get_or_create(numeric_effect, builder.get_buffer()).first;
}

template<FactKind T, Context C_SRC, Context C_DST>
View<Data<GroundNumericEffectOperator<T>>, C_DST>
ground(View<Data<NumericEffectOperator<T>>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    return visit(
        [&](auto&& arg)
        {
            return View<Data<GroundNumericEffectOperator<T>>, C_DST>(
                Data<GroundNumericEffectOperator<T>>(ground(arg, binding, builder, destination).get_index()),
                destination);
        },
        element.get_variant());
}

template<Context C_SRC, Context C_DST>
View<Index<GroundConjunctiveEffect>, C_DST>
ground(View<Index<ConjunctiveEffect>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    // Fetch and clear
    auto conj_effect_ptr = builder.get_builder<GroundConjunctiveEffect>();
    auto& conj_eff = *conj_effect_ptr;
    conj_eff.clear();

    // Fill data
    for (const auto literal : element.get_literals())
        conj_eff.literals.push_back(ground(literal, binding, builder, destination).get_index());
    for (const auto numeric_effect : element.get_numeric_effects())
        conj_eff.numeric_effects.push_back(ground(numeric_effect, binding, builder, destination).get_data());
    if (element.get_auxiliary_numeric_effect())
        conj_eff.auxiliary_numeric_effect = ground(*element.get_auxiliary_numeric_effect(), binding, builder, destination).get_data();

    // Canonicalize and Serialize
    canonicalize(conj_eff);
    return destination.get_or_create(conj_eff, builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST>
View<Index<GroundConditionalEffect>, C_DST>
ground(View<Index<ConditionalEffect>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    // Fetch and clear
    auto cond_effect_ptr = builder.get_builder<GroundConditionalEffect>();
    auto& cond_effect = *cond_effect_ptr;
    cond_effect.clear();

    // Fill data
    cond_effect.condition = ground(element.get_condition(), binding, builder, destination).get_index();
    cond_effect.effect = ground(element.get_effect(), binding, builder, destination).get_index();

    // Canonicalize and Serialize
    canonicalize(cond_effect);
    return destination.get_or_create(cond_effect, builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST>
View<Index<GroundAction>, C_DST> ground(View<Index<Action>, C_SRC> element,
                                        View<IndexList<Object>, C_DST> binding,
                                        const analysis::DomainListListList& cond_effect_domains,
                                        Builder& builder,
                                        C_DST& destination)
{
    // Fetch and clear
    auto action_ptr = builder.get_builder<GroundAction>();
    auto& action = *action_ptr;
    action.clear();

    // Fill data
    action.action = element.get_index();
    action.condition = ground(element.get_condition(), binding, builder, destination).get_index();

    thread_local IndexList<Object> full_binding;

    for (uint_t cond_effect_index = 0; cond_effect_index < element.get_effects().size(); ++cond_effect_index)
    {
        const auto cond_effect = element.get_effects()[cond_effect_index];
        const auto& parameter_domains = cond_effect_domains[cond_effect_index];

        // Ensure that we stripped off the action precondition parameter domains.
        assert(std::distance(parameter_domains.begin(), parameter_domains.end()) == static_cast<int>(cond_effect.get_condition().get_arity()));

        for_element_in_cartesian_set(
            parameter_domains.begin(),
            parameter_domains.end(),
            [&](auto&& binding_ext)
            {
                full_binding = binding.get_data();  // reset it
                full_binding.insert(full_binding.end(), binding_ext.begin(), binding_ext.end());

                action.effects.push_back(
                    ground(cond_effect, View<IndexList<Object>, C_DST>(full_binding, binding.get_context()), builder, destination).get_index());
            });
    }

    // Canonicalize and Serialize
    canonicalize(action);
    return destination.get_or_create(action, builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST>
View<Index<GroundAxiom>, C_DST> ground(View<Index<Axiom>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    // Fetch and clear
    auto axiom_ptr = builder.get_builder<GroundAxiom>();
    auto& axiom = *axiom_ptr;
    axiom.clear();

    // Fill data
    axiom.axiom = element.get_axiom().get_index();
    axiom.body = ground(element.get_body(), binding, builder, destination).get_index();
    axiom.head = ground(element.get_head(), binding, builder, destination).get_index();

    // Canonicalize and Serialize
    canonicalize(axiom);
    return destination.get_or_create(axiom, builder.get_buffer()).first;
}

}

#endif