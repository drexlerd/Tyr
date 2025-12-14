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

#ifndef TYR_FORMALISM_GROUNDER_COMMON_HPP_
#define TYR_FORMALISM_GROUNDER_COMMON_HPP_

#include "tyr/analysis/domains.hpp"
#include "tyr/common/itertools.hpp"
#include "tyr/formalism/builder.hpp"
#include "tyr/formalism/canonicalization.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/views.hpp"

namespace tyr::formalism
{
template<Context C_SRC, Context C_DST>
View<Index<Binding>, C_DST> ground_common(View<DataList<Term>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    // Fetch and clear
    auto result_binding_ptr = builder.template get_builder<Binding>();
    auto& result_binding = *result_binding_ptr;
    result_binding.clear();

    // Fill data
    for (const auto term : element)
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                    result_binding.objects.push_back(binding[uint_t(arg)].get_index());
                else if constexpr (std::is_same_v<Alternative, View<Index<Object>, C_SRC>>)
                    result_binding.objects.push_back(arg.get_index());
                else
                    static_assert(dependent_false<Alternative>::value, "Missing case");
            },
            term.get_variant());
    }

    // Canonicalize and Serialize
    canonicalize(result_binding);
    return destination.get_or_create(result_binding, builder.get_buffer()).first;
}

template<FactKind T, Context C_SRC, Context C_DST>
View<Index<GroundFunctionTerm<T>>, C_DST>
ground_common(View<Index<FunctionTerm<T>>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    // Fetch and clear
    auto fterm_ptr = builder.template get_builder<GroundFunctionTerm<T>>();
    auto& fterm = *fterm_ptr;
    fterm.clear();

    // Fill data
    fterm.function = element.get_function().get_index();
    fterm.binding = ground_common(element.get_terms(), binding, builder, destination).get_index();

    // Canonicalize and Serialize
    canonicalize(fterm);
    return destination.get_or_create(fterm, builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST>
View<Data<GroundFunctionExpression>, C_DST>
ground_common(View<Data<FunctionExpression>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return make_view(Data<GroundFunctionExpression>(arg), destination);
            else if constexpr (std::is_same_v<Alternative, View<Data<ArithmeticOperator<Data<FunctionExpression>>>, C_SRC>>)
            {
                return make_view(Data<GroundFunctionExpression>(ground_common(arg, binding, builder, destination).get_data()), destination);
            }
            else
                return make_view(Data<GroundFunctionExpression>(ground_common(arg, binding, builder, destination).get_index()), destination);
        },
        element.get_variant());
}

template<OpKind O, Context C_SRC, Context C_DST>
View<Index<UnaryOperator<O, Data<GroundFunctionExpression>>>, C_DST> ground_common(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, C_SRC> element,
                                                                                   View<IndexList<Object>, C_DST> binding,
                                                                                   Builder& builder,
                                                                                   C_DST& destination)
{
    // Fetch and clear
    auto unary_ptr = builder.template get_builder<UnaryOperator<O, Data<GroundFunctionExpression>>>();
    auto& unary = *unary_ptr;
    unary.clear();

    // Fill data
    unary.arg = ground_common(element.get_arg(), binding, builder, destination).get_data();

    // Canonicalize and Serialize
    canonicalize(unary);
    return destination.get_or_create(unary, builder.get_buffer()).first;
}

template<OpKind O, Context C_SRC, Context C_DST>
View<Index<BinaryOperator<O, Data<GroundFunctionExpression>>>, C_DST> ground_common(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, C_SRC> element,
                                                                                    View<IndexList<Object>, C_DST> binding,
                                                                                    Builder& builder,
                                                                                    C_DST& destination)
{
    // Fetch and clear
    auto binary_ptr = builder.template get_builder<BinaryOperator<O, Data<GroundFunctionExpression>>>();
    auto& binary = *binary_ptr;
    binary.clear();

    // Fill data
    binary.lhs = ground_common(element.get_lhs(), binding, builder, destination).get_data();
    binary.rhs = ground_common(element.get_rhs(), binding, builder, destination).get_data();

    // Canonicalize and Serialize
    canonicalize(binary);
    return destination.get_or_create(binary, builder.get_buffer()).first;
}

template<OpKind O, Context C_SRC, Context C_DST>
View<Index<MultiOperator<O, Data<GroundFunctionExpression>>>, C_DST> ground_common(View<Index<MultiOperator<O, Data<FunctionExpression>>>, C_SRC> element,
                                                                                   View<IndexList<Object>, C_DST> binding,
                                                                                   Builder& builder,
                                                                                   C_DST& destination)
{
    // Fetch and clear
    auto multi_ptr = builder.template get_builder<MultiOperator<O, Data<GroundFunctionExpression>>>();
    auto& multi = *multi_ptr;
    multi.clear();

    // Fill data
    for (const auto arg : element.get_args())
        multi.args.push_back(ground_common(arg, binding, builder, destination).get_data());

    // Canonicalize and Serialize
    canonicalize(multi);
    return destination.get_or_create(multi, builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST>
View<Data<BooleanOperator<Data<GroundFunctionExpression>>>, C_DST> ground_common(View<Data<BooleanOperator<Data<FunctionExpression>>>, C_SRC> element,
                                                                                 View<IndexList<Object>, C_DST> binding,
                                                                                 Builder& builder,
                                                                                 C_DST& destination)
{
    return visit(
        [&](auto&& arg)
        {
            return make_view(
                Data<BooleanOperator<Data<GroundFunctionExpression>>>(ground_common(arg, binding, builder, destination).get_index(), element.get_arity()),
                destination);
        },
        element.get_variant());
}

template<Context C_SRC, Context C_DST>
View<Data<ArithmeticOperator<Data<GroundFunctionExpression>>>, C_DST> ground_common(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, C_SRC> element,
                                                                                    View<IndexList<Object>, C_DST> binding,
                                                                                    Builder& builder,
                                                                                    C_DST& destination)
{
    return visit(
        [&](auto&& arg) {
            return make_view(Data<ArithmeticOperator<Data<GroundFunctionExpression>>>(ground_common(arg, binding, builder, destination).get_index()),
                             destination);
        },
        element.get_variant());
}

}

#endif