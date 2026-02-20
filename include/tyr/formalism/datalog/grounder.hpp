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

#ifndef TYR_FORMALISM_DATALOG_GROUNDER_DATALOG_HPP_
#define TYR_FORMALISM_DATALOG_GROUNDER_DATALOG_HPP_

#include "tyr/analysis/domains.hpp"
#include "tyr/common/itertools.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/canonicalization.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/grounder.hpp"
#include "tyr/formalism/datalog/indices.hpp"
#include "tyr/formalism/datalog/merge.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/declarations.hpp"

namespace tyr::formalism::datalog
{
struct GrounderContext
{
    Builder& builder;
    Repository& destination;
    IndexList<Object>& binding;
};

struct ConstGrounderContext
{
    Builder& builder;
    const Repository& destination;
    IndexList<Object>& binding;
};

/**
 * ground
 */

std::pair<Index<Binding>, bool> ground(View<DataList<Term>, Repository> element, GrounderContext& context);

std::pair<Index<Binding>, bool> ground(const IndexList<Object>& element, GrounderContext& context);

template<FactKind T>
std::pair<Index<GroundFunctionTerm<T>>, bool> ground(View<Index<FunctionTerm<T>>, Repository> element, GrounderContext& context);

Data<GroundFunctionExpression> ground(View<Data<FunctionExpression>, Repository> element, GrounderContext& context);

template<OpKind O>
std::pair<Index<UnaryOperator<O, Data<GroundFunctionExpression>>>, bool> ground(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, Repository> element,
                                                                                GrounderContext& context);

template<OpKind O>
std::pair<Index<BinaryOperator<O, Data<GroundFunctionExpression>>>, bool> ground(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, Repository> element,
                                                                                 GrounderContext& context);

template<OpKind O>
std::pair<Index<MultiOperator<O, Data<GroundFunctionExpression>>>, bool> ground(View<Index<MultiOperator<O, Data<FunctionExpression>>>, Repository> element,
                                                                                GrounderContext& context);

Data<BooleanOperator<Data<GroundFunctionExpression>>> ground(View<Data<BooleanOperator<Data<FunctionExpression>>>, Repository> element,
                                                             GrounderContext& context);

Data<ArithmeticOperator<Data<GroundFunctionExpression>>> ground(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, Repository> element,
                                                                GrounderContext& context);

template<FactKind T>
std::pair<Index<GroundAtom<T>>, bool> ground(View<Index<Atom<T>>, Repository> element, GrounderContext& context);

template<FactKind T>
std::pair<Index<GroundLiteral<T>>, bool> ground(View<Index<Literal<T>>, Repository> element, GrounderContext& context);

std::pair<Index<GroundConjunctiveCondition>, bool> ground(View<Index<ConjunctiveCondition>, Repository> element, GrounderContext& context);

std::pair<Index<GroundRule>, bool> ground(View<Index<Rule>, Repository> element, GrounderContext& context);

/**
 * ground_into_buffer
 */

template<FactKind T>
void ground_into_buffer(View<Index<Atom<T>>, Repository> element, const IndexList<Object>& binding, Data<GroundAtom<T>>& out_atom);

template<FactKind T>
void ground_into_buffer(View<Index<FunctionTerm<T>>, Repository> element, const IndexList<Object>& binding, Data<GroundFunctionTerm<T>>& out_fterm);

/**
 * is_ground
 */

bool is_ground(View<Data<Term>, Repository> element);

template<FactKind T>
bool is_ground(View<Index<FunctionTerm<T>>, Repository> element);

bool is_ground(View<Data<FunctionExpression>, Repository> element);

template<OpKind O>
bool is_ground(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, Repository> element);

template<OpKind O>
bool is_ground(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, Repository> element);

template<OpKind O>
bool is_ground(View<Index<MultiOperator<O, Data<FunctionExpression>>>, Repository> element);

bool is_ground(View<Data<BooleanOperator<Data<FunctionExpression>>>, Repository> element);

bool is_ground(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, Repository> element);

template<FactKind T>
bool is_ground(View<Index<Atom<T>>, Repository> element);

template<FactKind T>
bool is_ground(View<Index<Literal<T>>, Repository> element);

/**
 * Implementations
 */

/**
 * ground
 */

inline std::pair<Index<Binding>, bool> ground(View<DataList<Term>, Repository> element, GrounderContext& context)
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
                else if constexpr (std::is_same_v<Alternative, View<Index<Object>, Repository>>)
                    binding.objects.push_back(arg.get_index());
                else
                    static_assert(dependent_false<Alternative>::value, "Missing case");
            },
            term.get_variant());
    }

    // Canonicalize and Serialize
    canonicalize(binding);
    return context.destination.get_or_create(binding, context.builder.get_buffer());
}

inline std::pair<Index<Binding>, bool> ground(const IndexList<Object>& element, GrounderContext& context)
{
    // Fetch and clear
    auto binding_ptr = context.builder.template get_builder<Binding>();
    auto& binding = *binding_ptr;
    binding.clear();

    // Fill data
    binding.objects = element;

    // Canonicalize and Serialize
    canonicalize(binding);
    return context.destination.get_or_create(binding, context.builder.get_buffer());
}

template<FactKind T>
inline std::pair<Index<GroundFunctionTerm<T>>, bool> ground(View<Index<FunctionTerm<T>>, Repository> element, GrounderContext& context)
{
    // Fetch and clear
    auto fterm_ptr = context.builder.template get_builder<GroundFunctionTerm<T>>();
    auto& fterm = *fterm_ptr;
    fterm.clear();

    // Fill data
    fterm.index.group = element.get_function().get_index();
    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                    fterm.objects.push_back(context.binding[uint_t(arg)]);
                else if constexpr (std::is_same_v<Alternative, View<Index<Object>, Repository>>)
                    fterm.objects.push_back(arg.get_index());
                else
                    static_assert(dependent_false<Alternative>::value, "Missing case");
            },
            term.get_variant());
    }

    // Canonicalize and Serialize
    canonicalize(fterm);
    return context.destination.get_or_create(fterm, context.builder.get_buffer());
}

inline Data<GroundFunctionExpression> ground(View<Data<FunctionExpression>, Repository> element, GrounderContext& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return Data<GroundFunctionExpression>(arg);
            else if constexpr (std::is_same_v<Alternative, View<Data<ArithmeticOperator<Data<FunctionExpression>>>, Repository>>)
                return Data<GroundFunctionExpression>(ground(arg, context));
            else
                return Data<GroundFunctionExpression>(ground(arg, context).first);
        },
        element.get_variant());
}

template<OpKind O>
inline std::pair<Index<UnaryOperator<O, Data<GroundFunctionExpression>>>, bool>
ground(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, Repository> element, GrounderContext& context)
{
    // Fetch and clear
    auto unary_ptr = context.builder.template get_builder<UnaryOperator<O, Data<GroundFunctionExpression>>>();
    auto& unary = *unary_ptr;
    unary.clear();

    // Fill data
    unary.arg = ground(element.get_arg(), context);

    // Canonicalize and Serialize
    canonicalize(unary);
    return context.destination.get_or_create(unary, context.builder.get_buffer());
}

template<OpKind O>
inline std::pair<Index<BinaryOperator<O, Data<GroundFunctionExpression>>>, bool>
ground(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, Repository> element, GrounderContext& context)
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
    return context.destination.get_or_create(binary, context.builder.get_buffer());
}

template<OpKind O>
inline std::pair<Index<MultiOperator<O, Data<GroundFunctionExpression>>>, bool>
ground(View<Index<MultiOperator<O, Data<FunctionExpression>>>, Repository> element, GrounderContext& context)
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
    return context.destination.get_or_create(multi, context.builder.get_buffer());
}

inline Data<BooleanOperator<Data<GroundFunctionExpression>>> ground(View<Data<BooleanOperator<Data<FunctionExpression>>>, Repository> element,
                                                                    GrounderContext& context)
{
    return visit([&](auto&& arg) { return Data<BooleanOperator<Data<GroundFunctionExpression>>>(ground(arg, context).first); }, element.get_variant());
}

inline Data<ArithmeticOperator<Data<GroundFunctionExpression>>> ground(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, Repository> element,
                                                                       GrounderContext& context)
{
    return visit([&](auto&& arg) { return Data<ArithmeticOperator<Data<GroundFunctionExpression>>>(ground(arg, context).first); }, element.get_variant());
}

template<FactKind T>
inline std::pair<Index<GroundAtom<T>>, bool> ground(View<Index<Atom<T>>, Repository> element, GrounderContext& context)
{
    // Fetch and clear
    auto atom_ptr = context.builder.template get_builder<GroundAtom<T>>();
    auto& atom = *atom_ptr;
    atom.clear();

    // Fill data
    atom.index.group = element.get_predicate().get_index();
    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                    atom.objects.push_back(context.binding[uint_t(arg)]);
                else if constexpr (std::is_same_v<Alternative, View<Index<Object>, Repository>>)
                    atom.objects.push_back(arg.get_index());
                else
                    static_assert(dependent_false<Alternative>::value, "Missing case");
            },
            term.get_variant());
    }

    // Canonicalize and Serialize
    canonicalize(atom);
    return context.destination.get_or_create(atom, context.builder.get_buffer());
}

template<FactKind T>
inline std::pair<Index<GroundLiteral<T>>, bool> ground(View<Index<Literal<T>>, Repository> element, GrounderContext& context)
{
    // Fetch and clear
    auto ground_literal_ptr = context.builder.template get_builder<GroundLiteral<T>>();
    auto& ground_literal = *ground_literal_ptr;
    ground_literal.clear();

    // Fill data
    ground_literal.polarity = element.get_polarity();
    ground_literal.atom = ground(element.get_atom(), context).first;

    // Canonicalize and Serialize
    canonicalize(ground_literal);
    return context.destination.get_or_create(ground_literal, context.builder.get_buffer());
}

inline std::pair<Index<GroundConjunctiveCondition>, bool> ground(View<Index<ConjunctiveCondition>, Repository> element, GrounderContext& context)
{
    // Fetch and clear
    auto conj_cond_ptr = context.builder.template get_builder<GroundConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    // Fill data
    for (const auto literal : element.template get_literals<StaticTag>())
        conj_cond.static_literals.push_back(ground(literal, context).first);
    for (const auto literal : element.template get_literals<FluentTag>())
        conj_cond.fluent_literals.push_back(ground(literal, context).first);
    for (const auto numeric_constraint : element.get_numeric_constraints())
        conj_cond.numeric_constraints.push_back(ground(numeric_constraint, context));

    // Canonicalize and Serialize
    canonicalize(conj_cond);
    return context.destination.get_or_create(conj_cond, context.builder.get_buffer());
}

inline std::pair<Index<GroundRule>, bool> ground(View<Index<Rule>, Repository> element, GrounderContext& context)
{
    // Fetch and clear
    auto rule_ptr = context.builder.template get_builder<GroundRule>();
    auto& rule = *rule_ptr;
    rule.clear();

    // Fill data
    rule.rule = element.get_index();
    rule.binding = ground(context.binding, context).first;
    rule.body = ground(element.get_body(), context).first;
    rule.head = ground(element.get_head(), context).first;

    // Canonicalize and Serialize
    canonicalize(rule);
    return context.destination.get_or_create(rule, context.builder.get_buffer());
}

/**
 * ground_into_buffer
 */

template<FactKind T>
inline void ground_into_buffer(View<Index<Atom<T>>, Repository> element, const IndexList<Object>& binding, Data<GroundAtom<T>>& out_atom)
{
    // Fetch and clear
    out_atom.clear();

    // Fill data
    out_atom.index.group = element.get_predicate().get_index();
    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                    out_atom.objects.push_back(binding[uint_t(arg)]);
                else if constexpr (std::is_same_v<Alternative, View<Index<Object>, Repository>>)
                    out_atom.objects.push_back(arg.get_index());
                else
                    static_assert(dependent_false<Alternative>::value, "Missing case");
            },
            term.get_variant());
    }

    // Canonicalize and Serialize
    canonicalize(out_atom);
}

template<FactKind T>
inline void ground_into_buffer(View<Index<FunctionTerm<T>>, Repository> element, const IndexList<Object>& binding, Data<GroundFunctionTerm<T>>& out_fterm)
{
    // Fetch and clear
    out_fterm.clear();

    // Fill data
    out_fterm.index.group = element.get_function().get_index();
    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                    out_fterm.objects.push_back(binding[uint_t(arg)]);
                else if constexpr (std::is_same_v<Alternative, View<Index<Object>, Repository>>)
                    out_fterm.objects.push_back(arg.get_index());
                else
                    static_assert(dependent_false<Alternative>::value, "Missing case");
            },
            term.get_variant());
    }

    // Canonicalize and Serialize
    canonicalize(out_fterm);
}

/**
 * is_ground
 */

inline bool is_ground(View<Data<Term>, Repository> element)
{
    return visit(
        [&](auto&& arg) -> bool
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                return false;
            else if constexpr (std::is_same_v<Alternative, View<Index<Object>, Repository>>)
                return true;
            else
                static_assert(dependent_false<Alternative>::value, "Missing case");
        },
        element.get_variant());
}

template<FactKind T>
inline bool is_ground(View<Index<FunctionTerm<T>>, Repository> element)
{
    const auto terms = element.get_terms();
    return std::all_of(terms.begin(), terms.end(), [](auto&& arg) { return is_ground(arg); });
}

inline bool is_ground(View<Data<FunctionExpression>, Repository> element)
{
    return visit(
        [&](auto&& arg) -> bool
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return true;
            else
                return is_ground(arg);
        },
        element.get_variant());
}

template<OpKind O>
inline bool is_ground(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, Repository> element)
{
    return is_ground(element.get_arg());
}

template<OpKind O>
inline bool is_ground(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, Repository> element)
{
    return is_ground(element.get_lhs()) && is_ground(element.get_rhs());
}

template<OpKind O>
inline bool is_ground(View<Index<MultiOperator<O, Data<FunctionExpression>>>, Repository> element)
{
    const auto args = element.get_args();
    return std::all_of(args.begin(), args.end(), [](auto&& arg) { return is_ground(arg); });
}

inline bool is_ground(View<Data<BooleanOperator<Data<FunctionExpression>>>, Repository> element)
{
    return visit([&](auto&& arg) { return is_ground(arg); }, element.get_variant());
}

inline bool is_ground(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, Repository> element)
{
    return visit([&](auto&& arg) { return is_ground(arg); }, element.get_variant());
}

template<FactKind T>
inline bool is_ground(View<Index<Atom<T>>, Repository> element)
{
    const auto terms = element.get_terms();
    return std::all_of(terms.begin(), terms.end(), [](auto&& arg) { return is_ground(arg); });
}

template<FactKind T>
inline bool is_ground(View<Index<Literal<T>>, Repository> element)
{
    return is_ground(element.get_atom());
}

}

#endif