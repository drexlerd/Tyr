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

#include "tyr/formalism/datalog/merge.hpp"

#ifndef TYR_HEADER_INSTANTIATION
#include "tyr/formalism/datalog/merge.ipp"

namespace tyr::formalism::datalog
{
template std::pair<PredicateView<StaticTag>, bool> merge_d2d(PredicateView<StaticTag> element, MergeContext& context);
template std::pair<PredicateView<FluentTag>, bool> merge_d2d(PredicateView<FluentTag> element, MergeContext& context);

template std::pair<AtomView<StaticTag>, bool> merge_d2d(AtomView<StaticTag> element, MergeContext& context);
template std::pair<AtomView<FluentTag>, bool> merge_d2d(AtomView<FluentTag> element, MergeContext& context);

template std::pair<PredicateBindingView<StaticTag>, bool>
merge_d2d(PredicateView<StaticTag> predicate, PredicateBindingView<StaticTag> element, MergeContext& context);
template std::pair<PredicateBindingView<FluentTag>, bool>
merge_d2d(PredicateView<FluentTag> predicate, PredicateBindingView<FluentTag> element, MergeContext& context);

template std::pair<GroundAtomView<StaticTag>, bool> merge_d2d(GroundAtomView<StaticTag> element, MergeContext& context);
template std::pair<GroundAtomView<FluentTag>, bool> merge_d2d(GroundAtomView<FluentTag> element, MergeContext& context);

template std::pair<LiteralView<StaticTag>, bool> merge_d2d(LiteralView<StaticTag> element, MergeContext& context);
template std::pair<LiteralView<FluentTag>, bool> merge_d2d(LiteralView<FluentTag> element, MergeContext& context);

template std::pair<GroundLiteralView<StaticTag>, bool> merge_d2d(GroundLiteralView<StaticTag> element, MergeContext& context);
template std::pair<GroundLiteralView<FluentTag>, bool> merge_d2d(GroundLiteralView<FluentTag> element, MergeContext& context);

template std::pair<FunctionView<StaticTag>, bool> merge_d2d(FunctionView<StaticTag> element, MergeContext& context);
template std::pair<FunctionView<FluentTag>, bool> merge_d2d(FunctionView<FluentTag> element, MergeContext& context);

template std::pair<FunctionTermView<StaticTag>, bool> merge_d2d(FunctionTermView<StaticTag> element, MergeContext& context);
template std::pair<FunctionTermView<FluentTag>, bool> merge_d2d(FunctionTermView<FluentTag> element, MergeContext& context);

template std::pair<FunctionBindingView<StaticTag>, bool>
merge_d2d(FunctionView<StaticTag> function, FunctionBindingView<StaticTag> element, MergeContext& context);
template std::pair<FunctionBindingView<FluentTag>, bool>
merge_d2d(FunctionView<FluentTag> function, FunctionBindingView<FluentTag> element, MergeContext& context);

template std::pair<GroundFunctionTermView<StaticTag>, bool> merge_d2d(GroundFunctionTermView<StaticTag> element, MergeContext& context);
template std::pair<GroundFunctionTermView<FluentTag>, bool> merge_d2d(GroundFunctionTermView<FluentTag> element, MergeContext& context);

template std::pair<GroundFunctionTermValueView<StaticTag>, bool> merge_d2d(GroundFunctionTermValueView<StaticTag> element, MergeContext& context);
template std::pair<GroundFunctionTermValueView<FluentTag>, bool> merge_d2d(GroundFunctionTermValueView<FluentTag> element, MergeContext& context);

template std::pair<UnaryOperatorView<OpSub, Data<FunctionExpression>>, bool> merge_d2d(UnaryOperatorView<OpSub, Data<FunctionExpression>> element,
                                                                                       MergeContext& context);
template std::pair<UnaryOperatorView<OpSub, Data<GroundFunctionExpression>>, bool> merge_d2d(UnaryOperatorView<OpSub, Data<GroundFunctionExpression>> element,
                                                                                             MergeContext& context);

template std::pair<BinaryOperatorView<OpEq, Data<FunctionExpression>>, bool> merge_d2d(BinaryOperatorView<OpEq, Data<FunctionExpression>> element,
                                                                                       MergeContext& context);
template std::pair<BinaryOperatorView<OpNe, Data<FunctionExpression>>, bool> merge_d2d(BinaryOperatorView<OpNe, Data<FunctionExpression>> element,
                                                                                       MergeContext& context);
template std::pair<BinaryOperatorView<OpGe, Data<FunctionExpression>>, bool> merge_d2d(BinaryOperatorView<OpGe, Data<FunctionExpression>> element,
                                                                                       MergeContext& context);
template std::pair<BinaryOperatorView<OpGt, Data<FunctionExpression>>, bool> merge_d2d(BinaryOperatorView<OpGt, Data<FunctionExpression>> element,
                                                                                       MergeContext& context);
template std::pair<BinaryOperatorView<OpLe, Data<FunctionExpression>>, bool> merge_d2d(BinaryOperatorView<OpLe, Data<FunctionExpression>> element,
                                                                                       MergeContext& context);
template std::pair<BinaryOperatorView<OpLt, Data<FunctionExpression>>, bool> merge_d2d(BinaryOperatorView<OpLt, Data<FunctionExpression>> element,
                                                                                       MergeContext& context);
template std::pair<BinaryOperatorView<OpAdd, Data<FunctionExpression>>, bool> merge_d2d(BinaryOperatorView<OpAdd, Data<FunctionExpression>> element,
                                                                                        MergeContext& context);
template std::pair<BinaryOperatorView<OpSub, Data<FunctionExpression>>, bool> merge_d2d(BinaryOperatorView<OpSub, Data<FunctionExpression>> element,
                                                                                        MergeContext& context);
template std::pair<BinaryOperatorView<OpMul, Data<FunctionExpression>>, bool> merge_d2d(BinaryOperatorView<OpMul, Data<FunctionExpression>> element,
                                                                                        MergeContext& context);
template std::pair<BinaryOperatorView<OpDiv, Data<FunctionExpression>>, bool> merge_d2d(BinaryOperatorView<OpDiv, Data<FunctionExpression>> element,
                                                                                        MergeContext& context);
template std::pair<BinaryOperatorView<OpEq, Data<GroundFunctionExpression>>, bool> merge_d2d(BinaryOperatorView<OpEq, Data<GroundFunctionExpression>> element,
                                                                                             MergeContext& context);
template std::pair<BinaryOperatorView<OpNe, Data<GroundFunctionExpression>>, bool> merge_d2d(BinaryOperatorView<OpNe, Data<GroundFunctionExpression>> element,
                                                                                             MergeContext& context);
template std::pair<BinaryOperatorView<OpGe, Data<GroundFunctionExpression>>, bool> merge_d2d(BinaryOperatorView<OpGe, Data<GroundFunctionExpression>> element,
                                                                                             MergeContext& context);
template std::pair<BinaryOperatorView<OpGt, Data<GroundFunctionExpression>>, bool> merge_d2d(BinaryOperatorView<OpGt, Data<GroundFunctionExpression>> element,
                                                                                             MergeContext& context);
template std::pair<BinaryOperatorView<OpLe, Data<GroundFunctionExpression>>, bool> merge_d2d(BinaryOperatorView<OpLe, Data<GroundFunctionExpression>> element,
                                                                                             MergeContext& context);
template std::pair<BinaryOperatorView<OpLt, Data<GroundFunctionExpression>>, bool> merge_d2d(BinaryOperatorView<OpLt, Data<GroundFunctionExpression>> element,
                                                                                             MergeContext& context);
template std::pair<BinaryOperatorView<OpAdd, Data<GroundFunctionExpression>>, bool> merge_d2d(BinaryOperatorView<OpAdd, Data<GroundFunctionExpression>> element,
                                                                                              MergeContext& context);
template std::pair<BinaryOperatorView<OpSub, Data<GroundFunctionExpression>>, bool> merge_d2d(BinaryOperatorView<OpSub, Data<GroundFunctionExpression>> element,
                                                                                              MergeContext& context);
template std::pair<BinaryOperatorView<OpMul, Data<GroundFunctionExpression>>, bool> merge_d2d(BinaryOperatorView<OpMul, Data<GroundFunctionExpression>> element,
                                                                                              MergeContext& context);
template std::pair<BinaryOperatorView<OpDiv, Data<GroundFunctionExpression>>, bool> merge_d2d(BinaryOperatorView<OpDiv, Data<GroundFunctionExpression>> element,
                                                                                              MergeContext& context);

template std::pair<MultiOperatorView<OpAdd, Data<FunctionExpression>>, bool> merge_d2d(MultiOperatorView<OpAdd, Data<FunctionExpression>> element,
                                                                                       MergeContext& context);
template std::pair<MultiOperatorView<OpMul, Data<FunctionExpression>>, bool> merge_d2d(MultiOperatorView<OpMul, Data<FunctionExpression>> element,
                                                                                       MergeContext& context);
template std::pair<MultiOperatorView<OpAdd, Data<GroundFunctionExpression>>, bool> merge_d2d(MultiOperatorView<OpAdd, Data<GroundFunctionExpression>> element,
                                                                                             MergeContext& context);
template std::pair<MultiOperatorView<OpMul, Data<GroundFunctionExpression>>, bool> merge_d2d(MultiOperatorView<OpMul, Data<GroundFunctionExpression>> element,
                                                                                             MergeContext& context);

template Data<ArithmeticOperator<Data<FunctionExpression>>> merge_d2d(ArithmeticOperatorView<Data<FunctionExpression>> element, MergeContext& context);
template Data<ArithmeticOperator<Data<GroundFunctionExpression>>> merge_d2d(ArithmeticOperatorView<Data<GroundFunctionExpression>> element,
                                                                            MergeContext& context);
}

#endif
