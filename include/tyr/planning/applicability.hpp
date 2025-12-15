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

#ifndef TYR_PLANNING_APPLICABILITY_HPP_
#define TYR_PLANNING_APPLICABILITY_HPP_

#include "tyr/common/vector.hpp"
#include "tyr/formalism/arithmetic_operator_utils.hpp"
#include "tyr/formalism/boolean_operator_utils.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/planning/ground_numeric_effect_operator_utils.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/planning/node.hpp"

#include <algorithm>
#include <concepts>
#include <iterator>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace tyr::planning
{

template<typename Task>
struct StateContext
{
    Task& task;
    const UnpackedState<Task>& unpacked_state;
    float_t auxiliary_value;
};

/**
 * evaluate
 */

// Forward declarations

template<typename Task>
inline float_t evaluate(float_t element, const StateContext<Task>& context);

template<typename Task, formalism::ArithmeticOpKind O, formalism::Context C>
float_t evaluate(View<Index<formalism::UnaryOperator<O, Data<formalism::GroundFunctionExpression>>>, C> element, const StateContext<Task>& context);

template<typename Task, formalism::OpKind O, formalism::Context C>
float_t evaluate(View<Index<formalism::BinaryOperator<O, Data<formalism::GroundFunctionExpression>>>, C> element, const StateContext<Task>& context);

template<typename Task, formalism::ArithmeticOpKind O, formalism::Context C>
float_t evaluate(View<Index<formalism::MultiOperator<O, Data<formalism::GroundFunctionExpression>>>, C> element, const StateContext<Task>& context);

template<typename Task, formalism::FactKind T, formalism::Context C>
float_t evaluate(View<Index<formalism::GroundFunctionTerm<T>>, C> element, const StateContext<Task>& context);

template<typename Task, formalism::Context C>
float_t evaluate(View<Data<formalism::GroundFunctionExpression>, C> element, const StateContext<Task>& context);

template<typename Task, formalism::Context C>
float_t evaluate(View<Data<formalism::ArithmeticOperator<Data<formalism::GroundFunctionExpression>>>, C> element, const StateContext<Task>& context);

template<typename Task, formalism::Context C>
bool evaluate(View<Data<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>>, C> element, const StateContext<Task>& context);

template<typename Task, formalism::NumericEffectOpKind Op, formalism::FactKind T, formalism::Context C>
float_t evaluate(View<Index<formalism::GroundNumericEffect<Op, T>>, C> element, const StateContext<Task>& context);

template<typename Task, formalism::FactKind T, formalism::Context C>
float_t evaluate(View<Data<formalism::GroundNumericEffectOperator<T>>, C> element, const StateContext<Task>& context);

// Implementations

template<typename Task>
inline float_t evaluate(float_t element, const StateContext<Task>& context)
{
    return element;
}

template<typename Task, formalism::ArithmeticOpKind O, formalism::Context C>
float_t evaluate(View<Index<formalism::UnaryOperator<O, Data<formalism::GroundFunctionExpression>>>, C> element, const StateContext<Task>& context)
{
    return formalism::apply(O {}, evaluate(element.get_arg(), context));
}

template<typename Task, formalism::OpKind O, formalism::Context C>
float_t evaluate(View<Index<formalism::BinaryOperator<O, Data<formalism::GroundFunctionExpression>>>, C> element, const StateContext<Task>& context)
{
    return formalism::apply(O {}, evaluate(element.get_lhs(), context), evaluate(element.get_rhs(), context));
}

template<typename Task, formalism::ArithmeticOpKind O, formalism::Context C>
float_t evaluate(View<Index<formalism::MultiOperator<O, Data<formalism::GroundFunctionExpression>>>, C> element, const StateContext<Task>& context)
{
    const auto child_fexprs = element.get_args();

    return std::accumulate(std::next(child_fexprs.begin()),  // Start from the second expression
                           child_fexprs.end(),
                           evaluate(child_fexprs.front(), context),
                           [&](const auto& value, const auto& child_expr)
                           { return formalism::apply(formalism::OpMul {}, value, evaluate(child_expr, context)); });
}

template<typename Task, formalism::Context C>
float_t evaluate(View<Index<formalism::GroundFunctionTerm<formalism::StaticTag>>, C> element, const StateContext<Task>& context)
{
    return context.task.get(element.get_index());
}

template<typename Task, formalism::Context C>
float_t evaluate(View<Index<formalism::GroundFunctionTerm<formalism::FluentTag>>, C> element, const StateContext<Task>& context)
{
    return context.unpacked_state.get(element.get_index());
}

template<typename Task, formalism::Context C>
float_t evaluate(View<Index<formalism::GroundFunctionTerm<formalism::AuxiliaryTag>>, C> element, const StateContext<Task>& context)
{
    return context.auxiliary_value;
}

template<typename Task, formalism::Context C>
float_t evaluate(View<Data<formalism::GroundFunctionExpression>, C> element, const StateContext<Task>& context)
{
    return visit([&](auto&& arg) { return evaluate(arg, context); }, element.get_variant());
}

template<typename Task, formalism::Context C>
float_t evaluate(View<Data<formalism::ArithmeticOperator<Data<formalism::GroundFunctionExpression>>>, C> element, const StateContext<Task>& context)
{
    return visit([&](auto&& arg) { return evaluate(arg, context); }, element.get_variant());
}

template<typename Task, formalism::Context C>
bool evaluate(View<Data<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>>, C> element, const StateContext<Task>& context)
{
    return visit([&](auto&& arg) { return evaluate(arg, context); }, element.get_variant());
}

template<typename Task, formalism::NumericEffectOpKind Op, formalism::FactKind T, formalism::Context C>
float_t evaluate(View<Index<formalism::GroundNumericEffect<Op, T>>, C> element, const StateContext<Task>& context)
{
    return formalism::apply(Op {}, evaluate(element.get_fterm(), context), evaluate(element.get_fexpr(), context));
}

template<typename Task, formalism::FactKind T, formalism::Context C>
float_t evaluate(View<Data<formalism::GroundNumericEffectOperator<T>>, C> element, const StateContext<Task>& context)
{
    return visit([&](auto&& arg) { return evaluate(arg, context); }, element.get_variant());
}

/**
 * is_applicable_if_fires
 */

template<typename Task, formalism::Context C>
bool is_applicable_if_fires(View<Index<formalism::GroundConditionalEffect>, C> element,
                            const StateContext<Task>& context,
                            formalism::EffectFamilyList& ref_fluent_effect_families)
{
    return is_applicable(element.get_effect(), context, ref_fluent_effect_families) || !is_applicable(element.get_condition(), context);
}

/**
 * is_applicable
 */

template<typename Task, formalism::Context C>
bool is_applicable(View<Index<formalism::GroundLiteral<formalism::StaticTag>>, C> element, const StateContext<Task>& context)
{
    return context.task.test(element.get_atom().get_index()) == element.get_polarity();
}

template<typename Task, formalism::Context C>
bool is_applicable(View<Index<formalism::GroundLiteral<formalism::DerivedTag>>, C> element, const StateContext<Task>& context)
{
    return context.unpacked_state.test(element.get_atom().get_index()) == element.get_polarity();
}

template<typename Task, formalism::FactKind T, formalism::Context C>
bool is_applicable(View<IndexList<formalism::GroundLiteral<T>>, C> elements, const StateContext<Task>& context)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_applicable(arg, context); });
}

template<typename Task, formalism::Context C>
bool is_applicable(View<Data<formalism::FDRFact<formalism::FluentTag>>, C> element, const StateContext<Task>& context)
{
    return context.unpacked_state.get(element.get_variable().get_index()) == element.get_value();
}

template<typename Task, formalism::Context C>
bool is_applicable(View<DataList<formalism::FDRFact<formalism::FluentTag>>, C> elements, const StateContext<Task>& context)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_applicable(arg, context); });
}

template<typename Task, formalism::Context C>
bool is_applicable(View<DataList<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>>, C> elements, const StateContext<Task>& context)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return evaluate(arg, context); });
}

template<typename Task, formalism::NumericEffectOpKind Op, formalism::Context C>
bool is_applicable(View<Index<formalism::GroundNumericEffect<Op, formalism::FluentTag>>, C> element,
                   const StateContext<Task>& context,
                   formalism::EffectFamilyList& ref_fluent_effect_families)
{
    const auto fterm_index = element.get_fterm().get_index();
    ref_fluent_effect_families.resize(fterm_index.get_value() + 1, formalism::EffectFamily::NONE);

    // Check non-conflicting effects
    if (!is_compatible_effect_family(Op::family, ref_fluent_effect_families[fterm_index.get_value()]))
        return false;  /// incompatible effects

    ref_fluent_effect_families[fterm_index.get_value()] = Op::family;

    // Check fterm is well-defined in context
    if constexpr (!std::is_same_v<Op, formalism::OpAssign>)
    {
        if (std::isnan(context.unpacked_state.get(fterm_index)))
            return false;  /// target function is undefined and operator is not assign
    }

    // Check fexpr is well-defined in context
    return !std::isnan(evaluate(element.get_fexpr(), context));
}

template<typename Task, formalism::Context C>
bool is_applicable(View<Data<formalism::GroundNumericEffectOperator<formalism::FluentTag>>, C> element,
                   const StateContext<Task>& context,
                   formalism::EffectFamilyList& ref_fluent_effect_families)
{
    return visit([&](auto&& arg) { return is_applicable(arg, context, ref_fluent_effect_families); }, element.get_variant());
}

template<typename Task, formalism::Context C>
bool is_applicable(View<DataList<formalism::GroundNumericEffectOperator<formalism::FluentTag>>, C> elements,
                   const StateContext<Task>& context,
                   formalism::EffectFamilyList& ref_fluent_effect_families)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_applicable(arg, context, ref_fluent_effect_families); });
}

template<typename Task, formalism::Context C>
bool is_applicable(View<Index<formalism::GroundNumericEffect<formalism::OpIncrease, formalism::AuxiliaryTag>>, C> element, const StateContext<Task>& context)
{
    // Check fexpr is well-defined in context
    return !std::isnan(evaluate(element.get_fexpr(), context));
}

template<typename Task, formalism::Context C>
bool is_applicable(View<Data<formalism::GroundNumericEffectOperator<formalism::AuxiliaryTag>>, C> element, const StateContext<Task>& context)
{
    return visit([&](auto&& arg) { return is_applicable(arg, context); }, element.get_variant());
}

// GroundFDRConjunctiveCondition

template<typename Task, formalism::Context C>
bool is_applicable(View<Index<formalism::GroundFDRConjunctiveCondition>, C> element, const StateContext<Task>& context)
{
    return is_applicable(element.template get_facts<formalism::StaticTag>(), context)      //
           && is_applicable(element.template get_facts<formalism::FluentTag>(), context)   //
           && is_applicable(element.template get_facts<formalism::DerivedTag>(), context)  //
           && is_applicable(element.get_numeric_constraints(), context);
}

// GroundConjunctiveEffect

template<typename Task, formalism::Context C>
bool is_applicable(View<Index<formalism::GroundConjunctiveEffect>, C> element,
                   const StateContext<Task>& context,
                   formalism::EffectFamilyList& ref_fluent_effect_families)
{
    return is_applicable(element.get_numeric_effects(), context, ref_fluent_effect_families)
           && (!element.get_auxiliary_numeric_effect().has_value() || is_applicable(element.get_auxiliary_numeric_effect().value(), context));
}

// GroundConditionalEffect

template<typename Task, formalism::Context C>
bool is_applicable(View<Index<formalism::GroundConditionalEffect>, C> element,
                   const StateContext<Task>& context,
                   formalism::EffectFamilyList& ref_fluent_effect_families)
{
    return is_applicable(element.get_condition(), context, ref_fluent_effect_families)  //
           && is_applicable(element.get_effect(), context, ref_fluent_effect_families);
}

// GroundAction

template<typename Task, formalism::Context C>
bool is_applicable(View<Index<formalism::GroundAction>, C> element, const StateContext<Task>& context, formalism::EffectFamilyList& out_fluent_effect_families)
{
    return is_applicable(element.get_condition(), context)
           && std::all_of(element.get_effects().begin(),
                          element.get_effects().end(),
                          [&](auto&& cond_effect) { return is_applicable_if_fires(cond_effect, context, out_fluent_effect_families); });
}

// GroundAxiom

template<typename Task, formalism::Context C>
bool is_applicable(View<Index<formalism::GroundAxiom>, C> element, const StateContext<Task>& context)
{
    return is_applicable(element.get_body(), context);
}

/**
 * is_statically_applicable
 */

// GroundConjunctiveCondition

template<typename Task, formalism::Context C>
bool is_statically_applicable(View<Index<formalism::GroundConjunctiveCondition>, C> element, const StateContext<Task>& context)
{
    return is_applicable(element.template get_literals<formalism::StaticTag>(), context);
}

template<typename Task, formalism::Context C>
bool is_statically_applicable(View<Index<formalism::GroundFDRConjunctiveCondition>, C> element, const StateContext<Task>& context)
{
    return is_applicable(element.template get_facts<formalism::StaticTag>(), context);
}

// GroundRule

template<typename Task, formalism::Context C>
bool is_statically_applicable(View<Index<formalism::GroundRule>, C> element, const StateContext<Task>& context)
{
    return is_statically_applicable(element.get_body(), context);
}

// GroundAction

template<typename Task, formalism::Context C>
bool is_statically_applicable(View<Index<formalism::GroundAction>, C> element, const StateContext<Task>& context)
{
    return is_statically_applicable(element.get_condition(), context);
}

// GroundAxiom

template<typename Task, formalism::Context C>
bool is_statically_applicable(View<Index<formalism::GroundAxiom>, C> element, const StateContext<Task>& context)
{
    return is_statically_applicable(element.get_body(), context);
}

}

#endif
