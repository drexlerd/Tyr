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

extern std::ostream& print(std::ostream& os, const datalog::details::RuleToLiteralInfoMappings& el);

extern std::ostream& print(std::ostream& os, const datalog::details::RuleToLiteralPositionMappings& el);

extern std::ostream& print(std::ostream& os, const datalog::details::LiteralToRuleParameterMapping& el);

extern std::ostream& print(std::ostream& os, const datalog::details::LiteralToRuleInfo& el);

extern std::ostream& print(std::ostream& os, const datalog::details::LiteralToRuleInfos& el);

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const datalog::details::RuleToLiteralInfo<T>& el);

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const datalog::details::TaggedRuleToLiteralInfos<T>& el);

extern std::ostream& print(std::ostream& os, const datalog::details::RuleToLiteralInfos& el);

extern std::ostream& print(std::ostream& os, const datalog::StaticConsistencyGraph& el);

extern std::ostream& print(std::ostream& os, const datalog::kpkc::Vertex& el);

extern std::ostream& print(std::ostream& os, const datalog::kpkc::VertexPartitions& el);

extern std::ostream& print(std::ostream& os, const datalog::kpkc::DeduplicatedAdjacencyMatrix& el);

extern std::ostream& print(std::ostream& os, const datalog::kpkc::PartitionedAdjacencyMatrix& el);

extern std::ostream& print(std::ostream& os, const datalog::ProgramStatistics& el);

extern std::ostream& print(std::ostream& os, const datalog::RuleStatistics& el);

extern std::ostream& print(std::ostream& os, const datalog::AggregatedRuleStatistics& el);

extern std::ostream& print(std::ostream& os, const datalog::RuleWorkerStatistics& el);

extern std::ostream& print(std::ostream& os, const datalog::AggregatedRuleWorkerStatistics& el);

namespace datalog
{
namespace details
{
extern std::ostream& operator<<(std::ostream& os, const Vertex& el);

extern std::ostream& operator<<(std::ostream& os, const Edge& el);

extern std::ostream& operator<<(std::ostream& os, const RuleToLiteralInfoMappings& el);

extern std::ostream& operator<<(std::ostream& os, const RuleToLiteralPositionMappings& el);

extern std::ostream& operator<<(std::ostream& os, const LiteralToRuleParameterMapping& el);

extern std::ostream& operator<<(std::ostream& os, const LiteralToRuleInfo& el);

extern std::ostream& operator<<(std::ostream& os, const LiteralToRuleInfos& el);

template<formalism::FactKind T>
std::ostream& operator<<(std::ostream& os, const RuleToLiteralInfo<T>& el);

template<formalism::FactKind T>
std::ostream& operator<<(std::ostream& os, const TaggedRuleToLiteralInfos<T>& el);

extern std::ostream& operator<<(std::ostream& os, const RuleToLiteralInfos& el);
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