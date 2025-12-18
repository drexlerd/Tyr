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

#ifndef TYR_FORMALISM_MERGE_COMMON_HPP_
#define TYR_FORMALISM_MERGE_COMMON_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/builder.hpp"
#include "tyr/formalism/canonicalization.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/indices.hpp"
#include "tyr/formalism/views.hpp"

namespace tyr::formalism
{
class MergeCache
{
private:
    template<typename T_SRC, typename T_DST = T_SRC>
    struct MapEntryType
    {
        using value_type = std::pair<T_SRC, T_DST>;
        using container_type = UnorderedMap<Index<T_SRC>, Index<T_DST>>;

        container_type container;
    };

    using MergeStorage = std::tuple<MapEntryType<Variable>,
                                    MapEntryType<Object>,
                                    MapEntryType<Binding>,
                                    MapEntryType<Predicate<StaticTag>>,
                                    MapEntryType<Predicate<FluentTag>>,
                                    MapEntryType<Predicate<DerivedTag>>,
                                    MapEntryType<Predicate<FluentTag>, Predicate<DerivedTag>>,
                                    MapEntryType<Predicate<DerivedTag>, Predicate<FluentTag>>,
                                    MapEntryType<Atom<StaticTag>>,
                                    MapEntryType<Atom<FluentTag>>,
                                    MapEntryType<Atom<DerivedTag>>,
                                    MapEntryType<Atom<FluentTag>, Atom<DerivedTag>>,
                                    MapEntryType<Atom<DerivedTag>, Atom<FluentTag>>,
                                    MapEntryType<GroundAtom<StaticTag>>,
                                    MapEntryType<GroundAtom<FluentTag>>,
                                    MapEntryType<GroundAtom<DerivedTag>>,
                                    MapEntryType<GroundAtom<FluentTag>, GroundAtom<DerivedTag>>,
                                    MapEntryType<GroundAtom<DerivedTag>, GroundAtom<FluentTag>>,
                                    MapEntryType<Literal<StaticTag>>,
                                    MapEntryType<Literal<FluentTag>>,
                                    MapEntryType<Literal<DerivedTag>>,
                                    MapEntryType<Literal<FluentTag>, Literal<DerivedTag>>,
                                    MapEntryType<Literal<DerivedTag>, Literal<FluentTag>>,
                                    MapEntryType<GroundLiteral<StaticTag>>,
                                    MapEntryType<GroundLiteral<FluentTag>>,
                                    MapEntryType<GroundLiteral<DerivedTag>>,
                                    MapEntryType<GroundLiteral<FluentTag>, GroundLiteral<DerivedTag>>,
                                    MapEntryType<GroundLiteral<DerivedTag>, GroundLiteral<FluentTag>>,
                                    MapEntryType<Function<StaticTag>>,
                                    MapEntryType<Function<FluentTag>>,
                                    MapEntryType<Function<AuxiliaryTag>>,
                                    MapEntryType<FunctionTerm<StaticTag>>,
                                    MapEntryType<FunctionTerm<FluentTag>>,
                                    MapEntryType<FunctionTerm<AuxiliaryTag>>,
                                    MapEntryType<GroundFunctionTerm<StaticTag>>,
                                    MapEntryType<GroundFunctionTerm<FluentTag>>,
                                    MapEntryType<GroundFunctionTerm<AuxiliaryTag>>,
                                    MapEntryType<GroundFunctionTermValue<StaticTag>>,
                                    MapEntryType<GroundFunctionTermValue<FluentTag>>,
                                    MapEntryType<GroundFunctionTermValue<AuxiliaryTag>>,
                                    MapEntryType<UnaryOperator<OpSub, Data<FunctionExpression>>>,
                                    MapEntryType<BinaryOperator<OpAdd, Data<FunctionExpression>>>,
                                    MapEntryType<BinaryOperator<OpSub, Data<FunctionExpression>>>,
                                    MapEntryType<BinaryOperator<OpMul, Data<FunctionExpression>>>,
                                    MapEntryType<BinaryOperator<OpDiv, Data<FunctionExpression>>>,
                                    MapEntryType<MultiOperator<OpAdd, Data<FunctionExpression>>>,
                                    MapEntryType<MultiOperator<OpMul, Data<FunctionExpression>>>,
                                    MapEntryType<BinaryOperator<OpEq, Data<FunctionExpression>>>,
                                    MapEntryType<BinaryOperator<OpNe, Data<FunctionExpression>>>,
                                    MapEntryType<BinaryOperator<OpLe, Data<FunctionExpression>>>,
                                    MapEntryType<BinaryOperator<OpLt, Data<FunctionExpression>>>,
                                    MapEntryType<BinaryOperator<OpGe, Data<FunctionExpression>>>,
                                    MapEntryType<BinaryOperator<OpGt, Data<FunctionExpression>>>,
                                    MapEntryType<UnaryOperator<OpSub, Data<GroundFunctionExpression>>>,
                                    MapEntryType<BinaryOperator<OpAdd, Data<GroundFunctionExpression>>>,
                                    MapEntryType<BinaryOperator<OpSub, Data<GroundFunctionExpression>>>,
                                    MapEntryType<BinaryOperator<OpMul, Data<GroundFunctionExpression>>>,
                                    MapEntryType<BinaryOperator<OpDiv, Data<GroundFunctionExpression>>>,
                                    MapEntryType<MultiOperator<OpAdd, Data<GroundFunctionExpression>>>,
                                    MapEntryType<MultiOperator<OpMul, Data<GroundFunctionExpression>>>,
                                    MapEntryType<BinaryOperator<OpEq, Data<GroundFunctionExpression>>>,
                                    MapEntryType<BinaryOperator<OpNe, Data<GroundFunctionExpression>>>,
                                    MapEntryType<BinaryOperator<OpLe, Data<GroundFunctionExpression>>>,
                                    MapEntryType<BinaryOperator<OpLt, Data<GroundFunctionExpression>>>,
                                    MapEntryType<BinaryOperator<OpGe, Data<GroundFunctionExpression>>>,
                                    MapEntryType<BinaryOperator<OpGt, Data<GroundFunctionExpression>>>,
                                    MapEntryType<ConjunctiveCondition>,
                                    MapEntryType<Rule>,
                                    MapEntryType<GroundConjunctiveCondition>,
                                    MapEntryType<GroundRule>,
                                    MapEntryType<Program>,
                                    MapEntryType<NumericEffect<OpAssign, FluentTag>>,
                                    MapEntryType<NumericEffect<OpIncrease, FluentTag>>,
                                    MapEntryType<NumericEffect<OpDecrease, FluentTag>>,
                                    MapEntryType<NumericEffect<OpScaleUp, FluentTag>>,
                                    MapEntryType<NumericEffect<OpScaleDown, FluentTag>>,
                                    MapEntryType<NumericEffect<OpIncrease, AuxiliaryTag>>,
                                    MapEntryType<GroundNumericEffect<OpAssign, FluentTag>>,
                                    MapEntryType<GroundNumericEffect<OpIncrease, FluentTag>>,
                                    MapEntryType<GroundNumericEffect<OpDecrease, FluentTag>>,
                                    MapEntryType<GroundNumericEffect<OpScaleUp, FluentTag>>,
                                    MapEntryType<GroundNumericEffect<OpScaleDown, FluentTag>>,
                                    MapEntryType<GroundNumericEffect<OpIncrease, AuxiliaryTag>>,
                                    MapEntryType<FDRConjunctiveCondition>,
                                    MapEntryType<GroundFDRConjunctiveCondition>,
                                    MapEntryType<ConditionalEffect>,
                                    MapEntryType<GroundConditionalEffect>,
                                    MapEntryType<ConjunctiveEffect>,
                                    MapEntryType<GroundConjunctiveEffect>,
                                    MapEntryType<Action>,
                                    MapEntryType<GroundAction>,
                                    MapEntryType<Axiom>,
                                    MapEntryType<GroundAxiom>,
                                    MapEntryType<Metric>,
                                    MapEntryType<Domain>,
                                    MapEntryType<Task>,
                                    MapEntryType<FDRVariable<FluentTag>>>;

    MergeStorage m_maps;

public:
    MergeCache() = default;

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

template<Context C>
struct MergeContext
{
    Builder& builder;
    C& destination;
    MergeCache& cache;
};

/**
 * Forward declarations
 */

template<OpKind O, typename T, Context C_SRC, Context C_DST>
auto merge(View<Index<UnaryOperator<O, T>>, C_SRC> element, MergeContext<C_DST>& context);

template<OpKind O, typename T, Context C_SRC, Context C_DST>
auto merge(View<Index<BinaryOperator<O, T>>, C_SRC> element, MergeContext<C_DST>& context);

template<OpKind O, typename T, Context C_SRC, Context C_DST>
auto merge(View<Index<MultiOperator<O, T>>, C_SRC> element, MergeContext<C_DST>& context);

template<typename T, Context C_SRC, Context C_DST>
auto merge(View<Data<ArithmeticOperator<T>>, C_SRC> element, MergeContext<C_DST>& context);

template<typename T, Context C_SRC, Context C_DST>
auto merge(View<Data<BooleanOperator<T>>, C_SRC> element, MergeContext<C_DST>& context);

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Variable>, C_SRC> element, MergeContext<C_DST>& context);

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Object>, C_SRC> element, MergeContext<C_DST>& context);

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Binding>, C_SRC> element, MergeContext<C_DST>& context);

template<Context C_SRC, Context C_DST>
auto merge(View<Data<Term>, C_SRC> element, MergeContext<C_DST>& context);

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<Predicate<T>>, C_SRC> element, MergeContext<C_DST>& context);

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<Atom<T>>, C_SRC> element, MergeContext<C_DST>& context);

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<GroundAtom<T>>, C_SRC> element, MergeContext<C_DST>& context);

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<Literal<T>>, C_SRC> element, MergeContext<C_DST>& context);

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<GroundLiteral<T>>, C_SRC> element, MergeContext<C_DST>& context);

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<Function<T>>, C_SRC> element, MergeContext<C_DST>& context);

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<FunctionTerm<T>>, C_SRC> element, MergeContext<C_DST>& context);

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<GroundFunctionTerm<T>>, C_SRC> element, MergeContext<C_DST>& context);

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<GroundFunctionTermValue<T>>, C_SRC> element, MergeContext<C_DST>& context);

template<Context C_SRC, Context C_DST>
auto merge(View<Data<FunctionExpression>, C_SRC> element, MergeContext<C_DST>& context);

template<Context C_SRC, Context C_DST>
auto merge(View<Data<GroundFunctionExpression>, C_SRC> element, MergeContext<C_DST>& context);

template<Context C_SRC, Context C_DST>
auto merge(View<Index<ConjunctiveCondition>, C_SRC> element, MergeContext<C_DST>& context);

/**
 * Implementations
 */

template<typename T_SRC, typename T_DST, Context C_SRC, typename F>
auto with_cache(View<Index<T_SRC>, C_SRC> element, MergeCache& cache, F&& compute)
{
    auto& m = cache.template get<T_SRC, T_DST>();

    if (auto it = m.find(element.get_index()); it != m.end())
        return std::make_pair(it->second, false);

    auto result = compute();  // compute the merged element

    m.emplace(element.get_index(), result.first);

    return result;
}

template<OpKind O, typename T, Context C_SRC, Context C_DST>
auto merge(View<Index<UnaryOperator<O, T>>, C_SRC> element, MergeContext<C_DST>& context)
{
    return with_cache<UnaryOperator<O, T>, UnaryOperator<O, T>>(element,
                                                                context.cache,
                                                                [&]()
                                                                {
                                                                    auto unary_ptr = context.builder.template get_builder<UnaryOperator<O, T>>();
                                                                    auto& unary = *unary_ptr;
                                                                    unary.clear();

                                                                    unary.arg = merge(element.get_arg(), context);

                                                                    canonicalize(unary);
                                                                    return context.destination.get_or_create(unary, context.builder.get_buffer());
                                                                });
}

template<OpKind O, typename T, Context C_SRC, Context C_DST>
auto merge(View<Index<BinaryOperator<O, T>>, C_SRC> element, MergeContext<C_DST>& context)
{
    return with_cache<BinaryOperator<O, T>, BinaryOperator<O, T>>(element,
                                                                  context.cache,
                                                                  [&]()
                                                                  {
                                                                      auto binary_ptr = context.builder.template get_builder<BinaryOperator<O, T>>();
                                                                      auto& binary = *binary_ptr;
                                                                      binary.clear();

                                                                      binary.lhs = merge(element.get_lhs(), context);
                                                                      binary.rhs = merge(element.get_rhs(), context);

                                                                      canonicalize(binary);
                                                                      return context.destination.get_or_create(binary, context.builder.get_buffer());
                                                                  });
}

template<OpKind O, typename T, Context C_SRC, Context C_DST>
auto merge(View<Index<MultiOperator<O, T>>, C_SRC> element, MergeContext<C_DST>& context)
{
    return with_cache<MultiOperator<O, T>, MultiOperator<O, T>>(element,
                                                                context.cache,
                                                                [&]()
                                                                {
                                                                    auto multi_ptr = context.builder.template get_builder<MultiOperator<O, T>>();
                                                                    auto& multi = *multi_ptr;
                                                                    multi.clear();

                                                                    for (const auto arg : element.get_args())
                                                                        multi.args.push_back(merge(arg, context));

                                                                    canonicalize(multi);
                                                                    return context.destination.get_or_create(multi, context.builder.get_buffer());
                                                                });
}

template<typename T, Context C_SRC, Context C_DST>
auto merge(View<Data<ArithmeticOperator<T>>, C_SRC> element, MergeContext<C_DST>& context)
{
    return visit([&](auto&& arg) { return Data<ArithmeticOperator<T>>(merge(arg, context).first); }, element.get_variant());
}

template<typename T, Context C_SRC, Context C_DST>
auto merge(View<Data<BooleanOperator<T>>, C_SRC> element, MergeContext<C_DST>& context)
{
    return visit([&](auto&& arg) { return Data<BooleanOperator<T>>(merge(arg, context).first, element.get_arity()); }, element.get_variant());
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Variable>, C_SRC> element, MergeContext<C_DST>& context)
{
    return with_cache<Variable, Variable>(element,
                                          context.cache,
                                          [&]()
                                          {
                                              auto variable_ptr = context.builder.template get_builder<Variable>();
                                              auto& variable = *variable_ptr;
                                              variable.clear();

                                              variable.name = element.get_name();

                                              canonicalize(variable);
                                              return context.destination.get_or_create(variable, context.builder.get_buffer());
                                          });
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Object>, C_SRC> element, MergeContext<C_DST>& context)
{
    return with_cache<Object, Object>(element,
                                      context.cache,
                                      [&]()
                                      {
                                          auto object_ptr = context.builder.template get_builder<Object>();
                                          auto& object = *object_ptr;
                                          object.clear();

                                          object.name = element.get_name();

                                          canonicalize(object);
                                          return context.destination.get_or_create(object, context.builder.get_buffer());
                                      });
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Binding>, C_SRC> element, MergeContext<C_DST>& context)
{
    return with_cache<Binding, Binding>(element,
                                        context.cache,
                                        [&]()
                                        {
                                            auto binding_ptr = context.builder.template get_builder<Binding>();
                                            auto& binding = *binding_ptr;
                                            binding.clear();

                                            binding.objects = element.get_data().objects;

                                            canonicalize(binding);
                                            return context.destination.get_or_create(binding, context.builder.get_buffer());
                                        });
}

template<Context C_SRC, Context C_DST>
auto merge(View<Data<Term>, C_SRC> element, MergeContext<C_DST>& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                return Data<Term>(arg);
            else if constexpr (std::is_same_v<Alternative, View<Index<Object>, C_SRC>>)
                return Data<Term>(merge(arg, context).first);
            else
                static_assert(dependent_false<Alternative>::value, "Missing case");
        },
        element.get_variant());
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<Predicate<T>>, C_SRC> element, MergeContext<C_DST>& context)
{
    return with_cache<Predicate<T>, Predicate<T>>(element,
                                                  context.cache,
                                                  [&]()
                                                  {
                                                      auto predicate_ptr = context.builder.template get_builder<Predicate<T>>();
                                                      auto& predicate = *predicate_ptr;
                                                      predicate.clear();

                                                      predicate.name = element.get_name();
                                                      predicate.arity = element.get_arity();

                                                      canonicalize(predicate);
                                                      return context.destination.get_or_create(predicate, context.builder.get_buffer());
                                                  });
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<Atom<T>>, C_SRC> element, MergeContext<C_DST>& context)
{
    return with_cache<Atom<T>, Atom<T>>(element,
                                        context.cache,
                                        [&]()
                                        {
                                            auto atom_ptr = context.builder.template get_builder<Atom<T>>();
                                            auto& atom = *atom_ptr;
                                            atom.clear();

                                            atom.predicate = element.get_predicate().get_index();
                                            for (const auto term : element.get_terms())
                                                atom.terms.push_back(merge(term, context));

                                            canonicalize(atom);
                                            return context.destination.get_or_create(atom, context.builder.get_buffer());
                                        });
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<GroundAtom<T>>, C_SRC> element, MergeContext<C_DST>& context)
{
    return with_cache<GroundAtom<T>, GroundAtom<T>>(element,
                                                    context.cache,
                                                    [&]()
                                                    {
                                                        auto atom_ptr = context.builder.template get_builder<GroundAtom<T>>();
                                                        auto& atom = *atom_ptr;
                                                        atom.clear();

                                                        atom.predicate = element.get_predicate().get_index();
                                                        atom.binding = merge(element.get_binding(), context).first;

                                                        canonicalize(atom);
                                                        return context.destination.get_or_create(atom, context.builder.get_buffer());
                                                    });
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<Literal<T>>, C_SRC> element, MergeContext<C_DST>& context)
{
    return with_cache<Literal<T>, Literal<T>>(element,
                                              context.cache,
                                              [&]()
                                              {
                                                  auto literal_ptr = context.builder.template get_builder<Literal<T>>();
                                                  auto& literal = *literal_ptr;
                                                  literal.clear();

                                                  literal.polarity = element.get_polarity();
                                                  literal.atom = merge(element.get_atom(), context).first;

                                                  canonicalize(literal);
                                                  return context.destination.get_or_create(literal, context.builder.get_buffer());
                                              });
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<GroundLiteral<T>>, C_SRC> element, MergeContext<C_DST>& context)
{
    return with_cache<GroundLiteral<T>, GroundLiteral<T>>(element,
                                                          context.cache,
                                                          [&]()
                                                          {
                                                              auto literal_ptr = context.builder.template get_builder<GroundLiteral<T>>();
                                                              auto& literal = *literal_ptr;
                                                              literal.clear();

                                                              literal.polarity = element.get_polarity();
                                                              literal.atom = merge(element.get_atom(), context).first;

                                                              canonicalize(literal);
                                                              return context.destination.get_or_create(literal, context.builder.get_buffer());
                                                          });
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<Function<T>>, C_SRC> element, MergeContext<C_DST>& context)
{
    return with_cache<Function<T>, Function<T>>(element,
                                                context.cache,
                                                [&]()
                                                {
                                                    auto function_ptr = context.builder.template get_builder<Function<T>>();
                                                    auto& function = *function_ptr;
                                                    function.clear();

                                                    function.name = element.get_name();
                                                    function.arity = element.get_arity();

                                                    canonicalize(function);
                                                    return context.destination.get_or_create(function, context.builder.get_buffer());
                                                });
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<FunctionTerm<T>>, C_SRC> element, MergeContext<C_DST>& context)
{
    return with_cache<FunctionTerm<T>, FunctionTerm<T>>(element,
                                                        context.cache,
                                                        [&]()
                                                        {
                                                            auto fterm_ptr = context.builder.template get_builder<FunctionTerm<T>>();
                                                            auto& fterm = *fterm_ptr;
                                                            fterm.clear();

                                                            fterm.function = element.get_function().get_index();
                                                            for (const auto term : element.get_terms())
                                                                fterm.terms.push_back(merge(term, context));

                                                            canonicalize(fterm);
                                                            return context.destination.get_or_create(fterm, context.builder.get_buffer());
                                                        });
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<GroundFunctionTerm<T>>, C_SRC> element, MergeContext<C_DST>& context)
{
    return with_cache<GroundFunctionTerm<T>, GroundFunctionTerm<T>>(element,
                                                                    context.cache,
                                                                    [&]()
                                                                    {
                                                                        auto fterm_ptr = context.builder.template get_builder<GroundFunctionTerm<T>>();
                                                                        auto& fterm = *fterm_ptr;
                                                                        fterm.clear();

                                                                        fterm.function = element.get_function().get_index();
                                                                        fterm.binding = merge(element.get_binding(), context).first;

                                                                        canonicalize(fterm);
                                                                        return context.destination.get_or_create(fterm, context.builder.get_buffer());
                                                                    });
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<GroundFunctionTermValue<T>>, C_SRC> element, MergeContext<C_DST>& context)
{
    return with_cache<GroundFunctionTermValue<T>, GroundFunctionTermValue<T>>(
        element,
        context.cache,
        [&]()
        {
            auto fterm_value_ptr = context.builder.template get_builder<GroundFunctionTermValue<T>>();
            auto& fterm_value = *fterm_value_ptr;
            fterm_value.clear();

            fterm_value.fterm = merge(element.get_fterm(), context).first;
            fterm_value.value = element.get_value();

            canonicalize(fterm_value);
            return context.destination.get_or_create(fterm_value, context.builder.get_buffer());
        });
}

template<Context C_SRC, Context C_DST>
auto merge(View<Data<FunctionExpression>, C_SRC> element, MergeContext<C_DST>& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return Data<FunctionExpression>(arg);
            else if constexpr (std::is_same_v<Alternative, View<Data<ArithmeticOperator<Data<FunctionExpression>>>, C_SRC>>)
                return Data<FunctionExpression>(merge(arg, context));
            else
                return Data<FunctionExpression>(merge(arg, context).first);
        },
        element.get_variant());
}

template<Context C_SRC, Context C_DST>
auto merge(View<Data<GroundFunctionExpression>, C_SRC> element, MergeContext<C_DST>& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return Data<GroundFunctionExpression>(arg);
            else if constexpr (std::is_same_v<Alternative, View<Data<ArithmeticOperator<Data<GroundFunctionExpression>>>, C_SRC>>)
                return Data<GroundFunctionExpression>(merge(arg, context));
            else
                return Data<GroundFunctionExpression>(merge(arg, context).first);
        },
        element.get_variant());
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<ConjunctiveCondition>, C_SRC> element, MergeContext<C_DST>& context)
{
    return with_cache<ConjunctiveCondition, ConjunctiveCondition>(element,
                                                                  context.cache,
                                                                  [&]()
                                                                  {
                                                                      auto conj_cond_ptr = context.builder.template get_builder<ConjunctiveCondition>();
                                                                      auto& conj_cond = *conj_cond_ptr;
                                                                      conj_cond.clear();

                                                                      for (const auto literal : element.template get_literals<StaticTag>())
                                                                          conj_cond.static_literals.push_back(merge(literal, context).first);
                                                                      for (const auto literal : element.template get_literals<FluentTag>())
                                                                          conj_cond.fluent_literals.push_back(merge(literal, context).first);
                                                                      for (const auto numeric_constraint : element.get_numeric_constraints())
                                                                          conj_cond.numeric_constraints.push_back(merge(numeric_constraint, context));

                                                                      canonicalize(conj_cond);
                                                                      return context.destination.get_or_create(conj_cond, context.builder.get_buffer());
                                                                  });
}
}

#endif