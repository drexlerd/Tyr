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
 * Tags to distinguish relations and downstream types
 */

struct StaticTag;
struct FluentTag;

template<typename T>
concept IsStaticOrFluentTag = std::same_as<T, StaticTag> || std::same_as<T, FluentTag>;

/**
 * Forward declarations
 */

struct Term;
struct GroundTerm;

struct VariableIndex;
struct VariableImpl;
using Variable = const VariableImpl*;

struct SymbolIndex;
struct SymbolImpl;
using Symbol = const SymbolImpl*;

template<IsStaticOrFluentTag T>
struct RelationIndex;
template<IsStaticOrFluentTag T>
struct RelationImpl;
template<IsStaticOrFluentTag T>
using Relation = const RelationImpl<T>*;

template<IsStaticOrFluentTag T>
struct AtomIndex;
template<IsStaticOrFluentTag T>
struct AtomImpl;
template<IsStaticOrFluentTag T>
using Atom = const AtomImpl<T>*;

template<IsStaticOrFluentTag T>
struct LiteralIndex;
template<IsStaticOrFluentTag T>
struct LiteralImpl;
template<IsStaticOrFluentTag T>
using Literal = const LiteralImpl<T>*;

struct RuleIndex;
struct RuleImpl;
using Rule = const RuleImpl*;

template<IsStaticOrFluentTag T>
struct GroundAtomIndex;
template<IsStaticOrFluentTag T>
struct GroundAtomImpl;
template<IsStaticOrFluentTag T>
using GroundAtom = const GroundAtomImpl<T>*;

template<IsStaticOrFluentTag T>
struct GroundLiteralIndex;
template<IsStaticOrFluentTag T>
struct GroundLiteralImpl;
template<IsStaticOrFluentTag T>
using GroundLiteral = const GroundLiteralImpl<T>*;

struct GroundRuleIndex;
struct GroundRuleImpl;
using GroundRule = const GroundRuleImpl*;

struct ProgramIndex;
struct ProgramImpl;
using Program = const ProgramImpl*;
}

#endif
