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

/// @brief Make Repository a trivial context.
/// @param context
/// @return
inline const Repository& get_repository(const Repository& context) noexcept { return context; }

/// @brief Make ScopedRepository a trivial context.
/// @param context
/// @return
inline const ScopedRepository& get_repository(const ScopedRepository& context) noexcept { return context; }

template<typename Repo, typename Tag>
concept HasRepositoryAccessFor = requires(const Repo& r, Index<Tag> idx) {
    { r[idx] } -> std::same_as<const Data<Tag>&>;
};

template<typename T>
concept IsRepository = HasRepositoryAccessFor<T, Variable>;

template<typename T>
concept IsContext = requires(const T& a) {
    { get_repository(a) } -> IsRepository;
};

}

#endif
