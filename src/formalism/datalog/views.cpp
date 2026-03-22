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

#ifndef TYR_HEADER_INSTANTIATION

namespace tyr
{
/**
 * Views
 */

template class View<Index<formalism::Variable>, formalism::datalog::Repository>;
template class View<Index<formalism::Object>, formalism::datalog::Repository>;

template class View<Index<formalism::Predicate<formalism::StaticTag>>, formalism::datalog::Repository>;
template class View<Index<formalism::Predicate<formalism::FluentTag>>, formalism::datalog::Repository>;

template class View<Index<formalism::datalog::Atom<formalism::StaticTag>>, formalism::datalog::Repository>;
template class View<Index<formalism::datalog::Atom<formalism::FluentTag>>, formalism::datalog::Repository>;

template class View<Index<formalism::datalog::GroundAtom<formalism::StaticTag>>, formalism::datalog::Repository>;
template class View<Index<formalism::datalog::GroundAtom<formalism::FluentTag>>, formalism::datalog::Repository>;

template class View<Index<formalism::datalog::Literal<formalism::StaticTag>>, formalism::datalog::Repository>;
template class View<Index<formalism::datalog::Literal<formalism::FluentTag>>, formalism::datalog::Repository>;

template class View<Index<formalism::datalog::GroundLiteral<formalism::StaticTag>>, formalism::datalog::Repository>;
template class View<Index<formalism::datalog::GroundLiteral<formalism::FluentTag>>, formalism::datalog::Repository>;

template class View<Index<formalism::Function<formalism::StaticTag>>, formalism::datalog::Repository>;
template class View<Index<formalism::Function<formalism::FluentTag>>, formalism::datalog::Repository>;

template class View<Index<formalism::datalog::FunctionTerm<formalism::StaticTag>>, formalism::datalog::Repository>;
template class View<Index<formalism::datalog::FunctionTerm<formalism::FluentTag>>, formalism::datalog::Repository>;

template class View<Index<formalism::datalog::GroundFunctionTerm<formalism::StaticTag>>, formalism::datalog::Repository>;
template class View<Index<formalism::datalog::GroundFunctionTerm<formalism::FluentTag>>, formalism::datalog::Repository>;

template class View<Index<formalism::datalog::GroundFunctionTermValue<formalism::StaticTag>>, formalism::datalog::Repository>;
template class View<Index<formalism::datalog::GroundFunctionTermValue<formalism::FluentTag>>, formalism::datalog::Repository>;

template class View<Index<formalism::datalog::UnaryOperator<formalism::OpSub, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;

template class View<Index<formalism::datalog::BinaryOperator<formalism::OpAdd, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template class View<Index<formalism::datalog::BinaryOperator<formalism::OpSub, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template class View<Index<formalism::datalog::BinaryOperator<formalism::OpMul, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template class View<Index<formalism::datalog::BinaryOperator<formalism::OpDiv, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template class View<Index<formalism::datalog::BinaryOperator<formalism::OpEq, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template class View<Index<formalism::datalog::BinaryOperator<formalism::OpNe, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template class View<Index<formalism::datalog::BinaryOperator<formalism::OpLe, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template class View<Index<formalism::datalog::BinaryOperator<formalism::OpLt, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template class View<Index<formalism::datalog::BinaryOperator<formalism::OpGe, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template class View<Index<formalism::datalog::BinaryOperator<formalism::OpGt, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;

template class View<Index<formalism::datalog::MultiOperator<formalism::OpAdd, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template class View<Index<formalism::datalog::MultiOperator<formalism::OpMul, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;

template class View<Index<formalism::datalog::UnaryOperator<formalism::OpSub, Data<formalism::datalog::GroundFunctionExpression>>>,
                    formalism::datalog::Repository>;

template class View<Index<formalism::datalog::BinaryOperator<formalism::OpAdd, Data<formalism::datalog::GroundFunctionExpression>>>,
                    formalism::datalog::Repository>;
template class View<Index<formalism::datalog::BinaryOperator<formalism::OpSub, Data<formalism::datalog::GroundFunctionExpression>>>,
                    formalism::datalog::Repository>;
template class View<Index<formalism::datalog::BinaryOperator<formalism::OpMul, Data<formalism::datalog::GroundFunctionExpression>>>,
                    formalism::datalog::Repository>;
template class View<Index<formalism::datalog::BinaryOperator<formalism::OpDiv, Data<formalism::datalog::GroundFunctionExpression>>>,
                    formalism::datalog::Repository>;
template class View<Index<formalism::datalog::BinaryOperator<formalism::OpEq, Data<formalism::datalog::GroundFunctionExpression>>>,
                    formalism::datalog::Repository>;
template class View<Index<formalism::datalog::BinaryOperator<formalism::OpNe, Data<formalism::datalog::GroundFunctionExpression>>>,
                    formalism::datalog::Repository>;
template class View<Index<formalism::datalog::BinaryOperator<formalism::OpLe, Data<formalism::datalog::GroundFunctionExpression>>>,
                    formalism::datalog::Repository>;
template class View<Index<formalism::datalog::BinaryOperator<formalism::OpLt, Data<formalism::datalog::GroundFunctionExpression>>>,
                    formalism::datalog::Repository>;
template class View<Index<formalism::datalog::BinaryOperator<formalism::OpGe, Data<formalism::datalog::GroundFunctionExpression>>>,
                    formalism::datalog::Repository>;
template class View<Index<formalism::datalog::BinaryOperator<formalism::OpGt, Data<formalism::datalog::GroundFunctionExpression>>>,
                    formalism::datalog::Repository>;

template class View<Index<formalism::datalog::MultiOperator<formalism::OpAdd, Data<formalism::datalog::GroundFunctionExpression>>>,
                    formalism::datalog::Repository>;
template class View<Index<formalism::datalog::MultiOperator<formalism::OpMul, Data<formalism::datalog::GroundFunctionExpression>>>,
                    formalism::datalog::Repository>;

template class View<Data<formalism::datalog::ArithmeticOperator<Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template class View<Data<formalism::datalog::ArithmeticOperator<Data<formalism::datalog::GroundFunctionExpression>>>, formalism::datalog::Repository>;

template class View<Data<formalism::datalog::BooleanOperator<Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template class View<Data<formalism::datalog::BooleanOperator<Data<formalism::datalog::GroundFunctionExpression>>>, formalism::datalog::Repository>;

template class View<Data<formalism::datalog::FunctionExpression>, formalism::datalog::Repository>;
template class View<Data<formalism::datalog::GroundFunctionExpression>, formalism::datalog::Repository>;

template class View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository>;
template class View<Index<formalism::datalog::GroundConjunctiveCondition>, formalism::datalog::Repository>;

template class View<Index<formalism::datalog::Rule>, formalism::datalog::Repository>;
template class View<Index<formalism::datalog::GroundRule>, formalism::datalog::Repository>;
template class View<Index<formalism::datalog::Program>, formalism::datalog::Repository>;

template class View<Data<formalism::Term>, formalism::datalog::Repository>;

template class View<Index<formalism::RelationBinding<formalism::Predicate<formalism::StaticTag>>>, formalism::datalog::Repository>;
template class View<Index<formalism::RelationBinding<formalism::Predicate<formalism::FluentTag>>>, formalism::datalog::Repository>;

template class View<Index<formalism::RelationBinding<formalism::Function<formalism::StaticTag>>>, formalism::datalog::Repository>;
template class View<Index<formalism::RelationBinding<formalism::Function<formalism::FluentTag>>>, formalism::datalog::Repository>;

template class View<Index<formalism::RelationBinding<formalism::datalog::Rule>>, formalism::datalog::Repository>;

}

#endif
