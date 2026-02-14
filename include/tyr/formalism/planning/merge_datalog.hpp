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

#ifndef TYR_FORMALISM_PLANNING_MERGE_DATALOG_HPP_
#define TYR_FORMALISM_PLANNING_MERGE_DATALOG_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/canonicalization.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/indices.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/indices.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"
#include "tyr/formalism/views.hpp"

namespace tyr::formalism::planning
{

struct MergeDatalogContext
{
    formalism::datalog::Builder& builder;
    formalism::datalog::Repository& destination;
};

/**
 * Forward declarations
 */

// Common

inline auto merge_p2d(View<Index<formalism::Variable>, Repository> element, MergeDatalogContext& context);

inline auto merge_p2d(View<Index<formalism::Object>, Repository> element, MergeDatalogContext& context);

inline auto merge_p2d(View<Index<formalism::Binding>, Repository> element, MergeDatalogContext& context);

inline auto merge_p2d(View<Data<formalism::Term>, Repository> element, MergeDatalogContext& context);

// Propositional

template<FactKind T_SRC, FactKind T_DST = T_SRC>
inline auto merge_p2d(View<Index<formalism::Predicate<T_SRC>>, Repository> element, MergeDatalogContext& context);

template<FactKind T_SRC, FactKind T_DST = T_SRC>
inline auto merge_p2d(View<Index<Atom<T_SRC>>, Repository> element, MergeDatalogContext& context);

template<FactKind T_SRC, FactKind T_DST = T_SRC>
inline auto merge_p2d(View<Index<GroundAtom<T_SRC>>, Repository> element, MergeDatalogContext& context);

template<FactKind T_SRC, FactKind T_DST = T_SRC>
inline auto merge_p2d(View<Index<Literal<T_SRC>>, Repository> element, MergeDatalogContext& context);

template<FactKind T_SRC, FactKind T_DST = T_SRC>
inline auto merge_p2d(View<Index<GroundLiteral<T_SRC>>, Repository> element, MergeDatalogContext& context);

// Numeric

template<FactKind T>
inline auto merge_p2d(View<Index<formalism::Function<T>>, Repository> element, MergeDatalogContext& context);

template<FactKind T>
inline auto merge_p2d(View<Index<FunctionTerm<T>>, Repository> element, MergeDatalogContext& context);

template<FactKind T>
inline auto merge_p2d(View<Index<GroundFunctionTerm<T>>, Repository> element, MergeDatalogContext& context);

template<FactKind T>
inline auto merge_p2d(View<Index<GroundFunctionTermValue<T>>, Repository> element, MergeDatalogContext& context);

inline auto merge_p2d(View<Data<FunctionExpression>, Repository> element, MergeDatalogContext& context);

inline auto merge_p2d(View<Data<GroundFunctionExpression>, Repository> element, MergeDatalogContext& context);

template<OpKind O, typename T>
inline auto merge_p2d(View<Index<UnaryOperator<O, T>>, Repository> element, MergeDatalogContext& context);

template<OpKind O, typename T>
inline auto merge_p2d(View<Index<BinaryOperator<O, T>>, Repository> element, MergeDatalogContext& context);

template<OpKind O, typename T>
inline auto merge_p2d(View<Index<MultiOperator<O, T>>, Repository> element, MergeDatalogContext& context);

template<typename T>
inline auto merge_p2d(View<Data<ArithmeticOperator<T>>, Repository> element, MergeDatalogContext& context);

template<typename T>
inline auto merge_p2d(View<Data<BooleanOperator<T>>, Repository> element, MergeDatalogContext& context);

/**
 * Implementations
 */

template<typename T>
struct to_datalog_payload
{
    using type = T;  // default: unchanged
};

template<>
struct to_datalog_payload<Data<formalism::planning::FunctionExpression>>
{
    using type = Data<formalism::datalog::FunctionExpression>;
};

template<>
struct to_datalog_payload<Data<formalism::planning::GroundFunctionExpression>>
{
    using type = Data<formalism::datalog::GroundFunctionExpression>;
};

template<typename T>
using to_datalog_payload_t = typename to_datalog_payload<T>::type;

// Common

inline auto merge_p2d(View<Index<formalism::Variable>, Repository> element, MergeDatalogContext& context)
{
    auto variable_ptr = context.builder.template get_builder<formalism::Variable>();
    auto& variable = *variable_ptr;
    variable.clear();

    variable.name = element.get_name();

    canonicalize(variable);
    return context.destination.get_or_create(variable, context.builder.get_buffer());
}

inline auto merge_p2d(View<Index<formalism::Object>, Repository> element, MergeDatalogContext& context)
{
    auto object_ptr = context.builder.template get_builder<formalism::Object>();
    auto& object = *object_ptr;
    object.clear();

    object.name = element.get_name();

    canonicalize(object);
    return context.destination.get_or_create(object, context.builder.get_buffer());
}

inline auto merge_p2d(View<Index<formalism::Binding>, Repository> element, MergeDatalogContext& context)
{
    auto binding_ptr = context.builder.template get_builder<formalism::Binding>();
    auto& binding = *binding_ptr;
    binding.clear();

    binding.objects = element.get_data().objects;

    canonicalize(binding);
    return context.destination.get_or_create(binding, context.builder.get_buffer());
}

inline auto merge_p2d(View<Data<Term>, Repository> element, MergeDatalogContext& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                return Data<Term>(arg);
            else if constexpr (std::is_same_v<Alternative, View<Index<Object>, Repository>>)
                return Data<Term>(merge_p2d(arg, context).first);
            else
                static_assert(dependent_false<Alternative>::value, "Missing case");
        },
        element.get_variant());
}

// Propositional

template<FactKind T_SRC, FactKind T_DST>
inline auto merge_p2d(View<Index<formalism::Predicate<T_SRC>>, Repository> element, MergeDatalogContext& context)
{
    auto predicate_ptr = context.builder.template get_builder<formalism::Predicate<T_DST>>();
    auto& predicate = *predicate_ptr;
    predicate.clear();

    predicate.name = element.get_name();
    predicate.arity = element.get_arity();

    canonicalize(predicate);
    return context.destination.get_or_create(predicate, context.builder.get_buffer());
}

template<FactKind T_SRC, FactKind T_DST>
inline auto merge_p2d(View<Index<Atom<T_SRC>>, Repository> element, MergeDatalogContext& context)
{
    auto atom_ptr = context.builder.template get_builder<formalism::datalog::Atom<T_DST>>();
    auto& atom = *atom_ptr;
    atom.clear();

    atom.predicate = merge_p2d<T_SRC, T_DST>(element.get_predicate(), context).first;
    for (const auto term : element.get_terms())
        atom.terms.push_back(merge_p2d(term, context));

    canonicalize(atom);
    return context.destination.get_or_create(atom, context.builder.get_buffer());
}

template<FactKind T_SRC, FactKind T_DST>
inline auto merge_p2d(View<Index<GroundAtom<T_SRC>>, Repository> element, MergeDatalogContext& context)
{
    auto atom_ptr = context.builder.template get_builder<formalism::datalog::GroundAtom<T_DST>>();
    auto& atom = *atom_ptr;
    atom.clear();

    atom.index.group = merge_p2d<T_SRC, T_DST>(element.get_predicate(), context).first;
    atom.objects = element.get_data().objects;

    canonicalize(atom);
    return context.destination.get_or_create(atom, context.builder.get_buffer());
}

template<FactKind T_SRC, FactKind T_DST>
inline auto merge_p2d(View<Index<Literal<T_SRC>>, Repository> element, MergeDatalogContext& context)
{
    auto literal_ptr = context.builder.template get_builder<formalism::datalog::Literal<T_DST>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.polarity = element.get_polarity();
    literal.atom = merge_p2d<T_SRC, T_DST>(element.get_atom(), context).first;

    canonicalize(literal);
    return context.destination.get_or_create(literal, context.builder.get_buffer());
}

template<FactKind T_SRC, FactKind T_DST>
inline auto merge_p2d(View<Index<GroundLiteral<T_SRC>>, Repository> element, MergeDatalogContext& context)
{
    auto literal_ptr = context.builder.template get_builder<formalism::datalog::GroundLiteral<T_DST>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.polarity = element.get_polarity();
    literal.atom = merge_p2d<T_SRC, T_DST>(element.get_atom(), context).first;

    canonicalize(literal);
    return context.destination.get_or_create(literal, context.builder.get_buffer());
}

// Numeric

template<FactKind T>
    requires(!std::is_same_v<T, formalism::AuxiliaryTag>)
inline auto merge_p2d(View<Index<formalism::Function<T>>, Repository> element, MergeDatalogContext& context)
{
    auto function_ptr = context.builder.template get_builder<formalism::Function<T>>();
    auto& function = *function_ptr;
    function.clear();

    function.name = element.get_name();
    function.arity = element.get_arity();

    canonicalize(function);
    return context.destination.get_or_create(function, context.builder.get_buffer());
}

template<FactKind T>
    requires(std::is_same_v<T, formalism::AuxiliaryTag>)
inline auto merge_p2d(View<Index<formalism::Function<T>>, Repository> element, MergeDatalogContext& context)
{
    throw std::runtime_error("We never merge AuxiliaryTag");
}

template<FactKind T>
    requires(!std::is_same_v<T, formalism::AuxiliaryTag>)
inline auto merge_p2d(View<Index<FunctionTerm<T>>, Repository> element, MergeDatalogContext& context)
{
    auto fterm_ptr = context.builder.template get_builder<formalism::datalog::FunctionTerm<T>>();
    auto& fterm = *fterm_ptr;
    fterm.clear();

    fterm.function = element.get_function().get_index();
    for (const auto term : element.get_terms())
        fterm.terms.push_back(merge_p2d(term, context));

    canonicalize(fterm);
    return context.destination.get_or_create(fterm, context.builder.get_buffer());
}

template<FactKind T>
    requires(std::is_same_v<T, formalism::AuxiliaryTag>)
inline auto merge_p2d(View<Index<FunctionTerm<T>>, Repository> element, MergeDatalogContext& context)
{
    throw std::runtime_error("We never merge AuxiliaryTag");
}

template<FactKind T>
    requires(!std::is_same_v<T, formalism::AuxiliaryTag>)
inline auto merge_p2d(View<Index<GroundFunctionTerm<T>>, Repository> element, MergeDatalogContext& context)
{
    auto fterm_ptr = context.builder.template get_builder<formalism::datalog::GroundFunctionTerm<T>>();
    auto& fterm = *fterm_ptr;
    fterm.clear();

    fterm.index.group = element.get_function().get_index();
    fterm.objects = element.get_data().objects;

    canonicalize(fterm);
    return context.destination.get_or_create(fterm, context.builder.get_buffer());
}

template<FactKind T>
    requires(std::is_same_v<T, formalism::AuxiliaryTag>)
inline auto merge_p2d(View<Index<GroundFunctionTerm<T>>, Repository> element, MergeDatalogContext& context)
{
    throw std::runtime_error("We never merge AuxiliaryTag");
}

template<FactKind T>
    requires(!std::is_same_v<T, formalism::AuxiliaryTag>)
inline auto merge_p2d(View<Index<GroundFunctionTermValue<T>>, Repository> element, MergeDatalogContext& context)
{
    auto fterm_value_ptr = context.builder.template get_builder<formalism::datalog::GroundFunctionTermValue<T>>();
    auto& fterm_value = *fterm_value_ptr;
    fterm_value.clear();

    fterm_value.fterm = merge_p2d(element.get_fterm(), context).first;
    fterm_value.value = element.get_value();

    canonicalize(fterm_value);
    return context.destination.get_or_create(fterm_value, context.builder.get_buffer());
}

template<FactKind T>
    requires(std::is_same_v<T, formalism::AuxiliaryTag>)
inline auto merge_p2d(View<Index<GroundFunctionTermValue<T>>, Repository> element, MergeDatalogContext& context)
{
    throw std::runtime_error("We never merge AuxiliaryTag");
}

inline auto merge_p2d(View<Data<FunctionExpression>, Repository> element, MergeDatalogContext& context)
{
    return visit(
        [&](auto&& arg) -> Data<formalism::datalog::FunctionExpression>
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return Data<formalism::datalog::FunctionExpression>(arg);
            else if constexpr (std::is_same_v<Alternative, View<Data<ArithmeticOperator<Data<FunctionExpression>>>, Repository>>)
                return Data<formalism::datalog::FunctionExpression>(merge_p2d(arg, context));
            else if constexpr (std::is_same_v<Alternative, View<Index<FunctionTerm<AuxiliaryTag>>, Repository>>)
                throw std::logic_error("AuxiliaryTag FunctionTerm must not be merged.");
            else
                return Data<formalism::datalog::FunctionExpression>(merge_p2d(arg, context).first);
        },
        element.get_variant());
}

inline auto merge_p2d(View<Data<GroundFunctionExpression>, Repository> element, MergeDatalogContext& context)
{
    return visit(
        [&](auto&& arg) -> Data<formalism::datalog::GroundFunctionExpression>
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return Data<formalism::datalog::GroundFunctionExpression>(arg);
            else if constexpr (std::is_same_v<Alternative, View<Data<ArithmeticOperator<Data<GroundFunctionExpression>>>, Repository>>)
                return Data<formalism::datalog::GroundFunctionExpression>(merge_p2d(arg, context));
            else if constexpr (std::is_same_v<Alternative, View<Index<GroundFunctionTerm<AuxiliaryTag>>, Repository>>)
                throw std::logic_error("AuxiliaryTag GroundFunctionTerm must not be merged.");
            else
                return Data<formalism::datalog::GroundFunctionExpression>(merge_p2d(arg, context).first);
        },
        element.get_variant());
}

template<OpKind O, typename T>
inline auto merge_p2d(View<Index<UnaryOperator<O, T>>, Repository> element, MergeDatalogContext& context)
{
    using T_DST = to_datalog_payload_t<T>;

    auto unary_ptr = context.builder.template get_builder<formalism::datalog::UnaryOperator<O, T_DST>>();
    auto& unary = *unary_ptr;
    unary.clear();

    unary.arg = merge_p2d(element.get_arg(), context);

    canonicalize(unary);
    return context.destination.get_or_create(unary, context.builder.get_buffer());
}

template<OpKind O, typename T>
inline auto merge_p2d(View<Index<BinaryOperator<O, T>>, Repository> element, MergeDatalogContext& context)
{
    using T_DST = to_datalog_payload_t<T>;

    auto binary_ptr = context.builder.template get_builder<formalism::datalog::BinaryOperator<O, T_DST>>();
    auto& binary = *binary_ptr;
    binary.clear();

    binary.lhs = merge_p2d(element.get_lhs(), context);
    binary.rhs = merge_p2d(element.get_rhs(), context);

    canonicalize(binary);
    return context.destination.get_or_create(binary, context.builder.get_buffer());
}

template<OpKind O, typename T>
inline auto merge_p2d(View<Index<MultiOperator<O, T>>, Repository> element, MergeDatalogContext& context)
{
    using T_DST = to_datalog_payload_t<T>;

    auto multi_ptr = context.builder.template get_builder<formalism::datalog::MultiOperator<O, T_DST>>();
    auto& multi = *multi_ptr;
    multi.clear();

    for (const auto arg : element.get_args())
        multi.args.push_back(merge_p2d(arg, context));

    canonicalize(multi);
    return context.destination.get_or_create(multi, context.builder.get_buffer());
}

template<typename T>
inline auto merge_p2d(View<Data<ArithmeticOperator<T>>, Repository> element, MergeDatalogContext& context)
{
    using T_DST = to_datalog_payload_t<T>;

    return visit([&](auto&& arg) { return Data<formalism::datalog::ArithmeticOperator<T_DST>>(merge_p2d(arg, context).first); }, element.get_variant());
}

template<typename T>
inline auto merge_p2d(View<Data<BooleanOperator<T>>, Repository> element, MergeDatalogContext& context)
{
    using T_DST = to_datalog_payload_t<T>;

    return visit([&](auto&& arg) { return Data<formalism::datalog::BooleanOperator<T_DST>>(merge_p2d(arg, context).first); }, element.get_variant());
}

}

#endif