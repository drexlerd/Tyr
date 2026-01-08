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
#include "tyr/planning/heuristics/add.hpp"
#include "tyr/planning/lifted_task/heuristics/rpg.hpp"

namespace tyr::planning
{

template<>
class AddHeuristic<LiftedTask> : public RPGBase<AddHeuristic<LiftedTask>>
{
public:
    AddHeuristic(std::shared_ptr<LiftedTask> task) :
        RPGBase<AddHeuristic<LiftedTask>>(std::move(task)),
        m_aps(datalog::OrAnnotationPolicy(),
              std::vector<datalog::AndAnnotationPolicy<datalog::SumAggregation>>(this->m_workspace.rule_deltas.size()),
              datalog::OrAnnotationsList(this->m_task->get_rpg_program().get_program_context().get_program().get_predicates<formalism::FluentTag>().size()),
              std::vector<datalog::AndAnnotationsMap>(this->m_workspace.rule_deltas.size()),
              std::vector<datalog::HeadToWitness>(this->m_workspace.rule_deltas.size())),
        m_tp(datalog::TerminationPolicy<datalog::SumAggregation>(
            this->m_task->get_rpg_program().get_program_context().get_program().get_predicates<formalism::FluentTag>().size()))
    {
    }

    static std::shared_ptr<AddHeuristic<LiftedTask>> create(std::shared_ptr<LiftedTask> task) { return std::make_shared<AddHeuristic<LiftedTask>>(task); }

    float_t extract_cost_and_set_preferred_actions_impl(const State<LiftedTask>& state) { return m_tp.get_total_cost(m_aps.or_annot); }

    auto& get_annotation_policies_impl() noexcept { return m_aps; }
    auto& get_termination_policy_impl() noexcept { return m_tp; }

private:
    datalog::AnnotationPolicies<datalog::OrAnnotationPolicy, datalog::AndAnnotationPolicy<datalog::SumAggregation>> m_aps;
    datalog::TerminationPolicy<datalog::SumAggregation> m_tp;
};

}

#endif
