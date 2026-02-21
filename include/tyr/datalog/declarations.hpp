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

#ifndef TYR_DATALOG_DECLARATIONS_HPP_
#define TYR_DATALOG_DECLARATIONS_HPP_

#include "tyr/formalism/declarations.hpp"

namespace tyr::datalog
{
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

class StaticConsistencyGraph;

struct D2PWorkspace;
struct FactsWorkspace;
struct ConstFactsWorkspace;
struct P2DWorkspace;
template<typename OrAP, typename AndAP, typename TP>
struct ProgramWorkspace;
struct ConstProgramWorkspace;
template<typename AndAP>
struct RuleWorkspace;
struct ConstRuleWorkspace;

class RuleSchedulerStratum;

struct ProgramStatistics;
struct RuleStatistics;
struct AggregatedRuleStatistics;

namespace details
{
class Vertex;
class Edge;

struct InfoMappings;
struct PositionMappings;
struct ParameterMappings;
template<formalism::FactKind T>
struct LiteralInfo;
template<formalism::FactKind T>
struct TaggedIndexedLiterals;
struct IndexedLiterals;
struct LiteralAnchorInfo;
struct IndexedAnchors;
}

struct VertexAssignment;
struct EdgeAssignment;

namespace kpkc
{
struct Workspace;
struct Vertex;
struct VertexPartitions;
class DeduplicatedAdjacencyMatrix;
class PartitionedAdjacencyMatrix;
class DirtyPartitions;
}

}

#endif
