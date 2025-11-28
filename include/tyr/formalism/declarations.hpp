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
struct DerivedTag
{
};

template<typename T>
concept IsFactTag = std::same_as<T, StaticTag> || std::same_as<T, FluentTag> || std::same_as<T, DerivedTag>;

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

template<IsFactTag T>
struct Predicate
{
};

template<IsFactTag T>
struct Atom
{
};

template<IsFactTag T>
struct Literal
{
};

template<IsFactTag T>
struct GroundAtom
{
};

template<IsFactTag T>
struct GroundLiteral
{
};

template<IsFactTag T>
struct Function
{
};

template<IsFactTag T>
struct FunctionTerm
{
};

struct FunctionExpression
{
};

template<IsFactTag T>
struct GroundFunctionTerm
{
};

struct GroundFunctionExpression
{
};

template<IsFactTag T>
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

namespace planning
{
struct NumericEffect
{
};
struct GroundNumericEffect
{
};

struct ConditionalEffect
{
};
struct GroundConditionalEffect
{
};

struct ConjunctiveEffect
{
};
struct GroundConjunctiveEffect
{
};

struct Action
{
};
struct GroundAction
{
};

struct Axiom
{
};
struct GroundAxiom
{
};

struct Task
{
};
}

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
    && HasRepositoryAccessFor<T, UnaryOperator<OpSub, Data<FunctionExpression>>> && HasRepositoryAccessFor<T, BinaryOperator<OpAdd, Data<FunctionExpression>>>
    && HasRepositoryAccessFor<T, BinaryOperator<OpSub, Data<FunctionExpression>>> && HasRepositoryAccessFor<T, BinaryOperator<OpMul, Data<FunctionExpression>>>
    && HasRepositoryAccessFor<T, BinaryOperator<OpDiv, Data<FunctionExpression>>> && HasRepositoryAccessFor<T, MultiOperator<OpAdd, Data<FunctionExpression>>>
    && HasRepositoryAccessFor<T, MultiOperator<OpMul, Data<FunctionExpression>>> && HasRepositoryAccessFor<T, BinaryOperator<OpEq, Data<FunctionExpression>>>
    && HasRepositoryAccessFor<T, BinaryOperator<OpLe, Data<FunctionExpression>>> && HasRepositoryAccessFor<T, BinaryOperator<OpLt, Data<FunctionExpression>>>
    && HasRepositoryAccessFor<T, BinaryOperator<OpGe, Data<FunctionExpression>>> && HasRepositoryAccessFor<T, BinaryOperator<OpGt, Data<FunctionExpression>>>
    && HasRepositoryAccessFor<T, UnaryOperator<OpSub, Data<GroundFunctionExpression>>>
    && HasRepositoryAccessFor<T, BinaryOperator<OpAdd, Data<GroundFunctionExpression>>>
    && HasRepositoryAccessFor<T, BinaryOperator<OpSub, Data<GroundFunctionExpression>>>
    && HasRepositoryAccessFor<T, BinaryOperator<OpMul, Data<GroundFunctionExpression>>>
    && HasRepositoryAccessFor<T, BinaryOperator<OpDiv, Data<GroundFunctionExpression>>>
    && HasRepositoryAccessFor<T, MultiOperator<OpAdd, Data<GroundFunctionExpression>>>
    && HasRepositoryAccessFor<T, MultiOperator<OpMul, Data<GroundFunctionExpression>>>
    && HasRepositoryAccessFor<T, BinaryOperator<OpEq, Data<GroundFunctionExpression>>>
    && HasRepositoryAccessFor<T, BinaryOperator<OpLe, Data<GroundFunctionExpression>>>
    && HasRepositoryAccessFor<T, BinaryOperator<OpLt, Data<GroundFunctionExpression>>>
    && HasRepositoryAccessFor<T, BinaryOperator<OpGe, Data<GroundFunctionExpression>>>
    && HasRepositoryAccessFor<T, BinaryOperator<OpGt, Data<GroundFunctionExpression>>> && HasRepositoryAccessFor<T, Rule>
    && HasRepositoryAccessFor<T, GroundRule> && HasRepositoryAccessFor<T, Program>;

template<typename T>
concept IsContext = requires(const T& a) {
    { get_repository(a) } -> IsRepository;
};

}

#endif
