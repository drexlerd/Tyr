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

#include "metric.hpp"
#include "transition.hpp"
#include "tyr/analysis/domains.hpp"
#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/grounder_planning.hpp"
#include "tyr/formalism/merge_datalog.hpp"
#include "tyr/formalism/merge_planning.hpp"
#include "tyr/grounder/consistency_graph.hpp"
#include "tyr/grounder/execution_contexts.hpp"
#include "tyr/grounder/generator.hpp"
#include "tyr/planning/applicability.hpp"
#include "tyr/planning/domain.hpp"
#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/lifted_task/node.hpp"
#include "tyr/planning/lifted_task/packed_state.hpp"
#include "tyr/planning/lifted_task/state.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"
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
    auto merge_context = MergeContext { axiom_context.builder, *axiom_context.repository, axiom_context.task_to_program_execution_context.merge_cache };

    for (auto i = fluent_atoms.find_first(); i != boost::dynamic_bitset<>::npos; i = fluent_atoms.find_next(i))
        axiom_context.facts_execution_context.fact_sets.fluent_sets.predicate.insert(
            make_view(merge(make_view(Index<GroundAtom<FluentTag>>(i), atoms_context), merge_context).first, merge_context.destination));
}

static void insert_derived_atoms_to_fact_set(const boost::dynamic_bitset<>& derived_atoms,
                                             const OverlayRepository<Repository>& atoms_context,
                                             ProgramExecutionContext& axiom_context)
{
    /// --- Initialize FactSets
    auto merge_context = MergeContext { axiom_context.builder, *axiom_context.repository, axiom_context.task_to_program_execution_context.merge_cache };

    for (auto i = derived_atoms.find_first(); i != boost::dynamic_bitset<>::npos; i = derived_atoms.find_next(i))
        axiom_context.facts_execution_context.fact_sets.fluent_sets.predicate.insert(make_view(
            merge<DerivedTag, OverlayRepository<Repository>, Repository, FluentTag>(make_view(Index<GroundAtom<DerivedTag>>(i), atoms_context), merge_context)
                .first,
            merge_context.destination));
}

static void insert_numeric_variables_to_fact_set(const std::vector<float_t>& numeric_variables,
                                                 const OverlayRepository<Repository>& numeric_variables_context,
                                                 ProgramExecutionContext& axiom_context)
{
    /// --- Initialize FactSets
    auto merge_context = MergeContext { axiom_context.builder, *axiom_context.repository, axiom_context.task_to_program_execution_context.merge_cache };

    for (uint_t i = 0; i < numeric_variables.size(); ++i)
    {
        if (!std::isnan(numeric_variables[i]))
            axiom_context.facts_execution_context.fact_sets.fluent_sets.function.insert(
                make_view(merge(make_view(Index<GroundFunctionTerm<FluentTag>>(i), numeric_variables_context), merge_context).first, merge_context.destination),
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
    axiom_context.task_to_program_execution_context.clear();

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
    action_context.task_to_program_execution_context.clear();

    insert_fluent_atoms_to_fact_set(fluent_atoms, atoms_context, action_context);
    insert_derived_atoms_to_fact_set(derived_atoms, atoms_context, action_context);
    insert_numeric_variables_to_fact_set(numeric_variables, atoms_context, action_context);

    insert_fact_sets_into_assignment_sets(action_context);
}

static void read_derived_atoms_from_program_context(const AxiomEvaluatorProgram& axiom_program,
                                                    boost::dynamic_bitset<>& derived_atoms,
                                                    OverlayRepository<Repository>& task_repository,
                                                    ProgramExecutionContext& axiom_context)
{
    assert(derived_atoms.empty());

    axiom_context.program_to_task_execution_context.clear();

    /// --- Initialize derived atoms in unpacked state

    auto merge_context = MergeContext { axiom_context.builder, task_repository, axiom_context.program_to_task_execution_context.merge_cache };

    /// TODO: store facts by predicate such that we can swap the iteration, i.e., first over get_predicate_to_predicate_mapping, then facts of the predicate
    for (const auto fact : axiom_context.facts_execution_context.fact_sets.fluent_sets.predicate.get_facts())
    {
        if (axiom_program.get_predicate_to_predicate_mapping().contains(fact.get_predicate().get_index()))
        {
            // TODO: pass the predicate mapping here so that we can skip merging the predicate :)
            const auto ground_atom = merge<FluentTag, Repository, OverlayRepository<Repository>, DerivedTag>(fact, merge_context).first;

            set(ground_atom.get_value(), derived_atoms);
        }
    }
}

static void read_solution_and_instantiate_labeled_successor_nodes(
    const StateContext<LiftedTask>& state_context,
    OverlayRepository<Repository>& task_repository,
    ProgramExecutionContext& action_context,
    BinaryFDRContext<OverlayRepository<Repository>>& fdr_context,
    const ApplicableActionProgram& action_program,
    const std::vector<analysis::DomainListListList>& parameter_domains_per_cond_effect_per_action,
    std::vector<std::pair<View<Index<GroundAction>, OverlayRepository<Repository>>, Node<LiftedTask>>>& out_successors)
{
    out_successors.clear();

    auto& effect_families = action_context.planning_execution_context.effect_families;
    auto& assign = action_context.planning_execution_context.assign;

    action_context.task_to_task_execution_context.clear();

    /// TODO: store facts by predicate such that we can swap the iteration, i.e., first over predicate_to_actions_mapping, then facts of the predicate
    for (const auto fact : action_context.facts_execution_context.fact_sets.fluent_sets.predicate.get_facts())
    {
        if (action_program.get_predicate_to_actions_mapping().contains(fact.get_predicate().get_index()))
        {
            for (const auto action_index : action_program.get_predicate_to_actions_mapping().at(fact.get_predicate().get_index()))
            {
                auto grounder_context = GrounderContext { action_context.builder, task_repository, action_context.program_to_task_execution_context.binding };

                const auto action = make_view(action_index, grounder_context.destination);

                action_context.program_to_task_execution_context.binding = fact.get_binding().get_objects().get_data();

                const auto ground_action_index =
                    ground_planning(action, grounder_context, parameter_domains_per_cond_effect_per_action[action_index.get_value()], assign, fdr_context)
                        .first;

                const auto ground_action = make_view(ground_action_index, grounder_context.destination);

                effect_families.clear();
                if (is_applicable(ground_action, state_context, effect_families))
                    out_successors.emplace_back(ground_action, apply_action(state_context, ground_action));
            }
        }
    }
}

static std::vector<analysis::DomainListListList> compute_parameter_domains_per_cond_effect_per_action(View<Index<Task>, OverlayRepository<Repository>>& task)
{
    auto result = std::vector<analysis::DomainListListList> {};

    const auto variable_domains = analysis::compute_variable_domains(task);

    for (uint_t action_index = 0; action_index < task.get_domain().get_actions().size(); ++action_index)
    {
        const auto action = task.get_domain().get_actions()[action_index];

        auto parameter_domains_per_cond_effect = analysis::DomainListListList {};

        for (uint_t cond_effect_index = 0; cond_effect_index < action.get_effects().size(); ++cond_effect_index)
        {
            const auto cond_effect = action.get_effects()[cond_effect_index];

            assert(variable_domains.action_domains[action_index].second[cond_effect_index].size() == action.get_arity() + cond_effect.get_arity());

            auto parameter_domains = analysis::DomainListList {};

            for (uint_t i = action.get_arity(); i < action.get_arity() + cond_effect.get_arity(); ++i)
                parameter_domains.push_back(variable_domains.action_domains[action_index].second[cond_effect_index][i]);

            parameter_domains_per_cond_effect.push_back(std::move(parameter_domains));
        }

        result.push_back(std::move(parameter_domains_per_cond_effect));
    }

    return result;
}

inline void fill_atoms(valla::Slot<uint_t> slot,
                       const valla::IndexedHashSet<valla::Slot<uint_t>, uint_t>& uint_nodes,
                       std::vector<uint_t>& buffer,
                       boost::dynamic_bitset<>& atoms)
{
    buffer.clear();

    valla::read_sequence(slot, uint_nodes, std::back_inserter(buffer));

    if (!buffer.empty())
    {
        assert(std::is_sorted(buffer.begin(), buffer.end()));
        atoms.resize(buffer.back() + 1, false);
        for (const auto& atom_index : buffer)
            atoms.set(atom_index);
    }
}

inline void fill_numeric_variables(valla::Slot<uint_t> slot,
                                   const valla::IndexedHashSet<valla::Slot<uint_t>, uint_t>& uint_nodes,
                                   const valla::IndexedHashSet<float_t, uint_t>& float_nodes,
                                   std::vector<uint_t>& buffer,
                                   std::vector<float_t>& numeric_variables)
{
    buffer.clear();

    valla::read_sequence(slot, uint_nodes, std::back_inserter(buffer));

    if (!buffer.empty())
        valla::decode_from_unsigned_integrals(buffer, float_nodes, std::back_inserter(numeric_variables));
}

inline valla::Slot<uint_t>
create_atoms_slot(const boost::dynamic_bitset<>& atoms, std::vector<uint_t>& buffer, valla::IndexedHashSet<valla::Slot<uint_t>, uint_t>& uint_nodes)
{
    buffer.clear();

    const auto& bits = atoms;
    for (auto i = bits.find_first(); i != boost::dynamic_bitset<>::npos; i = bits.find_next(i))
        buffer.push_back(i);

    return valla::insert_sequence(buffer, uint_nodes);
}

inline valla::Slot<uint_t> create_numeric_variables_slot(const std::vector<float_t>& numeric_variables,
                                                         std::vector<uint_t>& buffer,
                                                         valla::IndexedHashSet<valla::Slot<uint_t>, uint_t>& uint_nodes,
                                                         valla::IndexedHashSet<float_t, uint_t>& float_nodes)
{
    buffer.clear();

    valla::encode_as_unsigned_integrals(numeric_variables, float_nodes, std::back_inserter(buffer));

    return valla::insert_sequence(buffer, uint_nodes);
}

LiftedTask::LiftedTask(DomainPtr domain,
                       RepositoryPtr repository,
                       OverlayRepositoryPtr<Repository> overlay_repository,
                       View<Index<Task>, OverlayRepository<Repository>> task,
                       formalism::BinaryFDRContext<formalism::OverlayRepository<formalism::Repository>> fdr_context) :
    m_domain(std::move(domain)),
    m_repository(std::move(repository)),
    m_overlay_repository(std::move(overlay_repository)),
    m_task(task),
    m_fdr_context(std::move(fdr_context)),
    m_uint_nodes(),
    m_float_nodes(),
    m_packed_states(),
    m_unpacked_state_pool(),
    m_static_atoms_bitset(),
    m_static_numeric_variables(),
    m_action_program(*this),
    m_axiom_program(*this),
    m_ground_program(*this),
    m_action_context(m_action_program.get_program(),
                     m_action_program.get_repository(),
                     m_action_program.get_domains(),
                     m_action_program.get_strata(),
                     m_action_program.get_listeners()),
    m_axiom_context(m_axiom_program.get_program(),
                    m_axiom_program.get_repository(),
                    m_axiom_program.get_domains(),
                    m_axiom_program.get_strata(),
                    m_axiom_program.get_listeners()),
    m_parameter_domains_per_cond_effect_per_action(compute_parameter_domains_per_cond_effect_per_action(task))
{
    for (const auto atom : task.template get_atoms<formalism::StaticTag>())
        set(atom.get_index().get_value(), m_static_atoms_bitset);

    for (const auto fterm_value : task.template get_fterm_values<formalism::StaticTag>())
        set(fterm_value.get_fterm().get_index().get_value(), fterm_value.get_value(), m_static_numeric_variables, std::numeric_limits<float_t>::quiet_NaN());
}

State<LiftedTask> LiftedTask::get_state(StateIndex state_index)
{
    const auto& packed_state = m_packed_states[state_index];

    auto unpacked_state = m_unpacked_state_pool.get_or_allocate();
    unpacked_state->clear();

    thread_local auto buffer = std::vector<uint_t> {};

    unpacked_state->get_index() = state_index;
    fill_atoms(packed_state.template get_atoms<formalism::FluentTag>(), m_uint_nodes, buffer, unpacked_state->template get_atoms<formalism::FluentTag>());
    fill_atoms(packed_state.template get_atoms<formalism::DerivedTag>(), m_uint_nodes, buffer, unpacked_state->template get_atoms<formalism::DerivedTag>());
    fill_numeric_variables(packed_state.get_numeric_variables(), m_uint_nodes, m_float_nodes, buffer, unpacked_state->get_numeric_variables());

    return State<LiftedTask>(*this, std::move(unpacked_state));
}

StateIndex LiftedTask::register_state(const UnpackedState<LiftedTask>& state)
{
    thread_local auto buffer = std::vector<uint_t> {};

    auto fluent_atoms = create_atoms_slot(state.template get_atoms<formalism::FluentTag>(), buffer, m_uint_nodes);
    auto derived_atoms = create_atoms_slot(state.template get_atoms<formalism::DerivedTag>(), buffer, m_uint_nodes);
    auto numeric_variables = create_numeric_variables_slot(state.get_numeric_variables(), buffer, m_uint_nodes, m_float_nodes);

    return m_packed_states.insert(PackedState<LiftedTask>(StateIndex(m_packed_states.size()), fluent_atoms, derived_atoms, numeric_variables));
}

void LiftedTask::compute_extended_state(UnpackedState<LiftedTask>& unpacked_state)
{
    auto& fluent_atoms = unpacked_state.template get_atoms<FluentTag>();
    auto& derived_atoms = unpacked_state.template get_atoms<DerivedTag>();

    insert_unextended_state(fluent_atoms, *m_overlay_repository, m_axiom_context);

    solve_bottom_up(m_axiom_context);

    read_derived_atoms_from_program_context(m_axiom_program, derived_atoms, *m_overlay_repository, m_axiom_context);
}

Node<LiftedTask> LiftedTask::get_initial_node()
{
    auto unpacked_state_ptr = m_unpacked_state_pool.get_or_allocate();
    auto& unpacked_state = *unpacked_state_ptr;
    unpacked_state.clear();

    auto& fluent_atoms = unpacked_state.template get_atoms<FluentTag>();
    auto& numeric_variables = unpacked_state.get_numeric_variables();

    for (const auto atom : m_task.get_atoms<FluentTag>())
        set(atom.get_index().get_value(), fluent_atoms);

    for (const auto fterm_value : m_task.get_fterm_values<FluentTag>())
        set(fterm_value.get_fterm().get_index().get_value(), fterm_value.get_value(), numeric_variables, std::numeric_limits<float_t>::quiet_NaN());

    compute_extended_state(unpacked_state);

    const auto state_index = register_state(unpacked_state);

    const auto state_context = StateContext<LiftedTask>(*this, unpacked_state, 0);

    const auto state_metric = evaluate_metric(get_task().get_metric(), get_task().get_auxiliary_fterm_value(), state_context);

    return Node<LiftedTask>(state_index, state_metric, *this);
}

std::vector<std::pair<View<Index<GroundAction>, OverlayRepository<Repository>>, Node<LiftedTask>>>
LiftedTask::get_labeled_successor_nodes(const Node<LiftedTask>& node)
{
    auto result = std::vector<std::pair<View<Index<GroundAction>, OverlayRepository<Repository>>, Node<LiftedTask>>> {};

    get_labeled_successor_nodes(node, result);

    return result;
}

void LiftedTask::get_labeled_successor_nodes(const Node<LiftedTask>& node,
                                             std::vector<std::pair<View<Index<GroundAction>, OverlayRepository<Repository>>, Node<LiftedTask>>>& out_nodes)
{
    out_nodes.clear();

    const auto state = node.get_state();
    const auto& fluent_atoms = state.get_atoms<FluentTag>();
    const auto& derived_atoms = state.get_atoms<DerivedTag>();
    const auto& numeric_variables = state.get_numeric_variables<FluentTag>();
    const auto state_context = StateContext<LiftedTask>(*this, state.get_unpacked_state(), node.get_state_metric());

    insert_extended_state(fluent_atoms, derived_atoms, numeric_variables, *m_overlay_repository, m_action_context);

    solve_bottom_up(m_action_context);

    read_solution_and_instantiate_labeled_successor_nodes(state_context,
                                                          *m_overlay_repository,
                                                          m_action_context,
                                                          m_fdr_context,
                                                          m_action_program,
                                                          m_parameter_domains_per_cond_effect_per_action,
                                                          out_nodes);
}

GroundTaskPtr LiftedTask::get_ground_task()
{
    auto ground_context = grounder::ProgramExecutionContext(m_ground_program.get_program(),
                                                            m_ground_program.get_repository(),
                                                            m_ground_program.get_domains(),
                                                            m_ground_program.get_strata(),
                                                            m_ground_program.get_listeners());

    solve_bottom_up(ground_context);

    auto aggregated_statistics = grounder::RuleExecutionContext::compute_aggregate_statistics(ground_context.rule_execution_contexts);

    auto to_ms = [](auto d) { return std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(d).count(); };

    std::cout << "num_rules: " << ground_context.rule_execution_contexts.size() << std::endl;
    std::cout << "init_total_time_min: " << to_ms(aggregated_statistics.init_total_time_min) << " ms" << std::endl;
    std::cout << "init_total_time_max: " << to_ms(aggregated_statistics.init_total_time_max) << " ms" << std::endl;
    std::cout << "init_total_time_median: " << to_ms(aggregated_statistics.init_total_time_median) << " ms" << std::endl;
    std::cout << "ground_total_time_min: " << to_ms(aggregated_statistics.ground_total_time_min) << " ms" << std::endl;
    std::cout << "ground_total_time_max: " << to_ms(aggregated_statistics.ground_total_time_max) << " ms" << std::endl;
    std::cout << "ground_total_time_median: " << to_ms(aggregated_statistics.ground_total_time_median) << " ms" << std::endl;
    std::cout << "ground_seq_total_time: " << to_ms(ground_context.statistics.ground_seq_total_time) << " ms" << std::endl;
    std::cout << "merge_seq_total_time: " << to_ms(ground_context.statistics.merge_seq_total_time) << " ms" << std::endl;
    const auto total_time = (ground_context.statistics.ground_seq_total_time + ground_context.statistics.merge_seq_total_time).count();
    const auto parallel_time = ground_context.statistics.ground_seq_total_time.count();
    std::cout << "parallel_fraction: " << ((total_time > 0) ? static_cast<double>(parallel_time) / total_time : 1.0) << std::endl;

    ground_context.program_to_task_execution_context.clear();

    const auto initial_node = get_initial_node();
    const auto initial_state = initial_node.get_state();
    const auto initial_state_context = StateContext(*this, initial_state.get_unpacked_state(), 0);

    auto& assign = ground_context.planning_execution_context.assign;
    ground_context.task_to_task_execution_context.clear();

    /// --- Ground Atoms

    auto fluent_atoms_set = UnorderedSet<Index<GroundAtom<FluentTag>>>();
    auto derived_atoms_set = UnorderedSet<Index<GroundAtom<DerivedTag>>>();

    for (const auto atom : m_task.get_atoms<FluentTag>())
        fluent_atoms_set.insert(atom.get_index());

    for (const auto fact : m_task.get_goal().get_facts<FluentTag>())
        for (const auto atom : fact.get_variable().get_atoms())
            fluent_atoms_set.insert(atom.get_index());

    for (const auto literal : m_task.get_goal().get_facts<DerivedTag>())
        derived_atoms_set.insert(literal.get_atom().get_index());

    /// --- Ground Actions

    auto ground_actions_set = UnorderedSet<Index<GroundAction>> {};

    /// TODO: store facts by predicate such that we can swap the iteration, i.e., first over get_predicate_to_actions_mapping, then facts of the predicate
    for (const auto fact : ground_context.facts_execution_context.fact_sets.fluent_sets.predicate.get_facts())
    {
        if (m_ground_program.get_predicate_to_actions_mapping().contains(fact.get_predicate().get_index()))
        {
            ground_context.program_to_task_execution_context.binding = fact.get_binding().get_objects().get_data();
            auto grounder_context = GrounderContext { ground_context.builder, *m_overlay_repository, ground_context.program_to_task_execution_context.binding };

            for (const auto action_index : m_ground_program.get_predicate_to_actions_mapping().at(fact.get_predicate().get_index()))
            {
                const auto action = make_view(action_index, grounder_context.destination);

                const auto ground_action_index =
                    ground_planning(action, grounder_context, m_parameter_domains_per_cond_effect_per_action[action_index.get_value()], assign, m_fdr_context)
                        .first;

                const auto ground_action = make_view(ground_action_index, grounder_context.destination);

                if (is_statically_applicable(ground_action, initial_state_context))
                {
                    ground_actions_set.insert(ground_action.get_index());

                    for (const auto fact : ground_action.get_condition().get_facts<FluentTag>())
                        for (const auto atom : fact.get_variable().get_atoms())
                            fluent_atoms_set.insert(atom.get_index());

                    for (const auto literal : ground_action.get_condition().get_facts<DerivedTag>())
                        derived_atoms_set.insert(literal.get_atom().get_index());

                    for (const auto cond_effect : ground_action.get_effects())
                    {
                        for (const auto fact : cond_effect.get_condition().get_facts<FluentTag>())
                            for (const auto atom : fact.get_variable().get_atoms())
                                fluent_atoms_set.insert(atom.get_index());

                        for (const auto literal : cond_effect.get_condition().get_facts<DerivedTag>())
                            derived_atoms_set.insert(literal.get_atom().get_index());

                        for (const auto fact : cond_effect.get_effect().get_facts())
                            for (const auto atom : fact.get_variable().get_atoms())
                                fluent_atoms_set.insert(atom.get_index());
                    }
                }
            }
        }
    }

    /// --- Ground Axioms

    auto ground_axioms_set = UnorderedSet<Index<GroundAxiom>> {};

    /// TODO: store facts by predicate such that we can swap the iteration, i.e., first over get_predicate_to_axioms_mapping, then facts of the predicate
    for (const auto fact : ground_context.facts_execution_context.fact_sets.fluent_sets.predicate.get_facts())
    {
        if (m_ground_program.get_predicate_to_axioms_mapping().contains(fact.get_predicate().get_index()))
        {
            ground_context.program_to_task_execution_context.binding = fact.get_binding().get_objects().get_data();
            auto grounder_context = GrounderContext { ground_context.builder, *m_overlay_repository, ground_context.program_to_task_execution_context.binding };

            for (const auto axiom_index : m_ground_program.get_predicate_to_axioms_mapping().at(fact.get_predicate().get_index()))
            {
                const auto axiom = make_view(axiom_index, grounder_context.destination);

                const auto ground_axiom_index = ground_planning(axiom, grounder_context, m_fdr_context).first;

                const auto ground_axiom = make_view(ground_axiom_index, grounder_context.destination);

                if (is_statically_applicable(ground_axiom, initial_state_context))
                {
                    ground_axioms_set.insert(ground_axiom.get_index());

                    for (const auto fact : ground_axiom.get_body().get_facts<FluentTag>())
                        for (const auto atom : fact.get_variable().get_atoms())
                            fluent_atoms_set.insert(atom.get_index());

                    for (const auto literal : ground_axiom.get_body().get_facts<DerivedTag>())
                        derived_atoms_set.insert(literal.get_atom().get_index());

                    derived_atoms_set.insert(ground_axiom.get_head().get_index());
                }
            }
        }
    }

    auto fluent_atoms = IndexList<GroundAtom<FluentTag>>(fluent_atoms_set.begin(), fluent_atoms_set.end());
    auto derived_atoms = IndexList<GroundAtom<DerivedTag>>(derived_atoms_set.begin(), derived_atoms_set.end());
    auto ground_actions = IndexList<GroundAction>(ground_actions_set.begin(), ground_actions_set.end());
    auto ground_axioms = IndexList<GroundAxiom>(ground_axioms_set.begin(), ground_axioms_set.end());

    canonicalize(fluent_atoms);
    canonicalize(derived_atoms);
    canonicalize(ground_actions);
    canonicalize(ground_axioms);

    std::cout << "Num fluent atoms: " << fluent_atoms.size() << std::endl;
    std::cout << "Num derived atoms: " << derived_atoms.size() << std::endl;
    std::cout << "Num ground actions: " << ground_actions.size() << std::endl;
    std::cout << "Num ground axioms: " << ground_axioms.size() << std::endl;

    return GroundTask::create(m_domain, m_repository, m_overlay_repository, m_task, fluent_atoms, derived_atoms, ground_actions, ground_axioms);
}

const ApplicableActionProgram& LiftedTask::get_action_program() const { return m_action_program; }

const AxiomEvaluatorProgram& LiftedTask::get_axiom_program() const { return m_axiom_program; }

const GroundTaskProgram& LiftedTask::get_ground_program() const { return m_ground_program; }

}
