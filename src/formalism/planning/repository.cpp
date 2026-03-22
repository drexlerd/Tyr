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

#include "tyr/formalism/planning/repository.hpp"

namespace tyr::formalism
{
#ifndef TYR_HEADER_INSTANTIATION

// BasicRelationRepository
template class BasicRelationRepository<Predicate<StaticTag>>;
template class BasicRelationRepository<Predicate<FluentTag>>;
template class BasicRelationRepository<Predicate<DerivedTag>>;
template class BasicRelationRepository<Function<StaticTag>>;
template class BasicRelationRepository<Function<FluentTag>>;
template class BasicRelationRepository<Function<AuxiliaryTag>>;
template class BasicRelationRepository<planning::Action>;
template class BasicRelationRepository<planning::Axiom>;

// BasicSymbolRepository
template class BasicSymbolRepository<Variable>;
template class BasicSymbolRepository<Object>;
template class BasicSymbolRepository<Predicate<StaticTag>>;
template class BasicSymbolRepository<Predicate<FluentTag>>;
template class BasicSymbolRepository<Predicate<DerivedTag>>;
template class BasicSymbolRepository<planning::Atom<StaticTag>>;
template class BasicSymbolRepository<planning::Atom<FluentTag>>;
template class BasicSymbolRepository<planning::Atom<DerivedTag>>;
template class BasicSymbolRepository<planning::GroundAtom<StaticTag>>;
template class BasicSymbolRepository<planning::GroundAtom<FluentTag>>;
template class BasicSymbolRepository<planning::GroundAtom<DerivedTag>>;
template class BasicSymbolRepository<planning::Literal<StaticTag>>;
template class BasicSymbolRepository<planning::Literal<FluentTag>>;
template class BasicSymbolRepository<planning::Literal<DerivedTag>>;
template class BasicSymbolRepository<planning::GroundLiteral<StaticTag>>;
template class BasicSymbolRepository<planning::GroundLiteral<FluentTag>>;
template class BasicSymbolRepository<planning::GroundLiteral<DerivedTag>>;
template class BasicSymbolRepository<Function<StaticTag>>;
template class BasicSymbolRepository<Function<FluentTag>>;
template class BasicSymbolRepository<Function<AuxiliaryTag>>;
template class BasicSymbolRepository<planning::FunctionTerm<StaticTag>>;
template class BasicSymbolRepository<planning::FunctionTerm<FluentTag>>;
template class BasicSymbolRepository<planning::FunctionTerm<AuxiliaryTag>>;
template class BasicSymbolRepository<planning::GroundFunctionTerm<StaticTag>>;
template class BasicSymbolRepository<planning::GroundFunctionTerm<FluentTag>>;
template class BasicSymbolRepository<planning::GroundFunctionTerm<AuxiliaryTag>>;
template class BasicSymbolRepository<planning::GroundFunctionTermValue<StaticTag>>;
template class BasicSymbolRepository<planning::GroundFunctionTermValue<FluentTag>>;
template class BasicSymbolRepository<planning::GroundFunctionTermValue<AuxiliaryTag>>;
template class BasicSymbolRepository<planning::UnaryOperator<OpSub, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<OpAdd, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<OpSub, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<OpMul, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<OpDiv, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::MultiOperator<OpAdd, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::MultiOperator<OpMul, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<OpEq, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<OpNe, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<OpLe, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<OpLt, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<OpGe, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<OpGt, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::UnaryOperator<OpSub, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<OpAdd, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<OpSub, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<OpMul, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<OpDiv, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::MultiOperator<OpAdd, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::MultiOperator<OpMul, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<OpEq, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<OpNe, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<OpLe, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<OpLt, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<OpGe, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<OpGt, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::NumericEffect<planning::OpAssign, FluentTag>>;
template class BasicSymbolRepository<planning::NumericEffect<planning::OpIncrease, FluentTag>>;
template class BasicSymbolRepository<planning::NumericEffect<planning::OpDecrease, FluentTag>>;
template class BasicSymbolRepository<planning::NumericEffect<planning::OpScaleUp, FluentTag>>;
template class BasicSymbolRepository<planning::NumericEffect<planning::OpScaleDown, FluentTag>>;
template class BasicSymbolRepository<planning::NumericEffect<planning::OpIncrease, AuxiliaryTag>>;
template class BasicSymbolRepository<planning::GroundNumericEffect<planning::OpAssign, FluentTag>>;
template class BasicSymbolRepository<planning::GroundNumericEffect<planning::OpIncrease, FluentTag>>;
template class BasicSymbolRepository<planning::GroundNumericEffect<planning::OpDecrease, FluentTag>>;
template class BasicSymbolRepository<planning::GroundNumericEffect<planning::OpScaleUp, FluentTag>>;
template class BasicSymbolRepository<planning::GroundNumericEffect<planning::OpScaleDown, FluentTag>>;
template class BasicSymbolRepository<planning::GroundNumericEffect<planning::OpIncrease, AuxiliaryTag>>;
template class BasicSymbolRepository<planning::ConditionalEffect>;
template class BasicSymbolRepository<planning::GroundConditionalEffect>;
template class BasicSymbolRepository<planning::ConjunctiveEffect>;
template class BasicSymbolRepository<planning::GroundConjunctiveEffect>;
template class BasicSymbolRepository<planning::Action>;
template class BasicSymbolRepository<planning::GroundAction>;
template class BasicSymbolRepository<planning::Axiom>;
template class BasicSymbolRepository<planning::GroundAxiom>;
template class BasicSymbolRepository<planning::Metric>;
template class BasicSymbolRepository<planning::Domain>;
template class BasicSymbolRepository<planning::Task>;
template class BasicSymbolRepository<planning::FDRVariable<FluentTag>>;
template class BasicSymbolRepository<planning::FDRVariable<DerivedTag>>;
template class BasicSymbolRepository<planning::ConjunctiveCondition>;
template class BasicSymbolRepository<planning::GroundConjunctiveCondition>;
template class BasicSymbolRepository<planning::FDRTask>;

// Outer repository
template class Repository<tyr::formalism::planning::SymbolRepository, tyr::formalism::planning::RelationRepository>;

#endif

namespace planning
{
static_assert(RepositoryConcept<Repository>);
static_assert(Context<Repository>);
}
}
