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

#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
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
float_t evaluate(float_t element, const StateContext<Kind>& context);

template<TaskKind Kind, formalism::ArithmeticOpKind O>
float_t evaluate(formalism::planning::GroundUnaryOperatorView<O> element, const StateContext<Kind>& context);

template<TaskKind Kind, formalism::ArithmeticOpKind O>
float_t evaluate(formalism::planning::GroundBinaryOperatorView<O> element, const StateContext<Kind>& context);

template<TaskKind Kind, formalism::BooleanOpKind O>
bool evaluate(formalism::planning::GroundBinaryOperatorView<O> element, const StateContext<Kind>& context);

template<TaskKind Kind, formalism::ArithmeticOpKind O>
float_t evaluate(formalism::planning::GroundMultiOperatorView<O> element, const StateContext<Kind>& context);

template<TaskKind Kind>
float_t evaluate(formalism::planning::GroundFunctionTermView<formalism::StaticTag> element, const StateContext<Kind>& context);

template<TaskKind Kind>
float_t evaluate(formalism::planning::GroundFunctionTermView<formalism::FluentTag> element, const StateContext<Kind>& context);

template<TaskKind Kind>
float_t evaluate(formalism::planning::GroundFunctionTermView<formalism::AuxiliaryTag> element, const StateContext<Kind>& context);

template<TaskKind Kind>
float_t evaluate(formalism::planning::GroundFunctionExpressionView element, const StateContext<Kind>& context);

template<TaskKind Kind>
float_t evaluate(formalism::planning::GroundArithmeticOperatorView element, const StateContext<Kind>& context);

template<TaskKind Kind>
bool evaluate(formalism::planning::GroundBooleanOperatorView element, const StateContext<Kind>& context);

template<TaskKind Kind, formalism::planning::NumericEffectOpKind Op, formalism::FactKind T>
float_t evaluate(formalism::planning::GroundNumericEffectView<Op, T> element, const StateContext<Kind>& context);

template<TaskKind Kind, formalism::FactKind T>
float_t evaluate(formalism::planning::GroundNumericEffectOperatorView<T> element, const StateContext<Kind>& context);

/**
 * is_applicable_if_fires
 */

template<TaskKind Kind>
bool is_applicable_if_fires(formalism::planning::GroundConditionalEffectView element,
                            const StateContext<Kind>& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families);

/**
 * is_applicable
 */

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundLiteralView<formalism::StaticTag> element, const StateContext<Kind>& context);

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundLiteralView<formalism::DerivedTag> element, const StateContext<Kind>& context);

template<TaskKind Kind, formalism::FactKind T>
bool is_applicable(formalism::planning::GroundLiteralListView<T> elements, const StateContext<Kind>& context);

template<TaskKind Kind>
bool is_applicable(formalism::planning::FDRFactView<formalism::FluentTag> element, const StateContext<Kind>& context);

template<TaskKind Kind>
bool is_applicable(formalism::planning::FDRFactListView<formalism::FluentTag> elements, const StateContext<Kind>& context);

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundBooleanOperatorView element, const StateContext<Kind>& context);

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundBooleanOperatorListView elements, const StateContext<Kind>& context);

template<TaskKind Kind, formalism::planning::NumericEffectOpKind Op>
bool is_applicable(formalism::planning::GroundNumericEffectView<Op, formalism::FluentTag> element,
                   const StateContext<Kind>& context,
                   formalism::planning::EffectFamilyList& ref_fluent_effect_families);

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundNumericEffectOperatorView<formalism::FluentTag> element,
                   const StateContext<Kind>& context,
                   formalism::planning::EffectFamilyList& ref_fluent_effect_families);

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundNumericEffectOperatorListView<formalism::FluentTag> elements,
                   const StateContext<Kind>& context,
                   formalism::planning::EffectFamilyList& ref_fluent_effect_families);

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundNumericEffectView<formalism::planning::OpIncrease, formalism::AuxiliaryTag> element,
                   const StateContext<Kind>& context);

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundNumericEffectOperatorView<formalism::AuxiliaryTag> element, const StateContext<Kind>& context);

// GroundConjunctiveCondition

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundConjunctiveConditionView element, const StateContext<Kind>& context);

// GroundConjunctiveEffect

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundConjunctiveEffectView element,
                   const StateContext<Kind>& context,
                   formalism::planning::EffectFamilyList& ref_fluent_effect_families);

// GroundConditionalEffectList

template<TaskKind Kind>
bool are_applicable_if_fires(formalism::planning::GroundConditionalEffectListView elements,
                             const StateContext<Kind>& context,
                             formalism::planning::EffectFamilyList& out_fluent_effect_families);

// GroundAction

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundActionView element,
                   const StateContext<Kind>& context,
                   formalism::planning::EffectFamilyList& out_fluent_effect_families);

// GroundAxiom

template<TaskKind Kind>
bool is_applicable(formalism::planning::GroundAxiomView element, const StateContext<Kind>& context);

/**
 * is_statically_applicable
 */

bool is_statically_applicable(formalism::planning::GroundLiteralView<formalism::StaticTag> element, const boost::dynamic_bitset<>& static_atoms);

bool is_statically_applicable(formalism::planning::GroundLiteralListView<formalism::StaticTag> elements, const boost::dynamic_bitset<>& static_atoms);

// GroundConjunctiveCondition

bool is_statically_applicable(formalism::planning::GroundConjunctiveConditionView element, const boost::dynamic_bitset<>& static_atoms);

// GroundAction

bool is_statically_applicable(formalism::planning::GroundActionView element, const boost::dynamic_bitset<>& static_atoms);

// GroundAxiom

bool is_statically_applicable(formalism::planning::GroundAxiomView element, const boost::dynamic_bitset<>& static_atoms);

/**
 * is_dynamically_applicable
 */

// GroundConjunctiveCondition

template<TaskKind Kind>
bool is_dynamically_applicable(formalism::planning::GroundConjunctiveConditionView element, const StateContext<Kind>& context);

/**
 * is_consistent
 */

// GroundConjunctiveCondition

bool is_consistent(formalism::planning::GroundConjunctiveConditionView element,
                   UnorderedMap<Index<formalism::planning::FDRVariable<formalism::FluentTag>>, formalism::planning::FDRValue>& fluent_assign,
                   UnorderedMap<Index<formalism::planning::GroundAtom<formalism::DerivedTag>>, bool>& derived_assign);

// GroundAction

bool is_consistent(formalism::planning::GroundActionView element,
                   UnorderedMap<Index<formalism::planning::FDRVariable<formalism::FluentTag>>, formalism::planning::FDRValue>& out_fluent_assign,
                   UnorderedMap<Index<formalism::planning::GroundAtom<formalism::DerivedTag>>, bool>& out_derived_assign);

// GroundAxiom

bool is_consistent(formalism::planning::GroundAxiomView element,
                   UnorderedMap<Index<formalism::planning::FDRVariable<formalism::FluentTag>>, formalism::planning::FDRValue>& out_fluent_assign,
                   UnorderedMap<Index<formalism::planning::GroundAtom<formalism::DerivedTag>>, bool>& out_derived_assign);

}

#ifdef TYR_HEADER_INSTANTIATION
#include "tyr/planning/applicability.ipp"
#endif

#endif
