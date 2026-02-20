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

#ifndef TYR_FORMALISM_DATALOG_GROUNDER_DATALOG_HPP_
#define TYR_FORMALISM_DATALOG_GROUNDER_DATALOG_HPP_

#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/indices.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/declarations.hpp"

namespace tyr::formalism::datalog
{
struct GrounderContext
{
    Builder& builder;
    Repository& destination;
    IndexList<Object>& binding;
};

struct ConstGrounderContext
{
    Builder& builder;
    const Repository& destination;
    IndexList<Object>& binding;
};

/**
 * ground
 */

extern std::pair<Index<Binding>, bool> ground(View<DataList<Term>, Repository> element, GrounderContext& context);

extern std::pair<Index<Binding>, bool> ground(const IndexList<Object>& element, GrounderContext& context);

template<FactKind T>
extern std::pair<Index<GroundFunctionTerm<T>>, bool> ground(View<Index<FunctionTerm<T>>, Repository> element, GrounderContext& context);

extern Data<GroundFunctionExpression> ground(View<Data<FunctionExpression>, Repository> element, GrounderContext& context);

template<OpKind O>
extern std::pair<Index<UnaryOperator<O, Data<GroundFunctionExpression>>>, bool>
ground(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, Repository> element, GrounderContext& context);

template<OpKind O>
extern std::pair<Index<BinaryOperator<O, Data<GroundFunctionExpression>>>, bool>
ground(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, Repository> element, GrounderContext& context);

template<OpKind O>
extern std::pair<Index<MultiOperator<O, Data<GroundFunctionExpression>>>, bool>
ground(View<Index<MultiOperator<O, Data<FunctionExpression>>>, Repository> element, GrounderContext& context);

extern Data<BooleanOperator<Data<GroundFunctionExpression>>> ground(View<Data<BooleanOperator<Data<FunctionExpression>>>, Repository> element,
                                                                    GrounderContext& context);

extern Data<ArithmeticOperator<Data<GroundFunctionExpression>>> ground(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, Repository> element,
                                                                       GrounderContext& context);

template<FactKind T>
extern std::pair<Index<GroundAtom<T>>, bool> ground(View<Index<Atom<T>>, Repository> element, GrounderContext& context);

template<FactKind T>
extern std::pair<Index<GroundLiteral<T>>, bool> ground(View<Index<Literal<T>>, Repository> element, GrounderContext& context);

extern std::pair<Index<GroundConjunctiveCondition>, bool> ground(View<Index<ConjunctiveCondition>, Repository> element, GrounderContext& context);

extern std::pair<Index<GroundRule>, bool> ground(View<Index<Rule>, Repository> element, GrounderContext& context);

/**
 * ground_into_buffer
 */

template<FactKind T>
extern void ground_into_buffer(View<Index<Atom<T>>, Repository> element, const IndexList<Object>& binding, Data<GroundAtom<T>>& out_atom);

template<FactKind T>
extern void ground_into_buffer(View<Index<FunctionTerm<T>>, Repository> element, const IndexList<Object>& binding, Data<GroundFunctionTerm<T>>& out_fterm);

/**
 * is_ground
 */

extern bool is_ground(View<Data<Term>, Repository> element);

template<FactKind T>
extern bool is_ground(View<Index<FunctionTerm<T>>, Repository> element);

extern bool is_ground(View<Data<FunctionExpression>, Repository> element);

template<OpKind O>
extern bool is_ground(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, Repository> element);

template<OpKind O>
extern bool is_ground(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, Repository> element);

template<OpKind O>
extern bool is_ground(View<Index<MultiOperator<O, Data<FunctionExpression>>>, Repository> element);

extern bool is_ground(View<Data<BooleanOperator<Data<FunctionExpression>>>, Repository> element);

extern bool is_ground(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, Repository> element);

template<FactKind T>
extern bool is_ground(View<Index<Atom<T>>, Repository> element);

template<FactKind T>
extern bool is_ground(View<Index<Literal<T>>, Repository> element);

}

#endif