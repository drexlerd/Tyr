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

#ifndef TYR_FORMALISM_PLANNING_MERGE_PLANNING_HPP_
#define TYR_FORMALISM_PLANNING_MERGE_PLANNING_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/canonicalization.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/planning/builder.hpp"
#include "tyr/formalism/planning/canonicalization.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/merge_planning_decl.hpp"
#include "tyr/formalism/planning/repository.hpp"

namespace tyr::formalism::planning
{

// Common

std::pair<VariableView, bool> merge_d2p(formalism::datalog::VariableView element, MergePlanningContext& context);

std::pair<ObjectView, bool> merge_d2p(formalism::datalog::ObjectView element, MergePlanningContext& context);

std::pair<BindingView, bool> merge_d2p(formalism::datalog::BindingView element, MergePlanningContext& context);

Data<formalism::Term> merge_d2p(formalism::datalog::TermView element, MergePlanningContext& context);

// Propositional

template<FactKind T_SRC, FactKind T_DST>
std::pair<PredicateView<T_DST>, bool> merge_d2p(formalism::datalog::PredicateView<T_SRC> element, MergePlanningContext& context);

template<FactKind T_SRC, FactKind T_DST>
std::pair<AtomView<T_DST>, bool> merge_d2p(formalism::datalog::AtomView<T_SRC> element, MergePlanningContext& context);

template<FactKind T_SRC, FactKind T_DST>
std::pair<PredicateBindingView<T_DST>, bool>
merge_d2p(PredicateView<T_DST> predicate, formalism::datalog::PredicateBindingView<T_SRC> element, MergePlanningContext& context);

template<FactKind T_SRC, FactKind T_DST>
std::pair<GroundAtomView<T_DST>, bool> merge_d2p(formalism::datalog::GroundAtomView<T_SRC> element, MergePlanningContext& context);

template<FactKind T_SRC, FactKind T_DST>
std::pair<GroundAtomView<T_DST>, bool> merge_d2p(formalism::datalog::PredicateBindingView<T_SRC> element, MergePlanningContext& context);

template<FactKind T_SRC, FactKind T_DST>
std::pair<LiteralView<T_DST>, bool> merge_d2p(formalism::datalog::LiteralView<T_SRC> element, MergePlanningContext& context);

template<FactKind T_SRC, FactKind T_DST>
std::pair<GroundLiteralView<T_DST>, bool> merge_d2p(formalism::datalog::GroundLiteralView<T_SRC> element, MergePlanningContext& context);

// Numeric

template<FactKind T>
std::pair<FunctionView<T>, bool> merge_d2p(formalism::datalog::FunctionView<T> element, MergePlanningContext& context);

template<FactKind T>
std::pair<FunctionTermView<T>, bool> merge_d2p(formalism::datalog::FunctionTermView<T> element, MergePlanningContext& context);

template<FactKind T>
std::pair<FunctionBindingView<T>, bool> merge_d2p(FunctionView<T> function, formalism::datalog::FunctionBindingView<T> element, MergePlanningContext& context);

template<FactKind T>
std::pair<GroundFunctionTermView<T>, bool> merge_d2p(formalism::datalog::GroundFunctionTermView<T> element, MergePlanningContext& context);

template<FactKind T>
std::pair<GroundFunctionTermValueView<T>, bool> merge_d2p(formalism::datalog::GroundFunctionTermValueView<T> element, MergePlanningContext& context);

Data<FunctionExpression> merge_d2p(formalism::datalog::FunctionExpressionView element, MergePlanningContext& context);

Data<GroundFunctionExpression> merge_d2p(formalism::datalog::GroundFunctionExpressionView element, MergePlanningContext& context);

template<OpKind O, typename T>
std::pair<UnaryOperatorView<O, to_planning_payload_t<T>>, bool> merge_d2p(formalism::datalog::UnaryOperatorView<O, T> element, MergePlanningContext& context);

template<OpKind O, typename T>
std::pair<BinaryOperatorView<O, to_planning_payload_t<T>>, bool> merge_d2p(formalism::datalog::BinaryOperatorView<O, T> element, MergePlanningContext& context);

template<OpKind O, typename T>
std::pair<MultiOperatorView<O, to_planning_payload_t<T>>, bool> merge_d2p(formalism::datalog::MultiOperatorView<O, T> element, MergePlanningContext& context);

template<typename T>
Data<ArithmeticOperator<to_planning_payload_t<T>>> merge_d2p(formalism::datalog::ArithmeticOperatorView<T> element, MergePlanningContext& context);

template<typename T>
Data<BooleanOperator<to_planning_payload_t<T>>> merge_d2p(formalism::datalog::BooleanOperatorView<T> element, MergePlanningContext& context);

}

#ifdef TYR_HEADER_INSTANTIATION
#include "tyr/formalism/planning/merge_planning.ipp"
#endif

#endif