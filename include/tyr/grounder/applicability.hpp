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

#ifndef TYR_GROUNDER_APPLICABILITY_HPP_
#define TYR_GROUNDER_APPLICABILITY_HPP_

#include "tyr/formalism/formalism.hpp"
#include "tyr/grounder/fact_set_adapter.hpp"

namespace tyr::grounder
{
inline auto evaluate(float_t element, const FactSetsAdapter& fact_sets) { return element; }

template<formalism::ArithmeticOpKind O, formalism::Context C>
auto evaluate(View<Index<formalism::UnaryOperator<O, Data<formalism::GroundFunctionExpression>>>, C> element, const FactSetsAdapter& fact_sets)
{
    return formalism::apply(O {}, evaluate(element.get_arg(), fact_sets));
}

template<formalism::OpKind O, formalism::Context C>
auto evaluate(View<Index<formalism::BinaryOperator<O, Data<formalism::GroundFunctionExpression>>>, C> element, const FactSetsAdapter& fact_sets)
{
    return formalism::apply(O {}, evaluate(element.get_lhs(), fact_sets), evaluate(element.get_rhs(), fact_sets));
}

template<formalism::ArithmeticOpKind O, formalism::Context C>
auto evaluate(View<Index<formalism::MultiOperator<O, Data<formalism::GroundFunctionExpression>>>, C> element, const FactSetsAdapter& fact_sets)
{
    const auto child_fexprs = element.get_args();

    return std::accumulate(std::next(child_fexprs.begin()),  // Start from the second expression
                           child_fexprs.end(),
                           evaluate(child_fexprs.front(), fact_sets),
                           [&](const auto& value, const auto& child_expr)
                           { return formalism::apply(formalism::OpMul {}, value, evaluate(child_expr, fact_sets)); });
}

template<formalism::FactKind T, formalism::Context C>
    requires(!std::is_same_v<T, formalism::AuxiliaryTag>)
float_t evaluate(View<Index<formalism::GroundFunctionTerm<T>>, C> element, const FactSetsAdapter& fact_sets)
{
    if (!fact_sets.contains(element.get_index()))
        return std::numeric_limits<float_t>::quiet_NaN();

    return fact_sets[element.get_index()];
}

template<formalism::Context C>
float_t evaluate(View<Index<formalism::GroundFunctionTerm<formalism::AuxiliaryTag>>, C> element, const FactSetsAdapter& fact_sets)
{
    throw std::logic_error("Program should not contain auxiliary functions.");
}

template<formalism::Context C>
auto evaluate(View<Data<formalism::GroundFunctionExpression>, C> element, const FactSetsAdapter& fact_sets)
{
    return visit([&](auto&& arg) { return evaluate(arg, fact_sets); }, element.get_variant());
}

template<formalism::Context C>
auto evaluate(View<Data<formalism::ArithmeticOperator<Data<formalism::GroundFunctionExpression>>>, formalism::OverlayRepository<C>> element,
              const FactSetsAdapter& fact_sets)
{
    return visit([&](auto&& arg) { return evaluate(arg, fact_sets); }, element.get_variant());
}

template<formalism::Context C>
auto evaluate(View<Data<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>>, C> element, const FactSetsAdapter& fact_sets)
{
    return visit([&](auto&& arg) { return evaluate(arg, fact_sets); }, element.get_variant());
}

template<formalism::FactKind T, formalism::Context C>
bool is_applicable(View<Index<formalism::GroundLiteral<T>>, C> element, const FactSetsAdapter& fact_sets)
{
    return fact_sets.template contains<T>(element.get_atom().get_index()) == element.get_polarity();
}

template<formalism::FactKind T, formalism::Context C>
bool is_applicable(View<IndexList<formalism::GroundLiteral<T>>, C> elements, const FactSetsAdapter& fact_sets)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_applicable(arg, fact_sets); });
}

template<formalism::Context C>
bool is_applicable(View<DataList<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>>, C> elements, const FactSetsAdapter& fact_sets)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return evaluate(arg, fact_sets); });
}

template<formalism::Context C>
bool is_applicable(View<Index<formalism::GroundConjunctiveCondition>, C> element, const FactSetsAdapter& fact_sets)
{
    return is_applicable(element.template get_literals<formalism::StaticTag>(), fact_sets)     //
           && is_applicable(element.template get_literals<formalism::FluentTag>(), fact_sets)  //
           && is_applicable(element.get_numeric_constraints(), fact_sets);
}

template<formalism::Context C>
bool is_applicable(View<Index<formalism::GroundRule>, C> element, const FactSetsAdapter& fact_sets)
{
    return is_applicable(element.get_body(), fact_sets);
}

template<formalism::Context C>
bool nullary_conditions_hold(View<Index<formalism::ConjunctiveCondition>, C> condition, const FactSetsAdapter& fact_sets) noexcept
{
    return is_applicable(condition.template get_nullary_literals<formalism::StaticTag>(), fact_sets)
           && is_applicable(condition.template get_nullary_literals<formalism::FluentTag>(), fact_sets);
}
}

#endif
