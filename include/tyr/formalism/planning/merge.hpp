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

#ifndef TYR_FORMALISM_PLANNING_MERGE_HPP_
#define TYR_FORMALISM_PLANNING_MERGE_HPP_

#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/indices.hpp"
#include "tyr/formalism/planning/views.hpp"

namespace tyr::formalism::planning
{

struct MergeContext
{
    Builder& builder;
    Repository& destination;
};

// Common

extern std::pair<Index<Variable>, bool> merge_p2p(View<Index<Variable>, Repository> element, MergeContext& context);

extern std::pair<Index<Object>, bool> merge_p2p(View<Index<Object>, Repository> element, MergeContext& context);

extern std::pair<Index<Binding>, bool> merge_p2p(View<Index<Binding>, Repository> element, MergeContext& context);

extern Data<Term> merge_p2p(View<Data<Term>, Repository> element, MergeContext& context);

// Propositional

template<FactKind T>
extern std::pair<Index<Predicate<T>>, bool> merge_p2p(View<Index<Predicate<T>>, Repository> element, MergeContext& context);

template<FactKind T>
extern std::pair<Index<Atom<T>>, bool> merge_p2p(View<Index<Atom<T>>, Repository> element, MergeContext& context);

template<FactKind T>
extern std::pair<Index<GroundAtom<T>>, bool> merge_p2p(View<Index<GroundAtom<T>>, Repository> element, MergeContext& context);

extern std::pair<Index<FDRVariable<FluentTag>>, bool> merge_p2p(View<Index<FDRVariable<FluentTag>>, Repository> element, MergeContext& context);

extern Data<FDRFact<FluentTag>> merge_p2p(View<Data<FDRFact<FluentTag>>, Repository> element, MergeContext& context);

template<FactKind T>
extern std::pair<Index<Literal<T>>, bool> merge_p2p(View<Index<Literal<T>>, Repository> element, MergeContext& context);

template<FactKind T>
extern std::pair<Index<GroundLiteral<T>>, bool> merge_p2p(View<Index<GroundLiteral<T>>, Repository> element, MergeContext& context);

// Numeric

template<FactKind T>
extern std::pair<Index<Function<T>>, bool> merge_p2p(View<Index<Function<T>>, Repository> element, MergeContext& context);

template<FactKind T>
extern std::pair<Index<FunctionTerm<T>>, bool> merge_p2p(View<Index<FunctionTerm<T>>, Repository> element, MergeContext& context);

template<FactKind T>
extern std::pair<Index<GroundFunctionTerm<T>>, bool> merge_p2p(View<Index<GroundFunctionTerm<T>>, Repository> element, MergeContext& context);

template<FactKind T>
extern std::pair<Index<GroundFunctionTermValue<T>>, bool> merge_p2p(View<Index<GroundFunctionTermValue<T>>, Repository> element, MergeContext& context);

extern Data<FunctionExpression> merge_p2p(View<Data<FunctionExpression>, Repository> element, MergeContext& context);

extern Data<GroundFunctionExpression> merge_p2p(View<Data<GroundFunctionExpression>, Repository> element, MergeContext& context);

template<OpKind O, typename T>
extern std::pair<Index<UnaryOperator<O, T>>, bool> merge_p2p(View<Index<UnaryOperator<O, T>>, Repository> element, MergeContext& context);

template<OpKind O, typename T>
extern std::pair<Index<BinaryOperator<O, T>>, bool> merge_p2p(View<Index<BinaryOperator<O, T>>, Repository> element, MergeContext& context);

template<OpKind O, typename T>
extern std::pair<Index<MultiOperator<O, T>>, bool> merge_p2p(View<Index<MultiOperator<O, T>>, Repository> element, MergeContext& context);

template<typename T>
extern Data<ArithmeticOperator<T>> merge_p2p(View<Data<ArithmeticOperator<T>>, Repository> element, MergeContext& context);

template<typename T>
extern Data<BooleanOperator<T>> merge_p2p(View<Data<BooleanOperator<T>>, Repository> element, MergeContext& context);

template<NumericEffectOpKind O, FactKind T>
extern std::pair<Index<NumericEffect<O, T>>, bool> merge_p2p(View<Index<NumericEffect<O, T>>, Repository> element, MergeContext& context);

template<FactKind T>
extern Data<NumericEffectOperator<T>> merge_p2p(View<Data<NumericEffectOperator<T>>, Repository> element, MergeContext& context);

template<NumericEffectOpKind O, FactKind T>
extern std::pair<Index<GroundNumericEffect<O, T>>, bool> merge_p2p(View<Index<GroundNumericEffect<O, T>>, Repository> element, MergeContext& context);

template<FactKind T>
extern Data<GroundNumericEffectOperator<T>> merge_p2p(View<Data<GroundNumericEffectOperator<T>>, Repository> element, MergeContext& context);

// Composite

extern std::pair<Index<ConjunctiveCondition>, bool> merge_p2p(View<Index<ConjunctiveCondition>, Repository> element, MergeContext& context);

extern std::pair<Index<Axiom>, bool> merge_p2p(View<Index<Axiom>, Repository> element, MergeContext& context);

extern std::pair<Index<Metric>, bool> merge_p2p(View<Index<Metric>, Repository> element, MergeContext& context);

}

#endif