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

#ifndef TYR_FORMALISM_PLANNING_MERGE_HPP_
#define TYR_FORMALISM_PLANNING_MERGE_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/planning/builder.hpp"
#include "tyr/formalism/planning/canonicalization.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/merge_decl.hpp"
#include "tyr/formalism/planning/repository.hpp"

namespace tyr::formalism::planning
{

// Common

std::pair<VariableView, bool> merge_p2p(VariableView element, MergeContext& context);

std::pair<ObjectView, bool> merge_p2p(ObjectView element, MergeContext& context);

template<FactKind T>
std::pair<PredicateBindingView<T>, bool> merge_p2p(PredicateBindingView<T> element, MergeContext& context);

template<FactKind T>
std::pair<FunctionBindingView<T>, bool> merge_p2p(FunctionBindingView<T> element, MergeContext& context);

std::pair<ActionBindingView, bool> merge_p2p(ActionBindingView element, MergeContext& context);

std::pair<AxiomBindingView, bool> merge_p2p(AxiomBindingView element, MergeContext& context);

Data<Term> merge_p2p(TermView element, MergeContext& context);

// Propositional

template<FactKind T>
std::pair<PredicateView<T>, bool> merge_p2p(PredicateView<T> element, MergeContext& context);

template<FactKind T>
std::pair<AtomView<T>, bool> merge_p2p(AtomView<T> element, MergeContext& context);

template<FactKind T>
std::pair<GroundAtomView<T>, bool> merge_p2p(GroundAtomView<T> element, MergeContext& context);

template<FactKind T>
std::pair<LiteralView<T>, bool> merge_p2p(LiteralView<T> element, MergeContext& context);

template<FactKind T>
std::pair<GroundLiteralView<T>, bool> merge_p2p(GroundLiteralView<T> element, MergeContext& context);

// Numeric

template<FactKind T>
std::pair<FunctionView<T>, bool> merge_p2p(FunctionView<T> element, MergeContext& context);

template<FactKind T>
std::pair<FunctionTermView<T>, bool> merge_p2p(FunctionTermView<T> element, MergeContext& context);

template<FactKind T>
std::pair<GroundFunctionTermView<T>, bool> merge_p2p(GroundFunctionTermView<T> element, MergeContext& context);

template<FactKind T>
std::pair<GroundFunctionTermValueView<T>, bool> merge_p2p(GroundFunctionTermValueView<T> element, MergeContext& context);

Data<FunctionExpression> merge_p2p(FunctionExpressionView element, MergeContext& context);

Data<GroundFunctionExpression> merge_p2p(GroundFunctionExpressionView element, MergeContext& context);

template<OpKind O, typename T>
std::pair<UnaryOperatorView<O, T>, bool> merge_p2p(UnaryOperatorView<O, T> element, MergeContext& context);

template<OpKind O, typename T>
std::pair<BinaryOperatorView<O, T>, bool> merge_p2p(BinaryOperatorView<O, T> element, MergeContext& context);

template<OpKind O, typename T>
std::pair<MultiOperatorView<O, T>, bool> merge_p2p(MultiOperatorView<O, T> element, MergeContext& context);

template<typename T>
Data<ArithmeticOperator<T>> merge_p2p(ArithmeticOperatorView<T> element, MergeContext& context);

template<typename T>
Data<BooleanOperator<T>> merge_p2p(BooleanOperatorView<T> element, MergeContext& context);

template<NumericEffectOpKind O, FactKind T>
std::pair<NumericEffectView<O, T>, bool> merge_p2p(NumericEffectView<O, T> element, MergeContext& context);

template<FactKind T>
Data<NumericEffectOperator<T>> merge_p2p(NumericEffectOperatorView<T> element, MergeContext& context);

template<NumericEffectOpKind O, FactKind T>
std::pair<GroundNumericEffectView<O, T>, bool> merge_p2p(GroundNumericEffectView<O, T> element, MergeContext& context);

template<FactKind T>
Data<GroundNumericEffectOperator<T>> merge_p2p(GroundNumericEffectOperatorView<T> element, MergeContext& context);

// Composite

std::pair<ConjunctiveConditionView, bool> merge_p2p(ConjunctiveConditionView element, MergeContext& context);

std::pair<AxiomView, bool> merge_p2p(AxiomView element, MergeContext& context);

std::pair<MetricView, bool> merge_p2p(MetricView element, MergeContext& context);

}

#ifdef TYR_HEADER_INSTANTIATION
#include "tyr/formalism/planning/merge.ipp"
#endif

#endif