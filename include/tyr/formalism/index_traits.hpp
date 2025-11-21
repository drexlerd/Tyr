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

#ifndef TYR_FORMALISM_INDEX_TRAITS_HPP_
#define TYR_FORMALISM_INDEX_TRAITS_HPP_

#include "tyr/common/type_traits.hpp"
#include "tyr/formalism/declarations.hpp"

namespace tyr
{

template<formalism::IsOp Op, typename T>
struct IndexTraits<formalism::UnaryOperatorIndex<Op, T>>
{
    using DataType = formalism::UnaryOperator<Op, T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::UnaryOperatorProxy<Op, T, C>;
};

template<formalism::IsOp Op, typename T>
struct IndexTraits<formalism::BinaryOperatorIndex<Op, T>>
{
    using DataType = formalism::BinaryOperator<Op, T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::BinaryOperatorProxy<Op, T, C>;
};

template<formalism::IsOp Op, typename T>
struct IndexTraits<formalism::MultiOperatorIndex<Op, T>>
{
    using DataType = formalism::MultiOperator<Op, T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::MultiOperatorProxy<Op, T, C>;
};

template<typename T>
struct IndexTraits<formalism::ArithmeticOperator<T>>
{
    using DataType = formalism::ArithmeticOperator<T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::ArithmeticOperatorProxy<T, C>;
};

template<typename T>
struct IndexTraits<formalism::BooleanOperator<T>>
{
    using DataType = formalism::BooleanOperator<T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::BooleanOperatorProxy<T, C>;
};

template<>
struct IndexTraits<formalism::VariableIndex>
{
    using DataType = formalism::Variable;
    template<formalism::IsContext C>
    using ProxyType = formalism::VariableProxy<C>;
};

template<>
struct IndexTraits<formalism::ObjectIndex>
{
    using DataType = formalism::Object;
    template<formalism::IsContext C>
    using ProxyType = formalism::ObjectProxy<C>;
};

template<>
struct IndexTraits<formalism::Term>
{
    using DataType = formalism::Term;
    template<formalism::IsContext C>
    using ProxyType = formalism::TermProxy<C>;
};

template<formalism::IsStaticOrFluentTag T>
struct IndexTraits<formalism::PredicateIndex<T>>
{
    using DataType = formalism::Predicate<T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::PredicateProxy<T, C>;
};

template<formalism::IsStaticOrFluentTag T>
struct IndexTraits<formalism::AtomIndex<T>>
{
    using DataType = formalism::Atom<T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::AtomProxy<T, C>;
};

template<formalism::IsStaticOrFluentTag T>
struct IndexTraits<formalism::LiteralIndex<T>>
{
    using DataType = formalism::Literal<T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::LiteralProxy<T, C>;
};

template<formalism::IsStaticOrFluentTag T>
struct IndexTraits<formalism::GroundAtomIndex<T>>
{
    using DataType = formalism::GroundAtom<T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::GroundAtomProxy<T, C>;
};

template<formalism::IsStaticOrFluentTag T>
struct IndexTraits<formalism::GroundLiteralIndex<T>>
{
    using DataType = formalism::GroundLiteral<T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::GroundLiteralProxy<T, C>;
};

template<formalism::IsStaticOrFluentTag T>
struct IndexTraits<formalism::FunctionIndex<T>>
{
    using DataType = formalism::Function<T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::FunctionProxy<T, C>;
};

template<formalism::IsStaticOrFluentTag T>
struct IndexTraits<formalism::FunctionTermIndex<T>>
{
    using DataType = formalism::FunctionTerm<T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::FunctionTermProxy<T, C>;
};

template<formalism::IsStaticOrFluentTag T>
struct IndexTraits<formalism::GroundFunctionTermIndex<T>>
{
    using DataType = formalism::GroundFunctionTerm<T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::GroundFunctionTermProxy<T, C>;
};

template<formalism::IsStaticOrFluentTag T>
struct IndexTraits<formalism::GroundFunctionTermValueIndex<T>>
{
    using DataType = formalism::GroundFunctionTermValue<T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::GroundFunctionTermValueProxy<T, C>;
};

template<>
struct IndexTraits<formalism::FunctionExpression>
{
    using DataType = formalism::FunctionExpression;
    template<formalism::IsContext C>
    using ProxyType = formalism::FunctionExpressionProxy<C>;
};

template<>
struct IndexTraits<formalism::GroundFunctionExpression>
{
    using DataType = formalism::GroundFunctionExpression;
    template<formalism::IsContext C>
    using ProxyType = formalism::GroundFunctionExpressionProxy<C>;
};

template<>
struct IndexTraits<formalism::RuleIndex>
{
    using DataType = formalism::Rule;
    template<formalism::IsContext C>
    using ProxyType = formalism::RuleProxy<C>;
};

template<>
struct IndexTraits<formalism::GroundRuleIndex>
{
    using DataType = formalism::GroundRule;
    template<formalism::IsContext C>
    using ProxyType = formalism::GroundRuleProxy<C>;
};

template<>
struct IndexTraits<formalism::ProgramIndex>
{
    using DataType = formalism::Program;
    template<formalism::IsContext C>
    using ProxyType = formalism::ProgramProxy<C>;
};

}

#endif
