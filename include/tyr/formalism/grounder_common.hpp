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
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/builder.hpp"
#include "tyr/formalism/canonicalization.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/merge_common.hpp"
#include "tyr/formalism/views.hpp"

namespace tyr::formalism
{
template<Context C>
struct GrounderContext
{
    Builder& builder;
    C& destination;
    IndexList<Object>& binding;
};

template<Context C_SRC, Context C_DST>
auto ground_common(View<DataList<Term>, C_SRC> element, GrounderContext<C_DST>& context)
{
    // Fetch and clear
    auto binding_ptr = context.builder.template get_builder<Binding>();
    auto& binding = *binding_ptr;
    binding.clear();

    // Fill data
    for (const auto term : element)
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                    binding.objects.push_back(context.binding[uint_t(arg)]);
                else if constexpr (std::is_same_v<Alternative, View<Index<Object>, C_SRC>>)
                    binding.objects.push_back(arg.get_index());
                else
                    static_assert(dependent_false<Alternative>::value, "Missing case");
            },
            term.get_variant());
    }

    // Canonicalize and Serialize
    canonicalize(binding);
    return context.destination.get_or_create(binding, context.builder.get_buffer());
}

template<Context C>
auto ground_common(const IndexList<Object>& element, GrounderContext<C>& context)
{
    // Fetch and clear
    auto binding_ptr = context.builder.template get_builder<Binding>();
    auto& binding = *binding_ptr;
    binding.clear();

    // Fill data
    binding.objects = element;

    // Canonicalize and Serialize
    canonicalize(binding);
    return context.destination.get_or_create(binding, context.builder.get_buffer());
}

template<FactKind T, Context C_SRC, Context C_DST>
auto ground_common(View<Index<FunctionTerm<T>>, C_SRC> element, GrounderContext<C_DST>& context)
{
    // Fetch and clear
    auto fterm_ptr = context.builder.template get_builder<GroundFunctionTerm<T>>();
    auto& fterm = *fterm_ptr;
    fterm.clear();

    // Fill data
    fterm.function = element.get_function().get_index();
    fterm.binding = ground_common(element.get_terms(), context).first;

    // Canonicalize and Serialize
    canonicalize(fterm);
    return context.destination.get_or_create(fterm, context.builder.get_buffer());
}

template<Context C_SRC, Context C_DST>
auto ground_common(View<Data<FunctionExpression>, C_SRC> element, GrounderContext<C_DST>& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return Data<GroundFunctionExpression>(arg);
            else if constexpr (std::is_same_v<Alternative, View<Data<ArithmeticOperator<Data<FunctionExpression>>>, C_SRC>>)
                return Data<GroundFunctionExpression>(ground_common(arg, context));
            else
                return Data<GroundFunctionExpression>(ground_common(arg, context).first);
        },
        element.get_variant());
}

template<OpKind O, Context C_SRC, Context C_DST>
auto ground_common(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, C_SRC> element, GrounderContext<C_DST>& context)
{
    // Fetch and clear
    auto unary_ptr = context.builder.template get_builder<UnaryOperator<O, Data<GroundFunctionExpression>>>();
    auto& unary = *unary_ptr;
    unary.clear();

    // Fill data
    unary.arg = ground_common(element.get_arg(), context);

    // Canonicalize and Serialize
    canonicalize(unary);
    return context.destination.get_or_create(unary, context.builder.get_buffer());
}

template<OpKind O, Context C_SRC, Context C_DST>
auto ground_common(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, C_SRC> element, GrounderContext<C_DST>& context)
{
    // Fetch and clear
    auto binary_ptr = context.builder.template get_builder<BinaryOperator<O, Data<GroundFunctionExpression>>>();
    auto& binary = *binary_ptr;
    binary.clear();

    // Fill data
    binary.lhs = ground_common(element.get_lhs(), context);
    binary.rhs = ground_common(element.get_rhs(), context);

    // Canonicalize and Serialize
    canonicalize(binary);
    return context.destination.get_or_create(binary, context.builder.get_buffer());
}

template<OpKind O, Context C_SRC, Context C_DST>
auto ground_common(View<Index<MultiOperator<O, Data<FunctionExpression>>>, C_SRC> element, GrounderContext<C_DST>& context)
{
    // Fetch and clear
    auto multi_ptr = context.builder.template get_builder<MultiOperator<O, Data<GroundFunctionExpression>>>();
    auto& multi = *multi_ptr;
    multi.clear();

    // Fill data
    for (const auto arg : element.get_args())
        multi.args.push_back(ground_common(arg, context));

    // Canonicalize and Serialize
    canonicalize(multi);
    return context.destination.get_or_create(multi, context.builder.get_buffer());
}

template<Context C_SRC, Context C_DST>
auto ground_common(View<Data<BooleanOperator<Data<FunctionExpression>>>, C_SRC> element, GrounderContext<C_DST>& context)
{
    return visit([&](auto&& arg) { return Data<BooleanOperator<Data<GroundFunctionExpression>>>(ground_common(arg, context).first, element.get_arity()); },
                 element.get_variant());
}

template<Context C_SRC, Context C_DST>
auto ground_common(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, C_SRC> element, GrounderContext<C_DST>& context)
{
    return visit([&](auto&& arg) { return Data<ArithmeticOperator<Data<GroundFunctionExpression>>>(ground_common(arg, context).first); },
                 element.get_variant());
}
}

#endif