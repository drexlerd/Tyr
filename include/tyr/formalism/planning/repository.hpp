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

#ifndef TYR_FORMALISM_PLANNING_REPOSITORY_HPP_
#define TYR_FORMALISM_PLANNING_REPOSITORY_HPP_

#include "tyr/buffer/declarations.hpp"
#include "tyr/buffer/indexed_hash_set.hpp"
#include "tyr/buffer/segmented_buffer.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/function_view.hpp"
#include "tyr/formalism/planning/canonicalization.hpp"
#include "tyr/formalism/planning/datas.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/indices.hpp"
#include "tyr/formalism/planning/views.hpp"
#include "tyr/formalism/predicate_view.hpp"
#include "tyr/formalism/relation_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/symbol_repository.hpp"

#include <cassert>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace tyr::formalism::planning
{

using SymbolRepository = tyr::formalism::SymbolRepository<Variable,
                                                          Object,
                                                          Binding,
                                                          Predicate<StaticTag>,
                                                          Predicate<FluentTag>,
                                                          Predicate<DerivedTag>,
                                                          Atom<StaticTag>,
                                                          Atom<FluentTag>,
                                                          Atom<DerivedTag>,
                                                          GroundAtom<StaticTag>,
                                                          GroundAtom<FluentTag>,
                                                          GroundAtom<DerivedTag>,
                                                          Literal<StaticTag>,
                                                          Literal<FluentTag>,
                                                          Literal<DerivedTag>,
                                                          GroundLiteral<StaticTag>,
                                                          GroundLiteral<FluentTag>,
                                                          GroundLiteral<DerivedTag>,
                                                          Function<StaticTag>,
                                                          Function<FluentTag>,
                                                          Function<AuxiliaryTag>,
                                                          FunctionTerm<StaticTag>,
                                                          FunctionTerm<FluentTag>,
                                                          FunctionTerm<AuxiliaryTag>,
                                                          GroundFunctionTerm<StaticTag>,
                                                          GroundFunctionTerm<FluentTag>,
                                                          GroundFunctionTerm<AuxiliaryTag>,
                                                          GroundFunctionTermValue<StaticTag>,
                                                          GroundFunctionTermValue<FluentTag>,
                                                          GroundFunctionTermValue<AuxiliaryTag>,
                                                          UnaryOperator<OpSub, Data<FunctionExpression>>,
                                                          BinaryOperator<OpAdd, Data<FunctionExpression>>,
                                                          BinaryOperator<OpSub, Data<FunctionExpression>>,
                                                          BinaryOperator<OpMul, Data<FunctionExpression>>,
                                                          BinaryOperator<OpDiv, Data<FunctionExpression>>,
                                                          MultiOperator<OpAdd, Data<FunctionExpression>>,
                                                          MultiOperator<OpMul, Data<FunctionExpression>>,
                                                          BinaryOperator<OpEq, Data<FunctionExpression>>,
                                                          BinaryOperator<OpNe, Data<FunctionExpression>>,
                                                          BinaryOperator<OpLe, Data<FunctionExpression>>,
                                                          BinaryOperator<OpLt, Data<FunctionExpression>>,
                                                          BinaryOperator<OpGe, Data<FunctionExpression>>,
                                                          BinaryOperator<OpGt, Data<FunctionExpression>>,
                                                          UnaryOperator<OpSub, Data<GroundFunctionExpression>>,
                                                          BinaryOperator<OpAdd, Data<GroundFunctionExpression>>,
                                                          BinaryOperator<OpSub, Data<GroundFunctionExpression>>,
                                                          BinaryOperator<OpMul, Data<GroundFunctionExpression>>,
                                                          BinaryOperator<OpDiv, Data<GroundFunctionExpression>>,
                                                          MultiOperator<OpAdd, Data<GroundFunctionExpression>>,
                                                          MultiOperator<OpMul, Data<GroundFunctionExpression>>,
                                                          BinaryOperator<OpEq, Data<GroundFunctionExpression>>,
                                                          BinaryOperator<OpNe, Data<GroundFunctionExpression>>,
                                                          BinaryOperator<OpLe, Data<GroundFunctionExpression>>,
                                                          BinaryOperator<OpLt, Data<GroundFunctionExpression>>,
                                                          BinaryOperator<OpGe, Data<GroundFunctionExpression>>,
                                                          BinaryOperator<OpGt, Data<GroundFunctionExpression>>,
                                                          NumericEffect<OpAssign, FluentTag>,
                                                          NumericEffect<OpIncrease, FluentTag>,
                                                          NumericEffect<OpDecrease, FluentTag>,
                                                          NumericEffect<OpScaleUp, FluentTag>,
                                                          NumericEffect<OpScaleDown, FluentTag>,
                                                          NumericEffect<OpIncrease, AuxiliaryTag>,
                                                          GroundNumericEffect<OpAssign, FluentTag>,
                                                          GroundNumericEffect<OpIncrease, FluentTag>,
                                                          GroundNumericEffect<OpDecrease, FluentTag>,
                                                          GroundNumericEffect<OpScaleUp, FluentTag>,
                                                          GroundNumericEffect<OpScaleDown, FluentTag>,
                                                          GroundNumericEffect<OpIncrease, AuxiliaryTag>,
                                                          ConditionalEffect,
                                                          GroundConditionalEffect,
                                                          ConjunctiveEffect,
                                                          GroundConjunctiveEffect,
                                                          Action,
                                                          GroundAction,
                                                          Axiom,
                                                          GroundAxiom,
                                                          Metric,
                                                          Domain,
                                                          Task,
                                                          FDRVariable<FluentTag>,
                                                          FDRVariable<DerivedTag>,
                                                          ConjunctiveCondition,
                                                          GroundConjunctiveCondition,
                                                          FDRTask>;

using RelationRepository = tyr::formalism::RelationRepository<Predicate<StaticTag>,
                                                              Predicate<FluentTag>,
                                                              Predicate<DerivedTag>,
                                                              Function<StaticTag>,
                                                              Function<FluentTag>,
                                                              Function<AuxiliaryTag>,
                                                              Action,
                                                              Axiom>;

using Repository = tyr::formalism::Repository<SymbolRepository, RelationRepository>;

using RepositoryPtr = std::shared_ptr<Repository>;

using ActionView = View<Index<Action>, Repository>;

using ActionListView = View<IndexList<Action>, Repository>;

template<typename T>
using ArithmeticOperatorView = View<Data<ArithmeticOperator<T>>, Repository>;
using LiftedArithmeticOperatorView = View<Data<ArithmeticOperator<Data<FunctionExpression>>>, Repository>;
using GroundArithmeticOperatorView = View<Data<ArithmeticOperator<Data<GroundFunctionExpression>>>, Repository>;

template<typename T>
using ArithmeticOperatorListView = View<DataList<ArithmeticOperator<T>>, Repository>;
using LiftedArithmeticOperatorListView = View<DataList<ArithmeticOperator<Data<FunctionExpression>>>, Repository>;
using GroundArithmeticOperatorListView = View<DataList<ArithmeticOperator<Data<GroundFunctionExpression>>>, Repository>;

template<FactKind T>
using AtomView = View<Index<Atom<T>>, Repository>;

template<FactKind T>
using AtomListView = View<IndexList<Atom<T>>, Repository>;

using AxiomView = View<Index<Axiom>, Repository>;

using AxiomListView = View<IndexList<Axiom>, Repository>;

template<OpKind Op, typename T>
using BinaryOperatorView = View<Index<BinaryOperator<Op, T>>, Repository>;
template<OpKind Op>
using LiftedBinaryOperatorView = View<Index<BinaryOperator<Op, Data<FunctionExpression>>>, Repository>;
template<OpKind Op>
using GroundBinaryOperatorView = View<Index<BinaryOperator<Op, Data<GroundFunctionExpression>>>, Repository>;

template<OpKind Op, typename T>
using BinaryOperatorListView = View<IndexList<BinaryOperator<Op, T>>, Repository>;
template<OpKind Op>
using LiftedBinaryOperatorListView = View<IndexList<BinaryOperator<Op, Data<FunctionExpression>>>, Repository>;
template<OpKind Op>
using GroundBinaryOperatorListView = View<IndexList<BinaryOperator<Op, Data<GroundFunctionExpression>>>, Repository>;

using BindingView = View<Index<Binding>, Repository>;
using BindingListView = View<IndexList<Binding>, Repository>;
template<FactKind T>
using PredicateBindingView = View<std::pair<Index<Predicate<T>>, Index<Binding>>, Repository>;
template<FactKind T>
using FunctionBindingView = View<std::pair<Index<Function<T>>, Index<Binding>>, Repository>;
using ActionBindingView = View<std::pair<Index<Action>, Index<Binding>>, Repository>;
using AxiomBindingView = View<std::pair<Index<Axiom>, Index<Binding>>, Repository>;

template<typename T>
using BooleanOperatorView = View<Data<BooleanOperator<T>>, Repository>;
using LiftedBooleanOperatorView = View<Data<BooleanOperator<Data<FunctionExpression>>>, Repository>;
using GroundBooleanOperatorView = View<Data<BooleanOperator<Data<GroundFunctionExpression>>>, Repository>;

template<typename T>
using BooleanOperatorListView = View<DataList<BooleanOperator<T>>, Repository>;
using LiftedBooleanOperatorListView = View<DataList<BooleanOperator<Data<FunctionExpression>>>, Repository>;
using GroundBooleanOperatorListView = View<DataList<BooleanOperator<Data<GroundFunctionExpression>>>, Repository>;

using ConditionalEffectView = View<Index<ConditionalEffect>, Repository>;

using ConditionalEffectListView = View<IndexList<ConditionalEffect>, Repository>;

using ConjunctiveConditionView = View<Index<ConjunctiveCondition>, Repository>;

using ConjunctiveConditionListView = View<IndexList<ConjunctiveCondition>, Repository>;

using ConjunctiveEffectView = View<Index<ConjunctiveEffect>, Repository>;

using ConjunctiveEffectListView = View<IndexList<ConjunctiveEffect>, Repository>;

using DomainView = View<Index<Domain>, Repository>;

using DomainListView = View<IndexList<Domain>, Repository>;

template<formalism::FactKind T>
using FDRFactView = View<Data<FDRFact<T>>, Repository>;

template<formalism::FactKind T>
using FDRFactListView = View<DataList<FDRFact<T>>, Repository>;

using FDRTaskView = View<Index<FDRTask>, Repository>;

using FDRTaskListView = View<IndexList<FDRTask>, Repository>;

template<formalism::FactKind T>
using FDRVariableView = View<Index<FDRVariable<T>>, Repository>;

template<formalism::FactKind T>
using FDRVariableListView = View<IndexList<FDRVariable<T>>, Repository>;

using FunctionExpressionView = View<Data<FunctionExpression>, Repository>;

using FunctionExpressionListView = View<DataList<FunctionExpression>, Repository>;

template<FactKind T>
using FunctionTermView = View<Index<FunctionTerm<T>>, Repository>;

template<FactKind T>
using FunctionTermListView = View<IndexList<FunctionTerm<T>>, Repository>;

template<FactKind T>
using FunctionView = View<Index<Function<T>>, Repository>;

template<FactKind T>
using FunctionListView = View<IndexList<Function<T>>, Repository>;

using GroundActionView = View<Index<GroundAction>, Repository>;

using GroundActionListView = View<IndexList<GroundAction>, Repository>;

template<FactKind T>
using GroundAtomView = View<Index<GroundAtom<T>>, Repository>;

template<FactKind T>
using GroundAtomListView = View<IndexList<GroundAtom<T>>, Repository>;

using GroundAxiomView = View<Index<GroundAxiom>, Repository>;

using GroundAxiomListView = View<IndexList<GroundAxiom>, Repository>;

using GroundConditionalEffectView = View<Index<GroundConditionalEffect>, Repository>;

using GroundConditionalEffectListView = View<IndexList<GroundConditionalEffect>, Repository>;

using GroundConjunctiveConditionView = View<Index<GroundConjunctiveCondition>, Repository>;

using GroundConjunctiveConditionListView = View<IndexList<GroundConjunctiveCondition>, Repository>;

using GroundConjunctiveEffectView = View<Index<GroundConjunctiveEffect>, Repository>;

using GroundConjunctiveEffectListView = View<IndexList<GroundConjunctiveEffect>, Repository>;

using GroundFunctionExpressionView = View<Data<GroundFunctionExpression>, Repository>;

using GroundFunctionExpressionListView = View<DataList<GroundFunctionExpression>, Repository>;

template<FactKind T>
using GroundFunctionTermValueView = View<Index<GroundFunctionTermValue<T>>, Repository>;

template<FactKind T>
using GroundFunctionTermValueListView = View<IndexList<GroundFunctionTermValue<T>>, Repository>;

template<FactKind T>
using GroundFunctionTermViewValuePair = std::pair<View<Index<GroundFunctionTerm<T>>, Repository>, float_t>;
template<FactKind T>
using GroundFunctionTermViewValuePairList = std::vector<GroundFunctionTermViewValuePair<T>>;

template<FactKind T>
using GroundFunctionTermView = View<Index<GroundFunctionTerm<T>>, Repository>;

template<FactKind T>
using GroundFunctionTermListView = View<IndexList<GroundFunctionTerm<T>>, Repository>;

template<FactKind T>
using GroundLiteralView = View<Index<GroundLiteral<T>>, Repository>;

template<FactKind T>
using GroundLiteralListView = View<IndexList<GroundLiteral<T>>, Repository>;

template<FactKind T>
using GroundNumericEffectOperatorView = View<Data<GroundNumericEffectOperator<T>>, Repository>;

template<FactKind T>
using GroundNumericEffectOperatorListView = View<DataList<GroundNumericEffectOperator<T>>, Repository>;

template<NumericEffectOpKind Op, FactKind T>
using GroundNumericEffectView = View<Index<GroundNumericEffect<Op, T>>, Repository>;

template<NumericEffectOpKind Op, FactKind T>
using GroundNumericEffectListView = View<IndexList<GroundNumericEffect<Op, T>>, Repository>;

template<FactKind T>
using LiteralView = View<Index<Literal<T>>, Repository>;

template<FactKind T>
using LiteralListView = View<IndexList<Literal<T>>, Repository>;

using MetricView = View<Index<Metric>, Repository>;

using MetricListView = View<IndexList<Metric>, Repository>;

template<OpKind Op, typename T>
using MultiOperatorView = View<Index<MultiOperator<Op, T>>, Repository>;
template<OpKind Op>
using LiftedMultiOperatorView = View<Index<MultiOperator<Op, Data<FunctionExpression>>>, Repository>;
template<OpKind Op>
using GroundMultiOperatorView = View<Index<MultiOperator<Op, Data<GroundFunctionExpression>>>, Repository>;

template<OpKind Op, typename T>
using MultiOperatorListView = View<IndexList<MultiOperator<Op, T>>, Repository>;
template<OpKind Op>
using LiftedMultiOperatorListView = View<IndexList<MultiOperator<Op, Data<FunctionExpression>>>, Repository>;
template<OpKind Op>
using GroundMultiOperatorListView = View<IndexList<MultiOperator<Op, Data<GroundFunctionExpression>>>, Repository>;

template<FactKind T>
using NumericEffectOperatorView = View<Data<NumericEffectOperator<T>>, Repository>;

template<FactKind T>
using NumericEffectOperatorListView = View<DataList<NumericEffectOperator<T>>, Repository>;

template<NumericEffectOpKind Op, FactKind T>
using NumericEffectView = View<Index<NumericEffect<Op, T>>, Repository>;

template<NumericEffectOpKind Op, FactKind T>
using NumericEffectListView = View<IndexList<NumericEffect<Op, T>>, Repository>;

using ObjectView = View<Index<Object>, Repository>;

using ObjectListView = View<IndexList<Object>, Repository>;

template<FactKind T>
using PredicateView = View<Index<Predicate<T>>, Repository>;

template<FactKind T>
using PredicateListView = View<IndexList<Predicate<T>>, Repository>;

using TaskView = View<Index<Task>, Repository>;

using TaskListView = View<IndexList<Task>, Repository>;

using TermView = View<Data<Term>, Repository>;

using TermListView = View<DataList<Term>, Repository>;

template<OpKind Op, typename T>
using UnaryOperatorView = View<Index<UnaryOperator<Op, T>>, Repository>;
template<OpKind Op>
using LiftedUnaryOperatorView = View<Index<UnaryOperator<Op, Data<FunctionExpression>>>, Repository>;
template<OpKind Op>
using GroundUnaryOperatorView = View<Index<UnaryOperator<Op, Data<GroundFunctionExpression>>>, Repository>;

template<OpKind Op, typename T>
using UnaryOperatorListView = View<IndexList<UnaryOperator<Op, T>>, Repository>;
template<OpKind Op>
using LiftedUnaryOperatorListView = View<IndexList<UnaryOperator<Op, Data<FunctionExpression>>>, Repository>;
template<OpKind Op>
using GroundUnaryOperatorListView = View<IndexList<UnaryOperator<Op, Data<GroundFunctionExpression>>>, Repository>;

using VariableView = View<Index<Variable>, Repository>;

using VariableListView = View<IndexList<Variable>, Repository>;

}

namespace tyr::formalism
{
/**
 * Explicit instantiations
 */

// BasicRelationRepository
extern template class BasicRelationRepository<Predicate<StaticTag>>;
extern template class BasicRelationRepository<Predicate<FluentTag>>;
extern template class BasicRelationRepository<Predicate<DerivedTag>>;
extern template class BasicRelationRepository<Function<StaticTag>>;
extern template class BasicRelationRepository<Function<FluentTag>>;
extern template class BasicRelationRepository<Function<AuxiliaryTag>>;
extern template class BasicRelationRepository<planning::Action>;
extern template class BasicRelationRepository<planning::Axiom>;

// BasicSymbolRepository
extern template class BasicSymbolRepository<Variable>;
extern template class BasicSymbolRepository<Object>;
extern template class BasicSymbolRepository<Binding>;
extern template class BasicSymbolRepository<Predicate<StaticTag>>;
extern template class BasicSymbolRepository<Predicate<FluentTag>>;
extern template class BasicSymbolRepository<Predicate<DerivedTag>>;
extern template class BasicSymbolRepository<planning::Atom<StaticTag>>;
extern template class BasicSymbolRepository<planning::Atom<FluentTag>>;
extern template class BasicSymbolRepository<planning::Atom<DerivedTag>>;
extern template class BasicSymbolRepository<planning::GroundAtom<StaticTag>>;
extern template class BasicSymbolRepository<planning::GroundAtom<FluentTag>>;
extern template class BasicSymbolRepository<planning::GroundAtom<DerivedTag>>;
extern template class BasicSymbolRepository<planning::Literal<StaticTag>>;
extern template class BasicSymbolRepository<planning::Literal<FluentTag>>;
extern template class BasicSymbolRepository<planning::Literal<DerivedTag>>;
extern template class BasicSymbolRepository<planning::GroundLiteral<StaticTag>>;
extern template class BasicSymbolRepository<planning::GroundLiteral<FluentTag>>;
extern template class BasicSymbolRepository<planning::GroundLiteral<DerivedTag>>;
extern template class BasicSymbolRepository<Function<StaticTag>>;
extern template class BasicSymbolRepository<Function<FluentTag>>;
extern template class BasicSymbolRepository<Function<AuxiliaryTag>>;
extern template class BasicSymbolRepository<planning::FunctionTerm<StaticTag>>;
extern template class BasicSymbolRepository<planning::FunctionTerm<FluentTag>>;
extern template class BasicSymbolRepository<planning::FunctionTerm<AuxiliaryTag>>;
extern template class BasicSymbolRepository<planning::GroundFunctionTerm<StaticTag>>;
extern template class BasicSymbolRepository<planning::GroundFunctionTerm<FluentTag>>;
extern template class BasicSymbolRepository<planning::GroundFunctionTerm<AuxiliaryTag>>;
extern template class BasicSymbolRepository<planning::GroundFunctionTermValue<StaticTag>>;
extern template class BasicSymbolRepository<planning::GroundFunctionTermValue<FluentTag>>;
extern template class BasicSymbolRepository<planning::GroundFunctionTermValue<AuxiliaryTag>>;
extern template class BasicSymbolRepository<planning::UnaryOperator<OpSub, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<OpAdd, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<OpSub, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<OpMul, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<OpDiv, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::MultiOperator<OpAdd, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::MultiOperator<OpMul, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<OpEq, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<OpNe, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<OpLe, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<OpLt, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<OpGe, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<OpGt, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::UnaryOperator<OpSub, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<OpAdd, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<OpSub, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<OpMul, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<OpDiv, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::MultiOperator<OpAdd, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::MultiOperator<OpMul, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<OpEq, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<OpNe, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<OpLe, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<OpLt, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<OpGe, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<OpGt, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::NumericEffect<planning::OpAssign, FluentTag>>;
extern template class BasicSymbolRepository<planning::NumericEffect<planning::OpIncrease, FluentTag>>;
extern template class BasicSymbolRepository<planning::NumericEffect<planning::OpDecrease, FluentTag>>;
extern template class BasicSymbolRepository<planning::NumericEffect<planning::OpScaleUp, FluentTag>>;
extern template class BasicSymbolRepository<planning::NumericEffect<planning::OpScaleDown, FluentTag>>;
extern template class BasicSymbolRepository<planning::NumericEffect<planning::OpIncrease, AuxiliaryTag>>;
extern template class BasicSymbolRepository<planning::GroundNumericEffect<planning::OpAssign, FluentTag>>;
extern template class BasicSymbolRepository<planning::GroundNumericEffect<planning::OpIncrease, FluentTag>>;
extern template class BasicSymbolRepository<planning::GroundNumericEffect<planning::OpDecrease, FluentTag>>;
extern template class BasicSymbolRepository<planning::GroundNumericEffect<planning::OpScaleUp, FluentTag>>;
extern template class BasicSymbolRepository<planning::GroundNumericEffect<planning::OpScaleDown, FluentTag>>;
extern template class BasicSymbolRepository<planning::GroundNumericEffect<planning::OpIncrease, AuxiliaryTag>>;
extern template class BasicSymbolRepository<planning::ConditionalEffect>;
extern template class BasicSymbolRepository<planning::GroundConditionalEffect>;
extern template class BasicSymbolRepository<planning::ConjunctiveEffect>;
extern template class BasicSymbolRepository<planning::GroundConjunctiveEffect>;
extern template class BasicSymbolRepository<planning::Action>;
extern template class BasicSymbolRepository<planning::GroundAction>;
extern template class BasicSymbolRepository<planning::Axiom>;
extern template class BasicSymbolRepository<planning::GroundAxiom>;
extern template class BasicSymbolRepository<planning::Metric>;
extern template class BasicSymbolRepository<planning::Domain>;
extern template class BasicSymbolRepository<planning::Task>;
extern template class BasicSymbolRepository<planning::FDRVariable<FluentTag>>;
extern template class BasicSymbolRepository<planning::FDRVariable<DerivedTag>>;
extern template class BasicSymbolRepository<planning::ConjunctiveCondition>;
extern template class BasicSymbolRepository<planning::GroundConjunctiveCondition>;
extern template class BasicSymbolRepository<planning::FDRTask>;

// Outer repository
extern template class Repository<tyr::formalism::planning::SymbolRepository, tyr::formalism::planning::RelationRepository>;
}

#endif
