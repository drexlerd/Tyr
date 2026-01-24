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

#ifndef TYR_FORMALISM_DATALOG_EXPRESSION_PROPERTIES_HPP_
#define TYR_FORMALISM_DATALOG_EXPRESSION_PROPERTIES_HPP_

#include "tyr/formalism/datalog/declarations.hpp"

namespace tyr::formalism::datalog
{
/**
 * Forward declarations
 */

template<FactKind T>
void collect_fterms(float_t element, UnorderedSet<Index<FunctionTerm<T>>>& result);

template<FactKind T1, FactKind T2, Context C>
void collect_fterms(View<Index<FunctionTerm<T1>>, C> element, UnorderedSet<Index<FunctionTerm<T2>>>& result);

template<FactKind T, Context C>
void collect_fterms(View<Data<FunctionExpression>, C> element, UnorderedSet<Index<FunctionTerm<T>>>& result);

template<FactKind T, ArithmeticOpKind O, Context C>
void collect_fterms(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, C> element, UnorderedSet<Index<FunctionTerm<T>>>& result);

template<FactKind T, OpKind O, Context C>
void collect_fterms(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, C> element, UnorderedSet<Index<FunctionTerm<T>>>& result);

template<FactKind T, ArithmeticOpKind O, Context C>
void collect_fterms(View<Index<MultiOperator<O, Data<FunctionExpression>>>, C> element, UnorderedSet<Index<FunctionTerm<T>>>& result);

template<FactKind T, Context C>
void collect_fterms(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, C> element, UnorderedSet<Index<FunctionTerm<T>>>& result);

template<FactKind T, Context C>
void collect_fterms(View<Data<BooleanOperator<Data<FunctionExpression>>>, C> element, UnorderedSet<Index<FunctionTerm<T>>>& result);

/**
 * Implementations
 */

template<FactKind T>
void collect_fterms(float_t element, UnorderedSet<Index<FunctionTerm<T>>>& result)
{
}

template<FactKind T1, FactKind T2, Context C>
void collect_fterms(View<Index<FunctionTerm<T1>>, C> element, UnorderedSet<Index<FunctionTerm<T2>>>& result)
{
    if constexpr (std::same_as<T1, T2>)
    {
        result.insert(element.get_index());
    }
}

template<FactKind T, Context C>
void collect_fterms(View<Data<FunctionExpression>, C> element, UnorderedSet<Index<FunctionTerm<T>>>& result)
{
    visit([&](auto&& arg) { collect_fterms(arg, result); }, element.get_variant());
}

template<FactKind T, ArithmeticOpKind O, Context C>
void collect_fterms(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, C> element, UnorderedSet<Index<FunctionTerm<T>>>& result)
{
    collect_fterms(element.get_arg(), result);
}

template<FactKind T, OpKind O, Context C>
void collect_fterms(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, C> element, UnorderedSet<Index<FunctionTerm<T>>>& result)
{
    collect_fterms(element.get_lhs(), result);
    collect_fterms(element.get_rhs(), result);
}

template<FactKind T, ArithmeticOpKind O, Context C>
void collect_fterms(View<Index<MultiOperator<O, Data<FunctionExpression>>>, C> element, UnorderedSet<Index<FunctionTerm<T>>>& result)
{
    for (const auto& arg : element.get_args())
        collect_fterms(arg, result);
}

template<FactKind T, Context C>
void collect_fterms(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, C> element, UnorderedSet<Index<FunctionTerm<T>>>& result)
{
    visit([&](auto&& arg) { collect_fterms(arg, result); }, element.get_variant());
}

template<FactKind T, Context C>
void collect_fterms(View<Data<BooleanOperator<Data<FunctionExpression>>>, C> element, UnorderedSet<Index<FunctionTerm<T>>>& result)
{
    visit([&](auto&& arg) { collect_fterms(arg, result); }, element.get_variant());
}

template<FactKind T, Context C>
auto collect_fterms(View<Data<BooleanOperator<Data<FunctionExpression>>>, C> element)
{
    auto result = UnorderedSet<Index<FunctionTerm<T>>> {};
    visit([&](auto&& arg) { collect_fterms(arg, result); }, element.get_variant());
    auto result_vec = IndexList<FunctionTerm<T>>(result.begin(), result.end());
    std::sort(result_vec.begin(), result_vec.end());
    return result_vec;
}
}

#endif