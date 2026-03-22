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

#include "tyr/formalism/datalog/builder.hpp"

namespace tyr::formalism
{

template class BasicBuilder<Variable>;
template class BasicBuilder<Object>;
template class BasicBuilder<RelationBinding<Predicate<StaticTag>>>;
template class BasicBuilder<RelationBinding<Predicate<FluentTag>>>;
template class BasicBuilder<RelationBinding<Function<StaticTag>>>;
template class BasicBuilder<RelationBinding<Function<FluentTag>>>;
template class BasicBuilder<RelationBinding<datalog::Rule>>;

template class BasicBuilder<Predicate<StaticTag>>;
template class BasicBuilder<Predicate<FluentTag>>;

template class BasicBuilder<datalog::Atom<StaticTag>>;
template class BasicBuilder<datalog::Atom<FluentTag>>;
template class BasicBuilder<datalog::GroundAtom<StaticTag>>;
template class BasicBuilder<datalog::GroundAtom<FluentTag>>;

template class BasicBuilder<datalog::Literal<StaticTag>>;
template class BasicBuilder<datalog::Literal<FluentTag>>;
template class BasicBuilder<datalog::GroundLiteral<StaticTag>>;
template class BasicBuilder<datalog::GroundLiteral<FluentTag>>;

template class BasicBuilder<Function<StaticTag>>;
template class BasicBuilder<Function<FluentTag>>;

template class BasicBuilder<datalog::FunctionTerm<StaticTag>>;
template class BasicBuilder<datalog::FunctionTerm<FluentTag>>;
template class BasicBuilder<datalog::GroundFunctionTerm<StaticTag>>;
template class BasicBuilder<datalog::GroundFunctionTerm<FluentTag>>;

template class BasicBuilder<datalog::GroundFunctionTermValue<StaticTag>>;
template class BasicBuilder<datalog::GroundFunctionTermValue<FluentTag>>;

template class BasicBuilder<datalog::UnaryOperator<OpSub, Data<datalog::FunctionExpression>>>;

template class BasicBuilder<datalog::BinaryOperator<OpAdd, Data<datalog::FunctionExpression>>>;
template class BasicBuilder<datalog::BinaryOperator<OpSub, Data<datalog::FunctionExpression>>>;
template class BasicBuilder<datalog::BinaryOperator<OpMul, Data<datalog::FunctionExpression>>>;
template class BasicBuilder<datalog::BinaryOperator<OpDiv, Data<datalog::FunctionExpression>>>;

template class BasicBuilder<datalog::MultiOperator<OpAdd, Data<datalog::FunctionExpression>>>;
template class BasicBuilder<datalog::MultiOperator<OpMul, Data<datalog::FunctionExpression>>>;

template class BasicBuilder<datalog::BinaryOperator<OpEq, Data<datalog::FunctionExpression>>>;
template class BasicBuilder<datalog::BinaryOperator<OpNe, Data<datalog::FunctionExpression>>>;
template class BasicBuilder<datalog::BinaryOperator<OpLe, Data<datalog::FunctionExpression>>>;
template class BasicBuilder<datalog::BinaryOperator<OpLt, Data<datalog::FunctionExpression>>>;
template class BasicBuilder<datalog::BinaryOperator<OpGe, Data<datalog::FunctionExpression>>>;
template class BasicBuilder<datalog::BinaryOperator<OpGt, Data<datalog::FunctionExpression>>>;

template class BasicBuilder<datalog::UnaryOperator<OpSub, Data<datalog::GroundFunctionExpression>>>;

template class BasicBuilder<datalog::BinaryOperator<OpAdd, Data<datalog::GroundFunctionExpression>>>;
template class BasicBuilder<datalog::BinaryOperator<OpSub, Data<datalog::GroundFunctionExpression>>>;
template class BasicBuilder<datalog::BinaryOperator<OpMul, Data<datalog::GroundFunctionExpression>>>;
template class BasicBuilder<datalog::BinaryOperator<OpDiv, Data<datalog::GroundFunctionExpression>>>;

template class BasicBuilder<datalog::MultiOperator<OpAdd, Data<datalog::GroundFunctionExpression>>>;
template class BasicBuilder<datalog::MultiOperator<OpMul, Data<datalog::GroundFunctionExpression>>>;

template class BasicBuilder<datalog::BinaryOperator<OpEq, Data<datalog::GroundFunctionExpression>>>;
template class BasicBuilder<datalog::BinaryOperator<OpNe, Data<datalog::GroundFunctionExpression>>>;
template class BasicBuilder<datalog::BinaryOperator<OpLe, Data<datalog::GroundFunctionExpression>>>;
template class BasicBuilder<datalog::BinaryOperator<OpLt, Data<datalog::GroundFunctionExpression>>>;
template class BasicBuilder<datalog::BinaryOperator<OpGe, Data<datalog::GroundFunctionExpression>>>;
template class BasicBuilder<datalog::BinaryOperator<OpGt, Data<datalog::GroundFunctionExpression>>>;

template class BasicBuilder<datalog::ConjunctiveCondition>;
template class BasicBuilder<datalog::Rule>;
template class BasicBuilder<datalog::GroundConjunctiveCondition>;
template class BasicBuilder<datalog::GroundRule>;
template class BasicBuilder<datalog::Program>;

}