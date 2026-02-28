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

#include "tyr/datalog/formatter.hpp"

#include "tyr/common/chrono.hpp"
#include "tyr/common/formatter.hpp"    // for to_string
#include "tyr/datalog/assignment.hpp"  // for EdgeAssignment, VertexAssignment
#include "tyr/datalog/consistency_graph.hpp"
#include "tyr/datalog/delta_kpkc.hpp"
#include "tyr/datalog/delta_kpkc_graph.hpp"
#include "tyr/datalog/formatter.hpp"
#include "tyr/datalog/statistics/program.hpp"
#include "tyr/datalog/statistics/rule.hpp"
#include "tyr/formalism/datalog/views.hpp"

#include <fmt/base.h>     // for vformat_to
#include <fmt/format.h>   // for format
#include <fmt/ostream.h>  // for print
#include <fmt/ranges.h>   // for join
#include <ostream>        // for ostream
#include <ranges>         // for transform, views
#include <string>         // for basic_string

namespace tyr
{
std::ostream& print(std::ostream& os, const datalog::VertexAssignment& el)
{
    fmt::print(os, "[{}/{}]", to_string(el.index), to_string(el.object));
    return os;
}

std::ostream& print(std::ostream& os, const datalog::EdgeAssignment& el)
{
    fmt::print(os, "[{}/{}, {}/{}]", to_string(el.first_index), to_string(el.first_object), to_string(el.second_index), to_string(el.second_object));
    return os;
}

std::ostream& print(std::ostream& os, const datalog::details::Vertex& el)
{
    fmt::print(os, "[{}/{}]", to_string(el.get_parameter_index()), to_string(el.get_object_index()));
    return os;
}

std::ostream& print(std::ostream& os, const datalog::details::Edge& el)
{
    fmt::print(os, "[{}, {}]", to_string(el.get_src()), to_string(el.get_dst()));
    return os;
}

std::ostream& print(std::ostream& os, const datalog::details::RuleToLiteralInfoMappings& el)
{
    os << "RuleToLiteralInfoMappings(\n";

    {
        IndentScope scope(os);

        os << print_indent << "parameter to literal infos = " << el.parameter_to_infos << "\n";

        os << print_indent << "parameter pairs to literal infos = " << el.parameter_pairs_to_infos << "\n";

        os << print_indent << "parameter to literal infos with constants = " << el.parameter_to_infos_with_constants << "\n";

        os << print_indent << "literal infos with constants = " << el.infos_with_constants << "\n";

        os << print_indent << "literal infos with constant pairs = " << el.infos_with_constant_pairs << "\n";
    }

    os << ")";

    return os;
}

std::ostream& print(std::ostream& os, const datalog::details::RuleToLiteralPositionMappings& el)
{
    os << "RuleToLiteralPositionMappings(\n";

    {
        IndentScope scope(os);

        os << print_indent << "constant positions = " << el.constant_positions << "\n";

        os << print_indent << "parameter to positions = " << el.parameter_to_positions << "\n";
    }

    os << ")";

    return os;
}

std::ostream& print(std::ostream& os, const datalog::details::LiteralToRuleParameterMapping& el)
{
    os << "LiteralToRuleParameterMapping(\n";

    {
        IndentScope scope(os);

        os << print_indent << "position to parameter = " << el.position_to_parameter << "\n";

        os << print_indent << "position parameter pairs = " << el.position_parameter_pairs << "\n";
    }

    os << ")";

    return os;
}

std::ostream& print(std::ostream& os, const datalog::details::LiteralToRuleInfo& el)
{
    os << "LiteralToRuleInfo(\n";

    {
        IndentScope scope(os);

        os << print_indent << " parameter_mappings = " << el.parameter_mappings << "\n";
    }

    os << ")";

    return os;
}

std::ostream& print(std::ostream& os, const datalog::details::LiteralToRuleInfos& el)
{
    os << "LiteralToRuleInfos(\n";

    {
        IndentScope scope(os);

        os << print_indent << " infos = " << el.infos << "\n";

        os << print_indent << " bound_parameters = " << el.bound_parameters << "\n";
    }

    os << ")";

    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const datalog::details::RuleToLiteralInfo<T>& el)
{
    os << "RuleToLiteralInfo(\n";

    {
        IndentScope scope(os);

        os << print_indent << "predicate = " << el.predicate << "\n";

        os << print_indent << "polarity = " << el.polarity << "\n";

        os << print_indent << "position mappings = " << el.position_mappings << "\n";
    }

    os << ")";

    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const datalog::details::TaggedRuleToLiteralInfos<T>& el)
{
    os << "TaggedRuleToLiteralInfos(\n";

    {
        IndentScope scope(os);

        os << print_indent << "literal infos = " << el.infos << "\n";

        os << print_indent << "info mappings = " << el.info_mappings << "\n";
    }

    os << ")";

    return os;
}

std::ostream& print(std::ostream& os, const datalog::details::RuleToLiteralInfos& el)
{
    os << "RuleToLiteralInfos(\n";

    {
        IndentScope scope(os);

        os << print_indent << "static indexed = " << el.static_indexed << "\n";

        os << print_indent << "fluent indexed = " << el.fluent_indexed << "\n";
    }

    os << ")";

    return os;
}

std::ostream& print(std::ostream& os, const datalog::StaticConsistencyGraph& el)
{
    // fmt::print(
    //     os,
    //     "graph Tree {{\n\n{}\n\n{}\n}}",
    //     fmt::join(el.get_vertices() | std::views::transform([&](auto&& arg) { return fmt::format("n{} [label=\"{}\"];", arg.get_index(), to_string(arg)); }),
    //               "\n"),
    //     fmt::join(el.get_edges()
    //                   | std::views::transform([&](auto&& arg) { return fmt::format("n{} -- n{};", arg.get_src().get_index(), arg.get_dst().get_index()); }),
    //               "\n"));

    return os;
}

std::ostream& print(std::ostream& os, const datalog::kpkc::Vertex& el)
{
    os << el.index;
    return os;
}

std::ostream& print(std::ostream& os, const datalog::kpkc::VertexPartitions& el)
{
    os << "VertexPartitions(\n";

    {
        IndentScope scope(os);

        os << print_indent << "partitions = [";
        for (uint_t p = 0; p < el.layout().k; ++p)
        {
            const auto& info = el.layout().info.infos[p];
            os << BitsetSpan<const uint64_t>(el.data().data() + info.block_offset, info.num_bits) << ", ";
        }
        os << "]\n";
    }

    os << ")";

    return os;
}

std::ostream& print(std::ostream& os, const datalog::kpkc::DeduplicatedAdjacencyMatrix& el)
{
    os << "DeduplicatedAdjacencyMatrix(\n";

    {
        IndentScope scope(os);

        os << print_indent << "adjacency lists = [\n";
        for (uint_t v = 0; v < el.layout().nv; ++v)
        {
            IndentScope scope2(os);
            os << print_indent << v << ": [";

            for (uint_t p = 0; p < el.layout().k; ++p)
                os << el.get_bitset(v, p) << ", ";
            os << "]\n";
        }
        os << "]\n";
    }

    os << ")";

    return os;
}

std::ostream& print(std::ostream& os, const datalog::kpkc::PartitionedAdjacencyMatrix& el)
{
    os << "PartitionedAdjacencyMatrix(\n";

    {
        IndentScope scope(os);

        os << print_indent << "affected partitions = [";
        for (uint_t p = 0; p < el.layout().k; ++p)
        {
            const auto& info = el.layout().info.infos[p];
            os << BitsetSpan<const uint64_t>(el.affected_partitions().data().data() + info.block_offset, info.num_bits) << ", ";
        }
        os << "]\n";

        os << print_indent << "delta partitions = [";
        for (uint_t p = 0; p < el.layout().k; ++p)
        {
            const auto& info = el.layout().info.infos[p];
            os << BitsetSpan<const uint64_t>(el.delta_partitions().data().data() + info.block_offset, info.num_bits) << ", ";
        }
        os << "]\n";

        os << print_indent << "adjacency lists = [\n";
        for (uint_t v = 0; v < el.layout().nv; ++v)
        {
            IndentScope scope2(os);
            os << print_indent << v << ": [";

            for (uint_t p = 0; p < el.layout().k; ++p)
                os << el.get_bitset(v, p) << ", ";
            os << "]\n";
        }
        os << "]\n";
    }

    os << ")";

    return os;
}

std::ostream& print(std::ostream& os, const datalog::ProgramStatistics& el)
{
    const double parallel_ns = static_cast<double>(to_ns(el.parallel_time));
    const double total_ns = static_cast<double>(to_ns(el.total_time));
    const double frac = parallel_ns > 0.0 && total_ns > 0.0 ? parallel_ns / total_ns : 1.0;

    const auto avg_total_us = el.num_executions > 0 ? to_us(el.total_time) / el.num_executions : 0.0;

    fmt::print(os,
               "[ProgramStatistics] N_exec = {:>10}    | executions\n"
               "[ProgramStatistics] T_seq  = {:>10} ms | sequential time\n"
               "[ProgramStatistics] T_par  = {:>10} ms | parallel time\n"
               "[ProgramStatistics] T_tot  = {:>10} ms | total time\n"
               "[ProgramStatistics] T_avg  = {:>10} us | average time\n"
               "[ProgramStatistics] PF     = {:>10.2f}    | parallel fraction (T_par / T_tot)",
               el.num_executions,
               to_ms(el.total_time) - to_ms(el.parallel_time),
               to_ms(el.parallel_time),
               to_ms(el.total_time),
               avg_total_us,
               frac);

    return os;
}

std::ostream& print(std::ostream& os, const datalog::RuleStatistics& el)
{
    const auto avg_total_us = el.num_executions > 0 ? to_us(el.total_time) / el.num_executions : 0.0;

    fmt::print(os,
               "[RuleStatistics] N_exec = {:>10}    | executions\n"
               "[RuleStatistics] T_seq  = {:>10} ms | sequential time\n"
               "[RuleStatistics] T_par  = {:>10} ms | parallel time\n"
               "[RuleStatistics] T_tot  = {:>10} ms | total time\n"
               "[RuleStatistics] T_avg  = {:>10} us | average time",
               el.num_executions,
               to_ms(el.initialize_time) + to_ms(el.process_pending_time),
               to_ms(el.process_generate_time),
               to_ms(el.total_time),
               avg_total_us);

    return os;
}

std::ostream& print(std::ostream& os, const datalog::AggregatedRuleStatistics& el)
{
    const auto avg_total_us = el.num_executions > 0 ? to_us(el.total_time) / el.num_executions : 0.0;

    const double tot_max_ns = static_cast<double>(to_ns(el.tot_time_max));
    const double tot_med_ns = static_cast<double>(to_ns(el.tot_time_median));
    const double tot_skew = tot_max_ns > 0.0 && tot_med_ns > 0.0 ? tot_max_ns / tot_med_ns : 1.0;

    const double avg_max_ns = static_cast<double>(el.avg_time_max.count());
    const double avg_med_ns = static_cast<double>(el.avg_time_median.count());
    const double avg_skew = avg_max_ns > 0.0 && avg_med_ns > 0.0 ? avg_max_ns / avg_med_ns : 1.0;

    const auto parallel_ns = static_cast<double>(to_ns(el.process_generate_time));
    const auto total_ns = static_cast<double>(to_ns(el.total_time));
    const double frac = parallel_ns > 0.0 && total_ns > 0.0 ? parallel_ns / total_ns : 1.0;

    fmt::print(os,
               "[AggregatedRuleStatistics] N_exec     = {:>10}    | executions\n"
               "[AggregatedRuleStatistics] N_samples  = {:>10}    | samples\n"
               "[AggregatedRuleStatistics] T_seq      = {:>10} ms | sequential time\n"
               "[AggregatedRuleStatistics] T_par      = {:>10} ms | parallel time\n"
               "[AggregatedRuleStatistics] T_tot      = {:>10} ms | total time\n"
               "[AggregatedRuleStatistics] T_avg      = {:>10} us | average time\n"
               "[AggregatedRuleStatistics] PF         = {:>10.2f}    | parallel fraction (T_par / T_tot)\n"
               "[AggregatedRuleStatistics] T_tot_min  = {:>10} ms | minimum total time\n"
               "[AggregatedRuleStatistics] T_tot_max  = {:>10} ms | maximum total time\n"
               "[AggregatedRuleStatistics] T_tot_med  = {:>10} ms | median total time\n"
               "[AggregatedRuleStatistics] T_tot_skew = {:>10.2f}    | skew total time (T_tot_max / T_tot_med)\n"
               "[AggregatedRuleStatistics] T_avg_min  = {:>10} us | minimum average time\n"
               "[AggregatedRuleStatistics] T_avg_max  = {:>10} us | maximum average time\n"
               "[AggregatedRuleStatistics] T_avg_med  = {:>10} us | median average time\n"
               "[AggregatedRuleStatistics] T_avg_skew = {:>10.2f}    | skew average time (T_avg_max / T_avg_med)",
               el.num_executions,
               el.sample_count,
               to_ms(el.initialize_time) + to_ms(el.process_pending_time),
               to_ms(el.process_generate_time),
               to_ms(el.total_time),
               avg_total_us,
               frac,
               to_ms(el.tot_time_min),
               to_ms(el.tot_time_max),
               to_ms(el.tot_time_median),
               tot_skew,
               to_us(el.avg_time_min),
               to_us(el.avg_time_max),
               to_us(el.avg_time_median),
               avg_skew);

    return os;
}

std::ostream& print(std::ostream& os, const datalog::RuleWorkerStatistics& el)
{
    const auto pen = static_cast<float_t>(el.num_pending_rules);
    const auto gen = static_cast<float_t>(el.num_generated_rules);
    auto overapproximation_ratio = (gen > 0) ? (pen + gen) / gen : float_t { 1.0 };

    fmt::print(os,
               "[RuleWorkerStatistics] N_exec = {:>10} | executions\n"
               "[RuleWorkerStatistics] N_gen  = {:>10} | generated rules\n"
               "[RuleWorkerStatistics] N_pen  = {:>10} | pending rules\n"
               "[RuleWorkerStatistics] OA     = {:>10.2f} | overapproximation ratio (1 + N_pen / N_gen)",
               el.num_executions,
               el.num_generated_rules,
               el.num_pending_rules,
               overapproximation_ratio);

    return os;
}

std::ostream& print(std::ostream& os, const datalog::AggregatedRuleWorkerStatistics& el)
{
    const auto pen = static_cast<float_t>(el.num_pending_rules);
    const auto gen = static_cast<float_t>(el.num_generated_rules);
    auto overapproximation_ratio = (gen > 0) ? (pen + gen) / gen : float_t { 1.0 };

    fmt::print(os,
               "[AggregatedRuleWorkerStatistics] N_exec =  {:>10} | executions\n"
               "[AggregatedRuleWorkerStatistics] N_gen  =  {:>10} | generated rules\n"
               "[AggregatedRuleWorkerStatistics] N_pen  =  {:>10} | pending rules\n"
               "[AggregatedRuleWorkerStatistics] OA     =  {:>10.2f} | overapproximation ratio (1 + N_pen / N_gen)",
               el.num_executions,
               el.num_generated_rules,
               el.num_pending_rules,
               overapproximation_ratio);

    return os;
}

namespace datalog
{
namespace details
{
std::ostream& operator<<(std::ostream& os, const RuleToLiteralInfoMappings& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const RuleToLiteralPositionMappings& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const LiteralToRuleParameterMapping& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const LiteralToRuleInfo& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const LiteralToRuleInfos& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const Vertex& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const Edge& el) { return print(os, el); }

template<formalism::FactKind T>
std::ostream& operator<<(std::ostream& os, const RuleToLiteralInfo<T>& el)
{
    return print(os, el);
}

template<formalism::FactKind T>
std::ostream& operator<<(std::ostream& os, const TaggedRuleToLiteralInfos<T>& el)
{
    return print(os, el);
}

std::ostream& operator<<(std::ostream& os, const RuleToLiteralInfos& el) { return print(os, el); }

}  // end namespace details

std::ostream& operator<<(std::ostream& os, const VertexAssignment& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const EdgeAssignment& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const StaticConsistencyGraph& el) { return print(os, el); }

namespace kpkc
{
std::ostream& operator<<(std::ostream& os, const Vertex& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const VertexPartitions& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const DeduplicatedAdjacencyMatrix& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const PartitionedAdjacencyMatrix& el) { return print(os, el); }
}

std::ostream& operator<<(std::ostream& os, const ProgramStatistics& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const RuleStatistics& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const AggregatedRuleStatistics& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const RuleWorkerStatistics& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const AggregatedRuleWorkerStatistics& el) { return print(os, el); }
}  // end namespace datalog
}
