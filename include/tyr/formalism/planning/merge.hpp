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

#ifndef TYR_FORMALISM_PLANNING_MERGE_HPP_
#define TYR_FORMALISM_PLANNING_MERGE_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/planning/builder.hpp"
#include "tyr/formalism/planning/canonicalization.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/fdr_context.hpp"
#include "tyr/formalism/planning/indices.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"

namespace tyr::formalism::planning
{

struct MergeContext
{
    Builder& builder;
    Repository& destination;
};

// Common

std::pair<Index<Variable>, bool> merge_p2p(View<Index<Variable>, Repository> element, MergeContext& context);

std::pair<Index<Object>, bool> merge_p2p(View<Index<Object>, Repository> element, MergeContext& context);

std::pair<Index<Binding>, bool> merge_p2p(View<Index<Binding>, Repository> element, MergeContext& context);

Data<Term> merge_p2p(View<Data<Term>, Repository> element, MergeContext& context);

// Propositional

template<FactKind T>
std::pair<Index<Predicate<T>>, bool> merge_p2p(View<Index<Predicate<T>>, Repository> element, MergeContext& context);

template<FactKind T>
std::pair<Index<Atom<T>>, bool> merge_p2p(View<Index<Atom<T>>, Repository> element, MergeContext& context);

template<FactKind T>
std::pair<Index<GroundAtom<T>>, bool> merge_p2p(View<Index<GroundAtom<T>>, Repository> element, MergeContext& context);

std::pair<Index<FDRVariable<FluentTag>>, bool> merge_p2p(View<Index<FDRVariable<FluentTag>>, Repository> element, MergeContext& context);

Data<FDRFact<FluentTag>> merge_p2p(View<Data<FDRFact<FluentTag>>, Repository> element, MergeContext& context);

template<FactKind T>
std::pair<Index<Literal<T>>, bool> merge_p2p(View<Index<Literal<T>>, Repository> element, MergeContext& context);

template<FactKind T>
std::pair<Index<GroundLiteral<T>>, bool> merge_p2p(View<Index<GroundLiteral<T>>, Repository> element, MergeContext& context);

// Numeric

template<FactKind T>
std::pair<Index<Function<T>>, bool> merge_p2p(View<Index<Function<T>>, Repository> element, MergeContext& context);

template<FactKind T>
std::pair<Index<FunctionTerm<T>>, bool> merge_p2p(View<Index<FunctionTerm<T>>, Repository> element, MergeContext& context);

template<FactKind T>
std::pair<Index<GroundFunctionTerm<T>>, bool> merge_p2p(View<Index<GroundFunctionTerm<T>>, Repository> element, MergeContext& context);

template<FactKind T>
std::pair<Index<GroundFunctionTermValue<T>>, bool> merge_p2p(View<Index<GroundFunctionTermValue<T>>, Repository> element, MergeContext& context);

Data<FunctionExpression> merge_p2p(View<Data<FunctionExpression>, Repository> element, MergeContext& context);

Data<GroundFunctionExpression> merge_p2p(View<Data<GroundFunctionExpression>, Repository> element, MergeContext& context);

template<OpKind O, typename T>
std::pair<Index<UnaryOperator<O, T>>, bool> merge_p2p(View<Index<UnaryOperator<O, T>>, Repository> element, MergeContext& context);

template<OpKind O, typename T>
std::pair<Index<BinaryOperator<O, T>>, bool> merge_p2p(View<Index<BinaryOperator<O, T>>, Repository> element, MergeContext& context);

template<OpKind O, typename T>
std::pair<Index<MultiOperator<O, T>>, bool> merge_p2p(View<Index<MultiOperator<O, T>>, Repository> element, MergeContext& context);

template<typename T>
Data<ArithmeticOperator<T>> merge_p2p(View<Data<ArithmeticOperator<T>>, Repository> element, MergeContext& context);

template<typename T>
Data<BooleanOperator<T>> merge_p2p(View<Data<BooleanOperator<T>>, Repository> element, MergeContext& context);

template<NumericEffectOpKind O, FactKind T>
std::pair<Index<NumericEffect<O, T>>, bool> merge_p2p(View<Index<NumericEffect<O, T>>, Repository> element, MergeContext& context);

template<FactKind T>
Data<NumericEffectOperator<T>> merge_p2p(View<Data<NumericEffectOperator<T>>, Repository> element, MergeContext& context);

template<NumericEffectOpKind O, FactKind T>
std::pair<Index<GroundNumericEffect<O, T>>, bool> merge_p2p(View<Index<GroundNumericEffect<O, T>>, Repository> element, MergeContext& context);

template<FactKind T>
Data<GroundNumericEffectOperator<T>> merge_p2p(View<Data<GroundNumericEffectOperator<T>>, Repository> element, MergeContext& context);

// Composite

std::pair<Index<ConjunctiveCondition>, bool> merge_p2p(View<Index<ConjunctiveCondition>, Repository> element, MergeContext& context);

std::pair<Index<Axiom>, bool> merge_p2p(View<Index<Axiom>, Repository> element, MergeContext& context);

std::pair<Index<Metric>, bool> merge_p2p(View<Index<Metric>, Repository> element, MergeContext& context);

/**
 * Implementations
 */

// Common

inline std::pair<Index<Variable>, bool> merge_p2p(View<Index<Variable>, Repository> element, MergeContext& context)
{
    auto variable_ptr = context.builder.template get_builder<Variable>();
    auto& variable = *variable_ptr;
    variable.clear();

    variable.name = element.get_name();

    canonicalize(variable);
    return context.destination.get_or_create(variable, context.builder.get_buffer());
}

inline std::pair<Index<Object>, bool> merge_p2p(View<Index<Object>, Repository> element, MergeContext& context)
{
    auto object_ptr = context.builder.template get_builder<Object>();
    auto& object = *object_ptr;
    object.clear();

    object.name = element.get_name();

    canonicalize(object);
    return context.destination.get_or_create(object, context.builder.get_buffer());
}

inline std::pair<Index<Binding>, bool> merge_p2p(View<Index<Binding>, Repository> element, MergeContext& context)
{
    auto binding_ptr = context.builder.template get_builder<Binding>();
    auto& binding = *binding_ptr;
    binding.clear();

    binding.objects = element.get_data().objects;

    canonicalize(binding);
    return context.destination.get_or_create(binding, context.builder.get_buffer());
}

inline Data<Term> merge_p2p(View<Data<Term>, Repository> element, MergeContext& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                return Data<Term>(arg);
            else if constexpr (std::is_same_v<Alternative, View<Index<Object>, Repository>>)
                return Data<Term>(merge_p2p(arg, context).first);
            else
                static_assert(dependent_false<Alternative>::value, "Missing case");
        },
        element.get_variant());
}

// Propositional

template<FactKind T>
inline std::pair<Index<Predicate<T>>, bool> merge_p2p(View<Index<Predicate<T>>, Repository> element, MergeContext& context)
{
    auto predicate_ptr = context.builder.template get_builder<Predicate<T>>();
    auto& predicate = *predicate_ptr;
    predicate.clear();

    predicate.name = element.get_name();
    predicate.arity = element.get_arity();

    canonicalize(predicate);
    return context.destination.get_or_create(predicate, context.builder.get_buffer());
}

template<FactKind T>
inline std::pair<Index<Atom<T>>, bool> merge_p2p(View<Index<Atom<T>>, Repository> element, MergeContext& context)
{
    auto atom_ptr = context.builder.template get_builder<Atom<T>>();
    auto& atom = *atom_ptr;
    atom.clear();

    atom.predicate = merge_p2p(element.get_predicate(), context).first;
    for (const auto term : element.get_terms())
        atom.terms.push_back(merge_p2p(term, context));

    canonicalize(atom);
    return context.destination.get_or_create(atom, context.builder.get_buffer());
}

template<FactKind T>
inline std::pair<Index<GroundAtom<T>>, bool> merge_p2p(View<Index<GroundAtom<T>>, Repository> element, MergeContext& context)
{
    auto atom_ptr = context.builder.template get_builder<GroundAtom<T>>();
    auto& atom = *atom_ptr;
    atom.clear();

    atom.predicate = merge_p2p(element.get_predicate(), context).first;
    atom.objects = element.get_data().objects;

    canonicalize(atom);
    return context.destination.get_or_create(atom, context.builder.get_buffer());
}

inline std::pair<Index<FDRVariable<FluentTag>>, bool> merge_p2p(View<Index<FDRVariable<FluentTag>>, Repository> element, MergeContext& context)
{
    auto variable_ptr = context.builder.template get_builder<FDRVariable<FluentTag>>();
    auto& variable = *variable_ptr;
    variable.clear();

    variable.domain_size = element.get_domain_size();
    for (const auto atom : element.get_atoms())
        variable.atoms.push_back(merge_p2p(atom, context).first);

    canonicalize(variable);
    return context.destination.get_or_create(variable, context.builder.get_buffer());
}

inline Data<FDRFact<FluentTag>> merge_p2p(View<Data<FDRFact<FluentTag>>, Repository> element, MergeContext& context)
{
    return Data<FDRFact<FluentTag>>(merge_p2p(element.get_variable(), context).first, element.get_value());
}

template<FactKind T>
inline std::pair<Index<Literal<T>>, bool> merge_p2p(View<Index<Literal<T>>, Repository> element, MergeContext& context)
{
    auto literal_ptr = context.builder.template get_builder<Literal<T>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.polarity = element.get_polarity();
    literal.atom = merge_p2p(element.get_atom(), context).first;

    canonicalize(literal);
    return context.destination.get_or_create(literal, context.builder.get_buffer());
}

template<FactKind T>
inline std::pair<Index<GroundLiteral<T>>, bool> merge_p2p(View<Index<GroundLiteral<T>>, Repository> element, MergeContext& context)
{
    auto literal_ptr = context.builder.template get_builder<GroundLiteral<T>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.polarity = element.get_polarity();
    literal.atom = merge_p2p(element.get_atom(), context).first;

    canonicalize(literal);
    return context.destination.get_or_create(literal, context.builder.get_buffer());
}

// Numeric

template<FactKind T>
inline std::pair<Index<Function<T>>, bool> merge_p2p(View<Index<Function<T>>, Repository> element, MergeContext& context)
{
    auto function_ptr = context.builder.template get_builder<Function<T>>();
    auto& function = *function_ptr;
    function.clear();

    function.name = element.get_name();
    function.arity = element.get_arity();

    canonicalize(function);
    return context.destination.get_or_create(function, context.builder.get_buffer());
}

template<FactKind T>
inline std::pair<Index<FunctionTerm<T>>, bool> merge_p2p(View<Index<FunctionTerm<T>>, Repository> element, MergeContext& context)
{
    auto fterm_ptr = context.builder.template get_builder<FunctionTerm<T>>();
    auto& fterm = *fterm_ptr;
    fterm.clear();

    fterm.function = element.get_function().get_index();
    for (const auto term : element.get_terms())
        fterm.terms.push_back(merge_p2p(term, context));

    canonicalize(fterm);
    return context.destination.get_or_create(fterm, context.builder.get_buffer());
}

template<FactKind T>
inline std::pair<Index<GroundFunctionTerm<T>>, bool> merge_p2p(View<Index<GroundFunctionTerm<T>>, Repository> element, MergeContext& context)
{
    auto fterm_ptr = context.builder.template get_builder<GroundFunctionTerm<T>>();
    auto& fterm = *fterm_ptr;
    fterm.clear();

    fterm.function = element.get_function().get_index();
    fterm.objects = element.get_data().objects;

    canonicalize(fterm);
    return context.destination.get_or_create(fterm, context.builder.get_buffer());
}

template<FactKind T>
inline std::pair<Index<GroundFunctionTermValue<T>>, bool> merge_p2p(View<Index<GroundFunctionTermValue<T>>, Repository> element, MergeContext& context)
{
    auto fterm_value_ptr = context.builder.template get_builder<GroundFunctionTermValue<T>>();
    auto& fterm_value = *fterm_value_ptr;
    fterm_value.clear();

    fterm_value.fterm = merge_p2p(element.get_fterm(), context).first;
    fterm_value.value = element.get_value();

    canonicalize(fterm_value);
    return context.destination.get_or_create(fterm_value, context.builder.get_buffer());
}

inline Data<FunctionExpression> merge_p2p(View<Data<FunctionExpression>, Repository> element, MergeContext& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return Data<FunctionExpression>(arg);
            else if constexpr (std::is_same_v<Alternative, View<Data<ArithmeticOperator<Data<FunctionExpression>>>, Repository>>)
                return Data<FunctionExpression>(merge_p2p(arg, context));
            else
                return Data<FunctionExpression>(merge_p2p(arg, context).first);
        },
        element.get_variant());
}

inline Data<GroundFunctionExpression> merge_p2p(View<Data<GroundFunctionExpression>, Repository> element, MergeContext& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return Data<GroundFunctionExpression>(arg);
            else if constexpr (std::is_same_v<Alternative, View<Data<ArithmeticOperator<Data<GroundFunctionExpression>>>, Repository>>)
                return Data<GroundFunctionExpression>(merge_p2p(arg, context));
            else
                return Data<GroundFunctionExpression>(merge_p2p(arg, context).first);
        },
        element.get_variant());
}

template<OpKind O, typename T>
inline std::pair<Index<UnaryOperator<O, T>>, bool> merge_p2p(View<Index<UnaryOperator<O, T>>, Repository> element, MergeContext& context)
{
    auto unary_ptr = context.builder.template get_builder<UnaryOperator<O, T>>();
    auto& unary = *unary_ptr;
    unary.clear();

    unary.arg = merge_p2p(element.get_arg(), context);

    canonicalize(unary);
    return context.destination.get_or_create(unary, context.builder.get_buffer());
}

template<OpKind O, typename T>
inline std::pair<Index<BinaryOperator<O, T>>, bool> merge_p2p(View<Index<BinaryOperator<O, T>>, Repository> element, MergeContext& context)
{
    auto binary_ptr = context.builder.template get_builder<BinaryOperator<O, T>>();
    auto& binary = *binary_ptr;
    binary.clear();

    binary.lhs = merge_p2p(element.get_lhs(), context);
    binary.rhs = merge_p2p(element.get_rhs(), context);

    canonicalize(binary);
    return context.destination.get_or_create(binary, context.builder.get_buffer());
}

template<OpKind O, typename T>
inline std::pair<Index<MultiOperator<O, T>>, bool> merge_p2p(View<Index<MultiOperator<O, T>>, Repository> element, MergeContext& context)
{
    auto multi_ptr = context.builder.template get_builder<MultiOperator<O, T>>();
    auto& multi = *multi_ptr;
    multi.clear();

    for (const auto arg : element.get_args())
        multi.args.push_back(merge_p2p(arg, context));

    canonicalize(multi);
    return context.destination.get_or_create(multi, context.builder.get_buffer());
}

template<typename T>
inline Data<ArithmeticOperator<T>> merge_p2p(View<Data<ArithmeticOperator<T>>, Repository> element, MergeContext& context)
{
    return visit([&](auto&& arg) { return Data<ArithmeticOperator<T>>(merge_p2p(arg, context).first); }, element.get_variant());
}

template<typename T>
inline Data<BooleanOperator<T>> merge_p2p(View<Data<BooleanOperator<T>>, Repository> element, MergeContext& context)
{
    return visit([&](auto&& arg) { return Data<BooleanOperator<T>>(merge_p2p(arg, context).first); }, element.get_variant());
}

template<NumericEffectOpKind O, FactKind T>
inline std::pair<Index<NumericEffect<O, T>>, bool> merge_p2p(View<Index<NumericEffect<O, T>>, Repository> element, MergeContext& context)
{
    auto numeric_effect_ptr = context.builder.template get_builder<NumericEffect<O, T>>();
    auto& numeric_effect = *numeric_effect_ptr;
    numeric_effect.clear();

    numeric_effect.fterm = merge_p2p(element.get_fterm(), context).first;
    numeric_effect.fexpr = merge_p2p(element.get_fexpr(), context);

    canonicalize(numeric_effect);
    return context.destination.get_or_create(numeric_effect, context.builder.get_buffer());
}

template<FactKind T>
inline Data<NumericEffectOperator<T>> merge_p2p(View<Data<NumericEffectOperator<T>>, Repository> element, MergeContext& context)
{
    return visit([&](auto&& arg) { return Data<NumericEffectOperator<T>>(merge_p2p(arg, context).first); }, element.get_variant());
}

template<NumericEffectOpKind O, FactKind T>
inline std::pair<Index<GroundNumericEffect<O, T>>, bool> merge_p2p(View<Index<GroundNumericEffect<O, T>>, Repository> element, MergeContext& context)
{
    auto numeric_effect_ptr = context.builder.template get_builder<GroundNumericEffect<O, T>>();
    auto& numeric_effect = *numeric_effect_ptr;
    numeric_effect.clear();

    numeric_effect.fterm = merge_p2p(element.get_fterm(), context).first;
    numeric_effect.fexpr = merge_p2p(element.get_fexpr(), context);

    canonicalize(numeric_effect);
    return context.destination.get_or_create(numeric_effect, context.builder.get_buffer());
}

template<FactKind T>
inline Data<GroundNumericEffectOperator<T>> merge_p2p(View<Data<GroundNumericEffectOperator<T>>, Repository> element, MergeContext& context)
{
    return visit([&](auto&& arg) { return Data<GroundNumericEffectOperator<T>>(merge_p2p(arg, context).first); }, element.get_variant());
}

// Composite

inline std::pair<Index<ConjunctiveCondition>, bool> merge_p2p(View<Index<ConjunctiveCondition>, Repository> element, MergeContext& context)
{
    auto conj_cond_ptr = context.builder.template get_builder<ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto variable : element.get_variables())
        conj_cond.variables.push_back(merge_p2p(variable, context).first);
    for (const auto literal : element.template get_literals<StaticTag>())
        conj_cond.static_literals.push_back(merge_p2p(literal, context).first);
    for (const auto literal : element.template get_literals<FluentTag>())
        conj_cond.fluent_literals.push_back(merge_p2p(literal, context).first);
    for (const auto literal : element.template get_literals<DerivedTag>())
        conj_cond.derived_literals.push_back(merge_p2p(literal, context).first);
    for (const auto numeric_constraint : element.get_numeric_constraints())
        conj_cond.numeric_constraints.push_back(merge_p2p(numeric_constraint, context));

    canonicalize(conj_cond);
    return context.destination.get_or_create(conj_cond, context.builder.get_buffer());
}

inline std::pair<Index<Axiom>, bool> merge_p2p(View<Index<Axiom>, Repository> element, MergeContext& context)
{
    auto axiom_ptr = context.builder.template get_builder<Axiom>();
    auto& axiom = *axiom_ptr;
    axiom.clear();

    for (const auto variable : element.get_variables())
        axiom.variables.push_back(merge_p2p(variable, context).first);
    axiom.body = merge_p2p(element.get_body(), context).first;
    axiom.head = merge_p2p(element.get_head(), context).first;

    canonicalize(axiom);
    return context.destination.get_or_create(axiom, context.builder.get_buffer());
}

inline std::pair<Index<Metric>, bool> merge_p2p(View<Index<Metric>, Repository> element, MergeContext& context)
{
    auto metric_ptr = context.builder.template get_builder<Metric>();
    auto& metric = *metric_ptr;
    metric.clear();

    metric.objective = element.get_objective();
    metric.fexpr = merge_p2p(element.get_fexpr(), context);

    canonicalize(metric);
    return context.destination.get_or_create(metric, context.builder.get_buffer());
}

}

#endif