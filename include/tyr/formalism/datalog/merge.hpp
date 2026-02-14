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

#ifndef TYR_FORMALISM_DATALOG_MERGE_HPP_
#define TYR_FORMALISM_DATALOG_MERGE_HPP_

#include "tyr/formalism/datalog/indices.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/indices.hpp"
#include "tyr/formalism/views.hpp"

namespace tyr::formalism::datalog
{

struct MergeContext
{
    Builder& builder;
    Repository& destination;
};

// Common

extern std::pair<Index<Variable>, bool> merge_d2d(View<Index<Variable>, Repository> element, MergeContext& context);

extern std::pair<Index<Object>, bool> merge_d2d(View<Index<Object>, Repository> element, MergeContext& context);

extern std::pair<Index<Binding>, bool> merge_d2d(View<Index<Binding>, Repository> element, MergeContext& context);

extern Data<Term> merge_d2d(View<Data<Term>, Repository> element, MergeContext& context);

// Propositional

template<FactKind T>
extern std::pair<Index<Predicate<T>>, bool> merge_d2d(View<Index<Predicate<T>>, Repository> element, MergeContext& context);

template<FactKind T>
extern std::pair<Index<Atom<T>>, bool> merge_d2d(View<Index<Atom<T>>, Repository> element, MergeContext& context);

template<FactKind T>
extern std::pair<Index<GroundAtom<T>>, bool> merge_d2d(View<Index<GroundAtom<T>>, Repository> element, MergeContext& context);

template<FactKind T>
extern std::pair<Index<Literal<T>>, bool> merge_d2d(View<Index<Literal<T>>, Repository> element, MergeContext& context);

template<FactKind T>
extern std::pair<Index<GroundLiteral<T>>, bool> merge_d2d(View<Index<GroundLiteral<T>>, Repository> element, MergeContext& context);

// Numeric

template<FactKind T>
extern std::pair<Index<Function<T>>, bool> merge_d2d(View<Index<Function<T>>, Repository> element, MergeContext& context);

template<FactKind T>
extern std::pair<Index<FunctionTerm<T>>, bool> merge_d2d(View<Index<FunctionTerm<T>>, Repository> element, MergeContext& context);

template<FactKind T>
extern std::pair<Index<GroundFunctionTerm<T>>, bool> merge_d2d(View<Index<GroundFunctionTerm<T>>, Repository> element, MergeContext& context);

template<FactKind T>
extern std::pair<Index<GroundFunctionTermValue<T>>, bool> merge_d2d(View<Index<GroundFunctionTermValue<T>>, Repository> element, MergeContext& context);

extern Data<FunctionExpression> merge_d2d(View<Data<FunctionExpression>, Repository> element, MergeContext& context);

extern Data<GroundFunctionExpression> merge_d2d(View<Data<GroundFunctionExpression>, Repository> element, MergeContext& context);

template<OpKind O, typename T>
extern std::pair<Index<UnaryOperator<O, T>>, bool> merge_d2d(View<Index<UnaryOperator<O, T>>, Repository> element, MergeContext& context);

template<OpKind O, typename T>
extern std::pair<Index<BinaryOperator<O, T>>, bool> merge_d2d(View<Index<BinaryOperator<O, T>>, Repository> element, MergeContext& context);

template<OpKind O, typename T>
extern std::pair<Index<MultiOperator<O, T>>, bool> merge_d2d(View<Index<MultiOperator<O, T>>, Repository> element, MergeContext& context);

template<typename T>
extern Data<ArithmeticOperator<T>> merge_d2d(View<Data<ArithmeticOperator<T>>, Repository> element, MergeContext& context);

template<typename T>
extern Data<BooleanOperator<T>> merge_d2d(View<Data<BooleanOperator<T>>, Repository> element, MergeContext& context);

extern std::pair<Index<ConjunctiveCondition>, bool> merge_d2d(View<Index<ConjunctiveCondition>, Repository> element, MergeContext& context);

extern std::pair<Index<GroundConjunctiveCondition>, bool> merge_d2d(View<Index<GroundConjunctiveCondition>, Repository> element, MergeContext& context);

extern std::pair<Index<Rule>, bool> merge_d2d(View<Index<Rule>, Repository> element, MergeContext& context);

extern std::pair<Index<GroundRule>, bool> merge_d2d(View<Index<GroundRule>, Repository> element, MergeContext& context);

}

#endif