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

#ifndef TYR_FORMALISM_DATALOG_REPOSITORY_HPP_
#define TYR_FORMALISM_DATALOG_REPOSITORY_HPP_

#include "tyr/buffer/declarations.hpp"
#include "tyr/buffer/indexed_hash_set.hpp"
#include "tyr/buffer/segmented_buffer.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/datalog/canonicalization.hpp"
#include "tyr/formalism/datalog/datas.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/indices.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/function_view.hpp"
#include "tyr/formalism/predicate_view.hpp"
#include "tyr/formalism/relation_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/repository_factory.hpp"
#include "tyr/formalism/symbol_repository.hpp"

#include <cassert>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace tyr::formalism::datalog
{
using SymbolRepository = tyr::formalism::SymbolRepository<Variable,
                                                          Object,
                                                          Binding,
                                                          Predicate<StaticTag>,
                                                          Predicate<FluentTag>,
                                                          Atom<StaticTag>,
                                                          Atom<FluentTag>,
                                                          GroundAtom<StaticTag>,
                                                          GroundAtom<FluentTag>,
                                                          Literal<StaticTag>,
                                                          Literal<FluentTag>,
                                                          GroundLiteral<StaticTag>,
                                                          GroundLiteral<FluentTag>,
                                                          Function<StaticTag>,
                                                          Function<FluentTag>,
                                                          FunctionTerm<StaticTag>,
                                                          FunctionTerm<FluentTag>,
                                                          GroundFunctionTerm<StaticTag>,
                                                          GroundFunctionTerm<FluentTag>,
                                                          GroundFunctionTermValue<StaticTag>,
                                                          GroundFunctionTermValue<FluentTag>,
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
                                                          ConjunctiveCondition,
                                                          Rule,
                                                          GroundConjunctiveCondition,
                                                          GroundRule,
                                                          Program>;

using RelationRepository = tyr::formalism::RelationRepository<Predicate<StaticTag>, Predicate<FluentTag>, Function<StaticTag>, Function<FluentTag>, Rule>;

using Repository = tyr::formalism::Repository<SymbolRepository, RelationRepository>;

using RepositoryPtr = std::shared_ptr<Repository>;

using RepositoryFactory = tyr::formalism::RepositoryFactory<SymbolRepository, RelationRepository>;

using RepositoryFactoryPtr = std::shared_ptr<RepositoryFactory>;

template<typename T>
using ArithmeticOperatorView = View<Data<ArithmeticOperator<T>>, Repository>;
using LiftedArithmeticOperatorView = View<Data<ArithmeticOperator<Data<FunctionExpression>>>, Repository>;
using GroundArithmeticOperatorView = View<Data<ArithmeticOperator<Data<GroundFunctionExpression>>>, Repository>;

template<typename T>
using ArithmeticOperatorListView = View<DataList<ArithmeticOperator<T>>, Repository>;
using LiftedArithmeticOperatorListView = View<DataList<ArithmeticOperator<Data<FunctionExpression>>>, Repository>;
using GroundArithmeticOperatorListView = View<DataList<ArithmeticOperator<Data<GroundFunctionExpression>>>, Repository>;

template<formalism::FactKind T>
using AtomView = View<Index<Atom<T>>, Repository>;

template<formalism::FactKind T>
using AtomListView = View<IndexList<Atom<T>>, Repository>;

template<formalism::OpKind Op, typename T>
using BinaryOperatorView = View<Index<BinaryOperator<Op, T>>, Repository>;
template<formalism::OpKind Op>
using LiftedBinaryOperatorView = View<Index<BinaryOperator<Op, Data<FunctionExpression>>>, Repository>;
template<formalism::OpKind Op>
using GroundBinaryOperatorView = View<Index<BinaryOperator<Op, Data<GroundFunctionExpression>>>, Repository>;

template<formalism::OpKind Op, typename T>
using BinaryOperatorListView = View<IndexList<BinaryOperator<Op, T>>, Repository>;
template<formalism::OpKind Op>
using LiftedBinaryOperatorListView = View<IndexList<BinaryOperator<Op, Data<FunctionExpression>>>, Repository>;
template<formalism::OpKind Op>
using GroundBinaryOperatorListView = View<IndexList<BinaryOperator<Op, Data<GroundFunctionExpression>>>, Repository>;

using BindingView = View<Index<Binding>, Repository>;
template<FactKind T>
using PredicateBindingView = View<RelationBindingIndex<Predicate<T>>, Repository>;
template<FactKind T>
using FunctionBindingView = View<RelationBindingIndex<Function<T>>, Repository>;
using RuleBindingView = View<RelationBindingIndex<Rule>, Repository>;

template<FactKind T>
using PredicateBindingVecView = View<RelationBindingsForwardRange<Predicate<T>, std::vector<Index<Binding>>>, Repository>;

template<typename T>
using BooleanOperatorView = View<Data<BooleanOperator<T>>, Repository>;
using LiftedBooleanOperatorView = View<Data<BooleanOperator<Data<FunctionExpression>>>, Repository>;
using GroundBooleanOperatorView = View<Data<BooleanOperator<Data<GroundFunctionExpression>>>, Repository>;

template<typename T>
using BooleanOperatorListView = View<DataList<BooleanOperator<T>>, Repository>;
using LiftedBooleanOperatorListView = View<DataList<BooleanOperator<Data<FunctionExpression>>>, Repository>;
using GroundBooleanOperatorListView = View<DataList<BooleanOperator<Data<GroundFunctionExpression>>>, Repository>;

using ConjunctiveConditionView = View<Index<ConjunctiveCondition>, Repository>;

using ConjunctiveConditionListView = View<IndexList<ConjunctiveCondition>, Repository>;

using FunctionExpressionView = View<Data<FunctionExpression>, Repository>;

using FunctionExpressionListView = View<DataList<FunctionExpression>, Repository>;

template<formalism::FactKind T>
using FunctionTermView = View<Index<FunctionTerm<T>>, Repository>;

template<formalism::FactKind T>
using FunctionTermListView = View<IndexList<FunctionTerm<T>>, Repository>;

template<FactKind T>
using FunctionView = View<Index<Function<T>>, Repository>;

template<FactKind T>
using FunctionListView = View<IndexList<Function<T>>, Repository>;

template<formalism::FactKind T>
using GroundAtomView = View<Index<GroundAtom<T>>, Repository>;

template<formalism::FactKind T>
using GroundAtomListView = View<IndexList<GroundAtom<T>>, Repository>;

using GroundConjunctiveConditionView = View<Index<GroundConjunctiveCondition>, Repository>;

using GroundConjunctiveConditionListView = View<IndexList<GroundConjunctiveCondition>, Repository>;

using GroundFunctionExpressionView = View<Data<GroundFunctionExpression>, Repository>;

using GroundFunctionExpressionListView = View<DataList<GroundFunctionExpression>, Repository>;

template<formalism::FactKind T>
using GroundFunctionTermValueView = View<Index<GroundFunctionTermValue<T>>, Repository>;

template<formalism::FactKind T>
using GroundFunctionTermValueListView = View<IndexList<GroundFunctionTermValue<T>>, Repository>;

template<formalism::FactKind T>
using GroundFunctionTermView = View<Index<GroundFunctionTerm<T>>, Repository>;

template<formalism::FactKind T>
using GroundFunctionTermListView = View<IndexList<GroundFunctionTerm<T>>, Repository>;

template<formalism::FactKind T>
using GroundLiteralView = View<Index<GroundLiteral<T>>, Repository>;

template<formalism::FactKind T>
using GroundLiteralListView = View<IndexList<GroundLiteral<T>>, Repository>;

using GroundRuleView = View<Index<GroundRule>, Repository>;

using GroundRuleListView = View<IndexList<GroundRule>, Repository>;

template<formalism::FactKind T>
using LiteralView = View<Index<Literal<T>>, Repository>;

template<formalism::FactKind T>
using LiteralListView = View<IndexList<Literal<T>>, Repository>;

template<formalism::OpKind Op, typename T>
using MultiOperatorView = View<Index<MultiOperator<Op, T>>, Repository>;
template<formalism::OpKind Op>
using LiftedMultiOperatorView = View<Index<MultiOperator<Op, Data<FunctionExpression>>>, Repository>;
template<formalism::OpKind Op>
using GroundMultiOperatorView = View<Index<MultiOperator<Op, Data<GroundFunctionExpression>>>, Repository>;

template<formalism::OpKind Op, typename T>
using MultiOperatorListView = View<IndexList<MultiOperator<Op, T>>, Repository>;
template<formalism::OpKind Op>
using LiftedMultiOperatorListView = View<IndexList<MultiOperator<Op, Data<FunctionExpression>>>, Repository>;
template<formalism::OpKind Op>
using GroundMultiOperatorListView = View<IndexList<MultiOperator<Op, Data<GroundFunctionExpression>>>, Repository>;

using ObjectView = View<Index<Object>, Repository>;

using ObjectListView = View<IndexList<Object>, Repository>;

template<FactKind T>
using PredicateView = View<Index<Predicate<T>>, Repository>;

template<FactKind T>
using PredicateListView = View<IndexList<Predicate<T>>, Repository>;

using ProgramView = View<Index<Program>, Repository>;

using ProgramListView = View<IndexList<Program>, Repository>;

using RuleView = View<Index<Rule>, Repository>;

using RuleListView = View<IndexList<Rule>, Repository>;

using TermView = View<Data<Term>, Repository>;

using TermListView = View<DataList<Term>, Repository>;

template<formalism::OpKind Op, typename T>
using UnaryOperatorView = View<Index<UnaryOperator<Op, T>>, Repository>;
template<formalism::OpKind Op>
using LiftedUnaryOperatorView = View<Index<UnaryOperator<Op, Data<FunctionExpression>>>, Repository>;
template<formalism::OpKind Op>
using GroundUnaryOperatorView = View<Index<UnaryOperator<Op, Data<GroundFunctionExpression>>>, Repository>;

template<formalism::OpKind Op, typename T>
using UnaryOperatorListView = View<IndexList<UnaryOperator<Op, T>>, Repository>;
template<formalism::OpKind Op>
using LiftedUnaryOperatorListView = View<IndexList<UnaryOperator<Op, Data<FunctionExpression>>>, Repository>;
template<formalism::OpKind Op>
using GroundUnaryOperatorListView = View<IndexList<UnaryOperator<Op, Data<GroundFunctionExpression>>>, Repository>;

using VariableView = View<Index<Variable>, Repository>;

using VariableListView = View<IndexList<Variable>, Repository>;

}

namespace tyr::formalism
{

#ifndef TYR_HEADER_INSTANTIATION

/**
 * Explicit instantiations
 */

// BasicRelationRepository
extern template class BasicRelationRepository<Predicate<StaticTag>>;
extern template class BasicRelationRepository<Predicate<FluentTag>>;
extern template class BasicRelationRepository<Function<StaticTag>>;
extern template class BasicRelationRepository<Function<FluentTag>>;
extern template class BasicRelationRepository<datalog::Rule>;

// BasicSymbolRepository
extern template class BasicSymbolRepository<Variable>;
extern template class BasicSymbolRepository<Object>;
extern template class BasicSymbolRepository<Binding>;
extern template class BasicSymbolRepository<Predicate<StaticTag>>;
extern template class BasicSymbolRepository<Predicate<FluentTag>>;
extern template class BasicSymbolRepository<datalog::Atom<StaticTag>>;
extern template class BasicSymbolRepository<datalog::Atom<FluentTag>>;
extern template class BasicSymbolRepository<datalog::GroundAtom<StaticTag>>;
extern template class BasicSymbolRepository<datalog::GroundAtom<FluentTag>>;
extern template class BasicSymbolRepository<datalog::Literal<StaticTag>>;
extern template class BasicSymbolRepository<datalog::Literal<FluentTag>>;
extern template class BasicSymbolRepository<datalog::GroundLiteral<StaticTag>>;
extern template class BasicSymbolRepository<datalog::GroundLiteral<FluentTag>>;
extern template class BasicSymbolRepository<Function<StaticTag>>;
extern template class BasicSymbolRepository<Function<FluentTag>>;
extern template class BasicSymbolRepository<datalog::FunctionTerm<StaticTag>>;
extern template class BasicSymbolRepository<datalog::FunctionTerm<FluentTag>>;
extern template class BasicSymbolRepository<datalog::GroundFunctionTerm<StaticTag>>;
extern template class BasicSymbolRepository<datalog::GroundFunctionTerm<FluentTag>>;
extern template class BasicSymbolRepository<datalog::GroundFunctionTermValue<StaticTag>>;
extern template class BasicSymbolRepository<datalog::GroundFunctionTermValue<FluentTag>>;
extern template class BasicSymbolRepository<datalog::UnaryOperator<OpSub, Data<datalog::FunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::BinaryOperator<OpAdd, Data<datalog::FunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::BinaryOperator<OpSub, Data<datalog::FunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::BinaryOperator<OpMul, Data<datalog::FunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::BinaryOperator<OpDiv, Data<datalog::FunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::MultiOperator<OpAdd, Data<datalog::FunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::MultiOperator<OpMul, Data<datalog::FunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::BinaryOperator<OpEq, Data<datalog::FunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::BinaryOperator<OpNe, Data<datalog::FunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::BinaryOperator<OpLe, Data<datalog::FunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::BinaryOperator<OpLt, Data<datalog::FunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::BinaryOperator<OpGe, Data<datalog::FunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::BinaryOperator<OpGt, Data<datalog::FunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::UnaryOperator<OpSub, Data<datalog::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::BinaryOperator<OpAdd, Data<datalog::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::BinaryOperator<OpSub, Data<datalog::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::BinaryOperator<OpMul, Data<datalog::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::BinaryOperator<OpDiv, Data<datalog::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::MultiOperator<OpAdd, Data<datalog::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::MultiOperator<OpMul, Data<datalog::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::BinaryOperator<OpEq, Data<datalog::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::BinaryOperator<OpNe, Data<datalog::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::BinaryOperator<OpLe, Data<datalog::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::BinaryOperator<OpLt, Data<datalog::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::BinaryOperator<OpGe, Data<datalog::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::BinaryOperator<OpGt, Data<datalog::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<datalog::ConjunctiveCondition>;
extern template class BasicSymbolRepository<datalog::Rule>;
extern template class BasicSymbolRepository<datalog::GroundConjunctiveCondition>;
extern template class BasicSymbolRepository<datalog::GroundRule>;
extern template class BasicSymbolRepository<datalog::Program>;

// Outer repository
extern template class Repository<tyr::formalism::datalog::SymbolRepository, tyr::formalism::datalog::RelationRepository>;
}

namespace tyr
{

/**
 * Views
 */

extern template class View<Index<formalism::Variable>, formalism::datalog::Repository>;
extern template class View<Index<formalism::Object>, formalism::datalog::Repository>;
extern template class View<Index<formalism::Binding>, formalism::datalog::Repository>;

extern template class View<Index<formalism::Predicate<formalism::StaticTag>>, formalism::datalog::Repository>;
extern template class View<Index<formalism::Predicate<formalism::FluentTag>>, formalism::datalog::Repository>;

extern template class View<Index<formalism::datalog::Atom<formalism::StaticTag>>, formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::Atom<formalism::FluentTag>>, formalism::datalog::Repository>;

extern template class View<Index<formalism::datalog::GroundAtom<formalism::StaticTag>>, formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::GroundAtom<formalism::FluentTag>>, formalism::datalog::Repository>;

extern template class View<Index<formalism::datalog::Literal<formalism::StaticTag>>, formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::Literal<formalism::FluentTag>>, formalism::datalog::Repository>;

extern template class View<Index<formalism::datalog::GroundLiteral<formalism::StaticTag>>, formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::GroundLiteral<formalism::FluentTag>>, formalism::datalog::Repository>;

extern template class View<Index<formalism::Function<formalism::StaticTag>>, formalism::datalog::Repository>;
extern template class View<Index<formalism::Function<formalism::FluentTag>>, formalism::datalog::Repository>;

extern template class View<Index<formalism::datalog::FunctionTerm<formalism::StaticTag>>, formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::FunctionTerm<formalism::FluentTag>>, formalism::datalog::Repository>;

extern template class View<Index<formalism::datalog::GroundFunctionTerm<formalism::StaticTag>>, formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::GroundFunctionTerm<formalism::FluentTag>>, formalism::datalog::Repository>;

extern template class View<Index<formalism::datalog::GroundFunctionTermValue<formalism::StaticTag>>, formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::GroundFunctionTermValue<formalism::FluentTag>>, formalism::datalog::Repository>;

extern template class View<Index<formalism::datalog::UnaryOperator<formalism::OpSub, Data<formalism::datalog::FunctionExpression>>>,
                           formalism::datalog::Repository>;

extern template class View<Index<formalism::datalog::BinaryOperator<formalism::OpAdd, Data<formalism::datalog::FunctionExpression>>>,
                           formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::BinaryOperator<formalism::OpSub, Data<formalism::datalog::FunctionExpression>>>,
                           formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::BinaryOperator<formalism::OpMul, Data<formalism::datalog::FunctionExpression>>>,
                           formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::BinaryOperator<formalism::OpDiv, Data<formalism::datalog::FunctionExpression>>>,
                           formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::BinaryOperator<formalism::OpEq, Data<formalism::datalog::FunctionExpression>>>,
                           formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::BinaryOperator<formalism::OpNe, Data<formalism::datalog::FunctionExpression>>>,
                           formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::BinaryOperator<formalism::OpLe, Data<formalism::datalog::FunctionExpression>>>,
                           formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::BinaryOperator<formalism::OpLt, Data<formalism::datalog::FunctionExpression>>>,
                           formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::BinaryOperator<formalism::OpGe, Data<formalism::datalog::FunctionExpression>>>,
                           formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::BinaryOperator<formalism::OpGt, Data<formalism::datalog::FunctionExpression>>>,
                           formalism::datalog::Repository>;

extern template class View<Index<formalism::datalog::MultiOperator<formalism::OpAdd, Data<formalism::datalog::FunctionExpression>>>,
                           formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::MultiOperator<formalism::OpMul, Data<formalism::datalog::FunctionExpression>>>,
                           formalism::datalog::Repository>;

extern template class View<Index<formalism::datalog::UnaryOperator<formalism::OpSub, Data<formalism::datalog::GroundFunctionExpression>>>,
                           formalism::datalog::Repository>;

extern template class View<Index<formalism::datalog::BinaryOperator<formalism::OpAdd, Data<formalism::datalog::GroundFunctionExpression>>>,
                           formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::BinaryOperator<formalism::OpSub, Data<formalism::datalog::GroundFunctionExpression>>>,
                           formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::BinaryOperator<formalism::OpMul, Data<formalism::datalog::GroundFunctionExpression>>>,
                           formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::BinaryOperator<formalism::OpDiv, Data<formalism::datalog::GroundFunctionExpression>>>,
                           formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::BinaryOperator<formalism::OpEq, Data<formalism::datalog::GroundFunctionExpression>>>,
                           formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::BinaryOperator<formalism::OpNe, Data<formalism::datalog::GroundFunctionExpression>>>,
                           formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::BinaryOperator<formalism::OpLe, Data<formalism::datalog::GroundFunctionExpression>>>,
                           formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::BinaryOperator<formalism::OpLt, Data<formalism::datalog::GroundFunctionExpression>>>,
                           formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::BinaryOperator<formalism::OpGe, Data<formalism::datalog::GroundFunctionExpression>>>,
                           formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::BinaryOperator<formalism::OpGt, Data<formalism::datalog::GroundFunctionExpression>>>,
                           formalism::datalog::Repository>;

extern template class View<Index<formalism::datalog::MultiOperator<formalism::OpAdd, Data<formalism::datalog::GroundFunctionExpression>>>,
                           formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::MultiOperator<formalism::OpMul, Data<formalism::datalog::GroundFunctionExpression>>>,
                           formalism::datalog::Repository>;

extern template class View<Data<formalism::datalog::ArithmeticOperator<Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
extern template class View<Data<formalism::datalog::ArithmeticOperator<Data<formalism::datalog::GroundFunctionExpression>>>, formalism::datalog::Repository>;

extern template class View<Data<formalism::datalog::BooleanOperator<Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
extern template class View<Data<formalism::datalog::BooleanOperator<Data<formalism::datalog::GroundFunctionExpression>>>, formalism::datalog::Repository>;

extern template class View<Data<formalism::datalog::FunctionExpression>, formalism::datalog::Repository>;
extern template class View<Data<formalism::datalog::GroundFunctionExpression>, formalism::datalog::Repository>;

extern template class View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::GroundConjunctiveCondition>, formalism::datalog::Repository>;

extern template class View<Index<formalism::datalog::Rule>, formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::GroundRule>, formalism::datalog::Repository>;
extern template class View<Index<formalism::datalog::Program>, formalism::datalog::Repository>;

extern template class View<Data<formalism::Term>, formalism::datalog::Repository>;

extern template class View<formalism::RelationBindingIndex<formalism::Predicate<formalism::StaticTag>>, formalism::datalog::Repository>;
extern template class View<formalism::RelationBindingIndex<formalism::Predicate<formalism::FluentTag>>, formalism::datalog::Repository>;

extern template class View<formalism::RelationBindingIndex<formalism::Function<formalism::StaticTag>>, formalism::datalog::Repository>;
extern template class View<formalism::RelationBindingIndex<formalism::Function<formalism::FluentTag>>, formalism::datalog::Repository>;

extern template class View<formalism::RelationBindingIndex<formalism::datalog::Rule>, formalism::datalog::Repository>;

#endif
}

#endif
