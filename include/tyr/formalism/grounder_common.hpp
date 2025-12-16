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
template<Context C_SRC, Context C_DST>
class GrounderCache
{
private:
    template<typename T_SRC, typename T_DST = T_SRC>
    struct MapEntryType
    {
        using value_type = std::pair<T_SRC, T_DST>;
        using container_type = UnorderedMap<std::pair<View<Index<T_SRC>, C_SRC>, View<Index<Binding>, C_DST>>, View<Index<T_DST>, C_DST>>;

        container_type container;
    };

    using GrounderStorage = std::tuple<MapEntryType<Atom<StaticTag>, GroundAtom<StaticTag>>,
                                       MapEntryType<Atom<FluentTag>, GroundAtom<FluentTag>>,
                                       MapEntryType<Atom<DerivedTag>, GroundAtom<DerivedTag>>,
                                       MapEntryType<Atom<FluentTag>, GroundAtom<DerivedTag>>,
                                       MapEntryType<Atom<DerivedTag>, GroundAtom<FluentTag>>,
                                       MapEntryType<Literal<StaticTag>, GroundLiteral<StaticTag>>,
                                       MapEntryType<Literal<FluentTag>, GroundLiteral<FluentTag>>,
                                       MapEntryType<Literal<DerivedTag>, GroundLiteral<DerivedTag>>,
                                       MapEntryType<Literal<FluentTag>, GroundLiteral<DerivedTag>>,
                                       MapEntryType<Literal<DerivedTag>, GroundLiteral<FluentTag>>,
                                       MapEntryType<FunctionTerm<StaticTag>, GroundFunctionTerm<StaticTag>>,
                                       MapEntryType<FunctionTerm<FluentTag>, GroundFunctionTerm<FluentTag>>,
                                       MapEntryType<FunctionTerm<AuxiliaryTag>, GroundFunctionTerm<AuxiliaryTag>>,
                                       MapEntryType<UnaryOperator<OpSub, Data<FunctionExpression>>, UnaryOperator<OpSub, Data<GroundFunctionExpression>>>,
                                       MapEntryType<BinaryOperator<OpAdd, Data<FunctionExpression>>, BinaryOperator<OpAdd, Data<GroundFunctionExpression>>>,
                                       MapEntryType<BinaryOperator<OpSub, Data<FunctionExpression>>, BinaryOperator<OpSub, Data<GroundFunctionExpression>>>,
                                       MapEntryType<BinaryOperator<OpMul, Data<FunctionExpression>>, BinaryOperator<OpMul, Data<GroundFunctionExpression>>>,
                                       MapEntryType<BinaryOperator<OpDiv, Data<FunctionExpression>>, BinaryOperator<OpDiv, Data<GroundFunctionExpression>>>,
                                       MapEntryType<MultiOperator<OpAdd, Data<FunctionExpression>>, MultiOperator<OpAdd, Data<GroundFunctionExpression>>>,
                                       MapEntryType<MultiOperator<OpMul, Data<FunctionExpression>>, MultiOperator<OpMul, Data<GroundFunctionExpression>>>,
                                       MapEntryType<BinaryOperator<OpEq, Data<FunctionExpression>>, BinaryOperator<OpEq, Data<GroundFunctionExpression>>>,
                                       MapEntryType<BinaryOperator<OpNe, Data<FunctionExpression>>, BinaryOperator<OpNe, Data<GroundFunctionExpression>>>,
                                       MapEntryType<BinaryOperator<OpLe, Data<FunctionExpression>>, BinaryOperator<OpLe, Data<GroundFunctionExpression>>>,
                                       MapEntryType<BinaryOperator<OpLt, Data<FunctionExpression>>, BinaryOperator<OpLt, Data<GroundFunctionExpression>>>,
                                       MapEntryType<BinaryOperator<OpGe, Data<FunctionExpression>>, BinaryOperator<OpGe, Data<GroundFunctionExpression>>>,
                                       MapEntryType<BinaryOperator<OpGt, Data<FunctionExpression>>, BinaryOperator<OpGt, Data<GroundFunctionExpression>>>,
                                       MapEntryType<ConjunctiveCondition, GroundConjunctiveCondition>,
                                       MapEntryType<Rule, GroundRule>,
                                       MapEntryType<NumericEffect<OpAssign, FluentTag>, GroundNumericEffect<OpAssign, FluentTag>>,
                                       MapEntryType<NumericEffect<OpIncrease, FluentTag>, GroundNumericEffect<OpIncrease, FluentTag>>,
                                       MapEntryType<NumericEffect<OpDecrease, FluentTag>, GroundNumericEffect<OpDecrease, FluentTag>>,
                                       MapEntryType<NumericEffect<OpScaleUp, FluentTag>, GroundNumericEffect<OpScaleUp, FluentTag>>,
                                       MapEntryType<NumericEffect<OpScaleDown, FluentTag>, GroundNumericEffect<OpScaleDown, FluentTag>>,
                                       MapEntryType<NumericEffect<OpIncrease, AuxiliaryTag>, GroundNumericEffect<OpIncrease, AuxiliaryTag>>,
                                       MapEntryType<FDRConjunctiveCondition, GroundFDRConjunctiveCondition>,
                                       MapEntryType<ConditionalEffect, GroundConditionalEffect>,
                                       MapEntryType<ConjunctiveEffect, GroundConjunctiveEffect>,
                                       MapEntryType<Action, GroundAction>,
                                       MapEntryType<Axiom, GroundAxiom>>;

    GrounderStorage m_maps;

public:
    GrounderCache() = default;

    template<typename T_SRC, typename T_DST = T_SRC>
    auto& get() noexcept
    {
        using Key = std::pair<T_SRC, T_DST>;
        return get_container<Key>(m_maps);
    }
    template<typename T_SRC, typename T_DST = T_SRC>
    const auto& get() const noexcept
    {
        using Key = std::pair<T_SRC, T_DST>;
        return get_container<Key>(m_maps);
    }

    void clear() noexcept
    {
        std::apply([](auto&... slots) { (slots.container.clear(), ...); }, m_maps);
    }
};

template<Context C_SRC, Context C_DST>
struct GrounderContext
{
    Builder& builder;
    C_DST& destination;
    View<Index<Binding>, C_DST> binding;
    GrounderCache<C_SRC, C_DST>& cache;
};

template<typename T_SRC, typename T_DST, Context C_SRC, Context C_DST, typename F>
auto with_cache(View<Index<T_SRC>, C_SRC> element, View<Index<Binding>, C_DST> binding, GrounderCache<C_SRC, C_DST>& cache, F&& compute)
{
    auto& m = cache.template get<T_SRC, T_DST>();

    auto key = std::pair { element, binding };

    if (auto it = m.find(key); it != m.end())
        return it->second;

    auto result = compute();  // compute the merged element

    m.emplace(std::move(key), result);

    return result;
}

template<Context C_SRC, Context C_DST>
View<Index<Binding>, C_DST> ground_common(View<DataList<Term>, C_SRC> element, GrounderContext<C_SRC, C_DST>& context)
{
    // Fetch and clear
    auto result_binding_ptr = context.builder.template get_builder<Binding>();
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
                    result_binding.objects.push_back(context.binding.get_data().objects[uint_t(arg)]);
                else if constexpr (std::is_same_v<Alternative, View<Index<Object>, C_SRC>>)
                    result_binding.objects.push_back(arg.get_index());
                else
                    static_assert(dependent_false<Alternative>::value, "Missing case");
            },
            term.get_variant());
    }

    // Canonicalize and Serialize
    canonicalize(result_binding);
    return context.destination.get_or_create(result_binding, context.builder.get_buffer()).first;
}

template<FactKind T, Context C_SRC, Context C_DST>
View<Index<GroundFunctionTerm<T>>, C_DST> ground_common(View<Index<FunctionTerm<T>>, C_SRC> element, GrounderContext<C_SRC, C_DST>& context)
{
    return with_cache<FunctionTerm<T>, GroundFunctionTerm<T>>(element,
                                                              context.binding,
                                                              context.cache,
                                                              [&]()
                                                              {
                                                                  // Fetch and clear
                                                                  auto fterm_ptr = context.builder.template get_builder<GroundFunctionTerm<T>>();
                                                                  auto& fterm = *fterm_ptr;
                                                                  fterm.clear();

                                                                  // Fill data
                                                                  fterm.function = element.get_function().get_index();
                                                                  fterm.binding = ground_common(element.get_terms(), context).get_index();

                                                                  // Canonicalize and Serialize
                                                                  canonicalize(fterm);
                                                                  return context.destination.get_or_create(fterm, context.builder.get_buffer()).first;
                                                              });
}

template<Context C_SRC, Context C_DST>
View<Data<GroundFunctionExpression>, C_DST> ground_common(View<Data<FunctionExpression>, C_SRC> element, GrounderContext<C_SRC, C_DST>& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return make_view(Data<GroundFunctionExpression>(arg), context.destination);
            else if constexpr (std::is_same_v<Alternative, View<Data<ArithmeticOperator<Data<FunctionExpression>>>, C_SRC>>)
                return make_view(Data<GroundFunctionExpression>(ground_common(arg, context).get_data()), context.destination);
            else
                return make_view(Data<GroundFunctionExpression>(ground_common(arg, context).get_index()), context.destination);
        },
        element.get_variant());
}

template<OpKind O, Context C_SRC, Context C_DST>
View<Index<UnaryOperator<O, Data<GroundFunctionExpression>>>, C_DST> ground_common(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, C_SRC> element,
                                                                                   GrounderContext<C_SRC, C_DST>& context)
{
    return with_cache<UnaryOperator<O, Data<FunctionExpression>>, UnaryOperator<O, Data<GroundFunctionExpression>>>(
        element,
        context.binding,
        context.cache,
        [&]()
        {
            // Fetch and clear
            auto unary_ptr = context.builder.template get_builder<UnaryOperator<O, Data<GroundFunctionExpression>>>();
            auto& unary = *unary_ptr;
            unary.clear();

            // Fill data
            unary.arg = ground_common(element.get_arg(), context).get_data();

            // Canonicalize and Serialize
            canonicalize(unary);
            return context.destination.get_or_create(unary, context.builder.get_buffer()).first;
        });
}

template<OpKind O, Context C_SRC, Context C_DST>
View<Index<BinaryOperator<O, Data<GroundFunctionExpression>>>, C_DST> ground_common(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, C_SRC> element,
                                                                                    GrounderContext<C_SRC, C_DST>& context)
{
    return with_cache<BinaryOperator<O, Data<FunctionExpression>>, BinaryOperator<O, Data<GroundFunctionExpression>>>(
        element,
        context.binding,
        context.cache,
        [&]()
        {
            // Fetch and clear
            auto binary_ptr = context.builder.template get_builder<BinaryOperator<O, Data<GroundFunctionExpression>>>();
            auto& binary = *binary_ptr;
            binary.clear();

            // Fill data
            binary.lhs = ground_common(element.get_lhs(), context).get_data();
            binary.rhs = ground_common(element.get_rhs(), context).get_data();

            // Canonicalize and Serialize
            canonicalize(binary);
            return context.destination.get_or_create(binary, context.builder.get_buffer()).first;
        });
}

template<OpKind O, Context C_SRC, Context C_DST>
View<Index<MultiOperator<O, Data<GroundFunctionExpression>>>, C_DST> ground_common(View<Index<MultiOperator<O, Data<FunctionExpression>>>, C_SRC> element,
                                                                                   GrounderContext<C_SRC, C_DST>& context)
{
    return with_cache<MultiOperator<O, Data<FunctionExpression>>, MultiOperator<O, Data<GroundFunctionExpression>>>(
        element,
        context.binding,
        context.cache,
        [&]()
        {
            // Fetch and clear
            auto multi_ptr = context.builder.template get_builder<MultiOperator<O, Data<GroundFunctionExpression>>>();
            auto& multi = *multi_ptr;
            multi.clear();

            // Fill data
            for (const auto arg : element.get_args())
                multi.args.push_back(ground_common(arg, context).get_data());

            // Canonicalize and Serialize
            canonicalize(multi);
            return context.destination.get_or_create(multi, context.builder.get_buffer()).first;
        });
}

template<Context C_SRC, Context C_DST>
View<Data<BooleanOperator<Data<GroundFunctionExpression>>>, C_DST> ground_common(View<Data<BooleanOperator<Data<FunctionExpression>>>, C_SRC> element,
                                                                                 GrounderContext<C_SRC, C_DST>& context)
{
    return visit(
        [&](auto&& arg) {
            return make_view(Data<BooleanOperator<Data<GroundFunctionExpression>>>(ground_common(arg, context).get_index(), element.get_arity()),
                             context.destination);
        },
        element.get_variant());
}

template<Context C_SRC, Context C_DST>
View<Data<ArithmeticOperator<Data<GroundFunctionExpression>>>, C_DST> ground_common(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, C_SRC> element,
                                                                                    GrounderContext<C_SRC, C_DST>& context)
{
    return visit([&](auto&& arg)
                 { return make_view(Data<ArithmeticOperator<Data<GroundFunctionExpression>>>(ground_common(arg, context).get_index()), context.destination); },
                 element.get_variant());
}

}

#endif