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

struct StaticTag;
struct FluentTag;

template<typename T>
concept IsStaticOrFluentTag = std::same_as<T, StaticTag> || std::same_as<T, FluentTag>;

/**
 * Forward declarations
 */

class Repository;

struct Term;
struct GroundTerm;

struct VariableIndex;
struct Variable;
class VariableProxy;

struct ObjectIndex;
struct Object;
class ObjectProxy;

template<IsStaticOrFluentTag T>
struct PredicateIndex;
template<IsStaticOrFluentTag T>
struct Predicate;
template<IsStaticOrFluentTag T>
class PredicateProxy;

template<IsStaticOrFluentTag T>
struct FunctionIndex;
template<IsStaticOrFluentTag T>
struct Function;
template<IsStaticOrFluentTag T>
class FunctionProxy;

template<IsStaticOrFluentTag T>
struct FunctionTermIndex;
template<IsStaticOrFluentTag T>
struct FunctionTerm;
template<IsStaticOrFluentTag T>
class FunctionTermProxy;

template<IsStaticOrFluentTag T>
struct AtomIndex;
template<IsStaticOrFluentTag T>
struct Atom;
template<IsStaticOrFluentTag T>
class AtomProxy;

template<IsStaticOrFluentTag T>
struct LiteralIndex;
template<IsStaticOrFluentTag T>
struct Literal;
template<IsStaticOrFluentTag T>
class LiteralProxy;

struct RuleIndex;
struct Rule;
class RuleProxy;

template<IsStaticOrFluentTag T>
struct GroundAtomIndex;
template<IsStaticOrFluentTag T>
struct GroundAtom;
template<IsStaticOrFluentTag T>
class GroundAtomProxy;

template<IsStaticOrFluentTag T>
struct GroundLiteralIndex;
template<IsStaticOrFluentTag T>
struct GroundLiteral;
template<IsStaticOrFluentTag T>
class GroundLiteralProxy;

struct GroundRuleIndex;
struct GroundRule;
class GroundRuleProxy;

struct ProgramIndex;
struct Program;
class ProgramProxy;
}

#endif
