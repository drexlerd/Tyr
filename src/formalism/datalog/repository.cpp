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

#include "tyr/formalism/datalog/repository.hpp"

namespace tyr::formalism
{
#ifndef TYR_HEADER_INSTANTIATION

// BasicRelationRepository
template class BasicRelationRepository<Predicate<StaticTag>>;
template class BasicRelationRepository<Predicate<FluentTag>>;
template class BasicRelationRepository<Function<StaticTag>>;
template class BasicRelationRepository<Function<FluentTag>>;
template class BasicRelationRepository<datalog::Rule>;

// BasicSymbolRepository
template class BasicSymbolRepository<Variable>;
template class BasicSymbolRepository<Object>;
template class BasicSymbolRepository<Predicate<StaticTag>>;
template class BasicSymbolRepository<Predicate<FluentTag>>;
template class BasicSymbolRepository<datalog::Atom<StaticTag>>;
template class BasicSymbolRepository<datalog::Atom<FluentTag>>;
template class BasicSymbolRepository<datalog::GroundAtom<StaticTag>>;
template class BasicSymbolRepository<datalog::GroundAtom<FluentTag>>;
template class BasicSymbolRepository<datalog::Literal<StaticTag>>;
template class BasicSymbolRepository<datalog::Literal<FluentTag>>;
template class BasicSymbolRepository<datalog::GroundLiteral<StaticTag>>;
template class BasicSymbolRepository<datalog::GroundLiteral<FluentTag>>;
template class BasicSymbolRepository<Function<StaticTag>>;
template class BasicSymbolRepository<Function<FluentTag>>;
template class BasicSymbolRepository<datalog::FunctionTerm<StaticTag>>;
template class BasicSymbolRepository<datalog::FunctionTerm<FluentTag>>;
template class BasicSymbolRepository<datalog::GroundFunctionTerm<StaticTag>>;
template class BasicSymbolRepository<datalog::GroundFunctionTerm<FluentTag>>;
template class BasicSymbolRepository<datalog::GroundFunctionTermValue<StaticTag>>;
template class BasicSymbolRepository<datalog::GroundFunctionTermValue<FluentTag>>;
template class BasicSymbolRepository<datalog::UnaryOperator<OpSub, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<OpAdd, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<OpSub, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<OpMul, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<OpDiv, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::MultiOperator<OpAdd, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::MultiOperator<OpMul, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<OpEq, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<OpNe, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<OpLe, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<OpLt, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<OpGe, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<OpGt, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::UnaryOperator<OpSub, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<OpAdd, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<OpSub, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<OpMul, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<OpDiv, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::MultiOperator<OpAdd, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::MultiOperator<OpMul, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<OpEq, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<OpNe, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<OpLe, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<OpLt, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<OpGe, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<OpGt, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::ConjunctiveCondition>;
template class BasicSymbolRepository<datalog::Rule>;
template class BasicSymbolRepository<datalog::GroundConjunctiveCondition>;
template class BasicSymbolRepository<datalog::GroundRule>;
template class BasicSymbolRepository<datalog::Program>;

// Outer repository
template class Repository<tyr::formalism::datalog::SymbolRepository, tyr::formalism::datalog::RelationRepository>;

#endif

namespace datalog
{
static_assert(RepositoryConcept<Repository>);
static_assert(Context<Repository>);
}
}
