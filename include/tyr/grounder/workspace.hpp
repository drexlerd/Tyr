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
#include "tyr/formalism/formalism.hpp"
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

    formalism::Builder builder;
    formalism::MergeCache<formalism::OverlayRepository<formalism::Repository>, formalism::OverlayRepository<formalism::Repository>> local_merge_cache;
    formalism::MergeCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository> global_merge_cache;

    FactsExecutionContext(View<Index<formalism::Program>, formalism::Repository> program, const analysis::VariableDomains& domains);

    FactsExecutionContext(View<Index<formalism::Program>, formalism::Repository> program,
                          TaggedFactSets<formalism::FluentTag> fluent_facts,
                          const analysis::VariableDomains& domains);

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

    analysis::VariableDomains domains;
    analysis::RuleStrata strata;
    analysis::Listeners listeners;

    FactsExecutionContext facts_execution_context;

    std::vector<RuleExecutionContext> rule_execution_contexts;

    oneapi::tbb::enumerable_thread_specific<grounder::ThreadExecutionContext> thread_execution_contexts;

    // Merge
    formalism::RepositoryPtr merge_repository;
    formalism::Builder builder;
    formalism::MergeCache<formalism::OverlayRepository<formalism::Repository>, formalism::OverlayRepository<formalism::Repository>> local_merge_cache;
    formalism::MergeCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository> global_merge_cache;
    formalism::MergeCache<formalism::Repository, formalism::Repository> merge_cache;
    UnorderedSet<View<Index<formalism::GroundRule>, formalism::Repository>> tmp_merge_rules;
    UnorderedSet<View<Index<formalism::GroundAtom<formalism::FluentTag>>, formalism::Repository>> tmp_merge_atoms;

    // Results
    UnorderedSet<View<Index<formalism::GroundRule>, formalism::Repository>> merge_rules;
    UnorderedSet<View<Index<formalism::GroundAtom<formalism::FluentTag>>, formalism::Repository>> merge_atoms;

    ProgramExecutionContext(View<Index<formalism::Program>, formalism::Repository> program, formalism::RepositoryPtr repository);
};

}

#endif
