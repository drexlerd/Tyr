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

#ifndef TYR_DATALOG_APPLICABILITY_HPP_
#define TYR_DATALOG_APPLICABILITY_HPP_

#include "tyr/common/vector.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/formalism/arithmetic_operator_utils.hpp"
#include "tyr/formalism/boolean_operator_utils.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/grounder.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/declarations.hpp"

#include <algorithm>
#include <concepts>
#include <iterator>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace tyr::datalog
{

/**
 * evaluate
 */

// Forward declarations

inline float_t evaluate(float_t element, const FactSets& fact_sets);

template<formalism::ArithmeticOpKind O>
inline auto
evaluate(View<Index<formalism::datalog::UnaryOperator<O, Data<formalism::datalog::GroundFunctionExpression>>>, formalism::datalog::Repository> element,
         const FactSets& fact_sets);

template<formalism::OpKind O>
inline auto
evaluate(View<Index<formalism::datalog::BinaryOperator<O, Data<formalism::datalog::GroundFunctionExpression>>>, formalism::datalog::Repository> element,
         const FactSets& fact_sets);

template<formalism::ArithmeticOpKind O>
inline auto
evaluate(View<Index<formalism::datalog::MultiOperator<O, Data<formalism::datalog::GroundFunctionExpression>>>, formalism::datalog::Repository> element,
         const FactSets& fact_sets);

template<formalism::FactKind T>
    requires(!std::is_same_v<T, formalism::AuxiliaryTag>)
inline float_t evaluate(View<Index<formalism::datalog::GroundFunctionTerm<T>>, formalism::datalog::Repository> element, const FactSets& fact_sets);

inline float_t evaluate(View<Index<formalism::datalog::GroundFunctionTerm<formalism::AuxiliaryTag>>, formalism::datalog::Repository> element,
                        const FactSets& fact_sets);

inline float_t evaluate(View<Data<formalism::datalog::GroundFunctionExpression>, formalism::datalog::Repository> element, const FactSets& fact_sets);

inline float_t
evaluate(View<Data<formalism::datalog::ArithmeticOperator<Data<formalism::datalog::GroundFunctionExpression>>>, formalism::datalog::Repository> element,
         const FactSets& fact_sets);

inline bool
evaluate(View<Data<formalism::datalog::BooleanOperator<Data<formalism::datalog::GroundFunctionExpression>>>, formalism::datalog::Repository> element,
         const FactSets& fact_sets);

// Implementations

inline float_t evaluate(float_t element, const FactSets& fact_sets) { return element; }

template<formalism::ArithmeticOpKind O>
inline auto
evaluate(View<Index<formalism::datalog::UnaryOperator<O, Data<formalism::datalog::GroundFunctionExpression>>>, formalism::datalog::Repository> element,
         const FactSets& fact_sets)
{
    return formalism::apply(O {}, evaluate(element.get_arg(), fact_sets));
}

template<formalism::OpKind O>
inline auto
evaluate(View<Index<formalism::datalog::BinaryOperator<O, Data<formalism::datalog::GroundFunctionExpression>>>, formalism::datalog::Repository> element,
         const FactSets& fact_sets)
{
    return formalism::apply(O {}, evaluate(element.get_lhs(), fact_sets), evaluate(element.get_rhs(), fact_sets));
}

template<formalism::ArithmeticOpKind O>
inline auto
evaluate(View<Index<formalism::datalog::MultiOperator<O, Data<formalism::datalog::GroundFunctionExpression>>>, formalism::datalog::Repository> element,
         const FactSets& fact_sets)
{
    const auto child_fexprs = element.get_args();

    return std::accumulate(std::next(child_fexprs.begin()),  // Start from the second expression
                           child_fexprs.end(),
                           evaluate(child_fexprs.front(), fact_sets),
                           [&](const auto& value, const auto& child_expr) { return formalism::apply(O {}, value, evaluate(child_expr, fact_sets)); });
}

template<formalism::FactKind T>
    requires(!std::is_same_v<T, formalism::AuxiliaryTag>)
inline float_t evaluate(View<Index<formalism::datalog::GroundFunctionTerm<T>>, formalism::datalog::Repository> element, const FactSets& fact_sets)
{
    if (!fact_sets.template get<T>().function.contains(element.get_index()))
        return std::numeric_limits<float_t>::quiet_NaN();
    return fact_sets.template get<T>().function[element.get_index()];
}

inline float_t evaluate(View<Index<formalism::datalog::GroundFunctionTerm<formalism::AuxiliaryTag>>, formalism::datalog::Repository> element,
                        const FactSets& fact_sets)
{
    throw std::logic_error("Program does not contain auxiliary function terms.");
}

inline float_t evaluate(View<Data<formalism::datalog::GroundFunctionExpression>, formalism::datalog::Repository> element, const FactSets& fact_sets)
{
    return visit([&](auto&& arg) { return evaluate(arg, fact_sets); }, element.get_variant());
}

inline float_t
evaluate(View<Data<formalism::datalog::ArithmeticOperator<Data<formalism::datalog::GroundFunctionExpression>>>, formalism::datalog::Repository> element,
         const FactSets& fact_sets)
{
    return visit([&](auto&& arg) { return evaluate(arg, fact_sets); }, element.get_variant());
}

inline bool
evaluate(View<Data<formalism::datalog::BooleanOperator<Data<formalism::datalog::GroundFunctionExpression>>>, formalism::datalog::Repository> element,
         const FactSets& fact_sets)
{
    return visit([&](auto&& arg) { return evaluate(arg, fact_sets); }, element.get_variant());
}

/**
 * evaluate_into_buffer
 */

// Forward declarations

inline float_t evaluate_into_buffer(float_t element, const FactSets& fact_sets, formalism::datalog::ConstGrounderContext& context);

template<formalism::ArithmeticOpKind O>
inline auto
evaluate_into_buffer(View<Index<formalism::datalog::UnaryOperator<O, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> element,
                     const FactSets& fact_sets,
                     formalism::datalog::ConstGrounderContext& context);

template<formalism::OpKind O>
inline auto
evaluate_into_buffer(View<Index<formalism::datalog::BinaryOperator<O, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> element,
                     const FactSets& fact_sets,
                     formalism::datalog::ConstGrounderContext& context);

template<formalism::ArithmeticOpKind O>
inline auto
evaluate_into_buffer(View<Index<formalism::datalog::MultiOperator<O, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> element,
                     const FactSets& fact_sets,
                     formalism::datalog::ConstGrounderContext& context);

template<formalism::FactKind T>
    requires(!std::is_same_v<T, formalism::AuxiliaryTag>)
inline float_t evaluate_into_buffer(View<Index<formalism::datalog::FunctionTerm<T>>, formalism::datalog::Repository> element,
                                    const FactSets& fact_sets,
                                    formalism::datalog::ConstGrounderContext& context);

inline float_t evaluate_into_buffer(View<Index<formalism::datalog::FunctionTerm<formalism::AuxiliaryTag>>, formalism::datalog::Repository> element,
                                    const FactSets& fact_sets,
                                    formalism::datalog::ConstGrounderContext& context);

inline float_t evaluate_into_buffer(View<Data<formalism::datalog::FunctionExpression>, formalism::datalog::Repository> element,
                                    const FactSets& fact_sets,
                                    formalism::datalog::ConstGrounderContext& context);

inline float_t
evaluate_into_buffer(View<Data<formalism::datalog::ArithmeticOperator<Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> element,
                     const FactSets& fact_sets,
                     formalism::datalog::ConstGrounderContext& context);

inline bool
evaluate_into_buffer(View<Data<formalism::datalog::BooleanOperator<Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> element,
                     const FactSets& fact_sets,
                     formalism::datalog::ConstGrounderContext& context);

// Implementations

inline float_t evaluate_into_buffer(float_t element, const FactSets& fact_sets, formalism::datalog::ConstGrounderContext& context) { return element; }

template<formalism::ArithmeticOpKind O>
inline auto
evaluate_into_buffer(View<Index<formalism::datalog::UnaryOperator<O, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> element,
                     const FactSets& fact_sets,
                     formalism::datalog::ConstGrounderContext& context)
{
    return formalism::apply(O {}, evaluate_into_buffer(element.get_arg(), fact_sets, context));
}

template<formalism::OpKind O>
inline auto
evaluate_into_buffer(View<Index<formalism::datalog::BinaryOperator<O, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> element,
                     const FactSets& fact_sets,
                     formalism::datalog::ConstGrounderContext& context)
{
    return formalism::apply(O {}, evaluate_into_buffer(element.get_lhs(), fact_sets, context), evaluate_into_buffer(element.get_rhs(), fact_sets, context));
}

template<formalism::ArithmeticOpKind O>
inline auto
evaluate_into_buffer(View<Index<formalism::datalog::MultiOperator<O, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> element,
                     const FactSets& fact_sets,
                     formalism::datalog::ConstGrounderContext& context)
{
    const auto child_fexprs = element.get_args();

    return std::accumulate(std::next(child_fexprs.begin()),  // Start from the second expression
                           child_fexprs.end(),
                           evaluate_into_buffer(child_fexprs.front(), fact_sets, context),
                           [&](const auto& value, const auto& child_expr)
                           { return formalism::apply(O {}, value, evaluate_into_buffer(child_expr, fact_sets, context)); });
}

template<formalism::FactKind T>
    requires(!std::is_same_v<T, formalism::AuxiliaryTag>)
inline float_t evaluate_into_buffer(View<Index<formalism::datalog::FunctionTerm<T>>, formalism::datalog::Repository> element,
                                    const FactSets& fact_sets,
                                    formalism::datalog::ConstGrounderContext& context)
{
    auto ground_fterm_ptr = context.builder.template get_builder<formalism::datalog::GroundFunctionTerm<T>>();
    auto& ground_fterm = *ground_fterm_ptr;
    formalism::datalog::ground_into_buffer(element, context.binding, ground_fterm);
    const auto program_ground_fterm = context.destination.find(ground_fterm);

    if (!program_ground_fterm)
        return std::numeric_limits<float_t>::quiet_NaN();

    if (!fact_sets.template get<T>().function.contains(program_ground_fterm->get_index()))
        return std::numeric_limits<float_t>::quiet_NaN();

    return fact_sets.template get<T>().function[program_ground_fterm->get_index()];
}

inline float_t evaluate_into_buffer(View<Index<formalism::datalog::FunctionTerm<formalism::AuxiliaryTag>>, formalism::datalog::Repository> element,
                                    const FactSets& fact_sets,
                                    formalism::datalog::ConstGrounderContext& context)
{
    throw std::logic_error("Program does not contain auxiliary function terms.");
}

inline float_t evaluate_into_buffer(View<Data<formalism::datalog::FunctionExpression>, formalism::datalog::Repository> element,
                                    const FactSets& fact_sets,
                                    formalism::datalog::ConstGrounderContext& context)
{
    return visit([&](auto&& arg) { return evaluate_into_buffer(arg, fact_sets, context); }, element.get_variant());
}

inline float_t
evaluate_into_buffer(View<Data<formalism::datalog::ArithmeticOperator<Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> element,
                     const FactSets& fact_sets,
                     formalism::datalog::ConstGrounderContext& context)
{
    return visit([&](auto&& arg) { return evaluate_into_buffer(arg, fact_sets, context); }, element.get_variant());
}

inline bool
evaluate_into_buffer(View<Data<formalism::datalog::BooleanOperator<Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> element,
                     const FactSets& fact_sets,
                     formalism::datalog::ConstGrounderContext& context)
{
    return visit([&](auto&& arg) { return evaluate_into_buffer(arg, fact_sets, context); }, element.get_variant());
}

/**
 * is_applicable
 */

template<formalism::FactKind T>
inline bool is_applicable(View<Index<formalism::datalog::GroundLiteral<T>>, formalism::datalog::Repository> element, const FactSets& fact_sets)
{
    return fact_sets.template get<T>().predicate.contains(element.get_atom().get_index()) == element.get_polarity();
}

template<formalism::FactKind T>
inline bool is_applicable(View<IndexList<formalism::datalog::GroundLiteral<T>>, formalism::datalog::Repository> elements, const FactSets& fact_sets)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_applicable(arg, fact_sets); });
}

inline bool
is_applicable(View<DataList<formalism::datalog::BooleanOperator<Data<formalism::datalog::GroundFunctionExpression>>>, formalism::datalog::Repository> elements,
              const FactSets& fact_sets)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return evaluate(arg, fact_sets); });
}

// GroundConjunctiveCondition

inline bool is_applicable(View<Index<formalism::datalog::GroundConjunctiveCondition>, formalism::datalog::Repository> element, const FactSets& fact_sets)
{
    return is_applicable(element.template get_literals<formalism::StaticTag>(), fact_sets)     //
           && is_applicable(element.template get_literals<formalism::FluentTag>(), fact_sets)  //
           && is_applicable(element.get_numeric_constraints(), fact_sets);
}

// GroundRule

inline bool is_applicable(View<Index<formalism::datalog::GroundRule>, formalism::datalog::Repository> element, const FactSets& fact_sets)
{
    return is_applicable(element.get_body(), fact_sets);
}

/**
 * is_valid_binding
 */

template<formalism::FactKind T>
inline bool is_valid_binding(View<Index<formalism::datalog::Literal<T>>, formalism::datalog::Repository> element,
                             const FactSets& fact_sets,
                             formalism::datalog::ConstGrounderContext& context)
{
    auto ground_atom_ptr = context.builder.template get_builder<formalism::datalog::GroundAtom<T>>();
    auto& ground_atom = *ground_atom_ptr;
    formalism::datalog::ground_into_buffer(element.get_atom(), context.binding, ground_atom);
    const auto program_ground_atom = context.destination.find(ground_atom);

    return (program_ground_atom) ? (fact_sets.template get<T>().predicate.contains(program_ground_atom.value()) == element.get_polarity()) :
                                   !element.get_polarity();
}

template<formalism::FactKind T>
inline bool is_valid_binding(View<IndexList<formalism::datalog::Literal<T>>, formalism::datalog::Repository> elements,
                             const FactSets& fact_sets,
                             formalism::datalog::ConstGrounderContext& context)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_valid_binding(arg, fact_sets, context); });
}

inline bool
is_valid_binding(View<Data<formalism::datalog::BooleanOperator<Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> element,
                 const FactSets& fact_sets,
                 formalism::datalog::ConstGrounderContext& context)
{
    return visit([&](auto&& arg) { return evaluate_into_buffer(arg, fact_sets, context); }, element.get_variant());
}

inline bool
is_valid_binding(View<DataList<formalism::datalog::BooleanOperator<Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> elements,
                 const FactSets& fact_sets,
                 formalism::datalog::ConstGrounderContext& context)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_valid_binding(arg, fact_sets, context); });
}

inline bool is_valid_binding(View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> element,
                             const FactSets& fact_sets,
                             formalism::datalog::ConstGrounderContext& context)
{
    return is_valid_binding(element.template get_literals<formalism::StaticTag>(), fact_sets, context)     //
           && is_valid_binding(element.template get_literals<formalism::FluentTag>(), fact_sets, context)  //
           && is_valid_binding(element.get_numeric_constraints(), fact_sets, context);
}

}

#endif
