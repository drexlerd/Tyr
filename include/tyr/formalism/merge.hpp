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

#ifndef TYR_FORMALISM_MERGE_HPP_
#define TYR_FORMALISM_MERGE_HPP_

#include "tyr/common/tuple.hpp"
#include "tyr/formalism/builder.hpp"
#include "tyr/formalism/canonicalization.hpp"
#include "tyr/formalism/datas.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/views.hpp"

namespace tyr::formalism
{

/**
 * Forward declarations
 */

template<OpKind O, typename T, Context C_SRC, Context C_DST>
auto merge(View<Index<UnaryOperator<O, T>>, C_SRC> element, Builder& builder, C_DST& destination);

template<OpKind O, typename T, Context C_SRC, Context C_DST>
auto merge(View<Index<BinaryOperator<O, T>>, C_SRC> element, Builder& builder, C_DST& destination);

template<OpKind O, typename T, Context C_SRC, Context C_DST>
auto merge(View<Index<MultiOperator<O, T>>, C_SRC> element, Builder& builder, C_DST& destination);

template<typename T, Context C_SRC, Context C_DST>
auto merge(View<Data<ArithmeticOperator<T>>, C_SRC> element, Builder& builder, C_DST& destination);

template<typename T, Context C_SRC, Context C_DST>
auto merge(View<Data<BooleanOperator<T>>, C_SRC> element, Builder& builder, C_DST& destination);

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Variable>, C_SRC> element, Builder& builder, C_DST& destination);

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Object>, C_SRC> element, Builder& builder, C_DST& destination);

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Binding>, C_SRC> element, Builder& builder, C_DST& destination);

template<Context C_SRC, Context C_DST>
auto merge(View<Data<Term>, C_SRC> element, Builder& builder, C_DST& destination);

template<FactKind T_SRC, Context C_SRC, Context C_DST, FactKind T_DST = T_SRC>
auto merge(View<Index<Predicate<T_SRC>>, C_SRC> element, Builder& builder, C_DST& destination);

template<FactKind T_SRC, Context C_SRC, Context C_DST, FactKind T_DST = T_SRC>
auto merge(View<Index<Atom<T_SRC>>, C_SRC> element, Builder& builder, C_DST& destination);

template<FactKind T_SRC, Context C_SRC, Context C_DST, FactKind T_DST = T_SRC>
auto merge(View<Index<GroundAtom<T_SRC>>, C_SRC> element, Builder& builder, C_DST& destination);

template<FactKind T_SRC, Context C_SRC, Context C_DST, FactKind T_DST = T_SRC>
auto merge(View<Index<Literal<T_SRC>>, C_SRC> element, Builder& builder, C_DST& destination);

template<FactKind T_SRC, Context C_SRC, Context C_DST, FactKind T_DST = T_SRC>
auto merge(View<Index<GroundLiteral<T_SRC>>, C_SRC> element, Builder& builder, C_DST& destination);

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<Function<T>>, C_SRC> element, Builder& builder, C_DST& destination);

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<FunctionTerm<T>>, C_SRC> element, Builder& builder, C_DST& destination);

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<GroundFunctionTerm<T>>, C_SRC> element, Builder& builder, C_DST& destination);

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<GroundFunctionTermValue<T>>, C_SRC> element, Builder& builder, C_DST& destination);

template<Context C_SRC, Context C_DST>
auto merge(View<Data<FunctionExpression>, C_SRC> element, Builder& builder, C_DST& destination);

template<NumericEffectOpKind O, FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<NumericEffect<O, T>>, C_SRC> element, Builder& builder, C_DST& destination);

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Data<NumericEffectOperator<T>>, C_SRC> element, Builder& builder, C_DST& destination);

template<NumericEffectOpKind O, FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<GroundNumericEffect<O, T>>, C_SRC> element, Builder& builder, C_DST& destination);

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Data<GroundNumericEffectOperator<T>>, C_SRC> element, Builder& builder, C_DST& destination);

template<Context C_SRC, Context C_DST>
auto merge(View<Data<GroundFunctionExpression>, C_SRC> element, Builder& builder, C_DST& destination);

template<Context C_SRC, Context C_DST>
auto merge(View<Index<ConjunctiveCondition>, C_SRC> element, Builder& builder, C_DST& destination);

template<Context C_SRC, Context C_DST>
auto merge(View<Index<GroundConjunctiveCondition>, C_SRC> element, Builder& builder, C_DST& destination);

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Rule>, C_SRC> element, Builder& builder, C_DST& destination);

template<Context C_SRC, Context C_DST>
auto merge(View<Index<GroundRule>, C_SRC> element, Builder& builder, C_DST& destination);

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Axiom>, C_SRC> element, Builder& builder, C_DST& destination);

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Metric>, C_SRC> element, Builder& builder, C_DST& destination);

/**
 * Implementations
 */

template<OpKind O, typename T, Context C_SRC, Context C_DST>
auto merge(View<Index<UnaryOperator<O, T>>, C_SRC> element, Builder& builder, C_DST& destination)
{
    auto unary_ptr = builder.template get_builder<UnaryOperator<O, T>>();
    auto& unary = *unary_ptr;
    unary.clear();

    unary.arg = merge(element.get_arg(), builder, destination).get_data();

    canonicalize(unary);
    return destination.get_or_create(unary, builder.get_buffer()).first;
}

template<OpKind O, typename T, Context C_SRC, Context C_DST>
auto merge(View<Index<BinaryOperator<O, T>>, C_SRC> element, Builder& builder, C_DST& destination)
{
    auto binary_ptr = builder.template get_builder<BinaryOperator<O, T>>();
    auto& binary = *binary_ptr;
    binary.clear();

    binary.lhs = merge(element.get_lhs(), builder, destination).get_data();
    binary.rhs = merge(element.get_rhs(), builder, destination).get_data();

    canonicalize(binary);
    return destination.get_or_create(binary, builder.get_buffer()).first;
}

template<OpKind O, typename T, Context C_SRC, Context C_DST>
auto merge(View<Index<MultiOperator<O, T>>, C_SRC> element, Builder& builder, C_DST& destination)
{
    auto multi_ptr = builder.template get_builder<MultiOperator<O, T>>();
    auto& multi = *multi_ptr;
    multi.clear();

    for (const auto arg : element.get_args())
        multi.args.push_back(merge(arg, builder, destination).get_data());

    canonicalize(multi);
    return destination.get_or_create(multi, builder.get_buffer()).first;
}

template<typename T, Context C_SRC, Context C_DST>
auto merge(View<Data<ArithmeticOperator<T>>, C_SRC> element, Builder& builder, C_DST& destination)
{
    return visit([&](auto&& arg)
                 { return View<Data<ArithmeticOperator<T>>, C_DST>(Data<ArithmeticOperator<T>>(merge(arg, builder, destination).get_index()), destination); },
                 element.get_variant());
}

template<typename T, Context C_SRC, Context C_DST>
auto merge(View<Data<BooleanOperator<T>>, C_SRC> element, Builder& builder, C_DST& destination)
{
    return visit(
        [&](auto&& arg) {
            return View<Data<BooleanOperator<T>>, C_DST>(Data<BooleanOperator<T>>(merge(arg, builder, destination).get_index(), element.get_arity()),
                                                         destination);
        },
        element.get_variant());
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Variable>, C_SRC> element, Builder& builder, C_DST& destination)
{
    auto variable_ptr = builder.template get_builder<Variable>();
    auto& variable = *variable_ptr;
    variable.clear();

    variable.name = element.get_name();

    canonicalize(variable);
    return destination.get_or_create(variable, builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Object>, C_SRC> element, Builder& builder, C_DST& destination)
{
    auto object_ptr = builder.template get_builder<Object>();
    auto& object = *object_ptr;
    object.clear();

    object.name = element.get_name();

    canonicalize(object);
    return destination.get_or_create(object, builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Binding>, C_SRC> element, Builder& builder, C_DST& destination)
{
    auto binding_ptr = builder.template get_builder<Binding>();
    auto& binding = *binding_ptr;
    binding.clear();

    binding.objects = element.get_objects().get_data();

    canonicalize(binding);
    return destination.get_or_create(binding, builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST>
auto merge(View<Data<Term>, C_SRC> element, Builder& builder, C_DST& destination)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                return View<Data<Term>, C_DST>(Data<Term>(arg), destination);
            else if constexpr (std::is_same_v<Alternative, View<Index<Object>, C_SRC>>)
                return View<Data<Term>, C_DST>(Data<Term>(merge(arg, builder, destination).get_index()), destination);
            else
                static_assert(dependent_false<Alternative>::value, "Missing case");
        },
        element.get_variant());
}

template<FactKind T_SRC, Context C_SRC, Context C_DST, FactKind T_DST>
auto merge(View<Index<Predicate<T_SRC>>, C_SRC> element, Builder& builder, C_DST& destination)
{
    auto predicate_ptr = builder.template get_builder<Predicate<T_DST>>();
    auto& predicate = *predicate_ptr;
    predicate.clear();

    predicate.name = element.get_name();
    predicate.arity = element.get_arity();

    canonicalize(predicate);
    return destination.get_or_create(predicate, builder.get_buffer()).first;
}

template<FactKind T_SRC, Context C_SRC, Context C_DST, FactKind T_DST>
auto merge(View<Index<Atom<T_SRC>>, C_SRC> element, Builder& builder, C_DST& destination)
{
    auto atom_ptr = builder.template get_builder<Atom<T_DST>>();
    auto& atom = *atom_ptr;
    atom.clear();

    if constexpr (std::is_same_v<T_SRC, T_DST>)
        atom.predicate = element.get_predicate().get_index();
    else
        atom.predicate = merge<T_SRC, C_SRC, C_DST, T_DST>(element.get_predicate(), builder, destination).get_index();
    for (const auto term : element.get_terms())
        atom.terms.push_back(merge(term, builder, destination).get_data());

    canonicalize(atom);
    return destination.get_or_create(atom, builder.get_buffer()).first;
}

template<FactKind T_SRC, Context C_SRC, Context C_DST, FactKind T_DST>
auto merge(View<Index<GroundAtom<T_SRC>>, C_SRC> element, Builder& builder, C_DST& destination)
{
    auto atom_ptr = builder.template get_builder<GroundAtom<T_DST>>();
    auto& atom = *atom_ptr;
    atom.clear();

    if constexpr (std::is_same_v<T_SRC, T_DST>)
        atom.predicate = element.get_predicate().get_index();
    else
        atom.predicate = merge<T_SRC, C_SRC, C_DST, T_DST>(element.get_predicate(), builder, destination).get_index();
    atom.binding = merge(element.get_binding(), builder, destination).get_index();

    canonicalize(atom);
    return destination.get_or_create(atom, builder.get_buffer()).first;
}

template<FactKind T_SRC, Context C_SRC, Context C_DST, FactKind T_DST>
auto merge(View<Index<Literal<T_SRC>>, C_SRC> element, Builder& builder, C_DST& destination)
{
    auto literal_ptr = builder.template get_builder<Literal<T_DST>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.polarity = element.get_polarity();
    literal.atom = merge<T_SRC, C_SRC, C_DST, T_DST>(element.get_atom(), builder, destination).get_index();

    canonicalize(literal);
    return destination.get_or_create(literal, builder.get_buffer()).first;
}

template<FactKind T_SRC, Context C_SRC, Context C_DST, FactKind T_DST>
auto merge(View<Index<GroundLiteral<T_SRC>>, C_SRC> element, Builder& builder, C_DST& destination)
{
    auto literal_ptr = builder.template get_builder<GroundLiteral<T_DST>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.polarity = element.get_polarity();
    literal.atom = merge<T_SRC, C_SRC, C_DST, T_DST>(element.get_atom(), builder, destination).get_index();

    canonicalize(literal);
    return destination.get_or_create(literal, builder.get_buffer()).first;
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<Function<T>>, C_SRC> element, Builder& builder, C_DST& destination)
{
    auto function_ptr = builder.template get_builder<Function<T>>();
    auto& function = *function_ptr;
    function.clear();

    function.name = element.get_name();
    function.arity = element.get_arity();

    canonicalize(function);
    return destination.get_or_create(function, builder.get_buffer()).first;
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<FunctionTerm<T>>, C_SRC> element, Builder& builder, C_DST& destination)
{
    auto fterm_ptr = builder.template get_builder<FunctionTerm<T>>();
    auto& fterm = *fterm_ptr;
    fterm.clear();

    fterm.function = element.get_function().get_index();
    for (const auto term : element.get_terms())
        fterm.terms.push_back(merge(term, builder, destination).get_data());

    canonicalize(fterm);
    return destination.get_or_create(fterm, builder.get_buffer()).first;
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<GroundFunctionTerm<T>>, C_SRC> element, Builder& builder, C_DST& destination)
{
    auto fterm_ptr = builder.template get_builder<GroundFunctionTerm<T>>();
    auto& fterm = *fterm_ptr;
    fterm.clear();

    fterm.function = element.get_function().get_index();
    fterm.binding = merge(element.get_binding(), builder, destination).get_index();

    canonicalize(fterm);
    return destination.get_or_create(fterm, builder.get_buffer()).first;
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<GroundFunctionTermValue<T>>, C_SRC> element, Builder& builder, C_DST& destination)
{
    auto fterm_value_ptr = builder.template get_builder<GroundFunctionTermValue<T>>();
    auto& fterm_value = *fterm_value_ptr;
    fterm_value.clear();

    fterm_value.fterm = merge(element.get_fterm(), builder, destination).get_index();
    fterm_value.value = element.get_value();

    canonicalize(fterm_value);
    return destination.get_or_create(fterm_value, builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST>
auto merge(View<Data<FunctionExpression>, C_SRC> element, Builder& builder, C_DST& destination)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
            {
                return View<Data<FunctionExpression>, C_DST>(Data<FunctionExpression>(arg), destination);
            }
            else if constexpr (std::is_same_v<Alternative, View<Data<ArithmeticOperator<Data<FunctionExpression>>>, C_SRC>>)
            {
                return View<Data<FunctionExpression>, C_DST>(Data<FunctionExpression>(merge(arg, builder, destination).get_data()), destination);
            }
            else
            {
                return View<Data<FunctionExpression>, C_DST>(Data<FunctionExpression>(merge(arg, builder, destination).get_index()), destination);
            }
        },
        element.get_variant());
}

template<NumericEffectOpKind O, FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<NumericEffect<O, T>>, C_SRC> element, Builder& builder, C_DST& destination)
{
    auto numeric_effect_ptr = builder.template get_builder<NumericEffect<O, T>>();
    auto& numeric_effect = *numeric_effect_ptr;
    numeric_effect.clear();

    numeric_effect.fterm = merge(element.get_fterm(), builder, destination).get_index();
    numeric_effect.fexpr = merge(element.get_fexpr(), builder, destination).get_data();

    canonicalize(numeric_effect);
    return destination.get_or_create(numeric_effect, builder.get_buffer()).first;
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Data<NumericEffectOperator<T>>, C_SRC> element, Builder& builder, C_DST& destination)
{
    return visit(
        [&](auto&& arg)
        { return View<Data<NumericEffectOperator<T>>, C_DST>(Data<NumericEffectOperator<T>>(merge(arg, builder, destination).get_index()), destination); },
        element.get_variant());
}

template<NumericEffectOpKind O, FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<GroundNumericEffect<O, T>>, C_SRC> element, Builder& builder, C_DST& destination)
{
    auto numeric_effect_ptr = builder.template get_builder<GroundNumericEffect<O, T>>();
    auto& numeric_effect = *numeric_effect_ptr;
    numeric_effect.clear();

    numeric_effect.fterm = merge(element.get_fterm(), builder, destination).get_index();
    numeric_effect.fexpr = merge(element.get_fexpr(), builder, destination).get_data();

    canonicalize(numeric_effect);
    return destination.get_or_create(numeric_effect, builder.get_buffer()).first;
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Data<GroundNumericEffectOperator<T>>, C_SRC> element, Builder& builder, C_DST& destination)
{
    return visit(
        [&](auto&& arg) {
            return View<Data<GroundNumericEffectOperator<T>>, C_DST>(Data<GroundNumericEffectOperator<T>>(merge(arg, builder, destination).get_index()),
                                                                     destination);
        },
        element.get_variant());
}

template<Context C_SRC, Context C_DST>
auto merge(View<Data<GroundFunctionExpression>, C_SRC> element, Builder& builder, C_DST& destination)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
            {
                return View<Data<GroundFunctionExpression>, C_DST>(Data<GroundFunctionExpression>(arg), destination);
            }
            else if constexpr (std::is_same_v<Alternative, View<Data<ArithmeticOperator<Data<GroundFunctionExpression>>>, C_SRC>>)
            {
                return View<Data<GroundFunctionExpression>, C_DST>(Data<GroundFunctionExpression>(merge(arg, builder, destination).get_data()), destination);
            }
            else
            {
                return View<Data<GroundFunctionExpression>, C_DST>(Data<GroundFunctionExpression>(merge(arg, builder, destination).get_index()), destination);
            }
        },
        element.get_variant());
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<ConjunctiveCondition>, C_SRC> element, Builder& builder, C_DST& destination)
{
    auto conj_cond_ptr = builder.template get_builder<ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto literal : element.template get_literals<StaticTag>())
        conj_cond.static_literals.push_back(merge(literal, builder, destination).get_index());
    for (const auto literal : element.template get_literals<FluentTag>())
        conj_cond.fluent_literals.push_back(merge(literal, builder, destination).get_index());
    for (const auto literal : element.template get_literals<DerivedTag>())
        conj_cond.derived_literals.push_back(merge(literal, builder, destination).get_index());
    for (const auto numeric_constraint : element.get_numeric_constraints())
        conj_cond.numeric_constraints.push_back(merge(numeric_constraint, builder, destination).get_data());
    for (const auto literal : element.template get_nullary_literals<StaticTag>())
        conj_cond.static_nullary_literals.push_back(merge(literal, builder, destination).get_index());
    for (const auto literal : element.template get_nullary_literals<FluentTag>())
        conj_cond.fluent_nullary_literals.push_back(merge(literal, builder, destination).get_index());
    for (const auto literal : element.template get_nullary_literals<DerivedTag>())
        conj_cond.derived_nullary_literals.push_back(merge(literal, builder, destination).get_index());
    for (const auto numeric_constraint : element.get_nullary_numeric_constraints())
        conj_cond.nullary_numeric_constraints.push_back(merge(numeric_constraint, builder, destination).get_data());

    canonicalize(conj_cond);
    return destination.get_or_create(conj_cond, builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<GroundConjunctiveCondition>, C_SRC> element, Builder& builder, C_DST& destination)
{
    auto conj_cond_ptr = builder.template get_builder<GroundConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto literal : element.template get_literals<StaticTag>())
        conj_cond.static_literals.push_back(merge(literal, builder, destination).get_index());
    for (const auto literal : element.template get_literals<FluentTag>())
        conj_cond.fluent_literals.push_back(merge(literal, builder, destination).get_index());
    for (const auto literal : element.template get_literals<DerivedTag>())
        conj_cond.derived_literals.push_back(merge(literal, builder, destination).get_index());
    for (const auto numeric_constraint : element.get_numeric_constraints())
        conj_cond.numeric_constraints.push_back(merge(numeric_constraint, builder, destination).get_data());

    canonicalize(conj_cond);
    return destination.get_or_create(conj_cond, builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Rule>, C_SRC> element, Builder& builder, C_DST& destination)
{
    auto rule_ptr = builder.template get_builder<Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    rule.body = merge(element.get_body(), builder, destination).get_index();
    rule.head = merge(element.get_head(), builder, destination).get_index();

    canonicalize(rule);
    return destination.get_or_create(rule, builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<GroundRule>, C_SRC> element, Builder& builder, C_DST& destination)
{
    auto rule_ptr = builder.template get_builder<GroundRule>();
    auto& rule = *rule_ptr;
    rule.clear();

    rule.rule = element.get_rule().get_index();
    rule.binding = merge(element.get_binding(), builder, destination).get_index();
    rule.body = merge(element.get_body(), builder, destination).get_index();
    rule.head = merge(element.get_head(), builder, destination).get_index();

    canonicalize(rule);
    return destination.get_or_create(rule, builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Axiom>, C_SRC> element, Builder& builder, C_DST& destination)
{
    auto axiom_ptr = builder.template get_builder<Axiom>();
    auto& axiom = *axiom_ptr;
    axiom.clear();

    axiom.body = merge(element.get_body(), builder, destination).get_index();
    axiom.head = merge(element.get_head(), builder, destination).get_index();

    canonicalize(axiom);
    return destination.get_or_create(axiom, builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Metric>, C_SRC> element, Builder& builder, C_DST& destination)
{
    auto metric_ptr = builder.template get_builder<Metric>();
    auto& metric = *metric_ptr;
    metric.clear();

    metric.objective = element.get_objective();
    metric.fexpr = merge(element.get_fexpr(), builder, destination).get_data();

    canonicalize(metric);
    return destination.get_or_create(metric, builder.get_buffer()).first;
}

}

#endif