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

#ifndef TYR_DATALOG_WORKSPACES_RULE_HPP_
#define TYR_DATALOG_WORKSPACES_RULE_HPP_

#include "tyr/datalog/consistency_graph.hpp"
#include "tyr/datalog/kpkc_data.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/overlay_repository.hpp"

#include <chrono>
#include <vector>

namespace tyr::datalog
{
struct RuleWorkspace
{
    kpkc::DenseKPartiteGraph consistency_graph;
    kpkc::Workspace kpkc_workspace;

    /// Merge stage into rule execution context
    std::shared_ptr<formalism::datalog::Repository> repository;
    formalism::OverlayRepository<formalism::datalog::Repository> overlay_repository;

    /// Bindings kept from iteration in stage
    IndexList<formalism::Object> binding;
    std::vector<Index<formalism::datalog::GroundAtom<formalism::FluentTag>>> ground_heads;

    struct Statistics
    {
        uint64_t num_executions = 0;
        std::chrono::nanoseconds init_total_time { 0 };
        std::chrono::nanoseconds ground_total_time { 0 };
    } statistics;

    struct AggregatedStatistics
    {
        std::chrono::nanoseconds init_total_time_min { 0 };
        std::chrono::nanoseconds init_total_time_max { 0 };
        std::chrono::nanoseconds init_total_time_median { 0 };

        std::chrono::nanoseconds ground_total_time_min { 0 };
        std::chrono::nanoseconds ground_total_time_max { 0 };
        std::chrono::nanoseconds ground_total_time_median { 0 };
    };

    static AggregatedStatistics compute_aggregate_statistics(const std::vector<RuleWorkspace>& contexts)
    {
        AggregatedStatistics result {};

        std::vector<std::chrono::nanoseconds> init_times;
        std::vector<std::chrono::nanoseconds> ground_times;

        init_times.reserve(contexts.size());
        ground_times.reserve(contexts.size());

        // Collect samples
        for (const auto& ctx : contexts)
        {
            if (ctx.statistics.num_executions == 0)
                continue;

            init_times.push_back(ctx.statistics.init_total_time);
            ground_times.push_back(ctx.statistics.ground_total_time);
        }

        if (init_times.empty())
            return result;  // all zero

        // Sort for min/max/median
        auto compute_stats = [](std::vector<std::chrono::nanoseconds>& v,
                                std::chrono::nanoseconds& out_min,
                                std::chrono::nanoseconds& out_max,
                                std::chrono::nanoseconds& out_median)
        {
            std::sort(v.begin(), v.end(), [](auto a, auto b) { return a.count() < b.count(); });

            out_min = v.front();
            out_max = v.back();

            size_t n = v.size();
            if (n % 2 == 1)
            {
                out_median = v[n / 2];
            }
            else
            {
                // average two middle values
                auto a = v[n / 2 - 1].count();
                auto b = v[n / 2].count();
                out_median = std::chrono::nanoseconds { (a + b) / 2 };
            }
        };

        compute_stats(init_times, result.init_total_time_min, result.init_total_time_max, result.init_total_time_median);

        compute_stats(ground_times, result.ground_total_time_min, result.ground_total_time_max, result.ground_total_time_median);

        return result;
    }

    RuleWorkspace(const formalism::datalog::Repository& parent, const StaticConsistencyGraph& static_consistency_graph);

    void clear() noexcept;

    void initialize(const StaticConsistencyGraph& static_consistency_graph, const AssignmentSets& assignment_sets);
};

struct ConstRuleWorkspace
{
    Index<formalism::datalog::Rule> rule;
    const formalism::datalog::Repository& repository;

    Index<formalism::datalog::Rule> fluent_rule;
    Index<formalism::datalog::GroundConjunctiveCondition> nullary_condition;
    Index<formalism::datalog::ConjunctiveCondition> unary_overapproximation_condition;
    Index<formalism::datalog::ConjunctiveCondition> binary_overapproximation_condition;
    Index<formalism::datalog::ConjunctiveCondition> unary_conflicting_overapproximation_condition;
    Index<formalism::datalog::ConjunctiveCondition> binary_conflicting_overapproximation_condition;

    StaticConsistencyGraph static_consistency_graph;

    auto get_rule() const noexcept { return make_view(rule, repository); }
    auto get_fluent_rule() const noexcept { return make_view(fluent_rule, repository); }
    auto get_nullary_condition() const noexcept { return make_view(nullary_condition, repository); }
    auto get_unary_overapproximation_condition() const noexcept { return make_view(unary_overapproximation_condition, repository); }
    auto get_binary_overapproximation_condition() const noexcept { return make_view(binary_overapproximation_condition, repository); }
    auto get_unary_conflicting_overapproximation_condition() const noexcept { return make_view(unary_conflicting_overapproximation_condition, repository); }
    auto get_binary_conflicting_overapproximation_condition() const noexcept { return make_view(binary_conflicting_overapproximation_condition, repository); }

    ConstRuleWorkspace(Index<formalism::datalog::Rule> rule,
                       formalism::datalog::Repository& repository,
                       const analysis::DomainListList& parameter_domains,
                       const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets);
};
}

#endif
