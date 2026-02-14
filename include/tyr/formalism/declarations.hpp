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
struct AuxiliaryTag
{
};

template<typename T>
concept FactKind = std::same_as<T, StaticTag> || std::same_as<T, FluentTag> || std::same_as<T, DerivedTag> || std::same_as<T, AuxiliaryTag>;

/**
 * Tags to dispatch operators
 */

struct OpEq
{
    static constexpr int kind = 0;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct OpNe
{
    static constexpr int kind = 1;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct OpLe
{
    static constexpr int kind = 2;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct OpLt
{
    static constexpr int kind = 3;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct OpGe
{
    static constexpr int kind = 4;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct OpGt
{
    static constexpr int kind = 5;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct OpAdd
{
    static constexpr int kind = 0;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct OpSub
{
    static constexpr int kind = 1;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct OpMul
{
    static constexpr int kind = 2;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct OpDiv
{
    static constexpr int kind = 3;
    auto identifying_members() const noexcept { return std::tie(kind); }
};

template<typename T>
concept BooleanOpKind =
    std::same_as<T, OpEq> || std::same_as<T, OpNe> || std::same_as<T, OpLe> || std::same_as<T, OpLt> || std::same_as<T, OpGe> || std::same_as<T, OpGt>;

template<typename T>
concept ArithmeticOpKind = std::same_as<T, OpAdd> || std::same_as<T, OpMul> || std::same_as<T, OpDiv> || std::same_as<T, OpSub>;

template<typename T>
concept OpKind = BooleanOpKind<T> || ArithmeticOpKind<T>;

/**
 * Formalism tag
 */

struct Variable
{
};

struct Object
{
};

struct Binding
{
};

struct Term
{
};

template<FactKind T>
struct Predicate
{
};

template<FactKind T>
struct Function
{
};

struct PositiveTag
{
};

struct NegativeTag
{
};

template<typename T>
concept PolarityKind = std::same_as<T, PositiveTag> || std::same_as<T, NegativeTag>;

}

#endif
