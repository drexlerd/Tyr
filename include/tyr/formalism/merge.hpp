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

#ifndef TYR_FORMALISM_MERGE_HPP_
#define TYR_FORMALISM_MERGE_HPP_

#include "tyr/formalism/builder.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"

#include <gtl/phmap.hpp>

namespace tyr::formalism
{
template<Context C_SRC, Context C_DST>
class MergeCache
{
private:
    template<typename T>
    using MapEntryType = boost::hana::pair<boost::hana::type<T>, UnorderedMap<View<Index<T>, C_SRC>, View<Index<T>, C_DST>>>;

    using HanaMap = boost::hana::map<MapEntryType<Variable>,
                                     MapEntryType<Object>,
                                     MapEntryType<Predicate<StaticTag>>,
                                     MapEntryType<Predicate<FluentTag>>,
                                     MapEntryType<Predicate<DerivedTag>>,
                                     MapEntryType<Atom<StaticTag>>,
                                     MapEntryType<Atom<FluentTag>>,
                                     MapEntryType<Atom<DerivedTag>>,
                                     MapEntryType<GroundAtom<StaticTag>>,
                                     MapEntryType<GroundAtom<FluentTag>>,
                                     MapEntryType<GroundAtom<DerivedTag>>,
                                     MapEntryType<Literal<StaticTag>>,
                                     MapEntryType<Literal<FluentTag>>,
                                     MapEntryType<Literal<DerivedTag>>,
                                     MapEntryType<GroundLiteral<StaticTag>>,
                                     MapEntryType<GroundLiteral<FluentTag>>,
                                     MapEntryType<GroundLiteral<DerivedTag>>,
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
                                     MapEntryType<Task>>;

    HanaMap maps;

public:
    MergeCache() = default;

    template<typename Tag>
    auto& get() noexcept
    {
        return boost::hana::at_key(maps, boost::hana::type<Tag> {});
    }
    template<typename Tag>
    const auto& get() const noexcept
    {
        return boost::hana::at_key(maps, boost::hana::type<Tag> {});
    }

    void clear() noexcept
    {
        boost::hana::for_each(maps,
                              [](auto&& pair)
                              {
                                  auto& map = boost::hana::second(pair);
                                  map.clear();
                              });
    }
};

template<typename Tag, typename C_SRC, typename C_DST, typename F>
auto with_cache(View<Index<Tag>, C_SRC> element, MergeCache<C_SRC, C_DST>& cache, F&& compute)
{
    auto& m = cache.template get<Tag>();

    if (auto it = m.find(element); it != m.end())
        return it->second;

    auto result = compute();  // compute the merged element

    m.emplace(element, result);

    return result;
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Object>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache)
{
    return with_cache<Object>(element,
                              cache,
                              [&]()
                              {
                                  auto& object = builder.get_object();

                                  object.name = element.get_name();

                                  canonicalize(object);
                                  return destination.get_or_create(object, builder.get_buffer()).first;
                              });
}

template<Context C_SRC, Context C_DST>
auto merge(float_t element, Builder&, C_DST&, MergeCache<C_SRC, C_DST>&)
{
    return element;
}

template<OpKind O, Context C_SRC, Context C_DST>
auto merge(View<Index<UnaryOperator<O, Data<GroundFunctionExpression>>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache)
{
    return with_cache<UnaryOperator<O, Data<GroundFunctionExpression>>>(element,
                                                                        cache,
                                                                        [&]()
                                                                        {
                                                                            auto& unary = builder.template get_ground_unary<O>();

                                                                            unary.arg = merge(element.get_arg(), builder, destination, cache).get_data();

                                                                            canonicalize(unary);
                                                                            return destination.get_or_create(unary, builder.get_buffer()).first;
                                                                        });
}

template<OpKind O, Context C_SRC, Context C_DST>
auto merge(View<Index<BinaryOperator<O, Data<GroundFunctionExpression>>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache)
{
    return with_cache<BinaryOperator<O, Data<GroundFunctionExpression>>>(element,
                                                                         cache,
                                                                         [&]()
                                                                         {
                                                                             auto& binary = builder.template get_ground_binary<O>();

                                                                             binary.lhs = merge(element.get_lhs(), builder, destination, cache).get_data();
                                                                             binary.rhs = merge(element.get_rhs(), builder, destination, cache).get_data();

                                                                             canonicalize(binary);
                                                                             return destination.get_or_create(binary, builder.get_buffer()).first;
                                                                         });
}

template<OpKind O, Context C_SRC, Context C_DST>
auto merge(View<Index<MultiOperator<O, Data<GroundFunctionExpression>>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache)
{
    return with_cache<MultiOperator<O, Data<GroundFunctionExpression>>>(element,
                                                                        cache,
                                                                        [&]()
                                                                        {
                                                                            auto& multi = builder.template get_ground_multi<O>();
                                                                            multi.clear();

                                                                            for (const auto arg : element.get_args())
                                                                                multi.args.push_back(merge(arg, builder, destination, cache).get_data());

                                                                            canonicalize(multi);
                                                                            return destination.get_or_create(multi, builder.get_buffer()).first;
                                                                        });
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<Predicate<T>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache)
{
    return with_cache<Predicate<T>>(element,
                                    cache,
                                    [&]()
                                    {
                                        auto& predicate = builder.template get_predicate<T>();

                                        predicate.name = element.get_name();

                                        canonicalize(predicate);
                                        return destination.get_or_create(predicate, builder.get_buffer()).first;
                                    });
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<GroundAtom<T>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache)
{
    return with_cache<GroundAtom<T>>(element,
                                     cache,
                                     [&]()
                                     {
                                         auto& atom = builder.template get_ground_atom<T>();
                                         atom.clear();

                                         atom.predicate = merge(element.get_predicate(), builder, destination, cache).get_index();
                                         for (const auto object : element.get_objects())
                                             atom.objects.push_back(merge(object, builder, destination, cache).get_index());

                                         canonicalize(atom);
                                         return destination.get_or_create(atom, builder.get_buffer()).first;
                                     });
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<GroundLiteral<T>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache)
{
    return with_cache<GroundLiteral<T>>(element,
                                        cache,
                                        [&]()
                                        {
                                            auto& literal = builder.template get_ground_literal<T>();

                                            literal.polarity = element.get_polarity();
                                            literal.atom = merge(element.get_atom(), builder, destination, cache).get_index();

                                            canonicalize(literal);
                                            return destination.get_or_create(literal, builder.get_buffer()).first;
                                        });
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<Function<T>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache)
{
    return with_cache<Function<T>>(element,
                                   cache,
                                   [&]()
                                   {
                                       auto& function = builder.template get_function<T>();

                                       function.name = element.get_name();

                                       canonicalize(function);
                                       return destination.get_or_create(function, builder.get_buffer()).first;
                                   });
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<GroundFunctionTerm<T>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache)
{
    return with_cache<GroundFunctionTerm<T>>(element,
                                             cache,
                                             [&]()
                                             {
                                                 auto& fterm = builder.template get_ground_fterm<T>();
                                                 fterm.clear();

                                                 fterm.function = merge(element.get_function(), builder, destination, cache).get_index();
                                                 for (const auto object : element.get_objects())
                                                     fterm.objects.push_back(merge(object, builder, destination, cache).get_index());

                                                 canonicalize(fterm);
                                                 return destination.get_or_create(fterm, builder.get_buffer()).first;
                                             });
}

template<Context C_SRC, Context C_DST>
auto merge(View<Data<GroundFunctionExpression>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
            {
                return View<Data<GroundFunctionExpression>, C_DST>(Data<GroundFunctionExpression>(merge(arg, builder, destination, cache)), destination);
            }
            else if constexpr (std::is_same_v<Alternative, View<Data<ArithmeticOperator<Data<GroundFunctionExpression>>>, C_SRC>>)
            {
                return View<Data<GroundFunctionExpression>, C_DST>(Data<GroundFunctionExpression>(merge(arg, builder, destination, cache).get_data()),
                                                                   destination);
            }
            else
            {
                return View<Data<GroundFunctionExpression>, C_DST>(Data<GroundFunctionExpression>(merge(arg, builder, destination, cache).get_index()),
                                                                   destination);
            }
        },
        element.get_variant());
}

template<Context C_SRC, Context C_DST>
auto merge(View<Data<ArithmeticOperator<Data<GroundFunctionExpression>>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache)
{
    return visit(
        [&](auto&& arg)
        {
            return View<Data<ArithmeticOperator<Data<GroundFunctionExpression>>>, C_DST>(
                Data<ArithmeticOperator<Data<GroundFunctionExpression>>>(merge(arg, builder, destination, cache).get_index()),
                destination);
        },
        element.get_variant());
}

template<Context C_SRC, Context C_DST>
auto merge(View<Data<BooleanOperator<Data<GroundFunctionExpression>>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache)
{
    return visit(
        [&](auto&& arg)
        {
            return View<Data<BooleanOperator<Data<GroundFunctionExpression>>>, C_DST>(
                Data<BooleanOperator<Data<GroundFunctionExpression>>>(merge(arg, builder, destination, cache).get_index()),
                destination);
        },
        element.get_variant());
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<GroundConjunctiveCondition>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache)
{
    return with_cache<GroundConjunctiveCondition>(element,
                                                  cache,
                                                  [&]()
                                                  {
                                                      auto& conj_cond = builder.get_ground_conj_cond();
                                                      conj_cond.clear();

                                                      for (const auto literal : element.template get_literals<StaticTag>())
                                                          conj_cond.static_literals.push_back(merge(literal, builder, destination, cache).get_index());
                                                      for (const auto literal : element.template get_literals<FluentTag>())
                                                          conj_cond.fluent_literals.push_back(merge(literal, builder, destination, cache).get_index());
                                                      for (const auto numeric_constraint : element.get_numeric_constraints())
                                                          conj_cond.numeric_constraints.push_back(
                                                              merge(numeric_constraint, builder, destination, cache).get_data());

                                                      canonicalize(conj_cond);
                                                      return destination.get_or_create(conj_cond, builder.get_buffer()).first;
                                                  });
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<GroundRule>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache)
{
    return with_cache<GroundRule>(element,
                                  cache,
                                  [&]()
                                  {
                                      auto& rule = builder.get_ground_rule();

                                      rule.body = merge(element.get_body(), builder, destination, cache).get_index();
                                      rule.head = merge(element.get_head(), builder, destination, cache).get_index();

                                      canonicalize(rule);
                                      return destination.get_or_create(rule, builder.get_buffer()).first;
                                  });
}
}

#endif