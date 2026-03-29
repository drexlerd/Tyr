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

#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/macros.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/arithmetic_operator_utils.hpp"
#include "tyr/formalism/boolean_operator_utils.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/ground_numeric_effect_operator_utils.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"
#include "tyr/planning/applicability_decl.hpp"
#include "tyr/planning/node.hpp"

#include <algorithm>
#include <boost/dynamic_bitset.hpp>
#include <concepts>
#include <iterator>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace tyr::planning
{

/**
 * evaluate
 */

template<TaskKind Kind>
float_t evaluate(float_t element, const StateContext<Kind>& context)
{
    return element;
}

template<TaskKind Kind, formalism::ArithmeticOpKind O>
float_t evaluate(formalism::planning::GroundUnaryOperatorView<O> element, const StateContext<Kind>& context)
{
    return formalism::apply(O {}, evaluate(element.get_arg(), context));
}

template<TaskKind Kind, formalism::ArithmeticOpKind O>
float_t evaluate(formalism::planning::GroundBinaryOperatorView<O> element, const StateContext<Kind>& context)
{
    return formalism::apply(O {}, evaluate(element.get_lhs(), context), evaluate(element.get_rhs(), context));
}

template<TaskKind Kind, formalism::BooleanOpKind O>
bool evaluate(formalism::planning::GroundBinaryOperatorView<O> element, const StateContext<Kind>& context)
{
    return formalism::apply(O {}, evaluate(element.get_lhs(), context), evaluate(element.get_rhs(), context));
}

template<TaskKind Kind, formalism::ArithmeticOpKind O>
float_t evaluate(formalism::planning::GroundMultiOperatorView<O> element, const StateContext<Kind>& context)
{
    const auto child_fexprs = element.get_args();

    return std::accumulate(std::next(child_fexprs.begin()),  // Start from the second expression
                           child_fexprs.end(),
                           evaluate(child_fexprs.front(), context),
                           [&](const auto& value, const auto& child_expr)
                           { return formalism::apply(formalism::OpMul {}, value, evaluate(child_expr, context)); });
}

template<TaskKind Kind>
float_t evaluate(formalism::planning::GroundFunctionTermView<formalism::StaticTag> element, const StateContext<Kind>& context)
{
    return context.task.get(element.get_index());
}

template<TaskKind Kind>
float_t evaluate(formalism::planning::GroundFunctionTermView<formalism::FluentTag> element, const StateContext<Kind>& context)
{
    return context.unpacked_state.get(element.get_index());
}

template<TaskKind Kind>
float_t evaluate(formalism::planning::GroundFunctionTermView<formalism::AuxiliaryTag> element, const StateContext<Kind>& context)
{
    return context.auxiliary_value;
}

template<TaskKind Kind>
float_t evaluate(formalism::planning::GroundFunctionExpressionView element, const StateContext<Kind>& context)
{
    return visit([&](auto&& arg) { return evaluate(arg, context); }, element.get_variant());
}

template<TaskKind Kind>
float_t evaluate(formalism::planning::GroundArithmeticOperatorView element, const StateContext<Kind>& context)
{
    return visit([&](auto&& arg) { return evaluate(arg, context); }, element.get_variant());
}

template<TaskKind Kind>
bool evaluate(formalism::planning::GroundBooleanOperatorView element, const StateContext<Kind>& context)
{
    return visit([&](auto&& arg) { return evaluate(arg, context); }, element.get_variant());
}

template<TaskKind Kind, formalism::planning::NumericEffectOpKind Op, formalism::FactKind T>
float_t evaluate(formalism::planning::GroundNumericEffectView<Op, T> element, const StateContext<Kind>& context)
{
    return formalism::planning::apply(Op {}, evaluate(element.get_fterm(), context), evaluate(element.get_fexpr(), context));
}

template<TaskKind Kind, formalism::FactKind T>
float_t evaluate(formalism::planning::GroundNumericEffectOperatorView<T> element, const StateContext<Kind>& context)
{
    return visit([&](auto&& arg) { return evaluate(arg, context); }, element.get_variant());
}

/**
 * is_applicable_if_fires
 */

template<TaskKind Kind>
bool is_applicable_if_fires(formalism::planning::GroundConditionalEffectView element,
                            const StateContext<Kind>& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families)
{
    if (!is_applicable(element.get_condition(), context))
        return true;

    // Important: only modify effect families if condition is satisfied
    return is_applicable(element.get_effect(), context, ref_fluent_effect_families);
}

/**
 * is_applicable
 */

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundLiteralView<formalism::StaticTag> element, const StateContext<Kind>& context)
{
    return context.task.test(element.get_atom().get_index()) == element.get_polarity();
}

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundLiteralView<formalism::DerivedTag> element, const StateContext<Kind>& context)
{
    return context.unpacked_state.test(element.get_atom().get_index()) == element.get_polarity();
}

template<TaskKind Kind, formalism::FactKind T>
bool is_applicable(formalism::planning::GroundLiteralListView<T> elements, const StateContext<Kind>& context)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_applicable(arg, context); });
}

template<TaskKind Kind>
bool is_applicable(formalism::planning::FDRFactView<formalism::FluentTag> element, const StateContext<Kind>& context)
{
    return context.unpacked_state.get(element.get_variable().get_index()) == element.get_value();
}

template<TaskKind Kind>
bool is_applicable(formalism::planning::FDRFactListView<formalism::FluentTag> elements, const StateContext<Kind>& context)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_applicable(arg, context); });
}

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundBooleanOperatorView element, const StateContext<Kind>& context)
{
    return evaluate(element, context);
}

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundBooleanOperatorListView elements, const StateContext<Kind>& context)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_applicable(arg, context); });
}

template<TaskKind Kind, formalism::planning::NumericEffectOpKind Op>
bool is_applicable(formalism::planning::GroundNumericEffectView<Op, formalism::FluentTag> element,
                   const StateContext<Kind>& context,
                   formalism::planning::EffectFamilyList& ref_fluent_effect_families)
{
    const auto fterm_index = element.get_fterm().get_index();
    ref_fluent_effect_families.resize(fterm_index.get_value() + 1, formalism::planning::EffectFamily::NONE);

    // Check non-conflicting effects
    if (!is_compatible_effect_family(Op::family, ref_fluent_effect_families[fterm_index.get_value()]))
        return false;  /// incompatible effects

    ref_fluent_effect_families[fterm_index.get_value()] = Op::family;

    // Check fterm is well-defined in context
    if constexpr (!std::is_same_v<Op, formalism::planning::OpAssign>)
    {
        if (std::isnan(context.unpacked_state.get(fterm_index)))
            return false;  /// target function is undefined and operator is not assign
    }

    // Check fexpr is well-defined in context
    return !std::isnan(evaluate(element.get_fexpr(), context));
}

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundNumericEffectOperatorView<formalism::FluentTag> element,
                   const StateContext<Kind>& context,
                   formalism::planning::EffectFamilyList& ref_fluent_effect_families)
{
    return visit([&](auto&& arg) { return is_applicable(arg, context, ref_fluent_effect_families); }, element.get_variant());
}

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundNumericEffectOperatorListView<formalism::FluentTag> elements,
                   const StateContext<Kind>& context,
                   formalism::planning::EffectFamilyList& ref_fluent_effect_families)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_applicable(arg, context, ref_fluent_effect_families); });
}

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundNumericEffectView<formalism::planning::OpIncrease, formalism::AuxiliaryTag> element,
                   const StateContext<Kind>& context)
{
    // Check fexpr is well-defined in context
    return !std::isnan(evaluate(element.get_fexpr(), context));
}

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundNumericEffectOperatorView<formalism::AuxiliaryTag> element, const StateContext<Kind>& context)
{
    return visit([&](auto&& arg) { return is_applicable(arg, context); }, element.get_variant());
}

// GroundConjunctiveCondition

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundConjunctiveConditionView element, const StateContext<Kind>& context)
{
    return is_applicable(element.template get_facts<formalism::StaticTag>(), context)      //
           && is_applicable(element.template get_facts<formalism::FluentTag>(), context)   //
           && is_applicable(element.template get_facts<formalism::DerivedTag>(), context)  //
           && is_applicable(element.get_numeric_constraints(), context);
}

// GroundConjunctiveEffect

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundConjunctiveEffectView element,
                   const StateContext<Kind>& context,
                   formalism::planning::EffectFamilyList& ref_fluent_effect_families)
{
    return is_applicable(element.get_numeric_effects(), context, ref_fluent_effect_families)
           && (!element.get_auxiliary_numeric_effect().has_value() || is_applicable(element.get_auxiliary_numeric_effect().value(), context));
}

// GroundConditionalEffectList

template<TaskKind Kind>
bool are_applicable_if_fires(formalism::planning::GroundConditionalEffectListView elements,
                             const StateContext<Kind>& context,
                             formalism::planning::EffectFamilyList& out_fluent_effect_families)
{
    out_fluent_effect_families.clear();

    return std::all_of(elements.begin(),
                       elements.end(),
                       [&](auto&& cond_effect) { return is_applicable_if_fires(cond_effect, context, out_fluent_effect_families); });
}

// GroundAction

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundActionView element,
                   const StateContext<Kind>& context,
                   formalism::planning::EffectFamilyList& out_fluent_effect_families)
{
    return is_applicable(element.get_condition(), context) && are_applicable_if_fires(element.get_effects(), context, out_fluent_effect_families);
}

// GroundAxiom

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundAxiomView element, const StateContext<Kind>& context)
{
    return is_applicable(element.get_body(), context);
}

/**
 * is_statically_applicable
 */

TYR_INLINE_IMPL bool is_statically_applicable(formalism::planning::GroundLiteralView<formalism::StaticTag> element, const boost::dynamic_bitset<>& static_atoms)
{
    return tyr::test(uint_t(element.get_atom().get_index()), static_atoms) == element.get_polarity();
}

TYR_INLINE_IMPL bool is_statically_applicable(formalism::planning::GroundLiteralListView<formalism::StaticTag> elements,
                                              const boost::dynamic_bitset<>& static_atoms)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_statically_applicable(arg, static_atoms); });
}

// GroundConjunctiveCondition

TYR_INLINE_IMPL bool is_statically_applicable(formalism::planning::GroundConjunctiveConditionView element, const boost::dynamic_bitset<>& static_atoms)
{
    return is_statically_applicable(element.template get_facts<formalism::StaticTag>(), static_atoms);
}

// GroundAction

TYR_INLINE_IMPL bool is_statically_applicable(formalism::planning::GroundActionView element, const boost::dynamic_bitset<>& static_atoms)
{
    return is_statically_applicable(element.get_condition(), static_atoms);
}

// GroundAxiom

TYR_INLINE_IMPL bool is_statically_applicable(formalism::planning::GroundAxiomView element, const boost::dynamic_bitset<>& static_atoms)
{
    return is_statically_applicable(element.get_body(), static_atoms);
}

/**
 * is_dynamically_applicable
 */

// GroundConjunctiveCondition

template<TaskKind Kind>
bool is_dynamically_applicable(formalism::planning::GroundConjunctiveConditionView element, const StateContext<Kind>& context)
{
    return is_applicable(element.template get_facts<formalism::FluentTag>(), context)      //
           && is_applicable(element.template get_facts<formalism::DerivedTag>(), context)  //
           && is_applicable(element.get_numeric_constraints(), context);
}

/**
 * is_consistent
 */

// GroundConjunctiveCondition

TYR_INLINE_IMPL bool is_consistent(formalism::planning::GroundConjunctiveConditionView element,
                                   UnorderedMap<Index<formalism::planning::FDRVariable<formalism::FluentTag>>, formalism::planning::FDRValue>& fluent_assign,
                                   UnorderedMap<Index<formalism::planning::GroundAtom<formalism::DerivedTag>>, bool>& derived_assign)
{
    for (const auto fact : element.template get_facts<formalism::FluentTag>())
    {
        if (const auto it = fluent_assign.find(fact.get_variable().get_index()); it != fluent_assign.end() && it->second != fact.get_value())
            return false;
        else
            fluent_assign.emplace(fact.get_variable().get_index(), fact.get_value());
    }

    for (const auto literal : element.template get_facts<formalism::DerivedTag>())
    {
        if (const auto it = derived_assign.find(literal.get_atom().get_index()); it != derived_assign.end() && it->second != literal.get_polarity())
            return false;
        else
            derived_assign.emplace(literal.get_atom().get_index(), literal.get_polarity());
    }

    return true;
}

// GroundAction

TYR_INLINE_IMPL bool
is_consistent(formalism::planning::GroundActionView element,
              UnorderedMap<Index<formalism::planning::FDRVariable<formalism::FluentTag>>, formalism::planning::FDRValue>& out_fluent_assign,
              UnorderedMap<Index<formalism::planning::GroundAtom<formalism::DerivedTag>>, bool>& out_derived_assign)
{
    out_fluent_assign.clear();
    out_derived_assign.clear();
    return is_consistent(element.get_condition(), out_fluent_assign, out_derived_assign);
}

// GroundAxiom

TYR_INLINE_IMPL bool
is_consistent(formalism::planning::GroundAxiomView element,
              UnorderedMap<Index<formalism::planning::FDRVariable<formalism::FluentTag>>, formalism::planning::FDRValue>& out_fluent_assign,
              UnorderedMap<Index<formalism::planning::GroundAtom<formalism::DerivedTag>>, bool>& out_derived_assign)
{
    out_fluent_assign.clear();
    out_derived_assign.clear();
    return is_consistent(element.get_body(), out_fluent_assign, out_derived_assign);
}

}
