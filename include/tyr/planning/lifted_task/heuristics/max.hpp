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

#ifndef TYR_PLANNING_LIFTED_TASK_HEURISTICS_MAX_HPP_
#define TYR_PLANNING_LIFTED_TASK_HEURISTICS_MAX_HPP_

#include "tyr/datalog/policies/annotation.hpp"
#include "tyr/datalog/policies/termination.hpp"
#include "tyr/formalism/planning/merge_datalog.hpp"
#include "tyr/planning/heuristics/max.hpp"
#include "tyr/planning/lifted_task/heuristics/rpg.hpp"

namespace tyr::planning
{

template<>
class MaxHeuristic<LiftedTask> :
    public RPGBase<MaxHeuristic<LiftedTask>,
                   datalog::OrAnnotationPolicy,
                   datalog::AndAnnotationPolicy<datalog::MaxAggregation>,
                   datalog::TerminationPolicy<datalog::MaxAggregation>>
{
public:
    MaxHeuristic(std::shared_ptr<LiftedTask> task) :
        RPGBase<MaxHeuristic<LiftedTask>,
                datalog::OrAnnotationPolicy,
                datalog::AndAnnotationPolicy<datalog::MaxAggregation>,
                datalog::TerminationPolicy<datalog::MaxAggregation>>(
            task,
            datalog::OrAnnotationPolicy(),
            datalog::AndAnnotationPolicy<datalog::MaxAggregation>(),
            datalog::TerminationPolicy<datalog::MaxAggregation>(
                task->get_rpg_program().get_program_context().get_program().get_predicates<formalism::FluentTag>().size()))
    {
    }

    static std::shared_ptr<MaxHeuristic<LiftedTask>> create(std::shared_ptr<LiftedTask> task) { return std::make_shared<MaxHeuristic<LiftedTask>>(task); }

    float_t extract_cost_and_set_preferred_actions_impl(const State<LiftedTask>& state) { return m_workspace.tp.get_total_cost(this->m_workspace.or_annot); }
};

}

#endif
