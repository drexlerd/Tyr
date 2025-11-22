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

#ifndef TYR_FORMALISM_DATA_TRAITS_HPP_
#define TYR_FORMALISM_DATA_TRAITS_HPP_

#include "tyr/common/type_traits.hpp"
#include "tyr/formalism/declarations.hpp"

namespace tyr
{

template<formalism::IsOp Op, typename T>
struct DataTraits<formalism::UnaryOperator<Op, T>>
{
    using IndexType = formalism::UnaryOperatorIndex<Op, T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::UnaryOperatorProxy<Op, T, C>;
};

template<formalism::IsOp Op, typename T>
struct DataTraits<formalism::BinaryOperator<Op, T>>
{
    using IndexType = formalism::BinaryOperatorIndex<Op, T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::BinaryOperatorProxy<Op, T, C>;
};

template<formalism::IsOp Op, typename T>
struct DataTraits<formalism::MultiOperator<Op, T>>
{
    using IndexType = formalism::MultiOperatorIndex<Op, T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::MultiOperatorProxy<Op, T, C>;
};

template<typename T>
struct DataTraits<formalism::ArithmeticOperator<T>>
{
    using IndexType = formalism::ArithmeticOperator<T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::ArithmeticOperatorProxy<T, C>;
};

template<typename T>
struct DataTraits<formalism::BooleanOperator<T>>
{
    using IndexType = formalism::BooleanOperator<T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::BooleanOperatorProxy<T, C>;
};

template<>
struct DataTraits<formalism::Variable>
{
    using IndexType = formalism::VariableIndex;
    template<formalism::IsContext C>
    using ProxyType = formalism::VariableProxy<C>;
};

template<>
struct DataTraits<formalism::Object>
{
    using IndexType = formalism::ObjectIndex;
    template<formalism::IsContext C>
    using ProxyType = formalism::ObjectProxy<C>;
};

template<>
struct DataTraits<formalism::Term>
{
    using IndexType = formalism::Term;
    template<formalism::IsContext C>
    using ProxyType = formalism::TermProxy<C>;
};

template<formalism::IsStaticOrFluentTag T>
struct DataTraits<formalism::Predicate<T>>
{
    using IndexType = formalism::PredicateIndex<T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::PredicateProxy<T, C>;
};

template<formalism::IsStaticOrFluentTag T>
struct DataTraits<formalism::Atom<T>>
{
    using IndexType = formalism::AtomIndex<T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::AtomProxy<T, C>;
};

template<formalism::IsStaticOrFluentTag T>
struct DataTraits<formalism::Literal<T>>
{
    using IndexType = formalism::LiteralIndex<T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::LiteralProxy<T, C>;
};

template<formalism::IsStaticOrFluentTag T>
struct DataTraits<formalism::GroundAtom<T>>
{
    using IndexType = formalism::GroundAtomIndex<T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::GroundAtomProxy<T, C>;
};

template<formalism::IsStaticOrFluentTag T>
struct DataTraits<formalism::GroundLiteral<T>>
{
    using IndexType = formalism::GroundLiteralIndex<T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::GroundLiteralProxy<T, C>;
};

template<formalism::IsStaticOrFluentTag T>
struct DataTraits<formalism::Function<T>>
{
    using IndexType = formalism::FunctionIndex<T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::FunctionProxy<T, C>;
};

template<formalism::IsStaticOrFluentTag T>
struct DataTraits<formalism::FunctionTerm<T>>
{
    using IndexType = formalism::FunctionTermIndex<T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::FunctionTermProxy<T, C>;
};

template<formalism::IsStaticOrFluentTag T>
struct DataTraits<formalism::GroundFunctionTerm<T>>
{
    using IndexType = formalism::GroundFunctionTermIndex<T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::GroundFunctionTermProxy<T, C>;
};

template<formalism::IsStaticOrFluentTag T>
struct DataTraits<formalism::GroundFunctionTermValue<T>>
{
    using IndexType = formalism::GroundFunctionTermValueIndex<T>;
    template<formalism::IsContext C>
    using ProxyType = formalism::GroundFunctionTermValueProxy<T, C>;
};

template<>
struct DataTraits<formalism::FunctionExpression>
{
    using IndexType = formalism::FunctionExpression;
    template<formalism::IsContext C>
    using ProxyType = formalism::FunctionExpressionProxy<C>;
};

template<>
struct DataTraits<formalism::GroundFunctionExpression>
{
    using IndexType = formalism::GroundFunctionExpression;
    template<formalism::IsContext C>
    using ProxyType = formalism::GroundFunctionExpressionProxy<C>;
};

template<>
struct DataTraits<formalism::Rule>
{
    using IndexType = formalism::RuleIndex;
    template<formalism::IsContext C>
    using ProxyType = formalism::RuleProxy<C>;
};

template<>
struct DataTraits<formalism::GroundRule>
{
    using IndexType = formalism::GroundRuleIndex;
    template<formalism::IsContext C>
    using ProxyType = formalism::GroundRuleProxy<C>;
};

template<>
struct DataTraits<formalism::Program>
{
    using IndexType = formalism::ProgramIndex;
    template<formalism::IsContext C>
    using ProxyType = formalism::ProgramProxy<C>;
};

}

#endif
