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

#include "tyr/planning/lifted_task.hpp"

#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/merge.hpp"
#include "tyr/grounder/generator.hpp"

using namespace tyr::formalism;
using namespace tyr::grounder;

namespace tyr::planning
{

static void insert_fluent_atoms(const boost::dynamic_bitset<>& fluent_atoms,
                                const OverlayRepository<Repository>& fluent_atoms_context,
                                ProgramExecutionContext& axiom_context)
{
    auto& destination = *axiom_context.repository;
    auto& fact_context = axiom_context.facts_execution_context;
    auto& fluent_predicate_fact_sets = fact_context.fact_sets.get<FluentTag>().predicate;
    auto& fluent_predicate_assignment_sets = fact_context.assignment_sets.get<FluentTag>().predicate;
    auto& merge_cache = fact_context.global_merge_cache;
    auto& builder = fact_context.builder;
    merge_cache.clear();
    fluent_predicate_fact_sets.reset();
    fluent_predicate_assignment_sets.reset();

    /// --- Initialize FactSets
    for (auto i = fluent_atoms.find_first(); i != boost::dynamic_bitset<>::npos; i = fluent_atoms.find_next(i))
        fluent_predicate_fact_sets.insert(
            merge(View<Index<GroundAtom<FluentTag>>, OverlayRepository<Repository>>(Index<GroundAtom<FluentTag>>(i), fluent_atoms_context),
                  builder,
                  destination,
                  merge_cache));

    /// --- Initialize AssignmentSets
    fluent_predicate_assignment_sets.insert(fluent_predicate_fact_sets.get_facts());

    /// --- Initialize RuleExecutionContext
    for (auto& rule_context : axiom_context.rule_execution_contexts)
        rule_context.initialize(fact_context.assignment_sets);
}

static void evaluate_axiom_program(ProgramExecutionContext& axiom_context, UnpackedState<LiftedTask>& unextended_state)
{
    /**
     * Parallel evaluation.
     */

    const uint_t num_rules = axiom_context.program.get_rules().size();

    tbb::parallel_for(uint_t { 0 },
                      num_rules,
                      [&](uint_t i)
                      {
                          auto& facts_execution_context = axiom_context.facts_execution_context;
                          auto& rule_execution_context = axiom_context.rule_execution_contexts[i];
                          auto& thread_execution_context = axiom_context.thread_execution_contexts.local();  // thread-local
                          thread_execution_context.clear();

                          ground(facts_execution_context, rule_execution_context, thread_execution_context);
                      });

    /**
     * Sequential merge.
     */

    /// --- Sequentially combine results into a temporary top-level repository to prevent modying the program's repository
    auto& builder = axiom_context.builder;
    auto& global_merge_cache = axiom_context.global_merge_cache;
    auto& merge_repository = *axiom_context.merge_repository;
    auto& tmp_merge_rules = axiom_context.tmp_merge_rules;
    merge_repository.clear();
    global_merge_cache.clear();
    tmp_merge_rules.clear();

    for (const auto& rule_execution_context : axiom_context.rule_execution_contexts)
        for (const auto rule : rule_execution_context.ground_rules)
            tmp_merge_rules.insert(merge(rule, builder, merge_repository, global_merge_cache));

    /// --- Copy the result into the program's repository
    auto& merge_cache = axiom_context.merge_cache;
    auto& merge_rules = axiom_context.merge_rules;
    merge_cache.clear();
    merge_rules.clear();

    for (const auto rule : tmp_merge_rules)
        merge_rules.insert(merge(rule, builder, *axiom_context.repository, merge_cache));

    std::cout << merge_rules << std::endl;
}

LiftedTask::LiftedTask(DomainPtr domain,
                       RepositoryPtr repository,
                       OverlayRepositoryPtr<Repository> overlay_repository,
                       View<Index<Task>, OverlayRepository<Repository>> task) :
    TaskMixin(std::move(domain), std::move(repository), std::move(overlay_repository), task),
    m_action_program(*this),
    m_axiom_program(*this),
    m_ground_program(*this),
    m_action_context(m_action_program.get_program(), m_action_program.get_repository()),
    m_axiom_context(m_axiom_program.get_program(), m_axiom_program.get_repository())
{
}

Node<LiftedTask> LiftedTask::get_initial_node_impl()
{
    auto unpacked_state_ptr = m_unpacked_state_pool.get_or_allocate();
    auto& unpacked_state = *unpacked_state_ptr;
    unpacked_state.clear();

    auto& fluent_atoms = unpacked_state.template get_atoms<FluentTag>();
    auto& derived_atoms = unpacked_state.template get_atoms<DerivedTag>();
    auto& numeric_variables = unpacked_state.get_numeric_variables();

    for (const auto atom : m_task.get_atoms<FluentTag>())
    {
        const auto atom_index = atom.get_index().get_value();
        if (atom_index >= fluent_atoms.size())
            fluent_atoms.resize(atom_index + 1, false);
        fluent_atoms.set(atom_index);
    }

    for (const auto fterm_value : m_task.get_fterm_values<FluentTag>())
    {
        const auto fterm_index = fterm_value.get_fterm().get_index().get_value();
        if (fterm_index >= numeric_variables.size())
            numeric_variables.resize(fterm_index + 1, std::numeric_limits<float_t>::quiet_NaN());
        numeric_variables[fterm_index] = fterm_value.get_value();
    }

    insert_fluent_atoms(fluent_atoms, *this->m_overlay_repository, m_axiom_context);
    evaluate_axiom_program(m_axiom_context, unpacked_state);

    const auto state_index = register_state(unpacked_state);
    const auto state_metric = float_t(0);  // TODO: evaluate metric

    return Node<LiftedTask>(state_index, state_metric, *this);
}

std::vector<std::pair<View<Index<GroundAction>, OverlayRepository<Repository>>, Node<LiftedTask>>>
LiftedTask::get_labeled_successor_nodes_impl(const Node<LiftedTask>& node)
{
    auto result = std::vector<std::pair<View<Index<GroundAction>, OverlayRepository<Repository>>, Node<LiftedTask>>> {};

    return result;
}

void LiftedTask::get_labeled_successor_nodes_impl(const Node<LiftedTask>& node,
                                                  std::vector<std::pair<View<Index<GroundAction>, OverlayRepository<Repository>>, Node<LiftedTask>>>& out_nodes)
{
}

GroundTask LiftedTask::get_ground_task() { return GroundTask(this->m_domain, this->m_repository, this->m_overlay_repository, this->m_task); }

const ApplicableActionProgram& LiftedTask::get_action_program() const { return m_action_program; }

const AxiomEvaluatorProgram& LiftedTask::get_axiom_program() const { return m_axiom_program; }

const GroundTaskProgram& LiftedTask::get_ground_program() const { return m_ground_program; }

}
