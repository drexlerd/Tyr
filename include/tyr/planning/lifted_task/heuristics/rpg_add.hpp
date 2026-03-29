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

#ifndef TYR_PLANNING_LIFTED_TASK_HEURISTICS_ADD_HPP_
#define TYR_PLANNING_LIFTED_TASK_HEURISTICS_ADD_HPP_

#include "tyr/datalog/policies/annotation.hpp"
#include "tyr/datalog/policies/termination.hpp"
#include "tyr/formalism/planning/merge_datalog.hpp"
#include "tyr/planning/heuristics/rpg_add.hpp"
#include "tyr/planning/lifted_task/heuristics/rpg.hpp"

namespace tyr::planning
{

template<>
class AddRPGHeuristic<LiftedTag> :
    public RPGBase<AddRPGHeuristic<LiftedTag>,
                   datalog::OrAnnotationPolicy,
                   datalog::AndAnnotationPolicy<datalog::SumAggregation>,
                   datalog::TerminationPolicy<datalog::SumAggregation>>
{
public:
    AddRPGHeuristic(std::shared_ptr<Task<LiftedTag>> task, ExecutionContextPtr execution_context);

    static std::shared_ptr<AddRPGHeuristic<LiftedTag>> create(std::shared_ptr<Task<LiftedTag>> task, ExecutionContextPtr execution_context);

    float_t extract_cost_and_set_preferred_actions_impl(const StateView<LiftedTag>& state);
};

}

#endif
