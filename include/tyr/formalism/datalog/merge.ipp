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

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/macros.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/canonicalization.hpp"
#include "tyr/formalism/datalog/indices.hpp"
#include "tyr/formalism/datalog/merge_decl.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/indices.hpp"
#include "tyr/formalism/views.hpp"

namespace tyr::formalism::datalog
{

// Common

TYR_INLINE_IMPL std::pair<VariableView, bool> merge_d2d(VariableView element, MergeContext& context)
{
    auto variable_ptr = context.builder.template get_builder<Variable>();
    auto& variable = *variable_ptr;
    variable.clear();

    variable.name = element.get_name();

    canonicalize(variable);
    return context.destination.get_or_create(variable);
}

TYR_INLINE_IMPL std::pair<ObjectView, bool> merge_d2d(ObjectView element, MergeContext& context)
{
    auto object_ptr = context.builder.template get_builder<Object>();
    auto& object = *object_ptr;
    object.clear();

    object.name = element.get_name();

    canonicalize(object);
    return context.destination.get_or_create(object);
}

TYR_INLINE_IMPL Data<Term> merge_d2d(TermView element, MergeContext& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                return Data<Term>(arg);
            else if constexpr (std::is_same_v<Alternative, ObjectView>)
                return Data<Term>(merge_d2d(arg, context).first.get_index());
            else
                static_assert(dependent_false<Alternative>::value, "Missing case");
        },
        element.get_variant());
}

// Propositional

template<FactKind T>
std::pair<PredicateView<T>, bool> merge_d2d(PredicateView<T> element, MergeContext& context)
{
    auto predicate_ptr = context.builder.template get_builder<Predicate<T>>();
    auto& predicate = *predicate_ptr;
    predicate.clear();

    predicate.name = element.get_name();
    predicate.arity = element.get_arity();

    canonicalize(predicate);
    return context.destination.get_or_create(predicate);
}

template<FactKind T>
std::pair<AtomView<T>, bool> merge_d2d(AtomView<T> element, MergeContext& context)
{
    auto atom_ptr = context.builder.template get_builder<Atom<T>>();
    auto& atom = *atom_ptr;
    atom.clear();

    atom.predicate = element.get_predicate().get_index();
    for (const auto term : element.get_terms())
        atom.terms.push_back(merge_d2d(term, context));

    canonicalize(atom);
    return context.destination.get_or_create(atom);
}

template<FactKind T>
std::pair<PredicateBindingView<T>, bool> merge_d2d(PredicateBindingView<T> element, MergeContext& context)
{
    auto binding_ptr = context.builder.template get_builder<RelationBinding<Predicate<T>>>();
    auto& binding = *binding_ptr;
    binding.clear();

    binding.relation = element.get_relation().get_index();
    for (const auto object : element.get_objects())
        binding.objects.push_back(object.get_index());

    canonicalize(binding);
    return context.destination.get_or_create(binding);
}

template<FactKind T>
std::pair<GroundAtomView<T>, bool> merge_d2d(GroundAtomView<T> element, MergeContext& context)
{
    auto atom_ptr = context.builder.template get_builder<GroundAtom<T>>();
    auto& atom = *atom_ptr;
    atom.clear();

    atom.binding = merge_d2d(element.get_row(), context).first.get_index();

    canonicalize(atom);
    return context.destination.get_or_create(atom);
}

template<FactKind T>
std::pair<LiteralView<T>, bool> merge_d2d(LiteralView<T> element, MergeContext& context)
{
    auto literal_ptr = context.builder.template get_builder<Literal<T>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.polarity = element.get_polarity();
    literal.atom = merge_d2d(element.get_atom(), context).first.get_index();

    canonicalize(literal);
    return context.destination.get_or_create(literal);
}

template<FactKind T>
std::pair<GroundLiteralView<T>, bool> merge_d2d(GroundLiteralView<T> element, MergeContext& context)
{
    auto literal_ptr = context.builder.template get_builder<GroundLiteral<T>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.polarity = element.get_polarity();
    literal.atom = merge_d2d(element.get_atom(), context).first.get_index();

    canonicalize(literal);
    return context.destination.get_or_create(literal);
}

// Numeric

template<FactKind T>
std::pair<FunctionView<T>, bool> merge_d2d(FunctionView<T> element, MergeContext& context)
{
    auto function_ptr = context.builder.template get_builder<Function<T>>();
    auto& function = *function_ptr;
    function.clear();

    function.name = element.get_name();
    function.arity = element.get_arity();

    canonicalize(function);
    return context.destination.get_or_create(function);
}

template<FactKind T>
std::pair<FunctionTermView<T>, bool> merge_d2d(FunctionTermView<T> element, MergeContext& context)
{
    auto fterm_ptr = context.builder.template get_builder<FunctionTerm<T>>();
    auto& fterm = *fterm_ptr;
    fterm.clear();

    fterm.function = element.get_function().get_index();
    for (const auto term : element.get_terms())
        fterm.terms.push_back(merge_d2d(term, context));

    canonicalize(fterm);
    return context.destination.get_or_create(fterm);
}

template<FactKind T>
std::pair<FunctionBindingView<T>, bool> merge_d2d(FunctionBindingView<T> element, MergeContext& context)
{
    auto binding_ptr = context.builder.template get_builder<RelationBinding<Function<T>>>();
    auto& binding = *binding_ptr;
    binding.clear();

    binding.relation = element.get_relation().get_index();
    for (const auto object : element.get_objects())
        binding.objects.push_back(object.get_index());

    canonicalize(binding);
    return context.destination.get_or_create(binding);
}

template<FactKind T>
std::pair<GroundFunctionTermView<T>, bool> merge_d2d(GroundFunctionTermView<T> element, MergeContext& context)
{
    auto fterm_ptr = context.builder.template get_builder<GroundFunctionTerm<T>>();
    auto& fterm = *fterm_ptr;
    fterm.clear();

    fterm.binding = merge_d2d(element.get_row(), context).first.get_index();

    canonicalize(fterm);
    return context.destination.get_or_create(fterm);
}

template<FactKind T>
std::pair<GroundFunctionTermValueView<T>, bool> merge_d2d(GroundFunctionTermValueView<T> element, MergeContext& context)
{
    auto fterm_value_ptr = context.builder.template get_builder<GroundFunctionTermValue<T>>();
    auto& fterm_value = *fterm_value_ptr;
    fterm_value.clear();

    fterm_value.fterm = merge_d2d(element.get_fterm(), context).first.get_index();
    fterm_value.value = element.get_value();

    canonicalize(fterm_value);
    return context.destination.get_or_create(fterm_value);
}

TYR_INLINE_IMPL Data<FunctionExpression> merge_d2d(FunctionExpressionView element, MergeContext& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return Data<FunctionExpression>(arg);
            else if constexpr (std::is_same_v<Alternative, LiftedArithmeticOperatorView>)
                return Data<FunctionExpression>(merge_d2d(arg, context));
            else
                return Data<FunctionExpression>(merge_d2d(arg, context).first.get_index());
        },
        element.get_variant());
}

TYR_INLINE_IMPL Data<GroundFunctionExpression> merge_d2d(GroundFunctionExpressionView element, MergeContext& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return Data<GroundFunctionExpression>(arg);
            else if constexpr (std::is_same_v<Alternative, GroundArithmeticOperatorView>)
                return Data<GroundFunctionExpression>(merge_d2d(arg, context));
            else
                return Data<GroundFunctionExpression>(merge_d2d(arg, context).first.get_index());
        },
        element.get_variant());
}

template<OpKind O, typename T>
std::pair<UnaryOperatorView<O, T>, bool> merge_d2d(UnaryOperatorView<O, T> element, MergeContext& context)
{
    auto unary_ptr = context.builder.template get_builder<UnaryOperator<O, T>>();
    auto& unary = *unary_ptr;
    unary.clear();

    unary.arg = merge_d2d(element.get_arg(), context);

    canonicalize(unary);
    return context.destination.get_or_create(unary);
}

template<OpKind O, typename T>
std::pair<BinaryOperatorView<O, T>, bool> merge_d2d(BinaryOperatorView<O, T> element, MergeContext& context)
{
    auto binary_ptr = context.builder.template get_builder<BinaryOperator<O, T>>();
    auto& binary = *binary_ptr;
    binary.clear();

    binary.lhs = merge_d2d(element.get_lhs(), context);
    binary.rhs = merge_d2d(element.get_rhs(), context);

    canonicalize(binary);
    return context.destination.get_or_create(binary);
}

template<OpKind O, typename T>
std::pair<MultiOperatorView<O, T>, bool> merge_d2d(MultiOperatorView<O, T> element, MergeContext& context)
{
    auto multi_ptr = context.builder.template get_builder<MultiOperator<O, T>>();
    auto& multi = *multi_ptr;
    multi.clear();

    for (const auto arg : element.get_args())
        multi.args.push_back(merge_d2d(arg, context));

    canonicalize(multi);
    return context.destination.get_or_create(multi);
}

template<typename T>
Data<ArithmeticOperator<T>> merge_d2d(ArithmeticOperatorView<T> element, MergeContext& context)
{
    return visit([&](auto&& arg) { return Data<ArithmeticOperator<T>>(merge_d2d(arg, context).first.get_index()); }, element.get_variant());
}

template<typename T>
Data<BooleanOperator<T>> merge_d2d(BooleanOperatorView<T> element, MergeContext& context)
{
    return visit([&](auto&& arg) { return Data<BooleanOperator<T>>(merge_d2d(arg, context).first.get_index()); }, element.get_variant());
}

TYR_INLINE_IMPL std::pair<ConjunctiveConditionView, bool> merge_d2d(ConjunctiveConditionView element, MergeContext& context)
{
    auto conj_cond_ptr = context.builder.template get_builder<ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto literal : element.template get_literals<StaticTag>())
        conj_cond.static_literals.push_back(merge_d2d(literal, context).first.get_index());
    for (const auto literal : element.template get_literals<FluentTag>())
        conj_cond.fluent_literals.push_back(merge_d2d(literal, context).first.get_index());
    for (const auto numeric_constraint : element.get_numeric_constraints())
        conj_cond.numeric_constraints.push_back(merge_d2d(numeric_constraint, context));

    canonicalize(conj_cond);
    return context.destination.get_or_create(conj_cond);
}

TYR_INLINE_IMPL std::pair<GroundConjunctiveConditionView, bool> merge_d2d(GroundConjunctiveConditionView element, MergeContext& context)
{
    auto conj_cond_ptr = context.builder.template get_builder<GroundConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto literal : element.template get_literals<StaticTag>())
        conj_cond.static_literals.push_back(merge_d2d(literal, context).first.get_index());
    for (const auto literal : element.template get_literals<FluentTag>())
        conj_cond.fluent_literals.push_back(merge_d2d(literal, context).first.get_index());
    for (const auto numeric_constraint : element.get_numeric_constraints())
        conj_cond.numeric_constraints.push_back(merge_d2d(numeric_constraint, context));

    canonicalize(conj_cond);
    return context.destination.get_or_create(conj_cond);
}

TYR_INLINE_IMPL std::pair<RuleView, bool> merge_d2d(RuleView element, MergeContext& context)
{
    auto rule_ptr = context.builder.template get_builder<Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    for (const auto variable : element.get_variables())
        rule.variables.push_back(merge_d2d(variable, context).first.get_index());
    rule.body = merge_d2d(element.get_body(), context).first.get_index();
    rule.head = merge_d2d(element.get_head(), context).first.get_index();

    canonicalize(rule);
    return context.destination.get_or_create(rule);
}

}
