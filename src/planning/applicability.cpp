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

#include "tyr/planning/applicability.hpp"

#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/ground_task/unpacked_state.hpp"
#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"

#ifndef TYR_HEADER_INSTANTIATION
#include "tyr/planning/applicability.ipp"

namespace tyr::planning
{

template float_t evaluate(float_t element, const StateContext<LiftedTag>& context);
template float_t evaluate(float_t element, const StateContext<GroundTag>& context);

template float_t evaluate(formalism::planning::GroundUnaryOperatorView<formalism::OpSub> element, const StateContext<LiftedTag>& context);
template float_t evaluate(formalism::planning::GroundUnaryOperatorView<formalism::OpSub> element, const StateContext<GroundTag>& context);

template float_t evaluate(formalism::planning::GroundBinaryOperatorView<formalism::OpAdd> element, const StateContext<LiftedTag>& context);
template float_t evaluate(formalism::planning::GroundBinaryOperatorView<formalism::OpSub> element, const StateContext<LiftedTag>& context);
template float_t evaluate(formalism::planning::GroundBinaryOperatorView<formalism::OpMul> element, const StateContext<LiftedTag>& context);
template float_t evaluate(formalism::planning::GroundBinaryOperatorView<formalism::OpDiv> element, const StateContext<LiftedTag>& context);
template float_t evaluate(formalism::planning::GroundBinaryOperatorView<formalism::OpAdd> element, const StateContext<GroundTag>& context);
template float_t evaluate(formalism::planning::GroundBinaryOperatorView<formalism::OpSub> element, const StateContext<GroundTag>& context);
template float_t evaluate(formalism::planning::GroundBinaryOperatorView<formalism::OpMul> element, const StateContext<GroundTag>& context);
template float_t evaluate(formalism::planning::GroundBinaryOperatorView<formalism::OpDiv> element, const StateContext<GroundTag>& context);

template bool evaluate(formalism::planning::GroundBinaryOperatorView<formalism::OpEq> element, const StateContext<LiftedTag>& context);
template bool evaluate(formalism::planning::GroundBinaryOperatorView<formalism::OpNe> element, const StateContext<LiftedTag>& context);
template bool evaluate(formalism::planning::GroundBinaryOperatorView<formalism::OpGe> element, const StateContext<LiftedTag>& context);
template bool evaluate(formalism::planning::GroundBinaryOperatorView<formalism::OpGt> element, const StateContext<LiftedTag>& context);
template bool evaluate(formalism::planning::GroundBinaryOperatorView<formalism::OpLe> element, const StateContext<LiftedTag>& context);
template bool evaluate(formalism::planning::GroundBinaryOperatorView<formalism::OpLt> element, const StateContext<LiftedTag>& context);
template bool evaluate(formalism::planning::GroundBinaryOperatorView<formalism::OpEq> element, const StateContext<GroundTag>& context);
template bool evaluate(formalism::planning::GroundBinaryOperatorView<formalism::OpNe> element, const StateContext<GroundTag>& context);
template bool evaluate(formalism::planning::GroundBinaryOperatorView<formalism::OpGe> element, const StateContext<GroundTag>& context);
template bool evaluate(formalism::planning::GroundBinaryOperatorView<formalism::OpGt> element, const StateContext<GroundTag>& context);
template bool evaluate(formalism::planning::GroundBinaryOperatorView<formalism::OpLe> element, const StateContext<GroundTag>& context);
template bool evaluate(formalism::planning::GroundBinaryOperatorView<formalism::OpLt> element, const StateContext<GroundTag>& context);

template float_t evaluate(formalism::planning::GroundMultiOperatorView<formalism::OpAdd> element, const StateContext<LiftedTag>& context);
template float_t evaluate(formalism::planning::GroundMultiOperatorView<formalism::OpMul> element, const StateContext<LiftedTag>& context);
template float_t evaluate(formalism::planning::GroundMultiOperatorView<formalism::OpAdd> element, const StateContext<GroundTag>& context);
template float_t evaluate(formalism::planning::GroundMultiOperatorView<formalism::OpMul> element, const StateContext<GroundTag>& context);

template float_t evaluate(formalism::planning::GroundFunctionTermView<formalism::StaticTag> element, const StateContext<LiftedTag>& context);
template float_t evaluate(formalism::planning::GroundFunctionTermView<formalism::StaticTag> element, const StateContext<GroundTag>& context);

template float_t evaluate(formalism::planning::GroundFunctionTermView<formalism::FluentTag> element, const StateContext<LiftedTag>& context);
template float_t evaluate(formalism::planning::GroundFunctionTermView<formalism::FluentTag> element, const StateContext<GroundTag>& context);

template float_t evaluate(formalism::planning::GroundFunctionTermView<formalism::AuxiliaryTag> element, const StateContext<LiftedTag>& context);
template float_t evaluate(formalism::planning::GroundFunctionTermView<formalism::AuxiliaryTag> element, const StateContext<GroundTag>& context);

template float_t evaluate(formalism::planning::GroundFunctionExpressionView element, const StateContext<LiftedTag>& context);
template float_t evaluate(formalism::planning::GroundFunctionExpressionView element, const StateContext<GroundTag>& context);

template float_t evaluate(formalism::planning::GroundArithmeticOperatorView element, const StateContext<LiftedTag>& context);
template float_t evaluate(formalism::planning::GroundArithmeticOperatorView element, const StateContext<GroundTag>& context);

template bool is_applicable(formalism::planning::GroundBooleanOperatorView element, const StateContext<LiftedTag>& context);
template bool is_applicable(formalism::planning::GroundBooleanOperatorView element, const StateContext<GroundTag>& context);

template bool evaluate(formalism::planning::GroundBooleanOperatorView element, const StateContext<LiftedTag>& context);
template bool evaluate(formalism::planning::GroundBooleanOperatorView element, const StateContext<GroundTag>& context);

template float_t evaluate(formalism::planning::GroundNumericEffectView<formalism::planning::OpAssign, formalism::FluentTag> element,
                          const StateContext<LiftedTag>& context);
template float_t evaluate(formalism::planning::GroundNumericEffectView<formalism::planning::OpIncrease, formalism::FluentTag> element,
                          const StateContext<LiftedTag>& context);
template float_t evaluate(formalism::planning::GroundNumericEffectView<formalism::planning::OpDecrease, formalism::FluentTag> element,
                          const StateContext<LiftedTag>& context);
template float_t evaluate(formalism::planning::GroundNumericEffectView<formalism::planning::OpScaleUp, formalism::FluentTag> element,
                          const StateContext<LiftedTag>& context);
template float_t evaluate(formalism::planning::GroundNumericEffectView<formalism::planning::OpScaleDown, formalism::FluentTag> element,
                          const StateContext<LiftedTag>& context);
template float_t evaluate(formalism::planning::GroundNumericEffectView<formalism::planning::OpIncrease, formalism::AuxiliaryTag> element,
                          const StateContext<LiftedTag>& context);
template float_t evaluate(formalism::planning::GroundNumericEffectView<formalism::planning::OpAssign, formalism::FluentTag> element,
                          const StateContext<GroundTag>& context);
template float_t evaluate(formalism::planning::GroundNumericEffectView<formalism::planning::OpIncrease, formalism::FluentTag> element,
                          const StateContext<GroundTag>& context);
template float_t evaluate(formalism::planning::GroundNumericEffectView<formalism::planning::OpDecrease, formalism::FluentTag> element,
                          const StateContext<GroundTag>& context);
template float_t evaluate(formalism::planning::GroundNumericEffectView<formalism::planning::OpScaleUp, formalism::FluentTag> element,
                          const StateContext<GroundTag>& context);
template float_t evaluate(formalism::planning::GroundNumericEffectView<formalism::planning::OpScaleDown, formalism::FluentTag> element,
                          const StateContext<GroundTag>& context);
template float_t evaluate(formalism::planning::GroundNumericEffectView<formalism::planning::OpIncrease, formalism::AuxiliaryTag> element,
                          const StateContext<GroundTag>& context);

template float_t evaluate(formalism::planning::GroundNumericEffectOperatorView<formalism::FluentTag> element, const StateContext<LiftedTag>& context);
template float_t evaluate(formalism::planning::GroundNumericEffectOperatorView<formalism::AuxiliaryTag> element, const StateContext<LiftedTag>& context);
template float_t evaluate(formalism::planning::GroundNumericEffectOperatorView<formalism::FluentTag> element, const StateContext<GroundTag>& context);
template float_t evaluate(formalism::planning::GroundNumericEffectOperatorView<formalism::AuxiliaryTag> element, const StateContext<GroundTag>& context);

/**
 * is_applicable_if_fires
 */

template bool is_applicable_if_fires(formalism::planning::GroundConditionalEffectView element,
                                     const StateContext<LiftedTag>& context,
                                     formalism::planning::EffectFamilyList& ref_fluent_effect_families);
template bool is_applicable_if_fires(formalism::planning::GroundConditionalEffectView element,
                                     const StateContext<GroundTag>& context,
                                     formalism::planning::EffectFamilyList& ref_fluent_effect_families);

/**
 * is_applicable
 */

template bool is_applicable(formalism::planning::GroundLiteralView<formalism::StaticTag> element, const StateContext<LiftedTag>& context);
template bool is_applicable(formalism::planning::GroundLiteralView<formalism::StaticTag> element, const StateContext<GroundTag>& context);

template bool is_applicable(formalism::planning::GroundLiteralView<formalism::DerivedTag> element, const StateContext<LiftedTag>& context);
template bool is_applicable(formalism::planning::GroundLiteralView<formalism::DerivedTag> element, const StateContext<GroundTag>& context);

template bool is_applicable(formalism::planning::GroundLiteralListView<formalism::StaticTag> elements, const StateContext<LiftedTag>& context);
template bool is_applicable(formalism::planning::GroundLiteralListView<formalism::DerivedTag> elements, const StateContext<LiftedTag>& context);
template bool is_applicable(formalism::planning::GroundLiteralListView<formalism::StaticTag> elements, const StateContext<GroundTag>& context);
template bool is_applicable(formalism::planning::GroundLiteralListView<formalism::DerivedTag> elements, const StateContext<GroundTag>& context);

template bool is_applicable(formalism::planning::FDRFactView<formalism::FluentTag> element, const StateContext<LiftedTag>& context);
template bool is_applicable(formalism::planning::FDRFactView<formalism::FluentTag> element, const StateContext<GroundTag>& context);

template bool is_applicable(formalism::planning::FDRFactListView<formalism::FluentTag> elements, const StateContext<LiftedTag>& context);
template bool is_applicable(formalism::planning::FDRFactListView<formalism::FluentTag> elements, const StateContext<GroundTag>& context);

template bool is_applicable(formalism::planning::GroundBooleanOperatorListView elements, const StateContext<LiftedTag>& context);
template bool is_applicable(formalism::planning::GroundBooleanOperatorListView elements, const StateContext<GroundTag>& context);

template bool is_applicable(formalism::planning::GroundNumericEffectView<formalism::planning::OpAssign, formalism::FluentTag> element,
                            const StateContext<LiftedTag>& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families);
template bool is_applicable(formalism::planning::GroundNumericEffectView<formalism::planning::OpIncrease, formalism::FluentTag> element,
                            const StateContext<LiftedTag>& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families);
template bool is_applicable(formalism::planning::GroundNumericEffectView<formalism::planning::OpDecrease, formalism::FluentTag> element,
                            const StateContext<LiftedTag>& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families);
template bool is_applicable(formalism::planning::GroundNumericEffectView<formalism::planning::OpScaleUp, formalism::FluentTag> element,
                            const StateContext<LiftedTag>& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families);
template bool is_applicable(formalism::planning::GroundNumericEffectView<formalism::planning::OpScaleDown, formalism::FluentTag> element,
                            const StateContext<LiftedTag>& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families);
template bool is_applicable(formalism::planning::GroundNumericEffectView<formalism::planning::OpAssign, formalism::FluentTag> element,
                            const StateContext<GroundTag>& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families);
template bool is_applicable(formalism::planning::GroundNumericEffectView<formalism::planning::OpIncrease, formalism::FluentTag> element,
                            const StateContext<GroundTag>& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families);
template bool is_applicable(formalism::planning::GroundNumericEffectView<formalism::planning::OpDecrease, formalism::FluentTag> element,
                            const StateContext<GroundTag>& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families);
template bool is_applicable(formalism::planning::GroundNumericEffectView<formalism::planning::OpScaleUp, formalism::FluentTag> element,
                            const StateContext<GroundTag>& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families);
template bool is_applicable(formalism::planning::GroundNumericEffectView<formalism::planning::OpScaleDown, formalism::FluentTag> element,
                            const StateContext<GroundTag>& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families);

template bool is_applicable(formalism::planning::GroundNumericEffectOperatorView<formalism::FluentTag> element,
                            const StateContext<LiftedTag>& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families);
template bool is_applicable(formalism::planning::GroundNumericEffectOperatorView<formalism::FluentTag> element,
                            const StateContext<GroundTag>& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families);

template bool is_applicable(formalism::planning::GroundNumericEffectOperatorListView<formalism::FluentTag> elements,
                            const StateContext<LiftedTag>& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families);
template bool is_applicable(formalism::planning::GroundNumericEffectOperatorListView<formalism::FluentTag> elements,
                            const StateContext<GroundTag>& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families);

template bool is_applicable(formalism::planning::GroundNumericEffectView<formalism::planning::OpIncrease, formalism::AuxiliaryTag> element,
                            const StateContext<LiftedTag>& context);
template bool is_applicable(formalism::planning::GroundNumericEffectView<formalism::planning::OpIncrease, formalism::AuxiliaryTag> element,
                            const StateContext<GroundTag>& context);

template bool is_applicable(formalism::planning::GroundNumericEffectOperatorView<formalism::AuxiliaryTag> element, const StateContext<LiftedTag>& context);
template bool is_applicable(formalism::planning::GroundNumericEffectOperatorView<formalism::AuxiliaryTag> element, const StateContext<GroundTag>& context);

// GroundConjunctiveCondition

template bool is_applicable(formalism::planning::GroundConjunctiveConditionView element, const StateContext<LiftedTag>& context);
template bool is_applicable(formalism::planning::GroundConjunctiveConditionView element, const StateContext<GroundTag>& context);

// GroundConjunctiveEffect

template bool is_applicable(formalism::planning::GroundConjunctiveEffectView element,
                            const StateContext<LiftedTag>& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families);
template bool is_applicable(formalism::planning::GroundConjunctiveEffectView element,
                            const StateContext<GroundTag>& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families);

// GroundConditionalEffectList

template bool are_applicable_if_fires(formalism::planning::GroundConditionalEffectListView elements,
                                      const StateContext<LiftedTag>& context,
                                      formalism::planning::EffectFamilyList& out_fluent_effect_families);
template bool are_applicable_if_fires(formalism::planning::GroundConditionalEffectListView elements,
                                      const StateContext<GroundTag>& context,
                                      formalism::planning::EffectFamilyList& out_fluent_effect_families);

// GroundAction

template bool is_applicable(formalism::planning::GroundActionView element,
                            const StateContext<LiftedTag>& context,
                            formalism::planning::EffectFamilyList& out_fluent_effect_families);
template bool is_applicable(formalism::planning::GroundActionView element,
                            const StateContext<GroundTag>& context,
                            formalism::planning::EffectFamilyList& out_fluent_effect_families);

// GroundAxiom

template bool is_applicable(formalism::planning::GroundAxiomView element, const StateContext<LiftedTag>& context);
template bool is_applicable(formalism::planning::GroundAxiomView element, const StateContext<GroundTag>& context);

/**
 * is_dynamically_applicable
 */

// GroundConjunctiveCondition

template bool is_dynamically_applicable(formalism::planning::GroundConjunctiveConditionView element, const StateContext<LiftedTag>& context);
template bool is_dynamically_applicable(formalism::planning::GroundConjunctiveConditionView element, const StateContext<GroundTag>& context);

}

#endif
