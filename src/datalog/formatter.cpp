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

std::ostream& print(std::ostream& os, const datalog::details::InfoMappings& el)
{
    os << "InfoMappings(\n";

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

std::ostream& print(std::ostream& os, const datalog::details::PositionMappings& el)
{
    os << "PositionMappings(\n";

    {
        IndentScope scope(os);

        os << print_indent << "constant positions = " << el.constant_positions << "\n";

        os << print_indent << "parameter to positions = " << el.parameter_to_positions << "\n";
    }

    os << ")";

    return os;
}

std::ostream& print(std::ostream& os, const datalog::details::ParameterMappings& el)
{
    os << "ParameterMappings(\n";

    {
        IndentScope scope(os);

        os << print_indent << "position to parameter = " << el.position_to_parameter << "\n";
    }

    os << ")";

    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const datalog::details::LiteralInfo<T>& el)
{
    os << "LiteralInfo(\n";

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
std::ostream& print(std::ostream& os, const datalog::details::TaggedIndexedLiterals<T>& el)
{
    os << "TaggedIndexedLiterals(\n";

    {
        IndentScope scope(os);

        os << print_indent << "literal infos = " << el.infos << "\n";

        os << print_indent << "info mappings = " << el.info_mappings << "\n";
    }

    os << ")";

    return os;
}

std::ostream& print(std::ostream& os, const datalog::details::IndexedLiterals& el)
{
    os << "IndexedLiterals(\n";

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

std::ostream& print(std::ostream& os, const datalog::ProgramStatistics& el)
{
    const double parallel_ms = static_cast<double>(to_ms(el.parallel_time));
    const double total_ms = static_cast<double>(to_ms(el.total_time));
    const double frac = parallel_ms > 0.0 && total_ms > 0.0 ? parallel_ms / total_ms : 1.0;

    const auto avg_total = el.num_executions > 0 ? to_us(el.total_time) / el.num_executions : 0.0;

    fmt::print(os,
               "[ProgramStatistics] Num executions: {}\n"
               "[ProgramStatistics] T_par - wallclock time inside parallel: {} ms\n"
               "[ProgramStatistics] T_total - wallclock time total: {} ms\n"
               "[ProgramStatistics] T_avg - average wallclock time total: {} us\n"
               "[ProgramStatistics] T_par / T_total - Parallel fraction: {:.2f}",
               el.num_executions,
               to_ms(el.parallel_time),
               to_ms(el.total_time),
               avg_total,
               frac);

    return os;
}

std::ostream& print(std::ostream& os, const datalog::RuleStatistics& el)
{
    fmt::print(os,
               "[RuleStatistics] Num executions: {}\n"
               "[RuleStatistics] Num bindings: {}\n"
               "[RuleStatistics] T_par - wallclock time inside parallel: {} ms\n"
               "[RuleStatistics] T_toal - wallclock time total: {} ms\n",
               el.num_executions,
               el.num_bindings,
               to_ms(el.parallel_time),
               to_ms(el.total_time));

    return os;
}

std::ostream& print(std::ostream& os, const datalog::AggregatedRuleStatistics& el)
{
    const double tot_parallel_max_ms = static_cast<double>(to_ms(el.tot_parallel_time_max));
    const double tot_parallel_med_ms = static_cast<double>(to_ms(el.tot_parallel_time_median));
    const double tot_skew = tot_parallel_max_ms > 0.0 && tot_parallel_med_ms > 0.0 ? tot_parallel_max_ms / tot_parallel_med_ms : 1.0;

    const double avg_parallel_max_ns = static_cast<double>(el.avg_parallel_time_max.count());
    const double avg_parallel_med_ns = static_cast<double>(el.avg_parallel_time_median.count());
    const double avg_skew = avg_parallel_max_ns > 0.0 && avg_parallel_med_ns > 0.0 ? avg_parallel_max_ns / avg_parallel_med_ns : 1.0;

    fmt::print(os,
               "[AggregatedRuleStatistics] Number of executions: {}\n"
               "[AggregatedRuleStatistics] Number of bindings: {}\n"
               "[AggregatedRuleStatistics] Number of samples: {}\n"
               "[AggregatedRuleStatistics] T_clique - total wallclock time inside generate clique: {} ms\n"
               "[AggregatedRuleStatistics] T_initialize - total wallclock time inside initialization of delta kpkc: {} ms\n"
               "[AggregatedRuleStatistics] T_clear_iteration - total wallclock time inside clear iteration: {} ms\n"
               "[AggregatedRuleStatistics] T_process_generate - wallclock time to process generate: {} ms\n"
               "[AggregatedRuleStatistics] T_process_pending - wallclock time to process pending: {} ms\n"
               "[AggregatedRuleStatistics] T_par - total wallclock time inside parallel: {} ms\n"
               "[AggregatedRuleStatistics] T_total - total wallclock time: {} ms\n"
               "[AggregatedRuleStatistics] T_tot_min_par - minimum total wallclock time inside parallel: {} ms\n"
               "[AggregatedRuleStatistics] T_tot_max_par - maximum total wallclock time inside parallel: {} ms\n"
               "[AggregatedRuleStatistics] T_tot_med_par - median total wallclock time inside parallel: {} ms\n"
               "[AggregatedRuleStatistics] T_tot_max_par / T_tot_med_par - Total skew: {:.2f}\n"
               "[AggregatedRuleStatistics] T_avg_min_par - minimum average wallclock time inside parallel: {} ns\n"
               "[AggregatedRuleStatistics] T_avg_max_par - maximum average wallclock time inside parallel: {} ns\n"
               "[AggregatedRuleStatistics] T_avg_med_par - median average wallclock time inside parallel: {} ns\n"
               "[AggregatedRuleStatistics] T_avg_max_par / T_avg_med_par - Average skew: {:.2f}",
               el.num_executions,
               el.num_bindings,
               el.sample_count,
               to_ms(el.generate_clique_time),
               to_ms(el.initialize_time),
               to_ms(el.clear_iteration_time),
               to_ms(el.process_generate_time),
               to_ms(el.process_pending_time),
               to_ms(el.parallel_time),
               to_ms(el.total_time),
               to_ms(el.tot_parallel_time_min),
               to_ms(el.tot_parallel_time_max),
               to_ms(el.tot_parallel_time_median),
               tot_skew,
               el.avg_parallel_time_min.count(),
               el.avg_parallel_time_max.count(),
               el.avg_parallel_time_median.count(),
               avg_skew);

    return os;
}

std::ostream& print(std::ostream& os, const datalog::RuleWorkerStatistics& el)
{
    fmt::print(os,
               "[RuleWorkerStatistics] Num executions: {}\n"
               "[RuleWorkerStatistics] T_process_clique - wallclock time to process cliques: {}\n"
               "[RuleWorkerStatistics] T_process_pending - wallclock time to process pending: {} ms\n"
               "[RuleWorkerStatistics] T_total - wallclock time total: {} ms\n",
               el.num_executions,
               to_ms(el.process_clique_time),
               to_ms(el.process_pending_time),
               to_ms(el.total_time));

    return os;
}

std::ostream& print(std::ostream& os, const datalog::AggregatedRuleWorkerStatistics& el)
{
    fmt::print(os,
               "[AggregatedRuleWorkerStatistics] Num executions: {}\n"
               "[AggregatedRuleWorkerStatistics] T_process_clique - wallclock time to process cliques: {}\n"
               "[AggregatedRuleWorkerStatistics] T_process_pending - wallclock time to process pending: {} ms\n"
               "[AggregatedRuleWorkerStatistics] T_total - wallclock time total: {} ms\n",
               el.num_executions,
               to_ms(el.process_clique_time),
               to_ms(el.process_pending_time),
               to_ms(el.total_time));

    return os;
}

namespace datalog
{
namespace details
{
std::ostream& operator<<(std::ostream& os, const InfoMappings& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const PositionMappings& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const ParameterMappings& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const Vertex& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const Edge& el) { return print(os, el); }

template<formalism::FactKind T>
std::ostream& operator<<(std::ostream& os, const LiteralInfo<T>& el)
{
    return print(os, el);
}

template<formalism::FactKind T>
std::ostream& operator<<(std::ostream& os, const TaggedIndexedLiterals<T>& el)
{
    return print(os, el);
}

std::ostream& operator<<(std::ostream& os, const IndexedLiterals& el) { return print(os, el); }

}  // end namespace details

std::ostream& operator<<(std::ostream& os, const VertexAssignment& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const EdgeAssignment& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const StaticConsistencyGraph& el) { return print(os, el); }

namespace kpkc
{
std::ostream& operator<<(std::ostream& os, const Vertex& el) { return print(os, el); }
}

std::ostream& operator<<(std::ostream& os, const ProgramStatistics& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const RuleStatistics& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const AggregatedRuleStatistics& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const RuleWorkerStatistics& el) { return print(os, el); }

std::ostream& operator<<(std::ostream& os, const AggregatedRuleWorkerStatistics& el) { return print(os, el); }

}  // end namespace datalog
}
