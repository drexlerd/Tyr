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
#include "tyr/planning/lifted_task/heuristics/rpg.hpp"

namespace tyr::planning
{

class Max : public RPGBase<Max>
{
public:
    Max(std::shared_ptr<LiftedTask> task) :
        RPGBase<Max>(std::move(task)),
        m_aps(datalog::OrAnnotationPolicy(),
              std::vector<datalog::AndAnnotationPolicy<datalog::MaxAggregation>>(this->m_workspace.rule_deltas.size()),
              datalog::OrAnnotationsList(this->m_task->get_rpg_program().get_program_context().get_program().get_predicates<formalism::FluentTag>().size()),
              std::vector<datalog::AndAnnotationsMap>(this->m_workspace.rule_deltas.size()),
              std::vector<datalog::HeadToWitness>(this->m_workspace.rule_deltas.size())),
        m_tp(datalog::TerminationPolicy<datalog::MaxAggregation>(
            this->m_task->get_rpg_program().get_program_context().get_program().get_predicates<formalism::FluentTag>().size()))
    {
    }

    float_t extract_cost_and_set_preferred_actions_impl(const State<LiftedTask>& state) { return m_tp.get_total_cost(m_aps.or_annot); }

    auto& get_annotation_policies_impl() noexcept { return m_aps; }
    auto& get_termination_policy_impl() noexcept { return m_tp; }

private:
    datalog::AnnotationPolicies<datalog::OrAnnotationPolicy, datalog::AndAnnotationPolicy<datalog::MaxAggregation>> m_aps;
    datalog::TerminationPolicy<datalog::MaxAggregation> m_tp;
};

}

#endif
