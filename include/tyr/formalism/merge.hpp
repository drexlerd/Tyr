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
class MergeCache
{
private:
    template<typename Tag>
    using MapEntryType = boost::hana::pair<boost::hana::type<Tag>, UnorderedMap<Index<Tag>, Index<Tag>>>;

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

template<IsContext C_DST>
auto merge(float_t element, C_DST&, MergeCache&)
{
    return element;
}

template<IsOp O, IsContext C_SRC, IsContext C_DST>
auto merge(View<Index<UnaryOperator<O, Data<GroundFunctionExpression>>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache& cache)
{
}

template<IsOp O, IsContext C_SRC, IsContext C_DST>
auto merge(View<Index<BinaryOperator<O, Data<GroundFunctionExpression>>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache& cache)
{
}

template<IsOp O, IsContext C_SRC, IsContext C_DST>
auto merge(View<Index<MultiOperator<O, Data<GroundFunctionExpression>>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache& cache)
{
}

template<IsStaticOrFluentTag T, IsContext C_SRC, IsContext C_DST>
auto merge(View<Index<GroundAtom<T>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache& cache)
{
}

template<IsStaticOrFluentTag T, IsContext C_SRC, IsContext C_DST>
auto merge(View<Index<GroundFunctionTerm<T>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache& cache)
{
}

template<IsContext C_SRC, IsContext C_DST>
auto merge(View<Data<GroundFunctionExpression>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache& cache)
{
}

template<IsContext C_SRC, IsContext C_DST>
auto merge(View<Data<ArithmeticOperator<Data<GroundFunctionExpression>>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache& cache)
{
}

template<IsContext C_SRC, IsContext C_DST>
auto merge(View<Data<BooleanOperator<Data<GroundFunctionExpression>>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache& cache)
{
}

template<IsContext C_SRC, IsContext C_DST>
auto merge(View<Index<GroundRule>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache& cache)
{
}
}

#endif