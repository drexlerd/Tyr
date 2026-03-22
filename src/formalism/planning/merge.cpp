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

#include "tyr/formalism/planning/merge.hpp"

#ifndef TYR_HEADER_INSTANTIATION
#include "tyr/formalism/planning/merge.ipp"

namespace tyr::formalism::planning
{

template std::pair<PredicateBindingView<StaticTag>, bool> merge_p2p(PredicateBindingView<StaticTag> element, MergeContext& context);
template std::pair<PredicateBindingView<FluentTag>, bool> merge_p2p(PredicateBindingView<FluentTag> element, MergeContext& context);
template std::pair<PredicateBindingView<DerivedTag>, bool> merge_p2p(PredicateBindingView<DerivedTag> element, MergeContext& context);

template std::pair<FunctionBindingView<StaticTag>, bool> merge_p2p(FunctionBindingView<StaticTag> element, MergeContext& context);
template std::pair<FunctionBindingView<FluentTag>, bool> merge_p2p(FunctionBindingView<FluentTag> element, MergeContext& context);
template std::pair<FunctionBindingView<AuxiliaryTag>, bool> merge_p2p(FunctionBindingView<AuxiliaryTag> element, MergeContext& context);

template std::pair<PredicateView<StaticTag>, bool> merge_p2p(PredicateView<StaticTag> element, MergeContext& context);
template std::pair<PredicateView<FluentTag>, bool> merge_p2p(PredicateView<FluentTag> element, MergeContext& context);
template std::pair<PredicateView<DerivedTag>, bool> merge_p2p(PredicateView<DerivedTag> element, MergeContext& context);

template std::pair<AtomView<StaticTag>, bool> merge_p2p(AtomView<StaticTag> element, MergeContext& context);
template std::pair<AtomView<FluentTag>, bool> merge_p2p(AtomView<FluentTag> element, MergeContext& context);
template std::pair<AtomView<DerivedTag>, bool> merge_p2p(AtomView<DerivedTag> element, MergeContext& context);

template std::pair<GroundAtomView<StaticTag>, bool> merge_p2p(GroundAtomView<StaticTag> element, MergeContext& context);
template std::pair<GroundAtomView<FluentTag>, bool> merge_p2p(GroundAtomView<FluentTag> element, MergeContext& context);
template std::pair<GroundAtomView<DerivedTag>, bool> merge_p2p(GroundAtomView<DerivedTag> element, MergeContext& context);

template std::pair<LiteralView<StaticTag>, bool> merge_p2p(LiteralView<StaticTag> element, MergeContext& context);
template std::pair<LiteralView<FluentTag>, bool> merge_p2p(LiteralView<FluentTag> element, MergeContext& context);
template std::pair<LiteralView<DerivedTag>, bool> merge_p2p(LiteralView<DerivedTag> element, MergeContext& context);

template std::pair<GroundLiteralView<StaticTag>, bool> merge_p2p(GroundLiteralView<StaticTag> element, MergeContext& context);
template std::pair<GroundLiteralView<FluentTag>, bool> merge_p2p(GroundLiteralView<FluentTag> element, MergeContext& context);
template std::pair<GroundLiteralView<DerivedTag>, bool> merge_p2p(GroundLiteralView<DerivedTag> element, MergeContext& context);

template std::pair<FunctionView<StaticTag>, bool> merge_p2p(FunctionView<StaticTag> element, MergeContext& context);
template std::pair<FunctionView<FluentTag>, bool> merge_p2p(FunctionView<FluentTag> element, MergeContext& context);
template std::pair<FunctionView<AuxiliaryTag>, bool> merge_p2p(FunctionView<AuxiliaryTag> element, MergeContext& context);

template std::pair<FunctionTermView<StaticTag>, bool> merge_p2p(FunctionTermView<StaticTag> element, MergeContext& context);
template std::pair<FunctionTermView<FluentTag>, bool> merge_p2p(FunctionTermView<FluentTag> element, MergeContext& context);
template std::pair<FunctionTermView<AuxiliaryTag>, bool> merge_p2p(FunctionTermView<AuxiliaryTag> element, MergeContext& context);

template std::pair<GroundFunctionTermView<StaticTag>, bool> merge_p2p(GroundFunctionTermView<StaticTag> element, MergeContext& context);
template std::pair<GroundFunctionTermView<FluentTag>, bool> merge_p2p(GroundFunctionTermView<FluentTag> element, MergeContext& context);
template std::pair<GroundFunctionTermView<AuxiliaryTag>, bool> merge_p2p(GroundFunctionTermView<AuxiliaryTag> element, MergeContext& context);

template std::pair<GroundFunctionTermValueView<StaticTag>, bool> merge_p2p(GroundFunctionTermValueView<StaticTag> element, MergeContext& context);
template std::pair<GroundFunctionTermValueView<FluentTag>, bool> merge_p2p(GroundFunctionTermValueView<FluentTag> element, MergeContext& context);
template std::pair<GroundFunctionTermValueView<AuxiliaryTag>, bool> merge_p2p(GroundFunctionTermValueView<AuxiliaryTag> element, MergeContext& context);

template std::pair<UnaryOperatorView<OpSub, Data<FunctionExpression>>, bool> merge_p2p(UnaryOperatorView<OpSub, Data<FunctionExpression>> element,
                                                                                       MergeContext& context);
template std::pair<UnaryOperatorView<OpSub, Data<GroundFunctionExpression>>, bool> merge_p2p(UnaryOperatorView<OpSub, Data<GroundFunctionExpression>> element,
                                                                                             MergeContext& context);

template std::pair<BinaryOperatorView<OpEq, Data<FunctionExpression>>, bool> merge_p2p(BinaryOperatorView<OpEq, Data<FunctionExpression>> element,
                                                                                       MergeContext& context);
template std::pair<BinaryOperatorView<OpNe, Data<FunctionExpression>>, bool> merge_p2p(BinaryOperatorView<OpNe, Data<FunctionExpression>> element,
                                                                                       MergeContext& context);
template std::pair<BinaryOperatorView<OpGe, Data<FunctionExpression>>, bool> merge_p2p(BinaryOperatorView<OpGe, Data<FunctionExpression>> element,
                                                                                       MergeContext& context);
template std::pair<BinaryOperatorView<OpGt, Data<FunctionExpression>>, bool> merge_p2p(BinaryOperatorView<OpGt, Data<FunctionExpression>> element,
                                                                                       MergeContext& context);
template std::pair<BinaryOperatorView<OpLe, Data<FunctionExpression>>, bool> merge_p2p(BinaryOperatorView<OpLe, Data<FunctionExpression>> element,
                                                                                       MergeContext& context);
template std::pair<BinaryOperatorView<OpLt, Data<FunctionExpression>>, bool> merge_p2p(BinaryOperatorView<OpLt, Data<FunctionExpression>> element,
                                                                                       MergeContext& context);
template std::pair<BinaryOperatorView<OpAdd, Data<FunctionExpression>>, bool> merge_p2p(BinaryOperatorView<OpAdd, Data<FunctionExpression>> element,
                                                                                        MergeContext& context);
template std::pair<BinaryOperatorView<OpSub, Data<FunctionExpression>>, bool> merge_p2p(BinaryOperatorView<OpSub, Data<FunctionExpression>> element,
                                                                                        MergeContext& context);
template std::pair<BinaryOperatorView<OpMul, Data<FunctionExpression>>, bool> merge_p2p(BinaryOperatorView<OpMul, Data<FunctionExpression>> element,
                                                                                        MergeContext& context);
template std::pair<BinaryOperatorView<OpDiv, Data<FunctionExpression>>, bool> merge_p2p(BinaryOperatorView<OpDiv, Data<FunctionExpression>> element,
                                                                                        MergeContext& context);
template std::pair<BinaryOperatorView<OpEq, Data<GroundFunctionExpression>>, bool> merge_p2p(BinaryOperatorView<OpEq, Data<GroundFunctionExpression>> element,
                                                                                             MergeContext& context);
template std::pair<BinaryOperatorView<OpNe, Data<GroundFunctionExpression>>, bool> merge_p2p(BinaryOperatorView<OpNe, Data<GroundFunctionExpression>> element,
                                                                                             MergeContext& context);
template std::pair<BinaryOperatorView<OpGe, Data<GroundFunctionExpression>>, bool> merge_p2p(BinaryOperatorView<OpGe, Data<GroundFunctionExpression>> element,
                                                                                             MergeContext& context);
template std::pair<BinaryOperatorView<OpGt, Data<GroundFunctionExpression>>, bool> merge_p2p(BinaryOperatorView<OpGt, Data<GroundFunctionExpression>> element,
                                                                                             MergeContext& context);
template std::pair<BinaryOperatorView<OpLe, Data<GroundFunctionExpression>>, bool> merge_p2p(BinaryOperatorView<OpLe, Data<GroundFunctionExpression>> element,
                                                                                             MergeContext& context);
template std::pair<BinaryOperatorView<OpLt, Data<GroundFunctionExpression>>, bool> merge_p2p(BinaryOperatorView<OpLt, Data<GroundFunctionExpression>> element,
                                                                                             MergeContext& context);
template std::pair<BinaryOperatorView<OpAdd, Data<GroundFunctionExpression>>, bool> merge_p2p(BinaryOperatorView<OpAdd, Data<GroundFunctionExpression>> element,
                                                                                              MergeContext& context);
template std::pair<BinaryOperatorView<OpSub, Data<GroundFunctionExpression>>, bool> merge_p2p(BinaryOperatorView<OpSub, Data<GroundFunctionExpression>> element,
                                                                                              MergeContext& context);
template std::pair<BinaryOperatorView<OpMul, Data<GroundFunctionExpression>>, bool> merge_p2p(BinaryOperatorView<OpMul, Data<GroundFunctionExpression>> element,
                                                                                              MergeContext& context);
template std::pair<BinaryOperatorView<OpDiv, Data<GroundFunctionExpression>>, bool> merge_p2p(BinaryOperatorView<OpDiv, Data<GroundFunctionExpression>> element,
                                                                                              MergeContext& context);

template std::pair<MultiOperatorView<OpAdd, Data<FunctionExpression>>, bool> merge_p2p(MultiOperatorView<OpAdd, Data<FunctionExpression>> element,
                                                                                       MergeContext& context);
template std::pair<MultiOperatorView<OpMul, Data<FunctionExpression>>, bool> merge_p2p(MultiOperatorView<OpMul, Data<FunctionExpression>> element,
                                                                                       MergeContext& context);
template std::pair<MultiOperatorView<OpAdd, Data<GroundFunctionExpression>>, bool> merge_p2p(MultiOperatorView<OpAdd, Data<GroundFunctionExpression>> element,
                                                                                             MergeContext& context);
template std::pair<MultiOperatorView<OpMul, Data<GroundFunctionExpression>>, bool> merge_p2p(MultiOperatorView<OpMul, Data<GroundFunctionExpression>> element,
                                                                                             MergeContext& context);

template Data<ArithmeticOperator<Data<FunctionExpression>>> merge_p2p(ArithmeticOperatorView<Data<FunctionExpression>> element, MergeContext& context);
template Data<ArithmeticOperator<Data<GroundFunctionExpression>>> merge_p2p(ArithmeticOperatorView<Data<GroundFunctionExpression>> element,
                                                                            MergeContext& context);

template Data<BooleanOperator<Data<FunctionExpression>>> merge_p2p(BooleanOperatorView<Data<FunctionExpression>> element, MergeContext& context);
template Data<BooleanOperator<Data<GroundFunctionExpression>>> merge_p2p(BooleanOperatorView<Data<GroundFunctionExpression>> element, MergeContext& context);

template std::pair<NumericEffectView<OpAssign, FluentTag>, bool> merge_p2p(NumericEffectView<OpAssign, FluentTag> element, MergeContext& context);
template std::pair<NumericEffectView<OpIncrease, FluentTag>, bool> merge_p2p(NumericEffectView<OpIncrease, FluentTag> element, MergeContext& context);
template std::pair<NumericEffectView<OpDecrease, FluentTag>, bool> merge_p2p(NumericEffectView<OpDecrease, FluentTag> element, MergeContext& context);
template std::pair<NumericEffectView<OpScaleUp, FluentTag>, bool> merge_p2p(NumericEffectView<OpScaleUp, FluentTag> element, MergeContext& context);
template std::pair<NumericEffectView<OpScaleDown, FluentTag>, bool> merge_p2p(NumericEffectView<OpScaleDown, FluentTag> element, MergeContext& context);

template std::pair<NumericEffectView<OpIncrease, AuxiliaryTag>, bool> merge_p2p(NumericEffectView<OpIncrease, AuxiliaryTag> element, MergeContext& context);

template Data<NumericEffectOperator<FluentTag>> merge_p2p(NumericEffectOperatorView<FluentTag> element, MergeContext& context);
template Data<NumericEffectOperator<AuxiliaryTag>> merge_p2p(NumericEffectOperatorView<AuxiliaryTag> element, MergeContext& context);

template std::pair<GroundNumericEffectView<OpAssign, FluentTag>, bool> merge_p2p(GroundNumericEffectView<OpAssign, FluentTag> element, MergeContext& context);
template std::pair<GroundNumericEffectView<OpIncrease, FluentTag>, bool> merge_p2p(GroundNumericEffectView<OpIncrease, FluentTag> element,
                                                                                   MergeContext& context);
template std::pair<GroundNumericEffectView<OpDecrease, FluentTag>, bool> merge_p2p(GroundNumericEffectView<OpDecrease, FluentTag> element,
                                                                                   MergeContext& context);
template std::pair<GroundNumericEffectView<OpScaleUp, FluentTag>, bool> merge_p2p(GroundNumericEffectView<OpScaleUp, FluentTag> element, MergeContext& context);
template std::pair<GroundNumericEffectView<OpScaleDown, FluentTag>, bool> merge_p2p(GroundNumericEffectView<OpScaleDown, FluentTag> element,
                                                                                    MergeContext& context);

template std::pair<GroundNumericEffectView<OpIncrease, AuxiliaryTag>, bool> merge_p2p(GroundNumericEffectView<OpIncrease, AuxiliaryTag> element,
                                                                                      MergeContext& context);

template Data<GroundNumericEffectOperator<FluentTag>> merge_p2p(GroundNumericEffectOperatorView<FluentTag> element, MergeContext& context);
template Data<GroundNumericEffectOperator<AuxiliaryTag>> merge_p2p(GroundNumericEffectOperatorView<AuxiliaryTag> element, MergeContext& context);

}

#endif
