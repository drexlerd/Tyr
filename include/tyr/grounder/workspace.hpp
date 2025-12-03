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
#include "tyr/formalism/merge.hpp"
#include "tyr/grounder/consistency_graph.hpp"
#include "tyr/grounder/declarations.hpp"
#include "tyr/grounder/kpkc_utils.hpp"

namespace tyr::grounder
{
struct FactsExecutionContext
{
    FactSets<formalism::Repository> fact_sets;
    AssignmentSets assignment_sets;

    FactsExecutionContext(View<Index<formalism::Program>, formalism::Repository> program, const analysis::VariableDomains& domains) :
        fact_sets(program),
        assignment_sets(program, domains, fact_sets)
    {
    }

    FactsExecutionContext(View<Index<formalism::Program>, formalism::Repository> program,
                          TaggedFactSets<formalism::FluentTag, formalism::Repository> fluent_facts,
                          const analysis::VariableDomains& domains) :
        fact_sets(program, fluent_facts),
        assignment_sets(program, domains, fact_sets)
    {
    }

    template<formalism::FactKind T>
    void reset() noexcept
    {
        fact_sets.template reset<T>();
        assignment_sets.template reset<T>();
    }

    void reset() noexcept
    {
        fact_sets.reset();
        assignment_sets.reset();
    }

    template<formalism::FactKind T>
    void insert(View<IndexList<formalism::GroundAtom<T>>, formalism::Repository> view)
    {
        fact_sets.insert(view);
        assignment_sets.insert(fact_sets.template get<T>());
    }

    template<formalism::FactKind T>
    void insert(View<IndexList<formalism::GroundFunctionTermValue<T>>, formalism::Repository> view)
    {
        fact_sets.insert(view);
        assignment_sets.insert(fact_sets.template get<T>());
    }
};

struct RuleExecutionContext
{
    const View<Index<formalism::Rule>, formalism::Repository> rule;
    const StaticConsistencyGraph<formalism::Repository> static_consistency_graph;

    kpkc::DenseKPartiteGraph consistency_graph;
    kpkc::Workspace kpkc_workspace;
    std::shared_ptr<formalism::Repository> local;
    formalism::OverlayRepository<formalism::Repository> repository;
    std::vector<View<Index<formalism::GroundRule>, formalism::OverlayRepository<formalism::Repository>>> ground_rules;

    RuleExecutionContext(View<Index<formalism::Rule>, formalism::Repository> rule,
                         const analysis::DomainListList& parameter_domains,
                         const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets,
                         const formalism::Repository& parent) :
        rule(rule),
        static_consistency_graph(rule.get_body(), parameter_domains, static_assignment_sets),
        consistency_graph(grounder::kpkc::allocate_dense_graph(static_consistency_graph)),
        kpkc_workspace(grounder::kpkc::allocate_workspace(static_consistency_graph)),
        local(std::make_shared<formalism::Repository>()),  // we have to use pointer, since the RuleExecutionContext is moved into a vector
        repository(parent, *local),
        ground_rules()
    {
    }

    void initialize(const AssignmentSets& assignment_sets)
    {
        grounder::kpkc::initialize_dense_graph_and_workspace(static_consistency_graph, assignment_sets, consistency_graph, kpkc_workspace);
    }
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

    ProgramExecutionContext(View<Index<formalism::Program>, formalism::Repository> program, formalism::RepositoryPtr repository) :
        program(program),
        repository(repository),
        domains(analysis::compute_variable_domains(program)),
        strata(analysis::compute_rule_stratification(program)),
        listeners(analysis::compute_listeners(strata)),
        facts_execution_context(program, domains),
        rule_execution_contexts()
    {
        for (uint_t i = 0; i < program.get_rules().size(); ++i)
        {
            rule_execution_contexts.emplace_back(program.get_rules()[i],
                                                 domains.rule_domains[i],
                                                 facts_execution_context.assignment_sets.static_sets,
                                                 *repository);
            rule_execution_contexts.back().initialize(facts_execution_context.assignment_sets);
        }
    }
};

struct ThreadExecutionContext
{
    IndexList<formalism::Object> binding;
    formalism::Builder builder;
    formalism::MergeCache<formalism::OverlayRepository<formalism::Repository>, formalism::OverlayRepository<formalism::Repository>> local_merge_cache;
    formalism::MergeCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository> global_merge_cache;

    ThreadExecutionContext() = default;

    void clear() noexcept
    {
        binding.clear();
        local_merge_cache.clear();
        global_merge_cache.clear();
    }
};

}

#endif
