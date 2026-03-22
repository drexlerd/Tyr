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

#ifndef TYR_FORMALISM_PLANNING_MERGE_DATALOG_HPP_
#define TYR_FORMALISM_PLANNING_MERGE_DATALOG_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/canonicalization.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/merge_datalog_decl.hpp"
#include "tyr/formalism/planning/repository.hpp"

namespace tyr::formalism::planning
{

// Common

std::pair<formalism::datalog::VariableView, bool> merge_p2d(VariableView element, MergeDatalogContext& context);

std::pair<formalism::datalog::ObjectView, bool> merge_p2d(ObjectView element, MergeDatalogContext& context);

Data<Term> merge_p2d(TermView element, MergeDatalogContext& context);

// Propositional

template<FactKind T_SRC, FactKind T_DST = T_SRC>
std::pair<formalism::datalog::PredicateView<T_DST>, bool> merge_p2d(PredicateView<T_SRC> element, MergeDatalogContext& context);

template<FactKind T_SRC, FactKind T_DST = T_SRC>
std::pair<formalism::datalog::AtomView<T_DST>, bool> merge_p2d(AtomView<T_SRC> element, MergeDatalogContext& context);

template<FactKind T_SRC, FactKind T_DST = T_SRC>
std::pair<formalism::datalog::PredicateBindingView<T_DST>, bool> merge_p2d(PredicateBindingView<T_SRC> element, MergeDatalogContext& context);

template<FactKind T_SRC, FactKind T_DST = T_SRC>
std::pair<formalism::datalog::GroundAtomView<T_DST>, bool> merge_p2d(GroundAtomView<T_SRC> element, MergeDatalogContext& context);

template<FactKind T_SRC, FactKind T_DST = T_SRC>
std::pair<formalism::datalog::LiteralView<T_DST>, bool> merge_p2d(LiteralView<T_SRC> element, MergeDatalogContext& context);

template<FactKind T_SRC, FactKind T_DST = T_SRC>
std::pair<formalism::datalog::GroundLiteralView<T_DST>, bool> merge_p2d(GroundLiteralView<T_SRC> element, MergeDatalogContext& context);

// Numeric

template<FactKind T>
std::pair<formalism::datalog::FunctionView<T>, bool> merge_p2d(FunctionView<T> element, MergeDatalogContext& context);

template<FactKind T>
std::pair<formalism::datalog::FunctionTermView<T>, bool> merge_p2d(FunctionTermView<T> element, MergeDatalogContext& context);

template<FactKind T>
std::pair<formalism::datalog::FunctionBindingView<T>, bool> merge_p2d(FunctionBindingView<T> element, MergeDatalogContext& context);

template<FactKind T>
std::pair<formalism::datalog::GroundFunctionTermView<T>, bool> merge_p2d(GroundFunctionTermView<T> element, MergeDatalogContext& context);

template<FactKind T>
std::pair<formalism::datalog::GroundFunctionTermValueView<T>, bool> merge_p2d(GroundFunctionTermValueView<T> element, MergeDatalogContext& context);

Data<formalism::datalog::FunctionExpression> merge_p2d(FunctionExpressionView element, MergeDatalogContext& context);

Data<formalism::datalog::GroundFunctionExpression> merge_p2d(GroundFunctionExpressionView element, MergeDatalogContext& context);

template<OpKind O, typename T>
std::pair<formalism::datalog::UnaryOperatorView<O, to_datalog_payload_t<T>>, bool> merge_p2d(UnaryOperatorView<O, T> element, MergeDatalogContext& context);

template<OpKind O, typename T>
std::pair<formalism::datalog::BinaryOperatorView<O, to_datalog_payload_t<T>>, bool> merge_p2d(BinaryOperatorView<O, T> element, MergeDatalogContext& context);

template<OpKind O, typename T>
std::pair<formalism::datalog::MultiOperatorView<O, to_datalog_payload_t<T>>, bool> merge_p2d(MultiOperatorView<O, T> element, MergeDatalogContext& context);

template<typename T>
Data<formalism::datalog::ArithmeticOperator<to_datalog_payload_t<T>>> merge_p2d(ArithmeticOperatorView<T> element, MergeDatalogContext& context);

template<typename T>
Data<formalism::datalog::BooleanOperator<to_datalog_payload_t<T>>> merge_p2d(BooleanOperatorView<T> element, MergeDatalogContext& context);

}

#ifdef TYR_HEADER_INSTANTIATION
#include "tyr/formalism/planning/merge_datalog.ipp"
#endif

#endif