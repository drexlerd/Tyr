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

#include "tyr/planning/lifted_task/heuristics/rpg_add.hpp"

namespace tyr::planning
{
AddRPGHeuristic<LiftedTag>::AddRPGHeuristic(std::shared_ptr<Task<LiftedTag>> task, ExecutionContextPtr execution_context) :
    RPGBase<AddRPGHeuristic<LiftedTag>,
            datalog::OrAnnotationPolicy,
            datalog::AndAnnotationPolicy<datalog::SumAggregation>,
            datalog::TerminationPolicy<datalog::SumAggregation>>(
        task,
        std::move(execution_context),
        datalog::OrAnnotationPolicy(),
        datalog::AndAnnotationPolicy<datalog::SumAggregation>(),
        datalog::TerminationPolicy<datalog::SumAggregation>(
            task->get_rpg_program().get_program_context().get_program().get_predicates<formalism::FluentTag>().size()))
{
}

std::shared_ptr<AddRPGHeuristic<LiftedTag>> AddRPGHeuristic<LiftedTag>::create(std::shared_ptr<Task<LiftedTag>> task, ExecutionContextPtr execution_context)
{
    return std::make_shared<AddRPGHeuristic<LiftedTag>>(std::move(task), std::move(execution_context));
}

float_t AddRPGHeuristic<LiftedTag>::extract_cost_and_set_preferred_actions_impl(const StateView<LiftedTag>& state)
{
    return m_workspace.tp.get_total_cost(this->m_workspace.or_annot);
}

}
