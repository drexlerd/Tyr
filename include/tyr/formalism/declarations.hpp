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

#ifndef TYR_FORMALISM_DECLARATIONS_HPP_
#define TYR_FORMALISM_DECLARATIONS_HPP_

#include "tyr/common/config.hpp"
#include "tyr/common/declarations.hpp"
#include "tyr/common/types.hpp"

namespace tyr::formalism
{

/**
 * Tags to distinguish predicates and downstream types
 */

struct StaticTag
{
};
struct FluentTag
{
};

template<typename T>
concept IsStaticOrFluentTag = std::same_as<T, StaticTag> || std::same_as<T, FluentTag>;

/**
 * Tags to dispatch operators
 */

struct OpEq
{
};
struct OpNe
{
};
struct OpLe
{
};
struct OpLt
{
};
struct OpGe
{
};
struct OpGt
{
};
struct OpAdd
{
};
struct OpSub
{
};
struct OpMul
{
};
struct OpDiv
{
};

template<typename T>
concept IsBooleanOp =
    std::same_as<T, OpEq> || std::same_as<T, OpNe> || std::same_as<T, OpLe> || std::same_as<T, OpLt> || std::same_as<T, OpGe> || std::same_as<T, OpGt>;

template<typename T>
concept IsArithmeticOp = std::same_as<T, OpAdd> || std::same_as<T, OpMul> || std::same_as<T, OpDiv> || std::same_as<T, OpSub>;

template<typename T>
concept IsOp = IsBooleanOp<T> || IsArithmeticOp<T>;

/**
 * Formalism tag
 */

template<IsOp Op, typename T>
struct UnaryOperator
{
};

template<IsOp Op, typename T>
struct BinaryOperator
{
};

template<IsOp Op, typename T>
struct MultiOperator
{
};

template<typename T>
class BooleanOperator
{
};
template<typename T>
class ArithmeticOperator
{
};

struct Variable
{
};

struct Object
{
};

struct Term
{
};

template<IsStaticOrFluentTag T>
struct Predicate
{
};

template<IsStaticOrFluentTag T>
struct Atom
{
};

template<IsStaticOrFluentTag T>
struct Literal
{
};

template<IsStaticOrFluentTag T>
struct GroundAtom
{
};

template<IsStaticOrFluentTag T>
struct GroundLiteral
{
};

template<IsStaticOrFluentTag T>
struct Function
{
};

template<IsStaticOrFluentTag T>
struct FunctionTerm
{
};

struct FunctionExpression
{
};

template<IsStaticOrFluentTag T>
struct GroundFunctionTerm
{
};

struct GroundFunctionExpression
{
};

template<IsStaticOrFluentTag T>
struct GroundFunctionTermValue
{
};

struct ConjunctiveCondition
{
};

struct GroundConjunctiveCondition
{
};

struct Rule
{
};

struct GroundRule
{
};

struct Program
{
};

/**
 * Context
 */

template<typename Repo, typename Tag>
concept HasRepositoryAccessFor = requires(const Repo& r, Index<Tag> idx) {
    { r[idx] } -> std::same_as<const Data<Tag>&>;
};

template<typename T>
concept IsRepository =
    HasRepositoryAccessFor<T, Variable> && HasRepositoryAccessFor<T, Object> && HasRepositoryAccessFor<T, Predicate<StaticTag>>
    && HasRepositoryAccessFor<T, Predicate<FluentTag>> && HasRepositoryAccessFor<T, Atom<StaticTag>> && HasRepositoryAccessFor<T, Atom<FluentTag>>
    && HasRepositoryAccessFor<T, GroundAtom<StaticTag>> && HasRepositoryAccessFor<T, GroundAtom<FluentTag>> && HasRepositoryAccessFor<T, Literal<StaticTag>>
    && HasRepositoryAccessFor<T, Literal<FluentTag>> && HasRepositoryAccessFor<T, GroundLiteral<StaticTag>>
    && HasRepositoryAccessFor<T, GroundLiteral<FluentTag>> && HasRepositoryAccessFor<T, Function<StaticTag>> && HasRepositoryAccessFor<T, Function<FluentTag>>
    && HasRepositoryAccessFor<T, FunctionTerm<StaticTag>> && HasRepositoryAccessFor<T, FunctionTerm<FluentTag>>
    && HasRepositoryAccessFor<T, GroundFunctionTerm<StaticTag>> && HasRepositoryAccessFor<T, GroundFunctionTerm<FluentTag>>
    && HasRepositoryAccessFor<T, GroundFunctionTermValue<StaticTag>> && HasRepositoryAccessFor<T, GroundFunctionTermValue<FluentTag>>
    && HasRepositoryAccessFor<T, UnaryOperator<OpSub, FunctionExpression>> && HasRepositoryAccessFor<T, BinaryOperator<OpAdd, FunctionExpression>>
    && HasRepositoryAccessFor<T, BinaryOperator<OpSub, FunctionExpression>> && HasRepositoryAccessFor<T, BinaryOperator<OpMul, FunctionExpression>>
    && HasRepositoryAccessFor<T, BinaryOperator<OpDiv, FunctionExpression>> && HasRepositoryAccessFor<T, MultiOperator<OpAdd, FunctionExpression>>
    && HasRepositoryAccessFor<T, MultiOperator<OpMul, FunctionExpression>> && HasRepositoryAccessFor<T, BinaryOperator<OpEq, FunctionExpression>>
    && HasRepositoryAccessFor<T, BinaryOperator<OpLe, FunctionExpression>> && HasRepositoryAccessFor<T, BinaryOperator<OpLt, FunctionExpression>>
    && HasRepositoryAccessFor<T, BinaryOperator<OpGe, FunctionExpression>> && HasRepositoryAccessFor<T, BinaryOperator<OpGt, FunctionExpression>>
    && HasRepositoryAccessFor<T, UnaryOperator<OpSub, GroundFunctionExpression>> && HasRepositoryAccessFor<T, BinaryOperator<OpAdd, GroundFunctionExpression>>
    && HasRepositoryAccessFor<T, BinaryOperator<OpSub, GroundFunctionExpression>> && HasRepositoryAccessFor<T, BinaryOperator<OpMul, GroundFunctionExpression>>
    && HasRepositoryAccessFor<T, BinaryOperator<OpDiv, GroundFunctionExpression>> && HasRepositoryAccessFor<T, MultiOperator<OpAdd, GroundFunctionExpression>>
    && HasRepositoryAccessFor<T, MultiOperator<OpMul, GroundFunctionExpression>> && HasRepositoryAccessFor<T, BinaryOperator<OpEq, GroundFunctionExpression>>
    && HasRepositoryAccessFor<T, BinaryOperator<OpLe, GroundFunctionExpression>> && HasRepositoryAccessFor<T, BinaryOperator<OpLt, GroundFunctionExpression>>
    && HasRepositoryAccessFor<T, BinaryOperator<OpGe, GroundFunctionExpression>> && HasRepositoryAccessFor<T, BinaryOperator<OpGt, GroundFunctionExpression>>
    && HasRepositoryAccessFor<T, Rule> && HasRepositoryAccessFor<T, GroundRule> && HasRepositoryAccessFor<T, Program>;

template<typename T>
concept IsContext = requires(const T& a) {
    { get_repository(a) } -> IsRepository;
};

}

#endif
