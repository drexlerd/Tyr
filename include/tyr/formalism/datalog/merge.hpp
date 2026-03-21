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

#ifndef TYR_FORMALISM_DATALOG_MERGE_HPP_
#define TYR_FORMALISM_DATALOG_MERGE_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/canonicalization.hpp"
#include "tyr/formalism/datalog/indices.hpp"
#include "tyr/formalism/datalog/merge_decl.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/indices.hpp"
#include "tyr/formalism/views.hpp"

namespace tyr::formalism::datalog
{

// Common

std::pair<VariableView, bool> merge_d2d(VariableView element, MergeContext& context);

std::pair<ObjectView, bool> merge_d2d(ObjectView element, MergeContext& context);

std::pair<BindingView, bool> merge_d2d(BindingView element, MergeContext& context);

Data<Term> merge_d2d(TermView element, MergeContext& context);

// Propositional

template<FactKind T>
std::pair<PredicateView<T>, bool> merge_d2d(PredicateView<T> element, MergeContext& context);

template<FactKind T>
std::pair<AtomView<T>, bool> merge_d2d(AtomView<T> element, MergeContext& context);

template<FactKind T>
std::pair<PredicateBindingView<T>, bool> merge_d2d(PredicateBindingView<T> element, MergeContext& context);

template<FactKind T>
std::pair<GroundAtomView<T>, bool> merge_d2d(GroundAtomView<T> element, MergeContext& context);

template<FactKind T>
std::pair<LiteralView<T>, bool> merge_d2d(LiteralView<T> element, MergeContext& context);

template<FactKind T>
std::pair<GroundLiteralView<T>, bool> merge_d2d(GroundLiteralView<T> element, MergeContext& context);

// Numeric

template<FactKind T>
std::pair<FunctionView<T>, bool> merge_d2d(FunctionView<T> element, MergeContext& context);

template<FactKind T>
std::pair<FunctionTermView<T>, bool> merge_d2d(FunctionTermView<T> element, MergeContext& context);

template<FactKind T>
std::pair<FunctionBindingView<T>, bool> merge_d2d(FunctionBindingView<T> element, MergeContext& context);

template<FactKind T>
std::pair<GroundFunctionTermView<T>, bool> merge_d2d(GroundFunctionTermView<T> element, MergeContext& context);

template<FactKind T>
std::pair<GroundFunctionTermValueView<T>, bool> merge_d2d(GroundFunctionTermValueView<T> element, MergeContext& context);

Data<FunctionExpression> merge_d2d(FunctionExpressionView element, MergeContext& context);

Data<GroundFunctionExpression> merge_d2d(GroundFunctionExpressionView element, MergeContext& context);

template<OpKind O, typename T>
std::pair<UnaryOperatorView<O, T>, bool> merge_d2d(UnaryOperatorView<O, T> element, MergeContext& context);

template<OpKind O, typename T>
std::pair<BinaryOperatorView<O, T>, bool> merge_d2d(BinaryOperatorView<O, T> element, MergeContext& context);

template<OpKind O, typename T>
std::pair<MultiOperatorView<O, T>, bool> merge_d2d(MultiOperatorView<O, T> element, MergeContext& context);

template<typename T>
Data<ArithmeticOperator<T>> merge_d2d(ArithmeticOperatorView<T> element, MergeContext& context);

template<typename T>
Data<BooleanOperator<T>> merge_d2d(BooleanOperatorView<T> element, MergeContext& context);

std::pair<ConjunctiveConditionView, bool> merge_d2d(ConjunctiveConditionView element, MergeContext& context);

std::pair<GroundConjunctiveConditionView, bool> merge_d2d(GroundConjunctiveConditionView element, MergeContext& context);

std::pair<RuleView, bool> merge_d2d(RuleView element, MergeContext& context);

std::pair<GroundRuleView, bool> merge_d2d(GroundRuleView element, MergeContext& context);

}

#ifdef TYR_HEADER_INSTANTIATION
#include "tyr/formalism/datalog/merge.ipp"
#endif

#endif