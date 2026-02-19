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

#ifndef TYR_DATALOG_FORMATTER_HPP_
#define TYR_DATALOG_FORMATTER_HPP_

#include "tyr/common/formatter.hpp"
#include "tyr/datalog/declarations.hpp"
#include "tyr/formalism/declarations.hpp"  // for Context

#include <iosfwd>  // for ostream

namespace tyr
{
extern std::ostream& print(std::ostream& os, const datalog::VertexAssignment& el);

extern std::ostream& print(std::ostream& os, const datalog::EdgeAssignment& el);

extern std::ostream& print(std::ostream& os, const datalog::details::Vertex& el);

extern std::ostream& print(std::ostream& os, const datalog::details::Edge& el);

extern std::ostream& print(std::ostream& os, const datalog::details::InfoMappings& el);

extern std::ostream& print(std::ostream& os, const datalog::details::PositionMappings& el);

extern std::ostream& print(std::ostream& os, const datalog::details::ParameterMappings& el);

extern std::ostream& print(std::ostream& os, const datalog::details::LiteralAnchorInfo& el);

extern std::ostream& print(std::ostream& os, const datalog::details::IndexedAnchors& el);

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const datalog::details::LiteralInfo<T>& el);

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const datalog::details::TaggedIndexedLiterals<T>& el);

extern std::ostream& print(std::ostream& os, const datalog::details::IndexedLiterals& el);

extern std::ostream& print(std::ostream& os, const datalog::StaticConsistencyGraph& el);

extern std::ostream& print(std::ostream& os, const datalog::kpkc::Vertex& el);

extern std::ostream& print(std::ostream& os, const datalog::kpkc::VertexPartitions& el);

extern std::ostream& print(std::ostream& os, const datalog::kpkc::DeduplicatedAdjacencyMatrix& el);

extern std::ostream& print(std::ostream& os, const datalog::kpkc::PartitionedAdjacencyMatrix& el);

extern std::ostream& print(std::ostream& os, const datalog::ProgramStatistics& el);

extern std::ostream& print(std::ostream& os, const datalog::RuleStatistics& el);

extern std::ostream& print(std::ostream& os, const datalog::AggregatedRuleStatistics& el);

namespace datalog
{
namespace details
{
extern std::ostream& operator<<(std::ostream& os, const Vertex& el);

extern std::ostream& operator<<(std::ostream& os, const Edge& el);

extern std::ostream& operator<<(std::ostream& os, const InfoMappings& el);

extern std::ostream& operator<<(std::ostream& os, const PositionMappings& el);

extern std::ostream& operator<<(std::ostream& os, const ParameterMappings& el);

extern std::ostream& operator<<(std::ostream& os, const LiteralAnchorInfo& el);

extern std::ostream& operator<<(std::ostream& os, const IndexedAnchors& el);

template<formalism::FactKind T>
std::ostream& operator<<(std::ostream& os, const LiteralInfo<T>& el);

template<formalism::FactKind T>
std::ostream& operator<<(std::ostream& os, const TaggedIndexedLiterals<T>& el);

extern std::ostream& operator<<(std::ostream& os, const IndexedLiterals& el);
}  // end namespace details

extern std::ostream& operator<<(std::ostream& os, const VertexAssignment& el);

extern std::ostream& operator<<(std::ostream& os, const EdgeAssignment& el);

extern std::ostream& operator<<(std::ostream& os, const StaticConsistencyGraph& el);

namespace kpkc
{
extern std::ostream& operator<<(std::ostream& os, const Vertex& el);

extern std::ostream& operator<<(std::ostream& os, const VertexPartitions& el);

extern std::ostream& operator<<(std::ostream& os, const DeduplicatedAdjacencyMatrix& el);

extern std::ostream& operator<<(std::ostream& os, const PartitionedAdjacencyMatrix& el);
}

extern std::ostream& operator<<(std::ostream& os, const ProgramStatistics& el);

extern std::ostream& operator<<(std::ostream& os, const RuleStatistics& el);

extern std::ostream& operator<<(std::ostream& os, const AggregatedRuleStatistics& el);

extern std::ostream& operator<<(std::ostream& os, const RuleWorkerStatistics& el);

extern std::ostream& operator<<(std::ostream& os, const AggregatedRuleWorkerStatistics& el);
}  // end namespace datalog
}

#endif