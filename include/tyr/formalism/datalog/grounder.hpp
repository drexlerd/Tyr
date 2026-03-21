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

#ifndef TYR_FORMALISM_DATALOG_GROUNDER_HPP_
#define TYR_FORMALISM_DATALOG_GROUNDER_HPP_

#include "tyr/analysis/domains.hpp"
#include "tyr/common/itertools.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/canonicalization.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/datalog/grounder_decl.hpp"
#include "tyr/formalism/datalog/indices.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/declarations.hpp"

namespace tyr::formalism::datalog
{

/**
 * Declarations
 */

std::pair<BindingView, bool> ground(TermListView element, GrounderContext& context);

std::pair<BindingView, bool> ground(const IndexList<Object>& element, GrounderContext& context);

template<FactKind T>
std::pair<FunctionBindingView<T>, bool> ground(TermListView terms, FunctionView<T> function, GrounderContext& context);

template<FactKind T>
std::pair<GroundFunctionTermView<T>, bool> ground(FunctionTermView<T> element, GrounderContext& context);

Data<GroundFunctionExpression> ground(FunctionExpressionView element, GrounderContext& context);

template<OpKind O>
std::pair<GroundUnaryOperatorView<O>, bool> ground(LiftedUnaryOperatorView<O> element, GrounderContext& context);

template<OpKind O>
std::pair<GroundBinaryOperatorView<O>, bool> ground(LiftedBinaryOperatorView<O> element, GrounderContext& context);

template<OpKind O>
std::pair<GroundMultiOperatorView<O>, bool> ground(LiftedMultiOperatorView<O> element, GrounderContext& context);

Data<BooleanOperator<Data<GroundFunctionExpression>>> ground(LiftedBooleanOperatorView element, GrounderContext& context);

Data<ArithmeticOperator<Data<GroundFunctionExpression>>> ground(LiftedArithmeticOperatorView element, GrounderContext& context);

template<FactKind T>
std::pair<PredicateBindingView<T>, bool> ground(TermListView terms, PredicateView<T> predicate, GrounderContext& context);

template<FactKind T>
std::pair<PredicateBindingView<T>, bool> ground_binding(AtomView<T> element, GrounderContext& context);

template<FactKind T>
std::pair<GroundAtomView<T>, bool> ground(AtomView<T> element, GrounderContext& context);

template<FactKind T>
std::pair<GroundLiteralView<T>, bool> ground(LiteralView<T> element, GrounderContext& context);

std::pair<GroundConjunctiveConditionView, bool> ground(ConjunctiveConditionView element, GrounderContext& context);

std::pair<RuleBindingView, bool> ground_binding(RuleView element, GrounderContext& context);

std::pair<GroundRuleView, bool> ground(RuleView element, GrounderContext& context);

}

#ifdef TYR_HEADER_INSTANTIATION
#include "tyr/formalism/datalog/grounder.ipp"
#endif

#endif