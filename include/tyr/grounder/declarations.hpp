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

#ifndef TYR_GROUNDER_DECLARATIONS_HPP_
#define TYR_GROUNDER_DECLARATIONS_HPP_

#include "tyr/formalism/declarations.hpp"

namespace tyr::grounder
{
struct FactSets;

template<formalism::FactKind T>
class PredicateAssignmentSet;
template<formalism::FactKind T>
class PredicateAssignmentSets;
template<formalism::FactKind T>
class FunctionAssignmentSet;
template<formalism::FactKind T>
class FunctionAssignmentSets;

template<formalism::FactKind T>
struct TaggedAssignmentSets;

struct AssignmentSets;

template<formalism::FactKind T>
class PredicateFactSet;
template<formalism::FactKind T>
class FunctionFactSet;

template<formalism::FactKind T>
struct TaggedFactSets;

struct FactSets;
struct FactsView;

class StaticConsistencyGraph;

namespace details
{
class Vertex;
class Edge;
}

struct VertexAssignment;
struct EdgeAssignment;

struct FactsExecutionContext;
struct RuleExecutionContext;
struct ProgramExecutionContext;
struct ThreadExecutionContext;

namespace kpkc
{
struct DenseKPartiteGraph;
struct Workspace;
}
}

#endif
