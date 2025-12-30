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

#ifndef TYR_FORMALISM_DATALOG_ARITY_HPP_
#define TYR_FORMALISM_DATALOG_ARITY_HPP_

#include "tyr/formalism/datalog/views.hpp"

namespace tyr::formalism::datalog
{
inline size_t effective_arity(float_t element);

template<ArithmeticOpKind O, Context C>
size_t effective_arity(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, C> element);

template<OpKind O, Context C>
size_t effective_arity(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, C> element);

template<ArithmeticOpKind O, Context C>
size_t effective_arity(View<Index<MultiOperator<O, Data<FunctionExpression>>>, C> element);

template<FactKind T, Context C>
size_t effective_arity(View<Index<FunctionTerm<T>>, C> element);

template<Context C>
size_t effective_arity(View<Data<FunctionExpression>, C> element);

template<Context C>
size_t effective_arity(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, C> element);

template<Context C>
size_t effective_arity(View<Data<BooleanOperator<Data<FunctionExpression>>>, C> element);

inline size_t effective_arity(float_t element) { return 0; }

template<ArithmeticOpKind O, Context C>
size_t effective_arity(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, C> element)
{
    return effective_arity(element.get_arg());
}

template<OpKind O, Context C>
size_t effective_arity(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, C> element)
{
    return std::max(effective_arity(element.get_lhs()), effective_arity(element.get_rhs()));
}

template<ArithmeticOpKind O, Context C>
size_t effective_arity(View<Index<MultiOperator<O, Data<FunctionExpression>>>, C> element)
{
    const auto child_fexprs = element.get_args();

    return std::accumulate(std::next(child_fexprs.begin()),  // Start from the second expression
                           child_fexprs.end(),
                           effective_arity(child_fexprs.front()),
                           [&](const auto& value, const auto& child_expr) { return std::max(value, effective_arity(child_expr)); });
}

template<FactKind T, Context C>
size_t effective_arity(View<Index<Function<T>>, C> element)
{
    return element.get_arity();
}

template<FactKind T, Context C>
size_t effective_arity(View<Index<FunctionTerm<T>>, C> element)
{
    return effective_arity(element.get_function());
}

template<Context C>
size_t effective_arity(View<Data<FunctionExpression>, C> element)
{
    return visit([&](auto&& arg) { return effective_arity(arg); }, element.get_variant());
}

template<Context C>
size_t effective_arity(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, C> element)
{
    return visit([&](auto&& arg) { return effective_arity(arg); }, element.get_variant());
}

template<Context C>
size_t effective_arity(View<Data<BooleanOperator<Data<FunctionExpression>>>, C> element)
{
    return visit([&](auto&& arg) { return effective_arity(arg); }, element.get_variant());
}

template<FactKind T, Context C>
size_t effective_arity(View<Index<Predicate<T>>, C> element)
{
    return element.get_arity();
}

template<FactKind T, Context C>
size_t effective_arity(View<Index<Atom<T>>, C> element)
{
    return effective_arity(element.get_predicate());
}

template<FactKind T, Context C>
size_t effective_arity(View<Index<Literal<T>>, C> element)
{
    return effective_arity(element.get_atom());
}

}

#endif