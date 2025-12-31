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

#ifndef TYR_DATALOG_EXECUTION_CONTEXTS_HPP_
#define TYR_DATALOG_EXECUTION_CONTEXTS_HPP_

#include "tyr/analysis/analysis.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/itertools.hpp"
#include "tyr/datalog/consistency_graph.hpp"
#include "tyr/datalog/declarations.hpp"
#include "tyr/datalog/kpkc_utils.hpp"
#include "tyr/datalog/rule_scheduler.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/grounder.hpp"
#include "tyr/formalism/datalog/merge.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/planning/builder.hpp"
#include "tyr/formalism/planning/grounder.hpp"
#include "tyr/formalism/planning/merge.hpp"
#include "tyr/formalism/planning/merge_datalog.hpp"
#include "tyr/formalism/planning/merge_planning.hpp"
#include "tyr/formalism/planning/repository.hpp"

#include <boost/dynamic_bitset.hpp>
#include <chrono>
#include <oneapi/tbb/enumerable_thread_specific.h>

namespace tyr::datalog
{
struct FactsExecutionContext
{
    FactSets fact_sets;
    AssignmentSets assignment_sets;

    FactsExecutionContext(View<Index<formalism::datalog::Program>, formalism::datalog::Repository> program, const analysis::ProgramVariableDomains& domains);

    FactsExecutionContext(View<Index<formalism::datalog::Program>, formalism::datalog::Repository> program,
                          TaggedFactSets<formalism::FluentTag> fluent_facts,
                          const analysis::ProgramVariableDomains& domains);

    template<formalism::FactKind T>
    void reset() noexcept;

    void reset() noexcept;

    template<formalism::FactKind T>
    void insert(View<IndexList<formalism::datalog::GroundAtom<T>>, formalism::datalog::Repository> view);

    template<formalism::FactKind T>
    void insert(View<IndexList<formalism::datalog::GroundFunctionTermValue<T>>, formalism::datalog::Repository> view);
};

struct RuleStageExecutionContext
{
    /// Merge thread into staging area
    formalism::datalog::RepositoryPtr repository;

    /// Ground heads encountered across iterations
    IndexList<formalism::Object> binding;
    UnorderedSet<Index<formalism::datalog::GroundAtom<formalism::FluentTag>>> ground_heads;
    formalism::datalog::MergeCache merge_cache;

    RuleStageExecutionContext();

    void clear() noexcept;
};

struct StaticRuleExecutionContext
{
    View<Index<formalism::datalog::Rule>, formalism::datalog::Repository> rule;
    View<Index<formalism::datalog::GroundConjunctiveCondition>, formalism::datalog::Repository> nullary_condition;
    View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> unary_overapproximation_condition;
    View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> binary_overapproximation_condition;
    View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> unary_conflicting_overapproximation_condition;
    View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> binary_conflicting_overapproximation_condition;
    StaticConsistencyGraph static_consistency_graph;

    static StaticRuleExecutionContext create(View<Index<formalism::datalog::Rule>, formalism::datalog::Repository> rule,
                                             formalism::datalog::Repository& repository,
                                             const analysis::DomainListList& parameter_domains,
                                             const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets);
};

struct RuleExecutionContext
{
    const View<Index<formalism::datalog::Rule>, formalism::datalog::Repository> rule;
    const View<Index<formalism::datalog::GroundConjunctiveCondition>, formalism::datalog::Repository> nullary_condition;
    const View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> unary_overapproximation_condition;
    const View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> binary_overapproximation_condition;
    const View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> unary_conflicting_overapproximation_condition;
    const View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> binary_conflicting_overapproximation_condition;
    const StaticConsistencyGraph static_consistency_graph;

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

    RuleExecutionContext(View<Index<formalism::datalog::Rule>, formalism::datalog::Repository> rule,
                         View<Index<formalism::datalog::GroundConjunctiveCondition>, formalism::datalog::Repository> nullary_condition,
                         View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> unary_overapproximation_condition,
                         View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> binary_overapproximation_condition,
                         View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> unary_conflicting_overapproximation_condition,
                         View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> binary_conflicting_overapproximation_condition,
                         const analysis::DomainListList& parameter_domains,
                         const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets,
                         const formalism::datalog::Repository& parent);

    void clear() noexcept;

    void initialize(const AssignmentSets& assignment_sets);
};

struct ThreadExecutionContext
{
    formalism::datalog::Builder builder;
    formalism::datalog::MergeCache merge_cache;

    ThreadExecutionContext() = default;

    void clear() noexcept;
};

struct ProgramToTaskExecutionContext
{
    ProgramToTaskExecutionContext() = default;

    formalism::planning::MergePlanningCache merge_cache;
    IndexList<formalism::Object> binding;

    void clear() noexcept;
};

struct TaskToProgramExecutionContext
{
    TaskToProgramExecutionContext() = default;

    formalism::planning::MergeDatalogCache merge_cache;

    void clear() noexcept;
};

struct ProgramExecutionContext
{
    /// --- Program & analysis
    const View<Index<formalism::datalog::Program>, formalism::datalog::Repository> program;
    const formalism::datalog::RepositoryPtr repository;
    const analysis::ProgramVariableDomains& domains;
    const analysis::RuleStrata& strata;
    const analysis::ListenerStrata& listeners;
    RuleSchedulerStrata rule_scheduler_strata;

    /// --- Builder
    formalism::planning::Builder planning_builder;
    formalism::datalog::Builder datalog_builder;

    /// --- Execution contexts
    FactsExecutionContext facts_execution_context;

    std::vector<RuleExecutionContext> rule_execution_contexts;
    std::vector<RuleStageExecutionContext> rule_stage_execution_contexts;

    oneapi::tbb::enumerable_thread_specific<datalog::ThreadExecutionContext> thread_execution_contexts;

    ProgramToTaskExecutionContext program_to_task_execution_context;
    TaskToProgramExecutionContext task_to_program_execution_context;

    struct Statistics
    {
        std::chrono::nanoseconds ground_seq_total_time { 0 };
        std::chrono::nanoseconds merge_seq_total_time { 0 };
    } statistics;

    ProgramExecutionContext(View<Index<formalism::datalog::Program>, formalism::datalog::Repository> program,
                            formalism::datalog::RepositoryPtr repository,
                            const analysis::ProgramVariableDomains& domains,
                            const analysis::RuleStrata& strata,
                            const analysis::ListenerStrata& listeners);
};

}

#endif
