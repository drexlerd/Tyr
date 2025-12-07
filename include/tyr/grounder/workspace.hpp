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
#include "tyr/formalism/compiler.hpp"
#include "tyr/formalism/merge.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/grounder/consistency_graph.hpp"
#include "tyr/grounder/declarations.hpp"
#include "tyr/grounder/kpkc_utils.hpp"

#include <oneapi/tbb/enumerable_thread_specific.h>

namespace tyr::grounder
{
struct FactsExecutionContext
{
    FactSets fact_sets;
    AssignmentSets assignment_sets;

    FactsExecutionContext(View<Index<formalism::Program>, formalism::Repository> program, const analysis::ProgramVariableDomains& domains);

    FactsExecutionContext(View<Index<formalism::Program>, formalism::Repository> program,
                          TaggedFactSets<formalism::FluentTag> fluent_facts,
                          const analysis::ProgramVariableDomains& domains);

    template<formalism::FactKind T>
    void reset() noexcept;

    void reset() noexcept;

    template<formalism::FactKind T>
    void insert(View<IndexList<formalism::GroundAtom<T>>, formalism::Repository> view);

    template<formalism::FactKind T>
    void insert(View<IndexList<formalism::GroundFunctionTermValue<T>>, formalism::Repository> view);
};

struct RuleExecutionContext
{
    const View<Index<formalism::Rule>, formalism::Repository> rule;
    const StaticConsistencyGraph static_consistency_graph;

    kpkc::DenseKPartiteGraph consistency_graph;
    kpkc::Workspace kpkc_workspace;
    std::shared_ptr<formalism::Repository> local;
    formalism::OverlayRepository<formalism::Repository> repository;
    std::vector<View<Index<formalism::GroundRule>, formalism::OverlayRepository<formalism::Repository>>> ground_rules;

    RuleExecutionContext(View<Index<formalism::Rule>, formalism::Repository> rule,
                         const analysis::DomainListList& parameter_domains,
                         const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets,
                         const formalism::Repository& parent);

    void clear() noexcept;

    void initialize(const AssignmentSets& assignment_sets);
};

struct ThreadExecutionContext
{
    IndexList<formalism::Object> binding;

    formalism::Builder builder;
    formalism::MergeCache<formalism::OverlayRepository<formalism::Repository>, formalism::OverlayRepository<formalism::Repository>> local_merge_cache;
    formalism::MergeCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository> global_merge_cache;

    ThreadExecutionContext() = default;

    void clear() noexcept;
};

struct FixedPointContext
{
};

struct ProgramExecutionContext
{
    const View<Index<formalism::Program>, formalism::Repository> program;
    const formalism::RepositoryPtr repository;

    analysis::ProgramVariableDomains domains;
    analysis::RuleStrata strata;
    analysis::Listeners listeners;

    FactsExecutionContext facts_execution_context;

    std::vector<RuleExecutionContext> rule_execution_contexts;

    oneapi::tbb::enumerable_thread_specific<grounder::ThreadExecutionContext> thread_execution_contexts;

    formalism::Builder builder;

    // Stage
    formalism::RepositoryPtr stage_repository;
    formalism::MergeCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository> stage_merge_cache;
    UnorderedSet<View<Index<formalism::GroundRule>, formalism::Repository>> stage_merge_rules;
    UnorderedSet<View<Index<formalism::GroundAtom<formalism::FluentTag>>, formalism::Repository>> stage_merge_atoms;

    void clear_stage() noexcept;

    // Stage to program
    formalism::MergeCache<formalism::Repository, formalism::Repository> stage_to_program_merge_cache;

    void clear_stage_to_program() noexcept;

    // Program to task
    formalism::MergeCache<formalism::Repository, formalism::OverlayRepository<formalism::Repository>> program_to_task_merge_cache;
    formalism::CompileCache<formalism::Repository, formalism::OverlayRepository<formalism::Repository>> program_to_task_compile_cache;

    void clear_program_to_task() noexcept;

    // Task to program
    formalism::MergeCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository> task_to_program_merge_cache;
    formalism::CompileCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository> task_to_program_compile_cache;

    void clear_task_to_program() noexcept;

    // Results
    UnorderedSet<View<Index<formalism::GroundRule>, formalism::Repository>> program_merge_rules;
    UnorderedSet<View<Index<formalism::GroundAtom<formalism::FluentTag>>, formalism::Repository>> program_merge_atoms;

    void clear_results() noexcept;

    ProgramExecutionContext(View<Index<formalism::Program>, formalism::Repository> program, formalism::RepositoryPtr repository);
};

}

#endif
