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

#ifndef TYR_PLANNING_TASK_UTILS_HPP_
#define TYR_PLANNING_TASK_UTILS_HPP_

#include "tyr/analysis/domains.hpp"
#include "tyr/common/config.hpp"
#include "tyr/datalog/declarations.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/planning/merge_datalog.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"

#include <boost/dynamic_bitset.hpp>
#include <valla/valla.hpp>
#include <vector>

namespace tyr::planning
{
extern void fill_atoms(valla::Slot<uint_t> slot,
                       const valla::IndexedHashSet<valla::Slot<uint_t>, uint_t>& uint_nodes,
                       std::vector<uint_t>& buffer,
                       boost::dynamic_bitset<>& atoms);

extern void fill_numeric_variables(valla::Slot<uint_t> slot,
                                   const valla::IndexedHashSet<valla::Slot<uint_t>, uint_t>& uint_nodes,
                                   const valla::IndexedHashSet<float_t, uint_t>& float_nodes,
                                   std::vector<uint_t>& buffer,
                                   std::vector<float_t>& numeric_variables);

extern valla::Slot<uint_t>
create_atoms_slot(const boost::dynamic_bitset<>& atoms, std::vector<uint_t>& buffer, valla::IndexedHashSet<valla::Slot<uint_t>, uint_t>& uint_nodes);

extern valla::Slot<uint_t> create_numeric_variables_slot(const std::vector<float_t>& numeric_variables,
                                                         std::vector<uint_t>& buffer,
                                                         valla::IndexedHashSet<valla::Slot<uint_t>, uint_t>& uint_nodes,
                                                         valla::IndexedHashSet<float_t, uint_t>& float_nodes);

extern void insert_fluent_atoms_to_fact_set(const boost::dynamic_bitset<>& fluent_atoms,
                                            const formalism::planning::Repository& atoms_context,
                                            formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& merge_context,
                                            datalog::TaggedFactSets<formalism::FluentTag>& fact_sets);

extern void insert_derived_atoms_to_fact_set(const boost::dynamic_bitset<>& derived_atoms,
                                             const formalism::planning::Repository& atoms_context,
                                             formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& merge_context,
                                             datalog::TaggedFactSets<formalism::FluentTag>& fact_sets);

extern void insert_numeric_variables_to_fact_set(const std::vector<float_t>& numeric_variables,
                                                 const formalism::planning::Repository& numeric_variables_context,
                                                 formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& merge_context,
                                                 datalog::TaggedFactSets<formalism::FluentTag>& fact_sets);

extern void insert_fact_sets_into_assignment_sets(const datalog::TaggedFactSets<formalism::FluentTag>& fact_sets,
                                                  datalog::TaggedAssignmentSets<formalism::FluentTag>& assignment_sets);

extern void insert_extended_state(const UnpackedState<LiftedTask>& unpacked_state,
                                  const formalism::planning::Repository& atoms_context,
                                  formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& merge_context,
                                  datalog::TaggedFactSets<formalism::FluentTag>& fact_sets,
                                  datalog::TaggedAssignmentSets<formalism::FluentTag>& assignment_sets);

extern std::vector<analysis::DomainListListList>
compute_parameter_domains_per_cond_effect_per_action(View<Index<formalism::planning::Task>, formalism::planning::Repository> task);
}

#endif