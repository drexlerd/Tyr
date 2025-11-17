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

enum class Constant;
enum class Variable;

template<IsStaticOrFluentTag T>
class RelationIndex;
template<IsStaticOrFluentTag T>
class Relation;

template<IsStaticOrFluentTag T>
class AtomIndex;
template<IsStaticOrFluentTag T>
class Atom;

template<IsStaticOrFluentTag T>
class LiteralIndex;
template<IsStaticOrFluentTag T>
class Literal;

class RuleIndex;
class Rule;

template<IsStaticOrFluentTag T>
class GroundAtomIndex;
template<IsStaticOrFluentTag T>
class GroundAtom;

template<IsStaticOrFluentTag T>
class GroundLiteralIndex;
template<IsStaticOrFluentTag T>
class GroundLiteral;

class GroundRuleIndex;
class GroundRule;
}

#endif
