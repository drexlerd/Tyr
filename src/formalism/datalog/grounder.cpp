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

#include "tyr/formalism/datalog/grounder.hpp"

#ifndef TYR_HEADER_INSTANTIATION
#include "tyr/formalism/datalog/grounder.ipp"

namespace tyr::formalism::datalog
{
template std::pair<FunctionBindingView<StaticTag>, bool> ground(TermListView terms, FunctionView<StaticTag> function, GrounderContext& context);
template std::pair<FunctionBindingView<FluentTag>, bool> ground(TermListView terms, FunctionView<FluentTag> function, GrounderContext& context);

template std::pair<GroundFunctionTermView<StaticTag>, bool> ground(FunctionTermView<StaticTag> element, GrounderContext& context);
template std::pair<GroundFunctionTermView<FluentTag>, bool> ground(FunctionTermView<FluentTag> element, GrounderContext& context);

template std::pair<GroundUnaryOperatorView<OpSub>, bool> ground(LiftedUnaryOperatorView<OpSub> element, GrounderContext& context);

template std::pair<GroundBinaryOperatorView<OpEq>, bool> ground(LiftedBinaryOperatorView<OpEq> element, GrounderContext& context);
template std::pair<GroundBinaryOperatorView<OpNe>, bool> ground(LiftedBinaryOperatorView<OpNe> element, GrounderContext& context);
template std::pair<GroundBinaryOperatorView<OpGe>, bool> ground(LiftedBinaryOperatorView<OpGe> element, GrounderContext& context);
template std::pair<GroundBinaryOperatorView<OpGt>, bool> ground(LiftedBinaryOperatorView<OpGt> element, GrounderContext& context);
template std::pair<GroundBinaryOperatorView<OpLe>, bool> ground(LiftedBinaryOperatorView<OpLe> element, GrounderContext& context);
template std::pair<GroundBinaryOperatorView<OpLt>, bool> ground(LiftedBinaryOperatorView<OpLt> element, GrounderContext& context);
template std::pair<GroundBinaryOperatorView<OpAdd>, bool> ground(LiftedBinaryOperatorView<OpAdd> element, GrounderContext& context);
template std::pair<GroundBinaryOperatorView<OpSub>, bool> ground(LiftedBinaryOperatorView<OpSub> element, GrounderContext& context);
template std::pair<GroundBinaryOperatorView<OpMul>, bool> ground(LiftedBinaryOperatorView<OpMul> element, GrounderContext& context);
template std::pair<GroundBinaryOperatorView<OpDiv>, bool> ground(LiftedBinaryOperatorView<OpDiv> element, GrounderContext& context);

template std::pair<GroundMultiOperatorView<OpAdd>, bool> ground(LiftedMultiOperatorView<OpAdd> element, GrounderContext& context);
template std::pair<GroundMultiOperatorView<OpMul>, bool> ground(LiftedMultiOperatorView<OpMul> element, GrounderContext& context);

template std::pair<PredicateBindingView<StaticTag>, bool> ground(TermListView terms, PredicateView<StaticTag> predicate, GrounderContext& context);
template std::pair<PredicateBindingView<FluentTag>, bool> ground(TermListView terms, PredicateView<FluentTag> predicate, GrounderContext& context);

template std::pair<GroundAtomView<StaticTag>, bool> ground(AtomView<StaticTag> element, GrounderContext& context);
template std::pair<GroundAtomView<FluentTag>, bool> ground(AtomView<FluentTag> element, GrounderContext& context);

template std::pair<GroundLiteralView<StaticTag>, bool> ground(LiteralView<StaticTag> element, GrounderContext& context);
template std::pair<GroundLiteralView<FluentTag>, bool> ground(LiteralView<FluentTag> element, GrounderContext& context);
}

#endif
