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

#ifndef TYR_FORMALISM_ARITY_HPP_
#define TYR_FORMALISM_ARITY_HPP_

#include "tyr/formalism/views.hpp"

namespace tyr::formalism
{
struct MaxArityResult
{
    size_t constraint = 0;
    size_t fterm = 0;

    friend MaxArityResult max_arity(const MaxArityResult& lhs, const MaxArityResult& rhs) noexcept
    {
        return MaxArityResult { std::max(lhs.constraint, rhs.constraint), std::max(lhs.fterm, rhs.fterm) };
    }

    size_t get_arity() const noexcept { return std::max(constraint, fterm); }
    bool is_nullary() const noexcept { return constraint == 0 && fterm == 0; }
};

inline MaxArityResult max_arity(float_t element);

template<ArithmeticOpKind O, Context C>
MaxArityResult max_arity(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, C> element);

template<OpKind O, Context C>
MaxArityResult max_arity(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, C> element);

template<ArithmeticOpKind O, Context C>
MaxArityResult max_arity(View<Index<MultiOperator<O, Data<FunctionExpression>>>, C> element);

template<FactKind T, Context C>
MaxArityResult max_arity(View<Index<FunctionTerm<T>>, C> element);

template<Context C>
MaxArityResult max_arity(View<Data<FunctionExpression>, C> element);

template<Context C>
MaxArityResult max_arity(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, C> element);

template<Context C>
MaxArityResult max_arity(View<Data<BooleanOperator<Data<FunctionExpression>>>, C> element);

inline MaxArityResult max_arity(float_t element) { return MaxArityResult(); }

template<ArithmeticOpKind O, Context C>
MaxArityResult max_arity(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, C> element)
{
    return max_arity(element.get_arg());
}

template<OpKind O, Context C>
MaxArityResult max_arity(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, C> element)
{
    return max_arity(max_arity(element.get_lhs()), max_arity(element.get_rhs()));
}

template<ArithmeticOpKind O, Context C>
MaxArityResult max_arity(View<Index<MultiOperator<O, Data<FunctionExpression>>>, C> element)
{
    const auto child_fexprs = element.get_args();

    return std::accumulate(std::next(child_fexprs.begin()),  // Start from the second expression
                           child_fexprs.end(),
                           max_arity(child_fexprs.front()),
                           [&](const auto& value, const auto& child_expr) { return max_arity(value, max_arity(child_expr)); });
}

template<FactKind T, Context C>
MaxArityResult max_arity(View<Index<FunctionTerm<T>>, C> element)
{
    return MaxArityResult { 0, element.get_function().get_arity() };
}

template<Context C>
MaxArityResult max_arity(View<Data<FunctionExpression>, C> element)
{
    return visit([&](auto&& arg) { return max_arity(arg); }, element.get_variant());
}

template<Context C>
MaxArityResult max_arity(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, C> element)
{
    return visit([&](auto&& arg) { return max_arity(arg); }, element.get_variant());
}

template<Context C>
MaxArityResult max_arity(View<Data<BooleanOperator<Data<FunctionExpression>>>, C> element)
{
    return visit([&](auto&& arg) { return max_arity(arg); }, element.get_variant());
}
}

#endif