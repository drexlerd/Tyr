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

#ifndef TYR_PLANNING_LIFTED_TASK_HEURISTICS_FF_HPP_
#define TYR_PLANNING_LIFTED_TASK_HEURISTICS_FF_HPP_

#include "tyr/datalog/policies/annotation.hpp"
#include "tyr/datalog/policies/termination.hpp"
#include "tyr/formalism/planning/merge_datalog.hpp"
#include "tyr/planning/lifted_task/heuristics/rpg.hpp"

namespace tyr::planning
{

class FF : public RPGBase<FF>
{
public:
    FF(std::shared_ptr<LiftedTask> task) :
        RPGBase<FF>(std::move(task)),
        m_aps(datalog::OrAnnotationPolicy(),
              std::vector<datalog::AndAnnotationPolicy<datalog::SumAggregation>>(this->m_workspace.rule_deltas.size()),
              datalog::OrAnnotationsList(this->m_task->get_rpg_program().get_program_context().get_program().get_predicates<formalism::FluentTag>().size()),
              std::vector<datalog::AndAnnotationsMap>(this->m_workspace.rule_deltas.size()),
              std::vector<datalog::HeadToWitness>(this->m_workspace.rule_deltas.size())),
        m_tp(datalog::TerminationPolicy(this->m_task->get_rpg_program().get_program_context().get_program().get_predicates<formalism::FluentTag>().size()))
    {
    }

    void set_goal_impl(View<Index<formalism::planning::GroundConjunctiveCondition>, formalism::OverlayRepository<formalism::planning::Repository>> goal)
    {
        this->m_workspace.facts.goal_fact_sets.reset();

        auto merge_context =
            formalism::planning::MergeDatalogContext { this->m_workspace.datalog_builder, this->m_workspace.repository, this->m_workspace.p2d.merge_cache };

        for (const auto fact : goal.get_facts<formalism::FluentTag>())
        {
            if (fact.get_value() != formalism::planning::FDRValue::none())
                this->m_workspace.facts.goal_fact_sets.insert(formalism::planning::merge_p2d(fact.get_atom(), merge_context).first);
        }
    }

    auto& get_annotation_policies_impl() noexcept { return m_aps; }
    auto& get_termination_policy_impl() noexcept { return m_tp; }

    float_t extract_cost_and_set_preferred_actions_impl(const State<LiftedTask>& state) { return m_tp.get_total_cost(m_aps.or_annot); }

    const UnorderedSet<Index<formalism::planning::GroundAction>>& get_preferred_actions() override { return m_preferred_actions; }

private:
    datalog::AnnotationPolicies<datalog::OrAnnotationPolicy, datalog::AndAnnotationPolicy<datalog::SumAggregation>> m_aps;
    datalog::TerminationPolicy m_tp;

    UnorderedSet<Index<formalism::planning::GroundAction>> m_preferred_actions;
};

}

#endif
