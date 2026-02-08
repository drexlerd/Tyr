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

#ifndef TYR_DATALOG_STATISTICS_RULE_HPP_
#define TYR_DATALOG_STATISTICS_RULE_HPP_

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace tyr::datalog
{
struct RuleStatistics
{
    uint64_t num_executions = 0;
    uint64_t num_bindings = 0;
    std::chrono::nanoseconds generate_clique_time { 0 };
    std::chrono::nanoseconds initialize_time { 0 };
    std::chrono::nanoseconds clear_iteration_time { 0 };
    std::chrono::nanoseconds process_generate_time { 0 };
    std::chrono::nanoseconds process_pending_time { 0 };
    std::chrono::nanoseconds parallel_time { 0 };
    std::chrono::nanoseconds total_time { 0 };
};

struct RuleWorkerStatistics
{
    uint64_t num_executions = 0;
    std::chrono::nanoseconds process_clique_time { 0 };
    std::chrono::nanoseconds process_pending_time { 0 };
    std::chrono::nanoseconds total_time { 0 };
};

struct AggregatedRuleStatistics
{
    uint64_t num_executions = 0;
    uint64_t num_bindings = 0;
    std::chrono::nanoseconds generate_clique_time { 0 };
    std::chrono::nanoseconds initialize_time { 0 };
    std::chrono::nanoseconds clear_iteration_time { 0 };
    std::chrono::nanoseconds process_generate_time { 0 };
    std::chrono::nanoseconds process_pending_time { 0 };
    std::chrono::nanoseconds parallel_time { 0 };
    std::chrono::nanoseconds total_time { 0 };

    size_t sample_count { 0 };
    std::chrono::nanoseconds tot_parallel_time_min { 0 };
    std::chrono::nanoseconds tot_parallel_time_max { 0 };
    std::chrono::nanoseconds tot_parallel_time_median { 0 };
    std::chrono::nanoseconds avg_parallel_time_min { 0 };
    std::chrono::nanoseconds avg_parallel_time_max { 0 };
    std::chrono::nanoseconds avg_parallel_time_median { 0 };
};

struct AggregatedRuleWorkerStatistics
{
    uint64_t num_executions = 0;
    std::chrono::nanoseconds process_clique_time { 0 };
    std::chrono::nanoseconds process_pending_time { 0 };
    std::chrono::nanoseconds total_time { 0 };
};

inline AggregatedRuleStatistics compute_aggregated_rule_statistics(const std::vector<datalog::RuleStatistics>& rules)
{
    AggregatedRuleStatistics result {};

    std::vector<std::chrono::nanoseconds> samples;
    samples.reserve(rules.size());
    std::vector<std::chrono::nanoseconds> avg_samples;
    avg_samples.reserve(rules.size());

    for (const auto& rs : rules)
    {
        if (rs.num_executions == 0)
            continue;

        samples.push_back(rs.parallel_time);
        avg_samples.push_back(rs.parallel_time / rs.num_executions);
        result.num_executions += rs.num_executions;
        result.num_bindings += rs.num_bindings;
        result.generate_clique_time += rs.generate_clique_time;
        result.parallel_time += rs.parallel_time;
        result.total_time += rs.total_time;
        result.initialize_time += rs.initialize_time;
        result.clear_iteration_time += rs.clear_iteration_time;
        result.process_generate_time += rs.process_generate_time;
        result.process_pending_time += rs.process_pending_time;
    }

    result.sample_count = samples.size();
    if (samples.empty())
        return result;

    std::sort(samples.begin(), samples.end(), [](auto a, auto b) { return a.count() < b.count(); });
    std::sort(avg_samples.begin(), avg_samples.end(), [](auto a, auto b) { return a.count() < b.count(); });

    result.tot_parallel_time_min = samples.front();
    result.tot_parallel_time_max = samples.back();

    result.avg_parallel_time_min = avg_samples.front();
    result.avg_parallel_time_max = avg_samples.back();

    const std::size_t n = samples.size();
    if (n % 2 == 1)
    {
        result.tot_parallel_time_median = samples[n / 2];
        result.avg_parallel_time_median = avg_samples[n / 2];
    }
    else
    {
        {
            const auto a = samples[n / 2 - 1].count();
            const auto b = samples[n / 2].count();
            result.tot_parallel_time_median = std::chrono::nanoseconds { (a + b) / 2 };
        }
        {
            const auto a = avg_samples[n / 2 - 1].count();
            const auto b = avg_samples[n / 2].count();
            result.avg_parallel_time_median = std::chrono::nanoseconds { (a + b) / 2 };
        }
    }

    return result;
}

inline AggregatedRuleWorkerStatistics compute_aggregated_rule_statistics(const std::vector<datalog::RuleWorkerStatistics>& rules)
{
    AggregatedRuleWorkerStatistics result {};

    for (const auto& rs : rules)
    {
        if (rs.num_executions == 0)
            continue;

        result.num_executions += rs.num_executions;
        result.process_clique_time += rs.process_clique_time;
        result.process_pending_time += rs.process_pending_time;
        result.total_time += rs.total_time;
    }

    return result;
}

}

#endif
