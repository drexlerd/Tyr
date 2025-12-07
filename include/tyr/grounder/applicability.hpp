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

#include "tyr/formalism/arithmetic_operator_utils.hpp"
#include "tyr/formalism/boolean_operator_utils.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/grounder/facts_view.hpp"

#include <algorithm>
#include <concepts>
#include <iterator>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace tyr::grounder
{

enum class EffectFamily
{
    ASSIGN = 0,
    INCREASE_DECREASE = 1,
    SCALE_UP_SCALE_DOWN = 2,
};

/**
 * evaluate
 */

// Forward declarations

inline auto evaluate(float_t element, const FactsView& facts_view);

template<formalism::ArithmeticOpKind O, formalism::Context C>
auto evaluate(View<Index<formalism::UnaryOperator<O, Data<formalism::GroundFunctionExpression>>>, C> element, const FactsView& facts_view);

template<formalism::OpKind O, formalism::Context C>
auto evaluate(View<Index<formalism::BinaryOperator<O, Data<formalism::GroundFunctionExpression>>>, C> element, const FactsView& facts_view);

template<formalism::ArithmeticOpKind O, formalism::Context C>
auto evaluate(View<Index<formalism::MultiOperator<O, Data<formalism::GroundFunctionExpression>>>, C> element, const FactsView& facts_view);

template<formalism::FactKind T, formalism::Context C>
    requires(!std::is_same_v<T, formalism::AuxiliaryTag>)
float_t evaluate(View<Index<formalism::GroundFunctionTerm<T>>, C> element, const FactsView& facts_view);

template<formalism::Context C>
float_t evaluate(View<Index<formalism::GroundFunctionTerm<formalism::AuxiliaryTag>>, C> element, const FactsView& facts_view);

template<formalism::Context C>
auto evaluate(View<Data<formalism::GroundFunctionExpression>, C> element, const FactsView& facts_view);

template<formalism::Context C>
auto evaluate(View<Data<formalism::ArithmeticOperator<Data<formalism::GroundFunctionExpression>>>, C> element, const FactsView& facts_view);

template<formalism::Context C>
auto evaluate(View<Data<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>>, C> element, const FactsView& facts_view);

// Implementations

inline auto evaluate(float_t element, const FactsView& facts_view) { return element; }

template<formalism::ArithmeticOpKind O, formalism::Context C>
auto evaluate(View<Index<formalism::UnaryOperator<O, Data<formalism::GroundFunctionExpression>>>, C> element, const FactsView& facts_view)
{
    return formalism::apply(O {}, evaluate(element.get_arg(), facts_view));
}

template<formalism::OpKind O, formalism::Context C>
auto evaluate(View<Index<formalism::BinaryOperator<O, Data<formalism::GroundFunctionExpression>>>, C> element, const FactsView& facts_view)
{
    return formalism::apply(O {}, evaluate(element.get_lhs(), facts_view), evaluate(element.get_rhs(), facts_view));
}

template<formalism::ArithmeticOpKind O, formalism::Context C>
auto evaluate(View<Index<formalism::MultiOperator<O, Data<formalism::GroundFunctionExpression>>>, C> element, const FactsView& facts_view)
{
    const auto child_fexprs = element.get_args();

    return std::accumulate(std::next(child_fexprs.begin()),  // Start from the second expression
                           child_fexprs.end(),
                           evaluate(child_fexprs.front(), facts_view),
                           [&](const auto& value, const auto& child_expr)
                           { return formalism::apply(formalism::OpMul {}, value, evaluate(child_expr, facts_view)); });
}

template<formalism::FactKind T, formalism::Context C>
    requires(!std::is_same_v<T, formalism::AuxiliaryTag>)
float_t evaluate(View<Index<formalism::GroundFunctionTerm<T>>, C> element, const FactsView& facts_view)
{
    if (!facts_view.contains(element.get_index()))
        return std::numeric_limits<float_t>::quiet_NaN();

    return facts_view[element.get_index()];
}

template<formalism::Context C>
float_t evaluate(View<Index<formalism::GroundFunctionTerm<formalism::AuxiliaryTag>>, C> element, const FactsView& facts_view)
{
    throw std::logic_error("Program should not contain auxiliary functions.");
}

template<formalism::Context C>
auto evaluate(View<Data<formalism::GroundFunctionExpression>, C> element, const FactsView& facts_view)
{
    return visit([&](auto&& arg) { return evaluate(arg, facts_view); }, element.get_variant());
}

template<formalism::Context C>
auto evaluate(View<Data<formalism::ArithmeticOperator<Data<formalism::GroundFunctionExpression>>>, C> element, const FactsView& facts_view)
{
    return visit([&](auto&& arg) { return evaluate(arg, facts_view); }, element.get_variant());
}

template<formalism::Context C>
auto evaluate(View<Data<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>>, C> element, const FactsView& facts_view)
{
    return visit([&](auto&& arg) { return evaluate(arg, facts_view); }, element.get_variant());
}

/**
 * is_applicable_if_fires
 */

template<formalism::Context C>
bool is_applicable_if_fires(View<Index<formalism::GroundConditionalEffect>, C> element,
                            const FactsView& facts_view,
                            std::vector<EffectFamily>& ref_fluent_effect_families,
                            EffectFamily& ref_auxiliary_effect_family)
{
    return !is_applicable(element.get_effect(), facts_view, ref_fluent_effect_families, ref_auxiliary_effect_family)
           || is_applicable(element.get_condition(), facts_view);
}

/**
 * is_applicable
 */

template<formalism::FactKind T, formalism::Context C>
bool is_applicable(View<Index<formalism::GroundLiteral<T>>, C> element, const FactsView& facts_view)
{
    return facts_view.template contains<T>(element.get_atom().get_index()) == element.get_polarity();
}

template<formalism::FactKind T, formalism::Context C>
bool is_applicable(View<IndexList<formalism::GroundLiteral<T>>, C> elements, const FactsView& facts_view)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_applicable(arg, facts_view); });
}

template<formalism::Context C>
bool is_applicable(View<DataList<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>>, C> elements, const FactsView& facts_view)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return evaluate(arg, facts_view); });
}

// GroundConjunctiveCondition

template<formalism::Context C>
bool is_applicable(View<Index<formalism::GroundConjunctiveCondition>, C> element, const FactsView& facts_view)
{
    return is_applicable(element.template get_literals<formalism::StaticTag>(), facts_view)      //
           && is_applicable(element.template get_literals<formalism::FluentTag>(), facts_view)   //
           && is_applicable(element.template get_literals<formalism::DerivedTag>(), facts_view)  //
           && is_applicable(element.get_numeric_constraints(), facts_view);
}

// GroundRule

template<formalism::Context C>
bool is_applicable(View<Index<formalism::GroundRule>, C> element, const FactsView& facts_view)
{
    return is_applicable(element.get_body(), facts_view);
}

// GroundConjunctiveEffect

template<formalism::Context C>
bool is_applicable(View<Index<formalism::GroundConjunctiveEffect>, C> element,
                   const FactsView& facts_view,
                   std::vector<EffectFamily>& ref_fluent_effect_families,
                   EffectFamily& ref_auxiliary_effect_family)
{
    return is_applicable(element.template get_literals<formalism::StaticTag>(), facts_view)      //
           && is_applicable(element.template get_literals<formalism::FluentTag>(), facts_view)   //
           && is_applicable(element.template get_literals<formalism::DerivedTag>(), facts_view)  //
           && is_applicable(element.get_numeric_effects(), facts_view, ref_fluent_effect_families, ref_auxiliary_effect_family);
}

// GroundConditionalEffect

template<formalism::Context C>
bool is_applicable(View<Index<formalism::GroundConditionalEffect>, C> element,
                   const FactsView& facts_view,
                   std::vector<EffectFamily>& ref_fluent_effect_families,
                   EffectFamily& ref_auxiliary_effect_family)
{
    return is_applicable(element.get_condition(), facts_view, ref_fluent_effect_families, ref_auxiliary_effect_family)  //
           && is_applicable(element.get_effect(), facts_view, ref_fluent_effect_families, ref_auxiliary_effect_family);
}

// GroundAction

template<formalism::Context C>
bool is_applicable(View<Index<formalism::GroundAction>, C> element,
                   const FactsView& facts_view,
                   std::vector<EffectFamily>& out_fluent_effect_families,
                   EffectFamily& out_auxiliary_effect_family)
{
    return is_applicable(element.get_condition(), facts_view)
           && std::all_of(element.get_effects().begin(),
                          element.get_effects().end(),
                          [&](auto&& cond_effect)
                          { return is_applicable_if_fires(cond_effect, facts_view, out_fluent_effect_families, out_auxiliary_effect_family); });
}

// GroundAxiom

template<formalism::Context C>
bool is_applicable(View<Index<formalism::GroundAxiom>, C> element, const FactsView& facts_view)
{
    return is_applicable(element.get_body(), facts_view);
}

/**
 * nullary_conditions_hold
 */

template<formalism::Context C>
bool nullary_conditions_hold(View<Index<formalism::ConjunctiveCondition>, C> condition, const FactsView& facts_view) noexcept
{
    return is_applicable(condition.template get_nullary_literals<formalism::StaticTag>(), facts_view)
           && is_applicable(condition.template get_nullary_literals<formalism::FluentTag>(), facts_view)
           && is_applicable(condition.template get_nullary_literals<formalism::DerivedTag>(), facts_view)
           && is_applicable(condition.get_nullary_numeric_constraints(), facts_view);
}
}

#endif
