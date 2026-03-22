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

#ifndef TYR_FORMALISM_PLANNING_GROUNDER_HPP_
#define TYR_FORMALISM_PLANNING_GROUNDER_HPP_

#include "tyr/analysis/domains.hpp"
#include "tyr/common/itertools.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/planning/builder.hpp"
#include "tyr/formalism/planning/canonicalization.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/fdr_context.hpp"
#include "tyr/formalism/planning/formatter.hpp"
#include "tyr/formalism/planning/grounder_decl.hpp"
#include "tyr/formalism/planning/merge.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"

namespace tyr::formalism::planning
{

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
std::pair<GroundAtomView<T>, bool> ground(AtomView<T> element, GrounderContext& context);

Data<FDRFact<FluentTag>> ground(AtomView<FluentTag> element, GrounderContext& context, FDRContext& fdr);

template<FactKind T>
std::pair<GroundLiteralView<T>, bool> ground(LiteralView<T> element, GrounderContext& context);

Data<FDRFact<FluentTag>> ground(LiteralView<FluentTag> element, GrounderContext& context, FDRContext& fdr);

std::pair<GroundConjunctiveConditionView, bool> ground(ConjunctiveConditionView element, GrounderContext& context, FDRContext& fdr);

template<NumericEffectOpKind Op, FactKind T>
std::pair<GroundNumericEffectView<Op, T>, bool> ground(NumericEffectView<Op, T> element, GrounderContext& context);

template<FactKind T>
Data<GroundNumericEffectOperator<T>> ground(NumericEffectOperatorView<T> element, GrounderContext& context);

std::pair<GroundConjunctiveEffectView, bool>
ground(ConjunctiveEffectView element, GrounderContext& context, UnorderedMap<Index<FDRVariable<FluentTag>>, FDRValue>& assign, FDRContext& fdr);

std::pair<GroundConditionalEffectView, bool>
ground(ConditionalEffectView element, GrounderContext& context, UnorderedMap<Index<FDRVariable<FluentTag>>, FDRValue>& assign, FDRContext& fdr);

std::pair<ActionBindingView, bool> ground(ActionView action, GrounderContext& context);

std::pair<GroundActionView, bool> ground(ActionView element,
                                         GrounderContext& context,
                                         const analysis::DomainListListList& cond_effect_domains,
                                         UnorderedMap<Index<FDRVariable<FluentTag>>, FDRValue>& assign,
                                         itertools::cartesian_set::Workspace<Index<formalism::Object>>& iter_workspace,
                                         FDRContext& fdr);

std::pair<AxiomBindingView, bool> ground(AxiomView axiom, GrounderContext& context);

std::pair<GroundAxiomView, bool> ground(AxiomView element, GrounderContext& context, FDRContext& fdr);

}

#ifdef TYR_HEADER_INSTANTIATION
#include "tyr/formalism/planning/grounder.ipp"
#endif

#endif