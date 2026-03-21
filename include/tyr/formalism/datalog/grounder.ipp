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

#include "tyr/analysis/domains.hpp"
#include "tyr/common/itertools.hpp"
#include "tyr/common/macros.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/canonicalization.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/datalog/grounder_decl.hpp"
#include "tyr/formalism/datalog/indices.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/declarations.hpp"

namespace tyr::formalism::datalog
{

TYR_INLINE_IMPL std::pair<BindingView, bool> ground(TermListView element, GrounderContext& context)
{
    // Fetch and clear
    auto binding_ptr = context.builder.template get_builder<Binding>();
    auto& binding = *binding_ptr;
    binding.clear();

    // Fill data
    for (const auto term : element)
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                    binding.objects.push_back(context.binding[uint_t(arg)]);
                else if constexpr (std::is_same_v<Alternative, ObjectView>)
                    binding.objects.push_back(arg.get_index());
                else
                    static_assert(dependent_false<Alternative>::value, "Missing case");
            },
            term.get_variant());
    }

    // Canonicalize and Serialize
    canonicalize(binding);
    return context.destination.get_or_create(binding);
}

TYR_INLINE_IMPL std::pair<BindingView, bool> ground(const IndexList<Object>& element, GrounderContext& context)
{
    // Fetch and clear
    auto binding_ptr = context.builder.template get_builder<Binding>();
    auto& binding = *binding_ptr;
    binding.clear();

    // Fill data
    binding.objects = element;

    // Canonicalize and Serialize
    canonicalize(binding);
    return context.destination.get_or_create(binding);
}

template<FactKind T>
std::pair<FunctionBindingView<T>, bool> ground(TermListView terms, FunctionView<T> function, GrounderContext& context)
{
    auto binding_ptr = context.builder.template get_builder<Binding>();
    auto& binding = *binding_ptr;
    binding.clear();

    for (const auto term : terms)
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                    binding.objects.push_back(context.binding[uint_t(arg)]);
                else if constexpr (std::is_same_v<Alternative, ObjectView>)
                    binding.objects.push_back(arg.get_index());
                else
                    static_assert(dependent_false<Alternative>::value, "Missing case");
            },
            term.get_variant());
    }

    // Canonicalize and Serialize
    canonicalize(binding);
    return context.destination.get_or_create(function, binding.objects);
}

template<FactKind T>
std::pair<GroundFunctionTermView<T>, bool> ground(FunctionTermView<T> element, GrounderContext& context)
{
    // Fetch and clear
    auto fterm_ptr = context.builder.template get_builder<GroundFunctionTerm<T>>();
    auto& fterm = *fterm_ptr;
    fterm.clear();

    // Fill data
    fterm.function = element.get_function().get_index();
    fterm.row = ground(element.get_terms(), element.get_function(), context).first.get_index().row;

    // Canonicalize and Serialize
    canonicalize(fterm);
    return context.destination.get_or_create(fterm);
}

TYR_INLINE_IMPL Data<GroundFunctionExpression> ground(FunctionExpressionView element, GrounderContext& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return Data<GroundFunctionExpression>(arg);
            else if constexpr (std::is_same_v<Alternative, LiftedArithmeticOperatorView>)
                return Data<GroundFunctionExpression>(ground(arg, context));
            else
                return Data<GroundFunctionExpression>(ground(arg, context).first.get_index());
        },
        element.get_variant());
}

template<OpKind O>
std::pair<GroundUnaryOperatorView<O>, bool> ground(LiftedUnaryOperatorView<O> element, GrounderContext& context)
{
    // Fetch and clear
    auto unary_ptr = context.builder.template get_builder<UnaryOperator<O, Data<GroundFunctionExpression>>>();
    auto& unary = *unary_ptr;
    unary.clear();

    // Fill data
    unary.arg = ground(element.get_arg(), context);

    // Canonicalize and Serialize
    canonicalize(unary);
    return context.destination.get_or_create(unary);
}

template<OpKind O>
std::pair<GroundBinaryOperatorView<O>, bool> ground(LiftedBinaryOperatorView<O> element, GrounderContext& context)
{
    // Fetch and clear
    auto binary_ptr = context.builder.template get_builder<BinaryOperator<O, Data<GroundFunctionExpression>>>();
    auto& binary = *binary_ptr;
    binary.clear();

    // Fill data
    binary.lhs = ground(element.get_lhs(), context);
    binary.rhs = ground(element.get_rhs(), context);

    // Canonicalize and Serialize
    canonicalize(binary);
    return context.destination.get_or_create(binary);
}

template<OpKind O>
std::pair<GroundMultiOperatorView<O>, bool> ground(LiftedMultiOperatorView<O> element, GrounderContext& context)
{
    // Fetch and clear
    auto multi_ptr = context.builder.template get_builder<MultiOperator<O, Data<GroundFunctionExpression>>>();
    auto& multi = *multi_ptr;
    multi.clear();

    // Fill data
    for (const auto arg : element.get_args())
        multi.args.push_back(ground(arg, context));

    // Canonicalize and Serialize
    canonicalize(multi);
    return context.destination.get_or_create(multi);
}

TYR_INLINE_IMPL Data<BooleanOperator<Data<GroundFunctionExpression>>> ground(LiftedBooleanOperatorView element, GrounderContext& context)
{
    return visit([&](auto&& arg) { return Data<BooleanOperator<Data<GroundFunctionExpression>>>(ground(arg, context).first.get_index()); },
                 element.get_variant());
}

TYR_INLINE_IMPL Data<ArithmeticOperator<Data<GroundFunctionExpression>>> ground(LiftedArithmeticOperatorView element, GrounderContext& context)
{
    return visit([&](auto&& arg) { return Data<ArithmeticOperator<Data<GroundFunctionExpression>>>(ground(arg, context).first.get_index()); },
                 element.get_variant());
}

template<FactKind T>
std::pair<PredicateBindingView<T>, bool> ground(TermListView terms, PredicateView<T> predicate, GrounderContext& context)
{
    auto binding_ptr = context.builder.template get_builder<Binding>();
    auto& binding = *binding_ptr;
    binding.clear();

    for (const auto term : terms)
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                    binding.objects.push_back(context.binding[uint_t(arg)]);
                else if constexpr (std::is_same_v<Alternative, ObjectView>)
                    binding.objects.push_back(arg.get_index());
                else
                    static_assert(dependent_false<Alternative>::value, "Missing case");
            },
            term.get_variant());
    }

    // Canonicalize and Serialize
    canonicalize(binding);
    return context.destination.get_or_create(predicate, binding.objects);
}

template<FactKind T>
std::pair<PredicateBindingView<T>, bool> ground_binding(AtomView<T> element, GrounderContext& context)
{
    auto binding_ptr = context.builder.template get_builder<Binding>();
    auto& binding = *binding_ptr;
    binding.clear();

    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                    binding.objects.push_back(context.binding[uint_t(arg)]);
                else if constexpr (std::is_same_v<Alternative, ObjectView>)
                    binding.objects.push_back(arg.get_index());
                else
                    static_assert(dependent_false<Alternative>::value, "Missing case");
            },
            term.get_variant());
    }

    // Canonicalize and Serialize
    canonicalize(binding);
    return context.destination.get_or_create(element.get_predicate(), binding.objects);
}

template<FactKind T>
std::pair<GroundAtomView<T>, bool> ground(AtomView<T> element, GrounderContext& context)
{
    // Fetch and clear
    auto atom_ptr = context.builder.template get_builder<GroundAtom<T>>();
    auto& atom = *atom_ptr;
    atom.clear();

    // Fill data
    atom.predicate = element.get_predicate().get_index();
    atom.row = ground(element.get_terms(), element.get_predicate(), context).first.get_index().row;

    // Canonicalize and Serialize
    canonicalize(atom);
    return context.destination.get_or_create(atom);
}

template<FactKind T>
std::pair<GroundLiteralView<T>, bool> ground(LiteralView<T> element, GrounderContext& context)
{
    // Fetch and clear
    auto ground_literal_ptr = context.builder.template get_builder<GroundLiteral<T>>();
    auto& ground_literal = *ground_literal_ptr;
    ground_literal.clear();

    // Fill data
    ground_literal.polarity = element.get_polarity();
    ground_literal.atom = ground(element.get_atom(), context).first.get_index();

    // Canonicalize and Serialize
    canonicalize(ground_literal);
    return context.destination.get_or_create(ground_literal);
}

TYR_INLINE_IMPL std::pair<GroundConjunctiveConditionView, bool> ground(ConjunctiveConditionView element, GrounderContext& context)
{
    // Fetch and clear
    auto conj_cond_ptr = context.builder.template get_builder<GroundConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    // Fill data
    for (const auto literal : element.template get_literals<StaticTag>())
        conj_cond.static_literals.push_back(ground(literal, context).first.get_index());
    for (const auto literal : element.template get_literals<FluentTag>())
        conj_cond.fluent_literals.push_back(ground(literal, context).first.get_index());
    for (const auto numeric_constraint : element.get_numeric_constraints())
        conj_cond.numeric_constraints.push_back(ground(numeric_constraint, context));

    // Canonicalize and Serialize
    canonicalize(conj_cond);
    return context.destination.get_or_create(conj_cond);
}

TYR_INLINE_IMPL std::pair<RuleBindingView, bool> ground_binding(RuleView element, GrounderContext& context)
{
    return context.destination.get_or_create(element, context.binding);
}

TYR_INLINE_IMPL std::pair<GroundRuleView, bool> ground(RuleView element, GrounderContext& context)
{
    // Fetch and clear
    auto rule_ptr = context.builder.template get_builder<GroundRule>();
    auto& rule = *rule_ptr;
    rule.clear();

    // Fill data
    rule.rule = element.get_index();
    rule.row = context.destination.get_or_create(element, context.binding).first.get_index().row;
    rule.body = ground(element.get_body(), context).first.get_index();
    rule.head = ground(element.get_head(), context).first.get_index();

    // Canonicalize and Serialize
    canonicalize(rule);
    return context.destination.get_or_create(rule);
}
}
