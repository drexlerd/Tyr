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

#include "tyr/formalism/planning/merge_datalog.hpp"

#ifndef TYR_HEADER_INSTANTIATION
#include "tyr/formalism/planning/merge_datalog.ipp"

namespace tyr::formalism::planning
{

template std::pair<formalism::datalog::PredicateView<StaticTag>, bool> merge_p2d(PredicateView<StaticTag> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::PredicateView<FluentTag>, bool> merge_p2d(PredicateView<FluentTag> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::PredicateView<FluentTag>, bool> merge_p2d(PredicateView<DerivedTag> element, MergeDatalogContext& context);

template std::pair<formalism::datalog::AtomView<StaticTag>, bool> merge_p2d(AtomView<StaticTag> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::AtomView<FluentTag>, bool> merge_p2d(AtomView<FluentTag> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::AtomView<FluentTag>, bool> merge_p2d(AtomView<DerivedTag> element, MergeDatalogContext& context);

template std::pair<formalism::datalog::PredicateBindingView<StaticTag>, bool> merge_p2d(PredicateBindingView<StaticTag> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::PredicateBindingView<FluentTag>, bool> merge_p2d(PredicateBindingView<FluentTag> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::PredicateBindingView<FluentTag>, bool> merge_p2d(PredicateBindingView<DerivedTag> element, MergeDatalogContext& context);

template std::pair<formalism::datalog::GroundAtomView<StaticTag>, bool> merge_p2d(GroundAtomView<StaticTag> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::GroundAtomView<FluentTag>, bool> merge_p2d(GroundAtomView<FluentTag> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::GroundAtomView<FluentTag>, bool> merge_p2d(GroundAtomView<DerivedTag> element, MergeDatalogContext& context);

template std::pair<formalism::datalog::LiteralView<StaticTag>, bool> merge_p2d(LiteralView<StaticTag> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::LiteralView<FluentTag>, bool> merge_p2d(LiteralView<FluentTag> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::LiteralView<FluentTag>, bool> merge_p2d(LiteralView<DerivedTag> element, MergeDatalogContext& context);

template std::pair<formalism::datalog::GroundLiteralView<StaticTag>, bool> merge_p2d(GroundLiteralView<StaticTag> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::GroundLiteralView<FluentTag>, bool> merge_p2d(GroundLiteralView<FluentTag> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::GroundLiteralView<FluentTag>, bool> merge_p2d(GroundLiteralView<DerivedTag> element, MergeDatalogContext& context);

// Numeric

template std::pair<formalism::datalog::FunctionView<StaticTag>, bool> merge_p2d(FunctionView<StaticTag> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::FunctionView<FluentTag>, bool> merge_p2d(FunctionView<FluentTag> element, MergeDatalogContext& context);

template std::pair<formalism::datalog::FunctionTermView<StaticTag>, bool> merge_p2d(FunctionTermView<StaticTag> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::FunctionTermView<FluentTag>, bool> merge_p2d(FunctionTermView<FluentTag> element, MergeDatalogContext& context);

template std::pair<formalism::datalog::FunctionBindingView<StaticTag>, bool> merge_p2d(FunctionBindingView<StaticTag> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::FunctionBindingView<FluentTag>, bool> merge_p2d(FunctionBindingView<FluentTag> element, MergeDatalogContext& context);

template std::pair<formalism::datalog::GroundFunctionTermView<StaticTag>, bool> merge_p2d(GroundFunctionTermView<StaticTag> element,
                                                                                          MergeDatalogContext& context);
template std::pair<formalism::datalog::GroundFunctionTermView<FluentTag>, bool> merge_p2d(GroundFunctionTermView<FluentTag> element,
                                                                                          MergeDatalogContext& context);

template std::pair<formalism::datalog::GroundFunctionTermValueView<StaticTag>, bool> merge_p2d(GroundFunctionTermValueView<StaticTag> element,
                                                                                               MergeDatalogContext& context);
template std::pair<formalism::datalog::GroundFunctionTermValueView<FluentTag>, bool> merge_p2d(GroundFunctionTermValueView<FluentTag> element,
                                                                                               MergeDatalogContext& context);

template std::pair<formalism::datalog::UnaryOperatorView<OpSub, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(UnaryOperatorView<OpSub, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::UnaryOperatorView<OpSub, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(UnaryOperatorView<OpSub, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);

template std::pair<formalism::datalog::BinaryOperatorView<OpEq, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<OpEq, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<OpNe, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<OpNe, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<OpGe, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<OpGe, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<OpGt, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<OpGt, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<OpLe, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<OpLe, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<OpLt, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<OpLt, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<OpAdd, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<OpAdd, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<OpSub, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<OpSub, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<OpMul, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<OpMul, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<OpDiv, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<OpDiv, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<OpEq, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<OpEq, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<OpNe, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<OpNe, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<OpGe, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<OpGe, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<OpGt, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<OpGt, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<OpLe, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<OpLe, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<OpLt, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<OpLt, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<OpAdd, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<OpAdd, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<OpSub, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<OpSub, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<OpMul, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<OpMul, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<OpDiv, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<OpDiv, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);

template std::pair<formalism::datalog::MultiOperatorView<OpAdd, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(MultiOperatorView<OpAdd, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::MultiOperatorView<OpMul, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(MultiOperatorView<OpMul, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::MultiOperatorView<OpAdd, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(MultiOperatorView<OpAdd, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::MultiOperatorView<OpMul, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(MultiOperatorView<OpMul, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);

template Data<formalism::datalog::ArithmeticOperator<Data<formalism::datalog::FunctionExpression>>>
merge_p2d(ArithmeticOperatorView<Data<FunctionExpression>> element, MergeDatalogContext& context);
template Data<formalism::datalog::ArithmeticOperator<Data<formalism::datalog::GroundFunctionExpression>>>
merge_p2d(ArithmeticOperatorView<Data<GroundFunctionExpression>> element, MergeDatalogContext& context);

template Data<formalism::datalog::BooleanOperator<Data<formalism::datalog::FunctionExpression>>>
merge_p2d(BooleanOperatorView<Data<FunctionExpression>> element, MergeDatalogContext& context);
template Data<formalism::datalog::BooleanOperator<Data<formalism::datalog::GroundFunctionExpression>>>
merge_p2d(BooleanOperatorView<Data<GroundFunctionExpression>> element, MergeDatalogContext& context);

}

#endif
