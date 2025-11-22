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
struct OpMul
{
};
struct OpDiv
{
};
struct OpSub
{
};

template<typename T>
concept IsOp = std::same_as<T, OpEq> || std::same_as<T, OpLe> || std::same_as<T, OpLt> || std::same_as<T, OpGe> || std::same_as<T, OpGt>
               || std::same_as<T, OpAdd> || std::same_as<T, OpMul> || std::same_as<T, OpDiv> || std::same_as<T, OpSub>;

/**
 * Forward declarations
 */

class Repository;
class ScopedRepository;

/**
 * Index types
 */

template<IsOp Op, typename T>
struct UnaryOperatorIndex;

template<IsOp Op, typename T>
struct BinaryOperatorIndex;

template<IsOp Op, typename T>
struct MultiOperatorIndex;

template<typename T>
struct ArithmeticOperator;

template<typename T>
struct BooleanOperator;

struct VariableIndex;

struct ObjectIndex;

struct Term;

template<IsStaticOrFluentTag T>
struct PredicateIndex;

template<IsStaticOrFluentTag T>
struct AtomIndex;

template<IsStaticOrFluentTag T>
struct LiteralIndex;

template<IsStaticOrFluentTag T>
struct GroundAtomIndex;

template<IsStaticOrFluentTag T>
struct GroundLiteralIndex;

template<IsStaticOrFluentTag T>
struct FunctionIndex;

template<IsStaticOrFluentTag T>
struct FunctionTermIndex;

template<IsStaticOrFluentTag T>
struct GroundFunctionTermIndex;

template<IsStaticOrFluentTag T>
struct GroundFunctionTermValueIndex;

struct FunctionExpression;

struct GroundFunctionExpression;

struct RuleIndex;

struct GroundRuleIndex;

struct ProgramIndex;

/**
 * Data types
 */

struct Double;

template<IsOp Op, typename T>
struct UnaryOperator;

template<IsOp Op, typename T>
struct BinaryOperator;

template<IsOp Op, typename T>
struct MultiOperator;

struct Variable;

struct Object;

template<IsStaticOrFluentTag T>
struct Predicate;

template<IsStaticOrFluentTag T>
struct Atom;

template<IsStaticOrFluentTag T>
struct Literal;

template<IsStaticOrFluentTag T>
struct GroundAtom;

template<IsStaticOrFluentTag T>
struct GroundLiteral;

template<IsStaticOrFluentTag T>
struct Function;

template<IsStaticOrFluentTag T>
struct FunctionTerm;

template<IsStaticOrFluentTag T>
struct GroundFunctionTerm;

template<IsStaticOrFluentTag T>
struct GroundFunctionTermValue;

struct Rule;

struct GroundRule;

struct Program;

/**
 * Context
 */

/// @brief Make Repository a trivial context.
/// @param context
/// @return
inline const Repository& get_repository(const Repository& context) noexcept { return context; }

/// @brief Make ScopedRepository a trivial context.
/// @param context
/// @return
inline const ScopedRepository& get_repository(const ScopedRepository& context) noexcept { return context; }

template<typename Repo, typename Data>
concept HasRepositoryAccessFor = requires(const Repo& r, typename DataTraits<Data>::IndexType idx) {
    { r[idx] } -> std::same_as<const Data&>;
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

/**
 * Proxies
 */

template<IsOp Op, typename T, IsContext C = Repository>
class UnaryOperatorProxy;

template<IsOp Op, typename T, IsContext C = Repository>
class BinaryOperatorProxy;

template<IsOp Op, typename T, IsContext C = Repository>
class MultiOperatorProxy;

template<typename T, IsContext C = Repository>
class ArithmeticOperatorProxy;

template<typename T, IsContext C = Repository>
class BooleanOperatorProxy;

template<IsContext C = Repository>
class VariableProxy;

template<IsContext C = Repository>
class ObjectProxy;

struct Term;
template<IsContext C = Repository>
class TermProxy;

template<IsStaticOrFluentTag T, IsContext C = Repository>
class PredicateProxy;

template<IsStaticOrFluentTag T, IsContext C = Repository>
class AtomProxy;

template<IsStaticOrFluentTag T, IsContext C = Repository>
class LiteralProxy;

template<IsStaticOrFluentTag T, IsContext C = Repository>
class GroundAtomProxy;

template<IsStaticOrFluentTag T, IsContext C = Repository>
class GroundLiteralProxy;

template<IsStaticOrFluentTag T, IsContext C = Repository>
class FunctionProxy;

template<IsStaticOrFluentTag T, IsContext C = Repository>
class FunctionTermProxy;

template<IsStaticOrFluentTag T, IsContext C = Repository>
class GroundFunctionTermProxy;

template<IsStaticOrFluentTag T, IsContext C = Repository>
class GroundFunctionTermValueProxy;

template<IsContext C = Repository>
class FunctionExpressionProxy;

template<IsContext C = Repository>
class GroundFunctionExpressionProxy;

template<IsContext C = Repository>
class RuleProxy;

template<IsContext C = Repository>
class GroundRuleProxy;

template<IsContext C = Repository>
class ProgramProxy;

}

#endif
