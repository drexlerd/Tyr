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

#ifndef TYR_FORMALISM_PROXY_TRAITS_HPP_
#define TYR_FORMALISM_PROXY_TRAITS_HPP_

#include "tyr/common/type_traits.hpp"
#include "tyr/formalism/declarations.hpp"

namespace tyr
{

template<formalism::IsOp Op, typename T, formalism::IsContext C>
struct ProxyTraits<formalism::UnaryOperatorProxy<Op, T, C>>
{
    using IndexType = formalism::UnaryOperatorIndex<Op, T>;
    using DataType = formalism::UnaryOperator<Op, T>;
};

template<formalism::IsOp Op, typename T, formalism::IsContext C>
struct ProxyTraits<formalism::BinaryOperatorProxy<Op, T, C>>
{
    using IndexType = formalism::BinaryOperatorIndex<Op, T>;
    using DataType = formalism::BinaryOperator<Op, T>;
};

template<formalism::IsOp Op, typename T, formalism::IsContext C>
struct ProxyTraits<formalism::MultiOperatorProxy<Op, T, C>>
{
    using IndexType = formalism::MultiOperatorIndex<Op, T>;
    using DataType = formalism::MultiOperator<Op, T>;
};

template<typename T, formalism::IsContext C>
struct ProxyTraits<formalism::ArithmeticOperatorProxy<T, C>>
{
    using IndexType = formalism::ArithmeticOperator<T>;
    using DataType = formalism::ArithmeticOperator<T>;
};

template<typename T, formalism::IsContext C>
struct ProxyTraits<formalism::BooleanOperatorProxy<T, C>>
{
    using IndexType = formalism::BooleanOperator<T>;
    using DataType = formalism::BooleanOperator<T>;
};

template<formalism::IsContext C>
struct ProxyTraits<formalism::VariableProxy<C>>
{
    using IndexType = formalism::VariableIndex;
    using DataType = formalism::Variable;
};

template<formalism::IsContext C>
struct ProxyTraits<formalism::ObjectProxy<C>>
{
    using IndexType = formalism::ObjectIndex;
    using DataType = formalism::Object;
};

template<formalism::IsContext C>
struct ProxyTraits<formalism::TermProxy<C>>
{
    using IndexType = formalism::Term;
    using DataType = formalism::Term;
};

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
struct ProxyTraits<formalism::PredicateProxy<T, C>>
{
    using IndexType = formalism::PredicateIndex<T>;
    using DataType = formalism::Predicate<T>;
};

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
struct ProxyTraits<formalism::AtomProxy<T, C>>
{
    using IndexType = formalism::AtomIndex<T>;
    using DataType = formalism::Atom<T>;
};

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
struct ProxyTraits<formalism::LiteralProxy<T, C>>
{
    using IndexType = formalism::LiteralIndex<T>;
    using DataType = formalism::Literal<T>;
};

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
struct ProxyTraits<formalism::GroundAtomProxy<T, C>>
{
    using IndexType = formalism::GroundAtomIndex<T>;
    using DataType = formalism::GroundAtom<T>;
};

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
struct ProxyTraits<formalism::GroundLiteralProxy<T, C>>
{
    using IndexType = formalism::GroundLiteralIndex<T>;
    using DataType = formalism::GroundLiteral<T>;
};

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
struct ProxyTraits<formalism::FunctionProxy<T, C>>
{
    using IndexType = formalism::FunctionIndex<T>;
    using DataType = formalism::Function<T>;
};

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
struct ProxyTraits<formalism::FunctionTermProxy<T, C>>
{
    using IndexType = formalism::FunctionTermIndex<T>;
    using DataType = formalism::FunctionTerm<T>;
};

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
struct ProxyTraits<formalism::GroundFunctionTermProxy<T, C>>
{
    using IndexType = formalism::GroundFunctionTermIndex<T>;
    using DataType = formalism::GroundFunctionTerm<T>;
};

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
struct ProxyTraits<formalism::GroundFunctionTermValueProxy<T, C>>
{
    using IndexType = formalism::GroundFunctionTermValueIndex<T>;
    using DataType = formalism::GroundFunctionTermValue<T>;
};

template<formalism::IsContext C>
struct ProxyTraits<formalism::FunctionExpressionProxy<C>>
{
    using IndexType = formalism::FunctionExpression;
    using DataType = formalism::FunctionExpression;
};

template<formalism::IsContext C>
struct ProxyTraits<formalism::GroundFunctionExpressionProxy<C>>
{
    using IndexType = formalism::GroundFunctionExpression;
    using DataType = formalism::GroundFunctionExpression;
};

template<formalism::IsContext C>
struct ProxyTraits<formalism::RuleProxy<C>>
{
    using IndexType = formalism::RuleIndex;
    using DataType = formalism::Rule;
};

template<formalism::IsContext C>
struct ProxyTraits<formalism::GroundRuleProxy<C>>
{
    using IndexType = formalism::GroundRuleIndex;
    using DataType = formalism::GroundRule;
};

template<formalism::IsContext C>
struct ProxyTraits<formalism::ProgramProxy<C>>
{
    using IndexType = formalism::ProgramIndex;
    using DataType = formalism::Program;
};

}

#endif
