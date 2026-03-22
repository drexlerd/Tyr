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

#ifndef TYR_HEADER_INSTANTIATION

namespace tyr
{
/**
 * Views
 */

// Views over indices
template class View<Index<formalism::planning::Action>, formalism::planning::Repository>;
template class View<Index<formalism::planning::Axiom>, formalism::planning::Repository>;
template class View<Index<formalism::planning::ConditionalEffect>, formalism::planning::Repository>;
template class View<Index<formalism::planning::ConjunctiveCondition>, formalism::planning::Repository>;
template class View<Index<formalism::planning::ConjunctiveEffect>, formalism::planning::Repository>;
template class View<Index<formalism::planning::Domain>, formalism::planning::Repository>;
template class View<Index<formalism::planning::FDRTask>, formalism::planning::Repository>;
template class View<Index<formalism::planning::GroundAction>, formalism::planning::Repository>;
template class View<Index<formalism::planning::GroundAxiom>, formalism::planning::Repository>;
template class View<Index<formalism::planning::GroundConditionalEffect>, formalism::planning::Repository>;
template class View<Index<formalism::planning::GroundConjunctiveCondition>, formalism::planning::Repository>;
template class View<Index<formalism::planning::GroundConjunctiveEffect>, formalism::planning::Repository>;
template class View<Index<formalism::planning::Metric>, formalism::planning::Repository>;
template class View<Index<formalism::Object>, formalism::planning::Repository>;
template class View<Index<formalism::planning::Task>, formalism::planning::Repository>;
template class View<Index<formalism::Variable>, formalism::planning::Repository>;

// FactKind-dependent index views
template class View<Index<formalism::planning::Atom<formalism::StaticTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::Atom<formalism::FluentTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::Atom<formalism::DerivedTag>>, formalism::planning::Repository>;

template class View<Index<formalism::planning::GroundAtom<formalism::StaticTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::GroundAtom<formalism::FluentTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::GroundAtom<formalism::DerivedTag>>, formalism::planning::Repository>;

template class View<Index<formalism::planning::Literal<formalism::StaticTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::Literal<formalism::FluentTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::Literal<formalism::DerivedTag>>, formalism::planning::Repository>;

template class View<Index<formalism::planning::GroundLiteral<formalism::StaticTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::GroundLiteral<formalism::FluentTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::GroundLiteral<formalism::DerivedTag>>, formalism::planning::Repository>;

template class View<Index<formalism::Function<formalism::StaticTag>>, formalism::planning::Repository>;
template class View<Index<formalism::Function<formalism::FluentTag>>, formalism::planning::Repository>;
template class View<Index<formalism::Function<formalism::AuxiliaryTag>>, formalism::planning::Repository>;

template class View<Index<formalism::planning::FunctionTerm<formalism::StaticTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::FunctionTerm<formalism::FluentTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::FunctionTerm<formalism::AuxiliaryTag>>, formalism::planning::Repository>;

template class View<Index<formalism::planning::GroundFunctionTerm<formalism::StaticTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::GroundFunctionTerm<formalism::AuxiliaryTag>>, formalism::planning::Repository>;

template class View<Index<formalism::planning::GroundFunctionTermValue<formalism::StaticTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::GroundFunctionTermValue<formalism::FluentTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::GroundFunctionTermValue<formalism::AuxiliaryTag>>, formalism::planning::Repository>;

template class View<Index<formalism::Predicate<formalism::StaticTag>>, formalism::planning::Repository>;
template class View<Index<formalism::Predicate<formalism::FluentTag>>, formalism::planning::Repository>;
template class View<Index<formalism::Predicate<formalism::DerivedTag>>, formalism::planning::Repository>;

template class View<Index<formalism::planning::FDRVariable<formalism::FluentTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::FDRVariable<formalism::DerivedTag>>, formalism::planning::Repository>;

// Operator index views: lifted
template class View<Index<formalism::planning::UnaryOperator<formalism::OpSub, Data<formalism::planning::FunctionExpression>>>,
                    formalism::planning::Repository>;

template class View<Index<formalism::planning::BinaryOperator<formalism::OpAdd, Data<formalism::planning::FunctionExpression>>>,
                    formalism::planning::Repository>;
template class View<Index<formalism::planning::BinaryOperator<formalism::OpSub, Data<formalism::planning::FunctionExpression>>>,
                    formalism::planning::Repository>;
template class View<Index<formalism::planning::BinaryOperator<formalism::OpMul, Data<formalism::planning::FunctionExpression>>>,
                    formalism::planning::Repository>;
template class View<Index<formalism::planning::BinaryOperator<formalism::OpDiv, Data<formalism::planning::FunctionExpression>>>,
                    formalism::planning::Repository>;
template class View<Index<formalism::planning::BinaryOperator<formalism::OpEq, Data<formalism::planning::FunctionExpression>>>,
                    formalism::planning::Repository>;
template class View<Index<formalism::planning::BinaryOperator<formalism::OpNe, Data<formalism::planning::FunctionExpression>>>,
                    formalism::planning::Repository>;
template class View<Index<formalism::planning::BinaryOperator<formalism::OpLe, Data<formalism::planning::FunctionExpression>>>,
                    formalism::planning::Repository>;
template class View<Index<formalism::planning::BinaryOperator<formalism::OpLt, Data<formalism::planning::FunctionExpression>>>,
                    formalism::planning::Repository>;
template class View<Index<formalism::planning::BinaryOperator<formalism::OpGe, Data<formalism::planning::FunctionExpression>>>,
                    formalism::planning::Repository>;
template class View<Index<formalism::planning::BinaryOperator<formalism::OpGt, Data<formalism::planning::FunctionExpression>>>,
                    formalism::planning::Repository>;

template class View<Index<formalism::planning::MultiOperator<formalism::OpAdd, Data<formalism::planning::FunctionExpression>>>,
                    formalism::planning::Repository>;
template class View<Index<formalism::planning::MultiOperator<formalism::OpMul, Data<formalism::planning::FunctionExpression>>>,
                    formalism::planning::Repository>;

// Operator index views: grounded
template class View<Index<formalism::planning::UnaryOperator<formalism::OpSub, Data<formalism::planning::GroundFunctionExpression>>>,
                    formalism::planning::Repository>;

template class View<Index<formalism::planning::BinaryOperator<formalism::OpAdd, Data<formalism::planning::GroundFunctionExpression>>>,
                    formalism::planning::Repository>;
template class View<Index<formalism::planning::BinaryOperator<formalism::OpSub, Data<formalism::planning::GroundFunctionExpression>>>,
                    formalism::planning::Repository>;
template class View<Index<formalism::planning::BinaryOperator<formalism::OpMul, Data<formalism::planning::GroundFunctionExpression>>>,
                    formalism::planning::Repository>;
template class View<Index<formalism::planning::BinaryOperator<formalism::OpDiv, Data<formalism::planning::GroundFunctionExpression>>>,
                    formalism::planning::Repository>;
template class View<Index<formalism::planning::BinaryOperator<formalism::OpEq, Data<formalism::planning::GroundFunctionExpression>>>,
                    formalism::planning::Repository>;
template class View<Index<formalism::planning::BinaryOperator<formalism::OpNe, Data<formalism::planning::GroundFunctionExpression>>>,
                    formalism::planning::Repository>;
template class View<Index<formalism::planning::BinaryOperator<formalism::OpLe, Data<formalism::planning::GroundFunctionExpression>>>,
                    formalism::planning::Repository>;
template class View<Index<formalism::planning::BinaryOperator<formalism::OpLt, Data<formalism::planning::GroundFunctionExpression>>>,
                    formalism::planning::Repository>;
template class View<Index<formalism::planning::BinaryOperator<formalism::OpGe, Data<formalism::planning::GroundFunctionExpression>>>,
                    formalism::planning::Repository>;
template class View<Index<formalism::planning::BinaryOperator<formalism::OpGt, Data<formalism::planning::GroundFunctionExpression>>>,
                    formalism::planning::Repository>;

template class View<Index<formalism::planning::MultiOperator<formalism::OpAdd, Data<formalism::planning::GroundFunctionExpression>>>,
                    formalism::planning::Repository>;
template class View<Index<formalism::planning::MultiOperator<formalism::OpMul, Data<formalism::planning::GroundFunctionExpression>>>,
                    formalism::planning::Repository>;

// Numeric effect index views
template class View<Index<formalism::planning::NumericEffect<formalism::planning::OpAssign, formalism::FluentTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::NumericEffect<formalism::planning::OpIncrease, formalism::FluentTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::NumericEffect<formalism::planning::OpDecrease, formalism::FluentTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::NumericEffect<formalism::planning::OpScaleUp, formalism::FluentTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::NumericEffect<formalism::planning::OpScaleDown, formalism::FluentTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::NumericEffect<formalism::planning::OpIncrease, formalism::AuxiliaryTag>>, formalism::planning::Repository>;

template class View<Index<formalism::planning::GroundNumericEffect<formalism::planning::OpAssign, formalism::FluentTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::GroundNumericEffect<formalism::planning::OpIncrease, formalism::FluentTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::GroundNumericEffect<formalism::planning::OpDecrease, formalism::FluentTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::GroundNumericEffect<formalism::planning::OpScaleUp, formalism::FluentTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::GroundNumericEffect<formalism::planning::OpScaleDown, formalism::FluentTag>>, formalism::planning::Repository>;
template class View<Index<formalism::planning::GroundNumericEffect<formalism::planning::OpIncrease, formalism::AuxiliaryTag>>, formalism::planning::Repository>;

// Data views
template class View<Data<formalism::planning::ArithmeticOperator<Data<formalism::planning::FunctionExpression>>>, formalism::planning::Repository>;
template class View<Data<formalism::planning::ArithmeticOperator<Data<formalism::planning::GroundFunctionExpression>>>, formalism::planning::Repository>;
template class View<Data<formalism::planning::BooleanOperator<Data<formalism::planning::FunctionExpression>>>, formalism::planning::Repository>;
template class View<Data<formalism::planning::BooleanOperator<Data<formalism::planning::GroundFunctionExpression>>>, formalism::planning::Repository>;
template class View<Data<formalism::planning::FunctionExpression>, formalism::planning::Repository>;
template class View<Data<formalism::planning::GroundFunctionExpression>, formalism::planning::Repository>;
template class View<Data<formalism::planning::NumericEffectOperator<formalism::FluentTag>>, formalism::planning::Repository>;
template class View<Data<formalism::planning::NumericEffectOperator<formalism::AuxiliaryTag>>, formalism::planning::Repository>;
template class View<Data<formalism::planning::GroundNumericEffectOperator<formalism::FluentTag>>, formalism::planning::Repository>;
template class View<Data<formalism::planning::GroundNumericEffectOperator<formalism::AuxiliaryTag>>, formalism::planning::Repository>;
template class View<Data<formalism::Term>, formalism::planning::Repository>;
template class View<Data<formalism::planning::FDRFact<formalism::FluentTag>>, formalism::planning::Repository>;
template class View<Data<formalism::planning::FDRFact<formalism::DerivedTag>>, formalism::planning::Repository>;

// Pair views
template class View<Index<formalism::RelationBinding<formalism::Predicate<formalism::StaticTag>>>, formalism::planning::Repository>;
template class View<Index<formalism::RelationBinding<formalism::Predicate<formalism::FluentTag>>>, formalism::planning::Repository>;
template class View<Index<formalism::RelationBinding<formalism::Predicate<formalism::DerivedTag>>>, formalism::planning::Repository>;

template class View<Index<formalism::RelationBinding<formalism::Function<formalism::StaticTag>>>, formalism::planning::Repository>;
template class View<Index<formalism::RelationBinding<formalism::Function<formalism::FluentTag>>>, formalism::planning::Repository>;
template class View<Index<formalism::RelationBinding<formalism::Function<formalism::AuxiliaryTag>>>, formalism::planning::Repository>;

template class View<Index<formalism::RelationBinding<formalism::planning::Action>>, formalism::planning::Repository>;
template class View<Index<formalism::RelationBinding<formalism::planning::Axiom>>, formalism::planning::Repository>;
}

#endif
