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

#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/scoped_repository.hpp"

#include <gtl/phmap.hpp>

namespace tyr::formalism
{
template<IsContext C_SRC, IsContext C_DST>
class MergeCache
{
private:
    template<typename T>
    using MapEntryType = boost::hana::pair<boost::hana::type<T>, UnorderedMap<View<Index<T>, C_SRC>, View<Index<T>, C_DST>>>;

    using HanaMap = boost::hana::map<MapEntryType<Variable>,
                                     MapEntryType<Object>,
                                     MapEntryType<Predicate<StaticTag>>,
                                     MapEntryType<Predicate<FluentTag>>,
                                     MapEntryType<Atom<StaticTag>>,
                                     MapEntryType<Atom<FluentTag>>,
                                     MapEntryType<GroundAtom<StaticTag>>,
                                     MapEntryType<GroundAtom<FluentTag>>,
                                     MapEntryType<Literal<StaticTag>>,
                                     MapEntryType<Literal<FluentTag>>,
                                     MapEntryType<GroundLiteral<StaticTag>>,
                                     MapEntryType<GroundLiteral<FluentTag>>,
                                     MapEntryType<Function<StaticTag>>,
                                     MapEntryType<Function<FluentTag>>,
                                     MapEntryType<FunctionTerm<StaticTag>>,
                                     MapEntryType<FunctionTerm<FluentTag>>,
                                     MapEntryType<GroundFunctionTerm<StaticTag>>,
                                     MapEntryType<GroundFunctionTerm<FluentTag>>,
                                     MapEntryType<GroundFunctionTermValue<StaticTag>>,
                                     MapEntryType<GroundFunctionTermValue<FluentTag>>,
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
                                     MapEntryType<Program>>;

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

template<IsContext C_SRC, IsContext C_DST>
auto merge(View<Index<Object>, C_SRC> element, Builder& builder, C_DST& destination, buffer::Buffer& buffer, MergeCache<C_SRC, C_DST>& cache)
{
    auto& t_cache = cache.template get<Object>();

    if (auto it = t_cache.find(element); it != t_cache.end())
        return it->second;

    auto& object = builder.get_object();

    object.index = element.get_index();
    object.name = element.get_name();

    canonicalize(object);
    auto result = destination.get_or_create(object, buffer).first;

    t_cache.emplace(element, result);

    return result;
}

template<IsContext C_SRC, IsContext C_DST>
auto merge(float_t element, Builder&, C_DST&, buffer::Buffer&, MergeCache<C_SRC, C_DST>&)
{
    return element;
}

template<IsOp O, IsContext C_SRC, IsContext C_DST>
auto merge(View<Index<UnaryOperator<O, Data<GroundFunctionExpression>>>, C_SRC> element,
           Builder& builder,
           C_DST& destination,
           buffer::Buffer& buffer,
           MergeCache<C_SRC, C_DST>& cache)
{
    auto& t_cache = cache.template get<UnaryOperator<O, Data<GroundFunctionExpression>>>();

    if (auto it = t_cache.find(element); it != t_cache.end())
        return it->second;

    auto& unary = builder.template get_ground_unary<O>();

    unary.index = element.get_index();
    unary.arg = merge(element.get_arg(), builder, destination, buffer, cache).get_data();

    canonicalize(unary);
    auto result = destination.get_or_create(unary, buffer).first;

    t_cache.emplace(element, result);

    return result;
}

template<IsOp O, IsContext C_SRC, IsContext C_DST>
auto merge(View<Index<BinaryOperator<O, Data<GroundFunctionExpression>>>, C_SRC> element,
           Builder& builder,
           C_DST& destination,
           buffer::Buffer& buffer,
           MergeCache<C_SRC, C_DST>& cache)
{
    auto& t_cache = cache.template get<BinaryOperator<O, Data<GroundFunctionExpression>>>();

    if (auto it = t_cache.find(element); it != t_cache.end())
        return it->second;

    auto& binary = builder.template get_ground_binary<O>();

    binary.index = element.get_index();
    binary.lhs = merge(element.get_lhs(), builder, destination, buffer, cache).get_data();
    binary.rhs = merge(element.get_rhs(), builder, destination, buffer, cache).get_data();

    canonicalize(binary);
    auto result = destination.get_or_create(binary, buffer).first;

    t_cache.emplace(element, result);

    return result;
}

template<IsOp O, IsContext C_SRC, IsContext C_DST>
auto merge(View<Index<MultiOperator<O, Data<GroundFunctionExpression>>>, C_SRC> element,
           Builder& builder,
           C_DST& destination,
           buffer::Buffer& buffer,
           MergeCache<C_SRC, C_DST>& cache)
{
    auto& t_cache = cache.template get<MultiOperator<O, Data<GroundFunctionExpression>>>();

    if (auto it = t_cache.find(element); it != t_cache.end())
        return it->second;

    auto& multi = builder.template get_ground_multi<O>();

    multi.index = element.get_index();
    multi.args.clear();
    for (const auto arg : element.get_args())
        multi.args.push_back(merge(arg, builder, destination, buffer, cache).get_data());

    canonicalize(multi);
    auto result = destination.get_or_create(multi, buffer).first;

    t_cache.emplace(element, result);

    return result;
}

template<IsStaticOrFluentTag T, IsContext C_SRC, IsContext C_DST>
auto merge(View<Index<Predicate<T>>, C_SRC> element, Builder& builder, C_DST& destination, buffer::Buffer& buffer, MergeCache<C_SRC, C_DST>& cache)
{
    auto& t_cache = cache.template get<Predicate<T>>();

    if (auto it = t_cache.find(element); it != t_cache.end())
        return it->second;

    auto& predicate = builder.template get_predicate<T>();

    predicate.index = element.get_index();
    predicate.name = element.get_name();

    canonicalize(predicate);
    auto result = destination.get_or_create(predicate, buffer).first;

    t_cache.emplace(element, result);

    return result;
}

template<IsStaticOrFluentTag T, IsContext C_SRC, IsContext C_DST>
auto merge(View<Index<GroundAtom<T>>, C_SRC> element, Builder& builder, C_DST& destination, buffer::Buffer& buffer, MergeCache<C_SRC, C_DST>& cache)
{
    auto& t_cache = cache.template get<GroundAtom<T>>();

    if (auto it = t_cache.find(element); it != t_cache.end())
        return it->second;

    auto& atom = builder.template get_ground_atom<T>();

    atom.index = element.get_index();
    atom.objects.clear();
    for (const auto object : element.get_objects())
        atom.objects.push_back(merge(object, builder, destination, buffer, cache).get_data());

    canonicalize(atom);
    auto result = destination.get_or_create(atom, buffer).first;

    t_cache.emplace(element, result);

    return result;
}

template<IsStaticOrFluentTag T, IsContext C_SRC, IsContext C_DST>
auto merge(View<Index<GroundLiteral<T>>, C_SRC> element, Builder& builder, C_DST& destination, buffer::Buffer& buffer, MergeCache<C_SRC, C_DST>& cache)
{
    auto& t_cache = cache.template get<GroundLiteral<T>>();

    if (auto it = t_cache.find(element); it != t_cache.end())
        return it->second;

    auto& literal = builder.template get_ground_literal<T>();

    literal.index = element.get_index();
    literal.polarity = element.get_polarity();
    literal.atom = merge(element.get_atom(), builder, destination, buffer, cache).get_data();

    canonicalize(literal);
    auto result = destination.get_or_create(literal, buffer).first;

    t_cache.emplace(element, result);

    return result;
}

template<IsStaticOrFluentTag T, IsContext C_SRC, IsContext C_DST>
auto merge(View<Index<GroundFunctionTerm<T>>, C_SRC> element, Builder& builder, C_DST& destination, buffer::Buffer& buffer, MergeCache<C_SRC, C_DST>& cache)
{
    auto& t_cache = cache.template get<GroundFunctionTerm<T>>();

    if (auto it = t_cache.find(element); it != t_cache.end())
        return it->second;

    auto& fterm = builder.template get_ground_fterm<T>();

    fterm.index = element.get_index();
    fterm.objects.clear();
    for (const auto object : element.get_objects())
        fterm.objects.push_back(merge(object, builder, destination, buffer, cache).get_data());

    canonicalize(fterm);
    auto result = destination.get_or_create(fterm, buffer).first;

    t_cache.emplace(element, result);

    return result;
}

template<IsContext C_SRC, IsContext C_DST>
auto merge(View<Data<GroundFunctionExpression>, C_SRC> element, Builder& builder, C_DST& destination, buffer::Buffer& buffer, MergeCache<C_SRC, C_DST>& cache)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
            {
                return View<Data<GroundFunctionExpression>, C_DST>(Data<GroundFunctionExpression>(merge(arg, builder, destination, buffer, cache)),
                                                                   destination);
            }
            else
            {
                return View<Data<GroundFunctionExpression>, C_DST>(Data<GroundFunctionExpression>(merge(arg, builder, destination, buffer, cache).get_data()),
                                                                   destination);
            }
        },
        element.get());
}

template<IsContext C_SRC, IsContext C_DST>
auto merge(View<Data<ArithmeticOperator<Data<GroundFunctionExpression>>>, C_SRC> element,
           Builder& builder,
           C_DST& destination,
           buffer::Buffer& buffer,
           MergeCache<C_SRC, C_DST>& cache)
{
    return visit(
        [&](auto&& arg)
        {
            return View<Data<ArithmeticOperator<Data<GroundFunctionExpression>>>, C_DST>(
                Data<ArithmeticOperator<Data<GroundFunctionExpression>>>(merge(arg, builder, destination, buffer, cache).get_data()),
                destination);
        },
        element.get());
}

template<IsContext C_SRC, IsContext C_DST>
auto merge(View<Data<BooleanOperator<Data<GroundFunctionExpression>>>, C_SRC> element,
           Builder& builder,
           C_DST& destination,
           buffer::Buffer& buffer,
           MergeCache<C_SRC, C_DST>& cache)
{
    return visit(
        [&](auto&& arg)
        {
            return View<Data<BooleanOperator<Data<GroundFunctionExpression>>>, C_DST>(
                Data<BooleanOperator<Data<GroundFunctionExpression>>>(merge(arg, builder, destination, buffer, cache).get_data()),
                destination);
        },
        element.get());
}

template<IsContext C_SRC, IsContext C_DST>
auto merge(View<Index<GroundConjunctiveCondition>, C_SRC> element,
           Builder& builder,
           C_DST& destination,
           buffer::Buffer& buffer,
           MergeCache<C_SRC, C_DST>& cache)
{
    auto& t_cache = cache.template get<GroundConjunctiveCondition>();

    if (auto it = t_cache.find(element); it != t_cache.end())
        return it->second;

    auto& conj_cond = builder.get_ground_conj_cond();

    conj_cond.objects.clear();
    conj_cond.static_literals.clear();
    conj_cond.fluent_literals.clear();
    conj_cond.numeric_constraints.clear();

    conj_cond.index = element.get_index();
    for (const auto object : element.get_objects())
        conj_cond.objects.push_back(merge(object, builder, destination, buffer, cache).get_data());
    for (const auto literal : element.template get_literals<StaticTag>())
        conj_cond.static_literals.push_back(merge(literal, builder, destination, buffer, cache).get_data());
    for (const auto literal : element.template get_literals<FluentTag>())
        conj_cond.fluent_literals.push_back(merge(literal, builder, destination, buffer, cache).get_data());
    for (const auto numeric_constraint : element.get_numeric_constraints())
        conj_cond.numeric_constraints.push_back(merge(numeric_constraint, builder, destination, buffer, cache).get_data());

    canonicalize(conj_cond);
    auto result = destination.get_or_create(conj_cond, buffer).first;

    t_cache.emplace(element, result);

    return result;
}

template<IsContext C_SRC, IsContext C_DST>
auto merge(View<Index<GroundRule>, C_SRC> element, Builder& builder, C_DST& destination, buffer::Buffer& buffer, MergeCache<C_SRC, C_DST>& cache)
{
    auto& t_cache = cache.template get<GroundRule>();

    if (auto it = t_cache.find(element); it != t_cache.end())
        return it->second;

    auto& rule = builder.get_ground_rule();

    rule.index = element.get_index();
    rule.body = merge(element.get_body(), builder, destination, buffer, cache).get_data();
    rule.head = merge(element.get_head(), builder, destination, buffer, cache).get_data();

    canonicalize(rule);
    auto result = destination.get_or_create(rule, buffer).first;

    t_cache.emplace(element, result);

    return result;
}
}

#endif