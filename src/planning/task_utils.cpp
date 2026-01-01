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

#include "task_utils.hpp"

#include "tyr/analysis/domains.hpp"
#include "tyr/common/config.hpp"
#include "tyr/datalog/assignment_sets.hpp"
#include "tyr/datalog/execution_contexts.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/formalism/datalog/merge.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/planning/merge_datalog.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"

#include <algorithm>
#include <boost/dynamic_bitset.hpp>
#include <valla/valla.hpp>
#include <vector>

namespace tyr::planning
{
void fill_atoms(valla::Slot<uint_t> slot,
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

void fill_numeric_variables(valla::Slot<uint_t> slot,
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

valla::Slot<uint_t>
create_atoms_slot(const boost::dynamic_bitset<>& atoms, std::vector<uint_t>& buffer, valla::IndexedHashSet<valla::Slot<uint_t>, uint_t>& uint_nodes)
{
    buffer.clear();

    const auto& bits = atoms;
    for (auto i = bits.find_first(); i != boost::dynamic_bitset<>::npos; i = bits.find_next(i))
        buffer.push_back(i);

    return valla::insert_sequence(buffer, uint_nodes);
}

valla::Slot<uint_t> create_numeric_variables_slot(const std::vector<float_t>& numeric_variables,
                                                  std::vector<uint_t>& buffer,
                                                  valla::IndexedHashSet<valla::Slot<uint_t>, uint_t>& uint_nodes,
                                                  valla::IndexedHashSet<float_t, uint_t>& float_nodes)
{
    buffer.clear();

    valla::encode_as_unsigned_integrals(numeric_variables, float_nodes, std::back_inserter(buffer));

    return valla::insert_sequence(buffer, uint_nodes);
}

void insert_fluent_atoms_to_fact_set(const boost::dynamic_bitset<>& fluent_atoms,
                                     const formalism::OverlayRepository<formalism::planning::Repository>& atoms_context,
                                     datalog::ProgramExecutionContext& axiom_context)
{
    /// --- Initialize FactSets
    auto merge_context = formalism::planning::MergeDatalogContext { axiom_context.datalog_builder,
                                                                    *axiom_context.repository,
                                                                    axiom_context.task_to_program_execution_context.merge_cache };

    for (auto i = fluent_atoms.find_first(); i != boost::dynamic_bitset<>::npos; i = fluent_atoms.find_next(i))
        axiom_context.facts_execution_context.fact_sets.fluent_sets.predicate.insert(make_view(
            formalism::planning::merge_p2d<formalism::FluentTag,
                                           formalism::OverlayRepository<formalism::planning::Repository>,
                                           formalism::datalog::Repository,
                                           formalism::FluentTag>(make_view(Index<formalism::planning::GroundAtom<formalism::FluentTag>>(i), atoms_context),
                                                                 merge_context)
                .first,
            merge_context.destination));
}

void insert_derived_atoms_to_fact_set(const boost::dynamic_bitset<>& derived_atoms,
                                      const formalism::OverlayRepository<formalism::planning::Repository>& atoms_context,
                                      datalog::ProgramExecutionContext& axiom_context)
{
    /// --- Initialize FactSets
    auto merge_context = formalism::planning::MergeDatalogContext { axiom_context.datalog_builder,
                                                                    *axiom_context.repository,
                                                                    axiom_context.task_to_program_execution_context.merge_cache };

    for (auto i = derived_atoms.find_first(); i != boost::dynamic_bitset<>::npos; i = derived_atoms.find_next(i))
        axiom_context.facts_execution_context.fact_sets.fluent_sets.predicate.insert(make_view(
            formalism::planning::merge_p2d<formalism::DerivedTag,
                                           formalism::OverlayRepository<formalism::planning::Repository>,
                                           formalism::datalog::Repository,
                                           formalism::FluentTag>(make_view(Index<formalism::planning::GroundAtom<formalism::DerivedTag>>(i), atoms_context),
                                                                 merge_context)
                .first,
            merge_context.destination));
}

void insert_numeric_variables_to_fact_set(const std::vector<float_t>& numeric_variables,
                                          const formalism::OverlayRepository<formalism::planning::Repository>& numeric_variables_context,
                                          datalog::ProgramExecutionContext& axiom_context)
{
    /// --- Initialize FactSets
    auto merge_context = formalism::planning::MergeDatalogContext { axiom_context.datalog_builder,
                                                                    *axiom_context.repository,
                                                                    axiom_context.task_to_program_execution_context.merge_cache };

    for (uint_t i = 0; i < numeric_variables.size(); ++i)
    {
        if (!std::isnan(numeric_variables[i]))
            axiom_context.facts_execution_context.fact_sets.fluent_sets.function.insert(
                make_view(formalism::planning::merge_p2d(
                              make_view(Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>>(i), numeric_variables_context),
                              merge_context)
                              .first,
                          merge_context.destination),
                numeric_variables[i]);
    }
}

void insert_fact_sets_into_assignment_sets(datalog::ProgramExecutionContext& program_context)
{
    auto& fluent_fact_sets = program_context.facts_execution_context.fact_sets.get<formalism::FluentTag>();
    auto& fluent_assignment_sets = program_context.facts_execution_context.assignment_sets.get<formalism::FluentTag>();

    /// --- Initialize AssignmentSets
    fluent_assignment_sets.insert(fluent_fact_sets);

    /// --- Initialize RuleExecutionContext
    for (auto& rule_context : program_context.rule_execution_contexts)
        rule_context.initialize(program_context.facts_execution_context.assignment_sets);
}

void insert_extended_state(const UnpackedState<LiftedTask>& unpacked_state,
                           const formalism::OverlayRepository<formalism::planning::Repository>& atoms_context,
                           datalog::ProgramExecutionContext& action_context)
{
    action_context.facts_execution_context.reset<formalism::FluentTag>();
    action_context.task_to_program_execution_context.clear();

    insert_fluent_atoms_to_fact_set(unpacked_state.get_atoms<formalism::FluentTag>(), atoms_context, action_context);
    insert_derived_atoms_to_fact_set(unpacked_state.get_atoms<formalism::DerivedTag>(), atoms_context, action_context);
    insert_numeric_variables_to_fact_set(unpacked_state.get_numeric_variables(), atoms_context, action_context);

    insert_fact_sets_into_assignment_sets(action_context);
}

std::vector<analysis::DomainListListList>
compute_parameter_domains_per_cond_effect_per_action(View<Index<formalism::planning::Task>, formalism::OverlayRepository<formalism::planning::Repository>> task)
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
}
