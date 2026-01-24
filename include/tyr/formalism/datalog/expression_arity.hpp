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

#ifndef TYR_FORMALISM_DATALOG_EXPRESSION_ARITY_HPP_
#define TYR_FORMALISM_DATALOG_EXPRESSION_ARITY_HPP_

#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/views.hpp"

namespace tyr::formalism::datalog
{

/**
 * collect_parameters
 */

inline void collect_parameters(float_t element, UnorderedSet<ParameterIndex>& result);

template<Context C>
void collect_parameters(View<Data<Term>, C> element, UnorderedSet<ParameterIndex>& result);

template<FactKind T, Context C>
void collect_parameters(View<Index<Atom<T>>, C> element, UnorderedSet<ParameterIndex>& result);

template<FactKind T, Context C>
void collect_parameters(View<Index<Literal<T>>, C> element, UnorderedSet<ParameterIndex>& result);

template<FactKind T, Context C>
void collect_parameters(View<Index<FunctionTerm<T>>, C> element, UnorderedSet<ParameterIndex>& result);

template<Context C>
void collect_parameters(View<Data<FunctionExpression>, C> element, UnorderedSet<ParameterIndex>& result);

template<ArithmeticOpKind O, Context C>
void collect_parameters(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, C> element, UnorderedSet<ParameterIndex>& result);

template<OpKind O, Context C>
void collect_parameters(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, C> element, UnorderedSet<ParameterIndex>& result);

template<ArithmeticOpKind O, Context C>
void collect_parameters(View<Index<MultiOperator<O, Data<FunctionExpression>>>, C> element, UnorderedSet<ParameterIndex>& result);

template<Context C>
void collect_parameters(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, C> element, UnorderedSet<ParameterIndex>& result);

template<Context C>
void collect_parameters(View<Data<BooleanOperator<Data<FunctionExpression>>>, C> element, UnorderedSet<ParameterIndex>& result);

template<Context C>
auto collect_parameters(View<Data<BooleanOperator<Data<FunctionExpression>>>, C> element);

/**
 * Implementations
 */

inline void collect_parameters(float_t element, UnorderedSet<ParameterIndex>& result) {}

template<Context C>
void collect_parameters(View<Data<Term>, C> element, UnorderedSet<ParameterIndex>& result)
{
    visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                result.insert(arg);
            else if constexpr (std::is_same_v<Alternative, View<Index<Object>, C>>) {}
            else
                static_assert(dependent_false<Alternative>::value, "Missing case");
        },
        element.get_variant());
}

template<FactKind T, Context C>
void collect_parameters(View<Index<Atom<T>>, C> element, UnorderedSet<ParameterIndex>& result)
{
    for (const auto term : element.get_terms())
        collect_parameters(term, result);
}

template<FactKind T, Context C>
void collect_parameters(View<Index<Literal<T>>, C> element, UnorderedSet<ParameterIndex>& result)
{
    collect_parameters(element.get_atom(), result);
}

template<FactKind T, Context C>
void collect_parameters(View<Index<FunctionTerm<T>>, C> element, UnorderedSet<ParameterIndex>& result)
{
    for (const auto term : element.get_terms())
        collect_parameters(term, result);
}

template<Context C>
void collect_parameters(View<Data<FunctionExpression>, C> element, UnorderedSet<ParameterIndex>& result)
{
    visit([&](auto&& arg) { collect_parameters(arg, result); }, element.get_variant());
}

template<ArithmeticOpKind O, Context C>
void collect_parameters(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, C> element, UnorderedSet<ParameterIndex>& result)
{
    collect_parameters(element.get_arg(), result);
}

template<OpKind O, Context C>
void collect_parameters(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, C> element, UnorderedSet<ParameterIndex>& result)
{
    collect_parameters(element.get_lhs(), result);
    collect_parameters(element.get_rhs(), result);
}

template<ArithmeticOpKind O, Context C>
void collect_parameters(View<Index<MultiOperator<O, Data<FunctionExpression>>>, C> element, UnorderedSet<ParameterIndex>& result)
{
    for (const auto arg : element.get_args())
        collect_parameters(arg, result);
}

template<Context C>
void collect_parameters(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, C> element, UnorderedSet<ParameterIndex>& result)
{
    visit([&](auto&& arg) { collect_parameters(arg, result); }, element.get_variant());
}

template<Context C>
void collect_parameters(View<Data<BooleanOperator<Data<FunctionExpression>>>, C> element, UnorderedSet<ParameterIndex>& result)
{
    visit([&](auto&& arg) { collect_parameters(arg, result); }, element.get_variant());
}

template<FactKind T, Context C>
auto collect_parameters(View<Index<Atom<T>>, C> element)
{
    auto result = UnorderedSet<ParameterIndex> {};
    collect_parameters(element, result);
    return result;
}

template<FactKind T, Context C>
auto collect_parameters(View<Index<Literal<T>>, C> element)
{
    auto result = UnorderedSet<ParameterIndex> {};
    collect_parameters(element, result);
    return result;
}

template<FactKind T, Context C>
auto collect_parameters(View<Index<FunctionTerm<T>>, C> element)
{
    auto result = UnorderedSet<ParameterIndex> {};
    collect_parameters(element, result);
    return result;
}

template<Context C>
auto collect_parameters(View<Data<BooleanOperator<Data<FunctionExpression>>>, C> element)
{
    auto result = UnorderedSet<ParameterIndex> {};
    visit([&](auto&& arg) { collect_parameters(arg, result); }, element.get_variant());
    return result;
}

/**
 * max_fterm_arity
 */

inline size_t max_fterm_arity(float_t element);

template<FactKind T, Context C>
size_t max_fterm_arity(View<Index<FunctionTerm<T>>, C> element);

template<Context C>
size_t max_fterm_arity(View<Data<FunctionExpression>, C> element);

template<ArithmeticOpKind O, Context C>
size_t max_fterm_arity(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, C> element);

template<OpKind O, Context C>
size_t max_fterm_arity(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, C> element);

template<ArithmeticOpKind O, Context C>
size_t max_fterm_arity(View<Index<MultiOperator<O, Data<FunctionExpression>>>, C> element);

template<Context C>
size_t max_fterm_arity(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, C> element);

template<Context C>
size_t max_fterm_arity(View<Data<BooleanOperator<Data<FunctionExpression>>>, C> element);

/**
 * Implementations
 */

inline size_t max_fterm_arity(float_t element) { return 0; }

template<FactKind T, Context C>
size_t max_fterm_arity(View<Index<FunctionTerm<T>>, C> element)
{
    return max_fterm_arity(element.get_function().get_arity());
}

template<Context C>
size_t max_fterm_arity(View<Data<FunctionExpression>, C> element)
{
    return visit([&](auto&& arg) { return max_fterm_arity(arg); }, element.get_variant());
}

template<ArithmeticOpKind O, Context C>
size_t max_fterm_arity(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, C> element)
{
    return max_fterm_arity(element.get_arg());
}

template<OpKind O, Context C>
size_t max_fterm_arity(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, C> element)
{
    return std::max(max_fterm_arity(element.get_lhs()), max_fterm_arity(element.get_rhs()));
}

template<ArithmeticOpKind O, Context C>
size_t max_fterm_arity(View<Index<MultiOperator<O, Data<FunctionExpression>>>, C> element)
{
    const auto child_fexprs = element.get_args();

    return std::accumulate(std::next(child_fexprs.begin()),  // Start from the second expression
                           child_fexprs.end(),
                           max_fterm_arity(child_fexprs.front()),
                           [&](const auto& value, const auto& child_expr) { return std::max(value, max_fterm_arity(child_expr)); });
}

template<Context C>
size_t max_fterm_arity(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, C> element)
{
    return visit([&](auto&& arg) { return max_fterm_arity(arg); }, element.get_variant());
}

template<Context C>
size_t max_fterm_arity(View<Data<BooleanOperator<Data<FunctionExpression>>>, C> element)
{
    return visit([&](auto&& arg) { return max_fterm_arity(arg); }, element.get_variant());
}

/**
 * kpkc_arity
 */

template<FactKind T, Context C>
size_t kpkc_arity(View<Index<Literal<T>>, C> element)
{
    return element.get_atom().get_predicate().get_arity();
}

template<FactKind T, Context C>
size_t kpkc_arity(View<Index<FunctionTerm<T>>, C> element)
{
    return element.get_function().get_arity();
}

template<Context C>
size_t kpkc_arity(View<Data<BooleanOperator<Data<FunctionExpression>>>, C> element)
{
    return std::max(max_fterm_arity(element), collect_parameters(element).size());
}

/**
 * parameter_arity
 */

template<FactKind T, Context C>
size_t parameter_arity(View<Index<Literal<T>>, C> element)
{
    return collect_parameters(element).size();
}

template<Context C>
size_t parameter_arity(View<Data<BooleanOperator<Data<FunctionExpression>>>, C> element)
{
    return collect_parameters(element).size();
}

}

#endif