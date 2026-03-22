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
 *<
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "tyr/formalism/planning/builder.hpp"

namespace tyr::formalism
{

template class BasicBuilder<Variable>;
template class BasicBuilder<Object>;
template class BasicBuilder<RelationBinding<Predicate<StaticTag>>>;
template class BasicBuilder<RelationBinding<Predicate<FluentTag>>>;
template class BasicBuilder<RelationBinding<Predicate<DerivedTag>>>;
template class BasicBuilder<RelationBinding<Function<StaticTag>>>;
template class BasicBuilder<RelationBinding<Function<FluentTag>>>;
template class BasicBuilder<RelationBinding<Function<AuxiliaryTag>>>;
template class BasicBuilder<RelationBinding<planning::Action>>;
template class BasicBuilder<RelationBinding<planning::Axiom>>;

template class BasicBuilder<Predicate<StaticTag>>;
template class BasicBuilder<Predicate<FluentTag>>;
template class BasicBuilder<Predicate<DerivedTag>>;

template class BasicBuilder<planning::Atom<StaticTag>>;
template class BasicBuilder<planning::Atom<FluentTag>>;
template class BasicBuilder<planning::Atom<DerivedTag>>;

template class BasicBuilder<planning::GroundAtom<StaticTag>>;
template class BasicBuilder<planning::GroundAtom<FluentTag>>;
template class BasicBuilder<planning::GroundAtom<DerivedTag>>;

template class BasicBuilder<planning::Literal<StaticTag>>;
template class BasicBuilder<planning::Literal<FluentTag>>;
template class BasicBuilder<planning::Literal<DerivedTag>>;

template class BasicBuilder<planning::GroundLiteral<StaticTag>>;
template class BasicBuilder<planning::GroundLiteral<FluentTag>>;
template class BasicBuilder<planning::GroundLiteral<DerivedTag>>;

template class BasicBuilder<Function<StaticTag>>;
template class BasicBuilder<Function<FluentTag>>;
template class BasicBuilder<Function<AuxiliaryTag>>;

template class BasicBuilder<planning::FunctionTerm<StaticTag>>;
template class BasicBuilder<planning::FunctionTerm<FluentTag>>;
template class BasicBuilder<planning::FunctionTerm<AuxiliaryTag>>;

template class BasicBuilder<planning::GroundFunctionTerm<StaticTag>>;
template class BasicBuilder<planning::GroundFunctionTerm<FluentTag>>;
template class BasicBuilder<planning::GroundFunctionTerm<AuxiliaryTag>>;

template class BasicBuilder<planning::GroundFunctionTermValue<StaticTag>>;
template class BasicBuilder<planning::GroundFunctionTermValue<FluentTag>>;
template class BasicBuilder<planning::GroundFunctionTermValue<AuxiliaryTag>>;

template class BasicBuilder<planning::UnaryOperator<OpSub, Data<planning::FunctionExpression>>>;

template class BasicBuilder<planning::BinaryOperator<OpAdd, Data<planning::FunctionExpression>>>;
template class BasicBuilder<planning::BinaryOperator<OpSub, Data<planning::FunctionExpression>>>;
template class BasicBuilder<planning::BinaryOperator<OpMul, Data<planning::FunctionExpression>>>;
template class BasicBuilder<planning::BinaryOperator<OpDiv, Data<planning::FunctionExpression>>>;

template class BasicBuilder<planning::MultiOperator<OpAdd, Data<planning::FunctionExpression>>>;
template class BasicBuilder<planning::MultiOperator<OpMul, Data<planning::FunctionExpression>>>;

template class BasicBuilder<planning::BinaryOperator<OpEq, Data<planning::FunctionExpression>>>;
template class BasicBuilder<planning::BinaryOperator<OpNe, Data<planning::FunctionExpression>>>;
template class BasicBuilder<planning::BinaryOperator<OpLe, Data<planning::FunctionExpression>>>;
template class BasicBuilder<planning::BinaryOperator<OpLt, Data<planning::FunctionExpression>>>;
template class BasicBuilder<planning::BinaryOperator<OpGe, Data<planning::FunctionExpression>>>;
template class BasicBuilder<planning::BinaryOperator<OpGt, Data<planning::FunctionExpression>>>;

template class BasicBuilder<planning::UnaryOperator<OpSub, Data<planning::GroundFunctionExpression>>>;

template class BasicBuilder<planning::BinaryOperator<OpAdd, Data<planning::GroundFunctionExpression>>>;
template class BasicBuilder<planning::BinaryOperator<OpSub, Data<planning::GroundFunctionExpression>>>;
template class BasicBuilder<planning::BinaryOperator<OpMul, Data<planning::GroundFunctionExpression>>>;
template class BasicBuilder<planning::BinaryOperator<OpDiv, Data<planning::GroundFunctionExpression>>>;

template class BasicBuilder<planning::MultiOperator<OpAdd, Data<planning::GroundFunctionExpression>>>;
template class BasicBuilder<planning::MultiOperator<OpMul, Data<planning::GroundFunctionExpression>>>;

template class BasicBuilder<planning::BinaryOperator<OpEq, Data<planning::GroundFunctionExpression>>>;
template class BasicBuilder<planning::BinaryOperator<OpNe, Data<planning::GroundFunctionExpression>>>;
template class BasicBuilder<planning::BinaryOperator<OpLe, Data<planning::GroundFunctionExpression>>>;
template class BasicBuilder<planning::BinaryOperator<OpLt, Data<planning::GroundFunctionExpression>>>;
template class BasicBuilder<planning::BinaryOperator<OpGe, Data<planning::GroundFunctionExpression>>>;
template class BasicBuilder<planning::BinaryOperator<OpGt, Data<planning::GroundFunctionExpression>>>;

template class BasicBuilder<planning::NumericEffect<planning::OpAssign, FluentTag>>;
template class BasicBuilder<planning::NumericEffect<planning::OpIncrease, FluentTag>>;
template class BasicBuilder<planning::NumericEffect<planning::OpDecrease, FluentTag>>;
template class BasicBuilder<planning::NumericEffect<planning::OpScaleUp, FluentTag>>;
template class BasicBuilder<planning::NumericEffect<planning::OpScaleDown, FluentTag>>;
template class BasicBuilder<planning::NumericEffect<planning::OpIncrease, AuxiliaryTag>>;

template class BasicBuilder<planning::GroundNumericEffect<planning::OpAssign, FluentTag>>;
template class BasicBuilder<planning::GroundNumericEffect<planning::OpIncrease, FluentTag>>;
template class BasicBuilder<planning::GroundNumericEffect<planning::OpDecrease, FluentTag>>;
template class BasicBuilder<planning::GroundNumericEffect<planning::OpScaleUp, FluentTag>>;
template class BasicBuilder<planning::GroundNumericEffect<planning::OpScaleDown, FluentTag>>;
template class BasicBuilder<planning::GroundNumericEffect<planning::OpIncrease, AuxiliaryTag>>;

template class BasicBuilder<planning::ConditionalEffect>;
template class BasicBuilder<planning::GroundConditionalEffect>;

template class BasicBuilder<planning::ConjunctiveEffect>;
template class BasicBuilder<planning::GroundConjunctiveEffect>;

template class BasicBuilder<planning::Action>;
template class BasicBuilder<planning::GroundAction>;

template class BasicBuilder<planning::Axiom>;
template class BasicBuilder<planning::GroundAxiom>;

template class BasicBuilder<planning::Metric>;
template class BasicBuilder<planning::Domain>;
template class BasicBuilder<planning::Task>;

template class BasicBuilder<planning::FDRVariable<FluentTag>>;
template class BasicBuilder<planning::FDRVariable<DerivedTag>>;

template class BasicBuilder<planning::FDRFact<FluentTag>>;
template class BasicBuilder<planning::FDRFact<DerivedTag>>;

template class BasicBuilder<planning::ConjunctiveCondition>;
template class BasicBuilder<planning::GroundConjunctiveCondition>;

template class BasicBuilder<planning::FDRTask>;

}  // namespace tyr::planning