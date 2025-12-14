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

#ifndef TYR_GROUNDER_EXECUTION_CONTEXTS_HPP_
#define TYR_GROUNDER_EXECUTION_CONTEXTS_HPP_

#include "tyr/analysis/analysis.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/formalism/builder.hpp"
#include "tyr/formalism/merge.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/grounder/consistency_graph.hpp"
#include "tyr/grounder/declarations.hpp"
#include "tyr/grounder/kpkc_utils.hpp"

#include <boost/dynamic_bitset.hpp>
#include <chrono>
#include <oneapi/tbb/enumerable_thread_specific.h>

namespace tyr::grounder
{
struct FactsExecutionContext
{
    FactSets<formalism::Repository> fact_sets;
    AssignmentSets<formalism::Repository> assignment_sets;

    FactsExecutionContext(View<Index<formalism::Program>, formalism::Repository> program, const analysis::ProgramVariableDomains& domains);

    FactsExecutionContext(View<Index<formalism::Program>, formalism::Repository> program,
                          TaggedFactSets<formalism::FluentTag, formalism::Repository> fluent_facts,
                          const analysis::ProgramVariableDomains& domains);

    template<formalism::FactKind T>
    void reset() noexcept;

    void reset() noexcept;

    template<formalism::FactKind T>
    void insert(View<IndexList<formalism::GroundAtom<T>>, formalism::Repository> view);

    template<formalism::FactKind T>
    void insert(View<IndexList<formalism::GroundFunctionTermValue<T>>, formalism::Repository> view);
};

struct RuleStageExecutionContext
{
    formalism::RepositoryPtr repository;  // backup for sequential merge
    UnorderedSet<View<Index<formalism::Binding>, formalism::Repository>> bindings;

    RuleStageExecutionContext();

    void clear() noexcept;
};

struct RuleExecutionContext
{
    /// --- Thread
    const View<Index<formalism::Rule>, formalism::Repository> rule;
    const StaticConsistencyGraph<formalism::Repository> static_consistency_graph;

    kpkc::DenseKPartiteGraph consistency_graph;
    kpkc::Workspace kpkc_workspace;
    std::shared_ptr<formalism::Repository> local;
    formalism::OverlayRepository<formalism::Repository> repository;  // deduplicate with the program repository

    std::vector<View<Index<formalism::Binding>, formalism::Repository>> bindings;

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

    static AggregatedStatistics compute_aggregate_statistics(const std::vector<RuleExecutionContext>& contexts)
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

    RuleExecutionContext(View<Index<formalism::Rule>, formalism::Repository> rule,
                         const analysis::DomainListList& parameter_domains,
                         const TaggedAssignmentSets<formalism::StaticTag, formalism::Repository>& static_assignment_sets,
                         const formalism::Repository& parent);

    void clear() noexcept;

    void initialize(const AssignmentSets<formalism::Repository>& assignment_sets);
};

struct ThreadExecutionContext
{
    IndexList<formalism::Object> binding;

    formalism::Builder builder;

    ThreadExecutionContext() = default;

    void clear() noexcept;
};

struct PlanningExecutionContext
{
    IndexList<formalism::Object> binding_full;
    formalism::EffectFamilyList effect_families;
    boost::dynamic_bitset<> positive_effects;
    boost::dynamic_bitset<> negative_effects;

    PlanningExecutionContext() = default;
};

struct ProgramResultsExecutionContext
{
    UnorderedSet<std::pair<View<Index<formalism::Rule>, formalism::Repository>, View<Index<formalism::Binding>, formalism::Repository>>> rule_binding_pairs;

    void clear() noexcept;
};

struct ProgramExecutionContext
{
    /// --- Program & analysis
    const View<Index<formalism::Program>, formalism::Repository> program;
    const formalism::RepositoryPtr repository;
    const analysis::ProgramVariableDomains& domains;
    const analysis::RuleStrata& strata;
    const analysis::Listeners& listeners;

    /// --- Builder
    formalism::Builder builder;

    /// --- Execution contexts
    FactsExecutionContext facts_execution_context;

    std::vector<RuleExecutionContext> rule_execution_contexts;
    std::vector<RuleStageExecutionContext> rule_stage_execution_contexts;

    oneapi::tbb::enumerable_thread_specific<grounder::ThreadExecutionContext> thread_execution_contexts;

    PlanningExecutionContext planning_execution_context;

    ProgramResultsExecutionContext program_results_execution_context;

    struct Statistics
    {
        std::chrono::nanoseconds ground_seq_total_time { 0 };
        std::chrono::nanoseconds merge_seq_total_time { 0 };
    } statistics;

    ProgramExecutionContext(View<Index<formalism::Program>, formalism::Repository> program,
                            formalism::RepositoryPtr repository,
                            const analysis::ProgramVariableDomains& domains,
                            const analysis::RuleStrata& strata,
                            const analysis::Listeners& listeners);
};

}

#endif
