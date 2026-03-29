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
#include "tyr/formalism/planning/merge_datalog_decl.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"
#include "tyr/planning/task.hpp"

namespace tyr::planning
{

// Fact Set

extern void insert_fluent_atoms_to_fact_set(const UnpackedState<LiftedTag>& state,
                                            const formalism::planning::Repository& repository,
                                            formalism::planning::MergeDatalogContext& merge_context,
                                            datalog::TaggedFactSets<formalism::FluentTag>& fact_sets);

}

#endif