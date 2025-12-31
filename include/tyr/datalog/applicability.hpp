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
#include "tyr/formalism/datalog/declarations.hpp"
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

template<formalism::ArithmeticOpKind O, formalism::datalog::Context C>
float_t evaluate(View<Index<formalism::datalog::UnaryOperator<O, Data<formalism::datalog::GroundFunctionExpression>>>, C> element, const FactSets& fact_sets);

template<formalism::OpKind O, formalism::datalog::Context C>
float_t evaluate(View<Index<formalism::datalog::BinaryOperator<O, Data<formalism::datalog::GroundFunctionExpression>>>, C> element, const FactSets& fact_sets);

template<formalism::ArithmeticOpKind O, formalism::datalog::Context C>
float_t evaluate(View<Index<formalism::datalog::MultiOperator<O, Data<formalism::datalog::GroundFunctionExpression>>>, C> element, const FactSets& fact_sets);

template<formalism::FactKind T, formalism::datalog::Context C>
    requires(!std::is_same_v<T, formalism::AuxiliaryTag>)
float_t evaluate(View<Index<formalism::datalog::GroundFunctionTerm<T>>, C> element, const FactSets& fact_sets);

template<formalism::datalog::Context C>
float_t evaluate(View<Index<formalism::datalog::GroundFunctionTerm<formalism::AuxiliaryTag>>, C> element, const FactSets& fact_sets);

template<formalism::datalog::Context C>
float_t evaluate(View<Data<formalism::datalog::GroundFunctionExpression>, C> element, const FactSets& fact_sets);

template<formalism::datalog::Context C>
float_t evaluate(View<Data<formalism::datalog::ArithmeticOperator<Data<formalism::datalog::GroundFunctionExpression>>>, C> element, const FactSets& fact_sets);

template<formalism::datalog::Context C>
bool evaluate(View<Data<formalism::datalog::BooleanOperator<Data<formalism::datalog::GroundFunctionExpression>>>, C> element, const FactSets& fact_sets);

// Implementations

inline float_t evaluate(float_t element, const FactSets& fact_sets) { return element; }

template<formalism::ArithmeticOpKind O, formalism::datalog::Context C>
float_t evaluate(View<Index<formalism::datalog::UnaryOperator<O, Data<formalism::datalog::GroundFunctionExpression>>>, C> element, const FactSets& fact_sets)
{
    return formalism::apply(O {}, evaluate(element.get_arg(), fact_sets));
}

template<formalism::OpKind O, formalism::datalog::Context C>
float_t evaluate(View<Index<formalism::datalog::BinaryOperator<O, Data<formalism::datalog::GroundFunctionExpression>>>, C> element, const FactSets& fact_sets)
{
    return formalism::apply(O {}, evaluate(element.get_lhs(), fact_sets), evaluate(element.get_rhs(), fact_sets));
}

template<formalism::ArithmeticOpKind O, formalism::datalog::Context C>
float_t evaluate(View<Index<formalism::datalog::MultiOperator<O, Data<formalism::datalog::GroundFunctionExpression>>>, C> element, const FactSets& fact_sets)
{
    const auto child_fexprs = element.get_args();

    return std::accumulate(std::next(child_fexprs.begin()),  // Start from the second expression
                           child_fexprs.end(),
                           evaluate(child_fexprs.front(), fact_sets),
                           [&](const auto& value, const auto& child_expr)
                           { return formalism::apply(formalism::OpMul {}, value, evaluate(child_expr, fact_sets)); });
}

template<formalism::FactKind T, formalism::datalog::Context C>
    requires(!std::is_same_v<T, formalism::AuxiliaryTag>)
float_t evaluate(View<Index<formalism::datalog::GroundFunctionTerm<T>>, C> element, const FactSets& fact_sets)
{
    if (!fact_sets.template get<T>().function.contains(element.get_index()))
        return std::numeric_limits<float_t>::quiet_NaN();
    return fact_sets.template get<T>().function[element.get_index()];
}

template<formalism::datalog::Context C>
float_t evaluate(View<Index<formalism::datalog::GroundFunctionTerm<formalism::AuxiliaryTag>>, C> element, const FactSets& fact_sets)
{
    throw std::logic_error("Program does not contain auxiliary function terms.");
}

template<formalism::datalog::Context C>
float_t evaluate(View<Data<formalism::datalog::GroundFunctionExpression>, C> element, const FactSets& fact_sets)
{
    return visit([&](auto&& arg) { return evaluate(arg, fact_sets); }, element.get_variant());
}

template<formalism::datalog::Context C>
float_t evaluate(View<Data<formalism::datalog::ArithmeticOperator<Data<formalism::datalog::GroundFunctionExpression>>>, C> element, const FactSets& fact_sets)
{
    return visit([&](auto&& arg) { return evaluate(arg, fact_sets); }, element.get_variant());
}

template<formalism::datalog::Context C>
bool evaluate(View<Data<formalism::datalog::BooleanOperator<Data<formalism::datalog::GroundFunctionExpression>>>, C> element, const FactSets& fact_sets)
{
    return visit([&](auto&& arg) { return evaluate(arg, fact_sets); }, element.get_variant());
}

/**
 * is_applicable
 */

template<formalism::FactKind T, formalism::datalog::Context C>
bool is_applicable(View<Index<formalism::datalog::GroundLiteral<T>>, C> element, const FactSets& fact_sets)
{
    return fact_sets.template get<T>().predicate.contains(element.get_atom().get_index()) == element.get_polarity();
}

template<formalism::FactKind T, formalism::datalog::Context C>
bool is_applicable(View<IndexList<formalism::datalog::GroundLiteral<T>>, C> elements, const FactSets& fact_sets)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_applicable(arg, fact_sets); });
}

template<formalism::datalog::Context C>
bool is_applicable(View<DataList<formalism::datalog::BooleanOperator<Data<formalism::datalog::GroundFunctionExpression>>>, C> elements,
                   const FactSets& fact_sets)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return evaluate(arg, fact_sets); });
}

// GroundConjunctiveCondition

template<formalism::datalog::Context C>
bool is_applicable(View<Index<formalism::datalog::GroundConjunctiveCondition>, C> element, const FactSets& fact_sets)
{
    return is_applicable(element.template get_literals<formalism::StaticTag>(), fact_sets)     //
           && is_applicable(element.template get_literals<formalism::FluentTag>(), fact_sets)  //
           && is_applicable(element.get_numeric_constraints(), fact_sets);
}

// GroundRule

template<formalism::datalog::Context C>
bool is_applicable(View<Index<formalism::datalog::GroundRule>, C> element, const FactSets& fact_sets)
{
    return is_applicable(element.get_body(), fact_sets);
}

}

#endif
