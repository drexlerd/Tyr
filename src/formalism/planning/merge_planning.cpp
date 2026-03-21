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

#include "tyr/formalism/planning/merge_planning.hpp"

#ifndef TYR_HEADER_INSTANTIATION
#include "tyr/formalism/planning/merge_planning.ipp"

namespace tyr::formalism::planning
{

template std::pair<PredicateView<StaticTag>, bool> merge_d2p(formalism::datalog::PredicateView<StaticTag> element, MergePlanningContext& context);
template std::pair<PredicateView<FluentTag>, bool> merge_d2p(formalism::datalog::PredicateView<FluentTag> element, MergePlanningContext& context);
template std::pair<PredicateView<DerivedTag>, bool> merge_d2p(formalism::datalog::PredicateView<FluentTag> element, MergePlanningContext& context);

template std::pair<AtomView<StaticTag>, bool> merge_d2p(formalism::datalog::AtomView<StaticTag> element, MergePlanningContext& context);
template std::pair<AtomView<FluentTag>, bool> merge_d2p(formalism::datalog::AtomView<FluentTag> element, MergePlanningContext& context);
template std::pair<AtomView<DerivedTag>, bool> merge_d2p(formalism::datalog::AtomView<FluentTag> element, MergePlanningContext& context);

template std::pair<GroundAtomView<StaticTag>, bool> merge_d2p<StaticTag, StaticTag>(formalism::datalog::PredicateBindingView<StaticTag> element,
                                                                                    MergePlanningContext& context);
template std::pair<GroundAtomView<FluentTag>, bool> merge_d2p<FluentTag, FluentTag>(formalism::datalog::PredicateBindingView<FluentTag> element,
                                                                                    MergePlanningContext& context);
template std::pair<GroundAtomView<DerivedTag>, bool> merge_d2p<FluentTag, DerivedTag>(formalism::datalog::PredicateBindingView<FluentTag> element,
                                                                                      MergePlanningContext& context);

template std::pair<PredicateBindingView<StaticTag>, bool>
merge_d2p(PredicateView<StaticTag> predicate, formalism::datalog::PredicateBindingView<StaticTag> element, MergePlanningContext& context);
template std::pair<PredicateBindingView<FluentTag>, bool>
merge_d2p(PredicateView<FluentTag> predicate, formalism::datalog::PredicateBindingView<FluentTag> element, MergePlanningContext& context);
template std::pair<PredicateBindingView<DerivedTag>, bool>
merge_d2p(PredicateView<DerivedTag> predicate, formalism::datalog::PredicateBindingView<FluentTag> element, MergePlanningContext& context);

template std::pair<GroundAtomView<StaticTag>, bool> merge_d2p(formalism::datalog::GroundAtomView<StaticTag> element, MergePlanningContext& context);
template std::pair<GroundAtomView<FluentTag>, bool> merge_d2p(formalism::datalog::GroundAtomView<FluentTag> element, MergePlanningContext& context);
template std::pair<GroundAtomView<DerivedTag>, bool> merge_d2p(formalism::datalog::GroundAtomView<FluentTag> element, MergePlanningContext& context);

template std::pair<LiteralView<StaticTag>, bool> merge_d2p(formalism::datalog::LiteralView<StaticTag> element, MergePlanningContext& context);
template std::pair<LiteralView<FluentTag>, bool> merge_d2p(formalism::datalog::LiteralView<FluentTag> element, MergePlanningContext& context);
template std::pair<LiteralView<DerivedTag>, bool> merge_d2p(formalism::datalog::LiteralView<FluentTag> element, MergePlanningContext& context);

template std::pair<GroundLiteralView<StaticTag>, bool> merge_d2p(formalism::datalog::GroundLiteralView<StaticTag> element, MergePlanningContext& context);
template std::pair<GroundLiteralView<FluentTag>, bool> merge_d2p(formalism::datalog::GroundLiteralView<FluentTag> element, MergePlanningContext& context);
template std::pair<GroundLiteralView<DerivedTag>, bool> merge_d2p(formalism::datalog::GroundLiteralView<FluentTag> element, MergePlanningContext& context);

// Numeric

template std::pair<FunctionView<StaticTag>, bool> merge_d2p(formalism::datalog::FunctionView<StaticTag> element, MergePlanningContext& context);
template std::pair<FunctionView<FluentTag>, bool> merge_d2p(formalism::datalog::FunctionView<FluentTag> element, MergePlanningContext& context);

template std::pair<FunctionTermView<StaticTag>, bool> merge_d2p(formalism::datalog::FunctionTermView<StaticTag> element, MergePlanningContext& context);
template std::pair<FunctionTermView<FluentTag>, bool> merge_d2p(formalism::datalog::FunctionTermView<FluentTag> element, MergePlanningContext& context);

template std::pair<FunctionBindingView<StaticTag>, bool>
merge_d2p(FunctionView<StaticTag> function, formalism::datalog::FunctionBindingView<StaticTag> element, MergePlanningContext& context);
template std::pair<FunctionBindingView<FluentTag>, bool>
merge_d2p(FunctionView<FluentTag> function, formalism::datalog::FunctionBindingView<FluentTag> element, MergePlanningContext& context);

template std::pair<GroundFunctionTermView<StaticTag>, bool> merge_d2p(formalism::datalog::GroundFunctionTermView<StaticTag> element,
                                                                      MergePlanningContext& context);
template std::pair<GroundFunctionTermView<FluentTag>, bool> merge_d2p(formalism::datalog::GroundFunctionTermView<FluentTag> element,
                                                                      MergePlanningContext& context);

template std::pair<GroundFunctionTermValueView<StaticTag>, bool> merge_d2p(formalism::datalog::GroundFunctionTermValueView<StaticTag> element,
                                                                           MergePlanningContext& context);
template std::pair<GroundFunctionTermValueView<FluentTag>, bool> merge_d2p(formalism::datalog::GroundFunctionTermValueView<FluentTag> element,
                                                                           MergePlanningContext& context);

template std::pair<UnaryOperatorView<OpSub, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::UnaryOperatorView<OpSub, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<UnaryOperatorView<OpSub, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::UnaryOperatorView<OpSub, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);

template std::pair<BinaryOperatorView<OpEq, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<OpEq, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<OpNe, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<OpNe, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<OpGe, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<OpGe, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<OpGt, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<OpGt, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<OpLe, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<OpLe, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<OpLt, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<OpLt, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<OpAdd, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<OpAdd, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<OpSub, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<OpSub, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<OpMul, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<OpMul, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<OpDiv, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<OpDiv, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<OpEq, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<OpEq, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<OpNe, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<OpNe, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<OpGe, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<OpGe, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<OpGt, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<OpGt, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<OpLe, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<OpLe, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<OpLt, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<OpLt, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<OpAdd, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<OpAdd, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<OpSub, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<OpSub, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<OpMul, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<OpMul, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<OpDiv, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<OpDiv, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);

template std::pair<MultiOperatorView<OpAdd, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::MultiOperatorView<OpAdd, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<MultiOperatorView<OpMul, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::MultiOperatorView<OpMul, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<MultiOperatorView<OpAdd, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::MultiOperatorView<OpAdd, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);
template std::pair<MultiOperatorView<OpMul, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::MultiOperatorView<OpMul, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);

template Data<ArithmeticOperator<Data<FunctionExpression>>>
merge_d2p(formalism::datalog::ArithmeticOperatorView<Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template Data<ArithmeticOperator<Data<GroundFunctionExpression>>>
merge_d2p(formalism::datalog::ArithmeticOperatorView<Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);

template Data<BooleanOperator<Data<FunctionExpression>>>
merge_d2p(formalism::datalog::BooleanOperatorView<Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template Data<BooleanOperator<Data<GroundFunctionExpression>>>
merge_d2p(formalism::datalog::BooleanOperatorView<Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);

}

#endif
