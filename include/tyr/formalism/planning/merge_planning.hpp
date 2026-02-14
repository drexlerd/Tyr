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

#ifndef TYR_FORMALISM_PLANNING_MERGE_PLANNING_HPP_
#define TYR_FORMALISM_PLANNING_MERGE_PLANNING_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/canonicalization.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/indices.hpp"
#include "tyr/formalism/planning/canonicalization.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/indices.hpp"
#include "tyr/formalism/planning/views.hpp"

namespace tyr::formalism::planning
{

template<Context C>
struct MergePlanningContext
{
    Builder& builder;
    C& destination;
};

/**
 * Forward declarations
 */

// Common

template<Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::Variable>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::Object>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::Binding>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Data<formalism::Term>, C_SRC> element, MergePlanningContext<C_DST>& context);

// Propositional

template<FactKind T_SRC, formalism::datalog::Context C_SRC, Context C_DST, FactKind T_DST = T_SRC>
auto merge_d2p(View<Index<formalism::Predicate<T_SRC>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<FactKind T_SRC, formalism::datalog::Context C_SRC, Context C_DST, FactKind T_DST = T_SRC>
auto merge_d2p(View<Index<formalism::datalog::Atom<T_SRC>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<FactKind T_SRC, formalism::datalog::Context C_SRC, Context C_DST, FactKind T_DST = T_SRC>
auto merge_d2p(View<Index<formalism::datalog::GroundAtom<T_SRC>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<FactKind T_SRC, formalism::datalog::Context C_SRC, Context C_DST, FactKind T_DST = T_SRC>
auto merge_d2p(View<Index<formalism::datalog::Literal<T_SRC>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<FactKind T_SRC, formalism::datalog::Context C_SRC, Context C_DST, FactKind T_DST = T_SRC>
auto merge_d2p(View<Index<formalism::datalog::GroundLiteral<T_SRC>>, C_SRC> element, MergePlanningContext<C_DST>& context);

// Numeric

template<FactKind T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::Function<T>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<FactKind T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::FunctionTerm<T>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<FactKind T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::GroundFunctionTerm<T>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<FactKind T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::GroundFunctionTermValue<T>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Data<formalism::datalog::FunctionExpression>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Data<formalism::datalog::GroundFunctionExpression>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<OpKind O, typename T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::UnaryOperator<O, T>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<OpKind O, typename T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::BinaryOperator<O, T>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<OpKind O, typename T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::MultiOperator<O, T>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<typename T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Data<formalism::datalog::ArithmeticOperator<T>>, C_SRC> element, MergePlanningContext<C_DST>& context);

template<typename T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Data<formalism::datalog::BooleanOperator<T>>, C_SRC> element, MergePlanningContext<C_DST>& context);

/**
 * Implementations
 */

template<typename T>
struct to_planning_payload
{
    using type = T;  // default: unchanged
};

template<>
struct to_planning_payload<Data<formalism::datalog::FunctionExpression>>
{
    using type = Data<formalism::planning::FunctionExpression>;
};

template<>
struct to_planning_payload<Data<formalism::datalog::GroundFunctionExpression>>
{
    using type = Data<formalism::planning::GroundFunctionExpression>;
};

template<typename T>
using to_planning_payload_t = typename to_planning_payload<T>::type;

// Common

template<formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::Variable>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    auto variable_ptr = context.builder.template get_builder<formalism::Variable>();
    auto& variable = *variable_ptr;
    variable.clear();

    variable.name = element.get_name();

    canonicalize(variable);
    return context.destination.get_or_create(variable, context.builder.get_buffer());
}

template<formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::Object>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    auto object_ptr = context.builder.template get_builder<formalism::Object>();
    auto& object = *object_ptr;
    object.clear();

    object.name = element.get_name();

    canonicalize(object);
    return context.destination.get_or_create(object, context.builder.get_buffer());
}

template<formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::Binding>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    auto binding_ptr = context.builder.template get_builder<formalism::Binding>();
    auto& binding = *binding_ptr;
    binding.clear();

    binding.objects = element.get_data().objects;

    canonicalize(binding);
    return context.destination.get_or_create(binding, context.builder.get_buffer());
}

template<formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Data<formalism::Term>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, formalism::ParameterIndex>)
                return Data<formalism::Term>(arg);
            else if constexpr (std::is_same_v<Alternative, View<Index<formalism::Object>, C_SRC>>)
                return Data<formalism::Term>(merge_d2p(arg, context).first);
            else
                static_assert(dependent_false<Alternative>::value, "Missing case");
        },
        element.get_variant());
}

// Propositional

template<FactKind T_SRC, formalism::datalog::Context C_SRC, Context C_DST, FactKind T_DST>
auto merge_d2p(View<Index<formalism::Predicate<T_SRC>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    auto predicate_ptr = context.builder.template get_builder<formalism::Predicate<T_DST>>();
    auto& predicate = *predicate_ptr;
    predicate.clear();

    predicate.name = element.get_name();
    predicate.arity = element.get_arity();

    canonicalize(predicate);
    return context.destination.get_or_create(predicate, context.builder.get_buffer());
}

template<FactKind T_SRC, formalism::datalog::Context C_SRC, Context C_DST, FactKind T_DST>
auto merge_d2p(View<Index<formalism::datalog::Atom<T_SRC>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    auto atom_ptr = context.builder.template get_builder<Atom<T_DST>>();
    auto& atom = *atom_ptr;
    atom.clear();

    atom.predicate = merge_d2p<T_SRC, C_SRC, C_DST, T_DST>(element.get_predicate(), context).first;
    for (const auto term : element.get_terms())
        atom.terms.push_back(merge_d2p(term, context));

    canonicalize(atom);
    return context.destination.get_or_create(atom, context.builder.get_buffer());
}

template<FactKind T_SRC, formalism::datalog::Context C_SRC, Context C_DST, FactKind T_DST>
auto merge_d2p(View<Index<formalism::datalog::GroundAtom<T_SRC>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    auto atom_ptr = context.builder.template get_builder<GroundAtom<T_DST>>();
    auto& atom = *atom_ptr;
    atom.clear();

    atom.predicate = merge_d2p<T_SRC, C_SRC, C_DST, T_DST>(element.get_predicate(), context).first;
    atom.objects = element.get_data().objects;

    canonicalize(atom);
    return context.destination.get_or_create(atom, context.builder.get_buffer());
}

template<FactKind T_SRC, formalism::datalog::Context C_SRC, Context C_DST, FactKind T_DST>
auto merge_d2p(View<Index<formalism::datalog::Literal<T_SRC>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    auto literal_ptr = context.builder.template get_builder<Literal<T_DST>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.polarity = element.get_polarity();
    literal.atom = merge_d2p<T_SRC, C_SRC, C_DST, T_DST>(element.get_atom(), context).first;

    canonicalize(literal);
    return context.destination.get_or_create(literal, context.builder.get_buffer());
}

template<FactKind T_SRC, formalism::datalog::Context C_SRC, Context C_DST, FactKind T_DST>
auto merge_d2p(View<Index<formalism::datalog::GroundLiteral<T_SRC>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    auto literal_ptr = context.builder.template get_builder<GroundLiteral<T_DST>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.polarity = element.get_polarity();
    literal.atom = merge_d2p<T_SRC, C_SRC, C_DST, T_DST>(element.get_atom(), context).first;

    canonicalize(literal);
    return context.destination.get_or_create(literal, context.builder.get_buffer());
}

// Numeric

template<FactKind T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::Function<T>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    auto function_ptr = context.builder.template get_builder<formalism::Function<T>>();
    auto& function = *function_ptr;
    function.clear();

    function.name = element.get_name();
    function.arity = element.get_arity();

    canonicalize(function);
    return context.destination.get_or_create(function, context.builder.get_buffer());
}

template<FactKind T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::FunctionTerm<T>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    auto fterm_ptr = context.builder.template get_builder<FunctionTerm<T>>();
    auto& fterm = *fterm_ptr;
    fterm.clear();

    fterm.function = element.get_function().get_index();
    for (const auto term : element.get_terms())
        fterm.terms.push_back(merge_d2p(term, context));

    canonicalize(fterm);
    return context.destination.get_or_create(fterm, context.builder.get_buffer());
}

template<FactKind T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::GroundFunctionTerm<T>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    auto fterm_ptr = context.builder.template get_builder<GroundFunctionTerm<T>>();
    auto& fterm = *fterm_ptr;
    fterm.clear();

    fterm.function = element.get_function().get_index();
    fterm.objects = element.get_data().objects;

    canonicalize(fterm);
    return context.destination.get_or_create(fterm, context.builder.get_buffer());
}

template<FactKind T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::GroundFunctionTermValue<T>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    auto fterm_value_ptr = context.builder.template get_builder<GroundFunctionTermValue<T>>();
    auto& fterm_value = *fterm_value_ptr;
    fterm_value.clear();

    fterm_value.fterm = merge_d2p(element.get_fterm(), context).first;
    fterm_value.value = element.get_value();

    canonicalize(fterm_value);
    return context.destination.get_or_create(fterm_value, context.builder.get_buffer());
}

template<formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Data<formalism::datalog::FunctionExpression>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return Data<FunctionExpression>(arg);
            else if constexpr (std::is_same_v<Alternative, View<Data<ArithmeticOperator<Data<FunctionExpression>>>, C_SRC>>)
                return Data<FunctionExpression>(merge_d2p(arg, context));
            else
                return Data<FunctionExpression>(merge_d2p(arg, context).first);
        },
        element.get_variant());
}

template<formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Data<formalism::datalog::GroundFunctionExpression>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return Data<GroundFunctionExpression>(arg);
            else if constexpr (std::is_same_v<Alternative, View<Data<ArithmeticOperator<Data<GroundFunctionExpression>>>, C_SRC>>)
                return Data<GroundFunctionExpression>(merge_d2p(arg, context));
            else
                return Data<GroundFunctionExpression>(merge_d2p(arg, context).first);
        },
        element.get_variant());
}

template<OpKind O, typename T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::UnaryOperator<O, T>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    using T_DST = to_planning_payload_t<T>;

    auto unary_ptr = context.builder.template get_builder<UnaryOperator<O, T_DST>>();
    auto& unary = *unary_ptr;
    unary.clear();

    unary.arg = merge_d2p(element.get_arg(), context);

    canonicalize(unary);
    return context.destination.get_or_create(unary, context.builder.get_buffer());
}

template<OpKind O, typename T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::BinaryOperator<O, T>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    using T_DST = to_planning_payload_t<T>;

    auto binary_ptr = context.builder.template get_builder<BinaryOperator<O, T_DST>>();
    auto& binary = *binary_ptr;
    binary.clear();

    binary.lhs = merge_d2p(element.get_lhs(), context);
    binary.rhs = merge_d2p(element.get_rhs(), context);

    canonicalize(binary);
    return context.destination.get_or_create(binary, context.builder.get_buffer());
}

template<OpKind O, typename T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Index<formalism::datalog::MultiOperator<O, T>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    using T_DST = to_planning_payload_t<T>;

    auto multi_ptr = context.builder.template get_builder<MultiOperator<O, T_DST>>();
    auto& multi = *multi_ptr;
    multi.clear();

    for (const auto arg : element.get_args())
        multi.args.push_back(merge_d2p(arg, context));

    canonicalize(multi);
    return context.destination.get_or_create(multi, context.builder.get_buffer());
}

template<typename T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Data<formalism::datalog::ArithmeticOperator<T>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    using T_DST = to_planning_payload_t<T>;

    return visit([&](auto&& arg) { return Data<ArithmeticOperator<T_DST>>(merge_d2p(arg, context).first); }, element.get_variant());
}

template<typename T, formalism::datalog::Context C_SRC, Context C_DST>
auto merge_d2p(View<Data<formalism::datalog::BooleanOperator<T>>, C_SRC> element, MergePlanningContext<C_DST>& context)
{
    using T_DST = to_planning_payload_t<T>;

    return visit([&](auto&& arg) { return Data<BooleanOperator<T_DST>>(merge_d2p(arg, context).first); }, element.get_variant());
}

}

#endif