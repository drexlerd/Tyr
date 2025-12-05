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

#include "tyr/formalism/compiler.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/merge.hpp"
#include "tyr/grounder/generator.hpp"
#include "tyr/solver/bottom_up.hpp"

using namespace tyr::formalism;
using namespace tyr::grounder;
using namespace tyr::solver;

namespace tyr::planning
{

static void insert_fluent_atoms_to_fact_set(const boost::dynamic_bitset<>& fluent_atoms,
                                            const OverlayRepository<Repository>& atoms_context,
                                            ProgramExecutionContext& axiom_context)
{
    /// --- Initialize FactSets
    for (auto i = fluent_atoms.find_first(); i != boost::dynamic_bitset<>::npos; i = fluent_atoms.find_next(i))
        axiom_context.facts_execution_context.fact_sets.fluent_sets.predicate.insert(
            merge(View<Index<GroundAtom<FluentTag>>, OverlayRepository<Repository>>(Index<GroundAtom<FluentTag>>(i), atoms_context),
                  axiom_context.builder,
                  *axiom_context.repository,
                  axiom_context.task_to_program_merge_cache));
}

static void insert_derived_atoms_to_fact_set(const boost::dynamic_bitset<>& derived_atoms,
                                             const OverlayRepository<Repository>& atoms_context,
                                             ProgramExecutionContext& axiom_context)
{
    /// --- Initialize FactSets
    for (auto i = derived_atoms.find_first(); i != boost::dynamic_bitset<>::npos; i = derived_atoms.find_next(i))
        axiom_context.facts_execution_context.fact_sets.fluent_sets.predicate.insert(
            compile<DerivedTag, FluentTag>(View<Index<GroundAtom<DerivedTag>>, OverlayRepository<Repository>>(Index<GroundAtom<DerivedTag>>(i), atoms_context),
                                           axiom_context.builder,
                                           *axiom_context.repository,
                                           axiom_context.task_to_program_compile_cache,
                                           axiom_context.task_to_program_merge_cache));
}

static void insert_numeric_variables_to_fact_set(const std::vector<float_t>& numeric_variables,
                                                 const OverlayRepository<Repository>& numeric_variables_context,
                                                 ProgramExecutionContext& axiom_context)
{
    /// --- Initialize FactSets
    for (uint_t i = 0; i < numeric_variables.size(); ++i)
    {
        if (!std::isnan(numeric_variables[i]))
            axiom_context.facts_execution_context.fact_sets.fluent_sets.function.insert(
                merge(View<Index<GroundFunctionTerm<FluentTag>>, OverlayRepository<Repository>>(Index<GroundFunctionTerm<FluentTag>>(i),
                                                                                                numeric_variables_context),
                      axiom_context.builder,
                      *axiom_context.repository,
                      axiom_context.task_to_program_merge_cache),
                numeric_variables[i]);
    }
}

static void insert_fact_sets_into_assignment_sets(ProgramExecutionContext& program_context)
{
    auto& fluent_predicate_fact_sets = program_context.facts_execution_context.fact_sets.get<FluentTag>().predicate;
    auto& fluent_predicate_assignment_sets = program_context.facts_execution_context.assignment_sets.get<FluentTag>().predicate;

    auto& fluent_function_fact_sets = program_context.facts_execution_context.fact_sets.get<FluentTag>().function;
    auto& fluent_function_assignment_sets = program_context.facts_execution_context.assignment_sets.get<FluentTag>().function;

    /// --- Initialize AssignmentSets
    fluent_predicate_assignment_sets.insert(fluent_predicate_fact_sets.get_facts());
    fluent_function_assignment_sets.insert(fluent_function_fact_sets.get_fterms(), fluent_function_fact_sets.get_values());

    /// --- Initialize RuleExecutionContext
    for (auto& rule_context : program_context.rule_execution_contexts)
        rule_context.initialize(program_context.facts_execution_context.assignment_sets);
}

static void
insert_unextended_state(const boost::dynamic_bitset<>& fluent_atoms, const OverlayRepository<Repository>& atoms_context, ProgramExecutionContext& axiom_context)
{
    axiom_context.facts_execution_context.reset<formalism::FluentTag>();
    axiom_context.clear_task_to_program();

    insert_fluent_atoms_to_fact_set(fluent_atoms, atoms_context, axiom_context);

    insert_fact_sets_into_assignment_sets(axiom_context);
}

static void insert_extended_state(const boost::dynamic_bitset<>& fluent_atoms,
                                  const boost::dynamic_bitset<>& derived_atoms,
                                  const std::vector<float_t>& numeric_variables,
                                  const OverlayRepository<Repository>& atoms_context,
                                  ProgramExecutionContext& action_context)
{
    action_context.facts_execution_context.reset<formalism::FluentTag>();
    action_context.clear_task_to_program();

    insert_fluent_atoms_to_fact_set(fluent_atoms, atoms_context, action_context);
    insert_derived_atoms_to_fact_set(derived_atoms, atoms_context, action_context);
    insert_numeric_variables_to_fact_set(numeric_variables, atoms_context, action_context);

    insert_fact_sets_into_assignment_sets(action_context);
}

static void read_derived_atoms_from_program_context(boost::dynamic_bitset<>& derived_atoms,
                                                    OverlayRepository<Repository>& task_repository,
                                                    ProgramExecutionContext& axiom_context)
{
    axiom_context.clear_program_to_task();

    /// --- Initialized derived atoms in unpacked state
    for (const auto atom : axiom_context.program_merge_atoms)
    {
        const auto derived_atom = compile<FluentTag, DerivedTag>(atom,
                                                                 axiom_context.builder,
                                                                 task_repository,
                                                                 axiom_context.program_to_task_compile_cache,
                                                                 axiom_context.program_to_task_merge_cache);

        const auto derived_atom_index = derived_atom.get_index().get_value();
        if (derived_atom_index >= derived_atoms.size())
            derived_atoms.resize(derived_atom_index + 1);
        derived_atoms.set(derived_atom_index);
    }
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

    insert_unextended_state(fluent_atoms, *this->m_overlay_repository, m_axiom_context);

    solve_bottom_up(m_axiom_context);

    read_derived_atoms_from_program_context(derived_atoms, *this->m_overlay_repository, m_axiom_context);

    std::cout << to_string(m_axiom_context.program_merge_atoms) << std::endl;

    const auto state_index = register_state(unpacked_state);
    const auto state_metric = float_t(0);  // TODO: evaluate metric

    return Node<LiftedTask>(state_index, state_metric, *this);
}

std::vector<std::pair<View<Index<GroundAction>, OverlayRepository<Repository>>, Node<LiftedTask>>>
LiftedTask::get_labeled_successor_nodes_impl(const Node<LiftedTask>& node)
{
    auto result = std::vector<std::pair<View<Index<GroundAction>, OverlayRepository<Repository>>, Node<LiftedTask>>> {};

    const auto state = node.get_state();
    const auto& fluent_atoms = state.get_atoms<FluentTag>();
    const auto& derived_atoms = state.get_atoms<DerivedTag>();
    const auto& numeric_variables = state.get_numeric_variables<FluentTag>();

    insert_extended_state(fluent_atoms, derived_atoms, numeric_variables, *this->m_overlay_repository, m_action_context);

    solve_bottom_up(m_action_context);

    std::cout << to_string(m_action_context.program_merge_atoms) << std::endl;

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
