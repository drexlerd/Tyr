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

#include "tyr/grounder/execution_contexts.hpp"

#include "tyr/formalism/formatter.hpp"

namespace tyr::grounder
{
/**
 * FactsExecutionContext
 */

FactsExecutionContext::FactsExecutionContext(View<Index<formalism::Program>, formalism::Repository> program, const analysis::ProgramVariableDomains& domains) :
    fact_sets(program),
    assignment_sets(program, domains, fact_sets)
{
}

FactsExecutionContext::FactsExecutionContext(View<Index<formalism::Program>, formalism::Repository> program,
                                             TaggedFactSets<formalism::FluentTag, formalism::Repository> fluent_facts,
                                             const analysis::ProgramVariableDomains& domains) :
    fact_sets(program, fluent_facts),
    assignment_sets(program, domains, fact_sets)
{
}

template<formalism::FactKind T>
void FactsExecutionContext::reset() noexcept
{
    fact_sets.template reset<T>();
    assignment_sets.template reset<T>();
}

template void FactsExecutionContext::reset<formalism::StaticTag>() noexcept;
template void FactsExecutionContext::reset<formalism::FluentTag>() noexcept;

void FactsExecutionContext::reset() noexcept
{
    fact_sets.reset();
    assignment_sets.reset();
}

template<formalism::FactKind T>
void FactsExecutionContext::insert(View<IndexList<formalism::GroundAtom<T>>, formalism::Repository> view)
{
    fact_sets.insert(view);
    assignment_sets.insert(fact_sets.template get<T>());
}

template void FactsExecutionContext::insert(View<IndexList<formalism::GroundAtom<formalism::StaticTag>>, formalism::Repository> view);
template void FactsExecutionContext::insert(View<IndexList<formalism::GroundAtom<formalism::FluentTag>>, formalism::Repository> view);

template<formalism::FactKind T>
void FactsExecutionContext::insert(View<IndexList<formalism::GroundFunctionTermValue<T>>, formalism::Repository> view)
{
    fact_sets.insert(view);
    assignment_sets.insert(fact_sets.template get<T>());
}

template void FactsExecutionContext::insert(View<IndexList<formalism::GroundFunctionTermValue<formalism::StaticTag>>, formalism::Repository> view);
template void FactsExecutionContext::insert(View<IndexList<formalism::GroundFunctionTermValue<formalism::FluentTag>>, formalism::Repository> view);

/**
 * RuleStageExecutionContext
 */

RuleStageExecutionContext::RuleStageExecutionContext() : repository(std::make_shared<formalism::Repository>()), bindings(), merge_cache() {}

void RuleStageExecutionContext::clear() noexcept
{
    repository->clear();
    bindings.clear();
    merge_cache.clear();
}

/**
 * RuleExecutionContext
 */

RuleExecutionContext::RuleExecutionContext(View<Index<formalism::Rule>, formalism::Repository> rule,
                                           const analysis::DomainListList& parameter_domains,
                                           const TaggedAssignmentSets<formalism::StaticTag, formalism::Repository>& static_assignment_sets,
                                           const formalism::Repository& parent) :
    rule(rule),
    static_consistency_graph(rule.get_body(), parameter_domains, 0, rule.get_arity(), static_assignment_sets),
    consistency_graph(grounder::kpkc::allocate_dense_graph(static_consistency_graph)),
    kpkc_workspace(grounder::kpkc::allocate_workspace(static_consistency_graph)),
    local(std::make_shared<formalism::Repository>()),  // we have to use pointer, since the RuleExecutionContext is moved into a vector
    repository(parent, *local),
    bindings()
{
}

void RuleExecutionContext::clear() noexcept
{
    local->clear();
    bindings.clear();
}

void RuleExecutionContext::initialize(const AssignmentSets<formalism::Repository>& assignment_sets)
{
    grounder::kpkc::initialize_dense_graph_and_workspace(static_consistency_graph, assignment_sets, consistency_graph, kpkc_workspace);
}

/**
 * ThreadExecutionContext
 */

void ThreadExecutionContext::clear() noexcept
{
    binding.clear();
    merge_cache.clear();
}

/**
 * ProgramResultsExecutionContext
 */

void ProgramResultsExecutionContext::clear() noexcept { rule_binding_pairs.clear(); }

/**
 * ProgramToTaskExecutionContext
 */

void ProgramToTaskExecutionContext::clear() noexcept { merge_cache.clear(); }

/**
 * TaskToProgramExecutionContext
 */

void TaskToProgramExecutionContext::clear() noexcept { merge_cache.clear(); }

/**
 * ProgramExecutionContext
 */

ProgramExecutionContext::ProgramExecutionContext(View<Index<formalism::Program>, formalism::Repository> program,
                                                 formalism::RepositoryPtr repository,
                                                 const analysis::ProgramVariableDomains& domains,
                                                 const analysis::RuleStrata& strata,
                                                 const analysis::Listeners& listeners) :
    program(program),
    repository(repository),
    domains(domains),
    strata(strata),
    listeners(listeners),
    builder(),
    facts_execution_context(program, domains),
    rule_execution_contexts(),
    rule_stage_execution_contexts(),
    thread_execution_contexts(),
    planning_execution_context(),
    program_results_execution_context(),
    program_to_task_execution_context(),
    task_to_program_execution_context()
{
    for (uint_t i = 0; i < program.get_rules().size(); ++i)
    {
        rule_execution_contexts.emplace_back(program.get_rules()[i], domains.rule_domains[i], facts_execution_context.assignment_sets.static_sets, *repository);
        rule_execution_contexts.back().initialize(facts_execution_context.assignment_sets);
    }
    rule_stage_execution_contexts.resize(rule_execution_contexts.size());
}

}
