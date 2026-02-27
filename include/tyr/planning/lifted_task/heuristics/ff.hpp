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
#include "tyr/formalism/planning/formatter.hpp"
#include "tyr/formalism/planning/grounder.hpp"
#include "tyr/formalism/planning/merge_datalog.hpp"
#include "tyr/planning/applicability.hpp"
#include "tyr/planning/heuristics/ff.hpp"
#include "tyr/planning/lifted_task/heuristics/rpg.hpp"

#include <boost/dynamic_bitset.hpp>
#include <cassert>

namespace tyr::planning
{

template<>
class FFHeuristic<LiftedTask> :
    public RPGBase<FFHeuristic<LiftedTask>,
                   datalog::OrAnnotationPolicy,
                   datalog::AndAnnotationPolicy<datalog::SumAggregation>,
                   datalog::TerminationPolicy<datalog::SumAggregation>>
{
public:
    FFHeuristic(std::shared_ptr<LiftedTask> task) :
        RPGBase<FFHeuristic<LiftedTask>,
                datalog::OrAnnotationPolicy,
                datalog::AndAnnotationPolicy<datalog::SumAggregation>,
                datalog::TerminationPolicy<datalog::SumAggregation>>(
            task,
            datalog::OrAnnotationPolicy(),
            datalog::AndAnnotationPolicy<datalog::SumAggregation>(),
            datalog::TerminationPolicy<datalog::SumAggregation>(
                task->get_rpg_program().get_program_context().get_program().get_predicates<formalism::FluentTag>().size())),
        m_markings(task->get_rpg_program().get_program_context().get_program().get_predicates<formalism::FluentTag>().size()),
        m_binding(),
        m_assign(),
        m_iter_workspace(),
        m_effect_families(),
        m_relaxed_plan(),
        m_preferred_actions()
    {
    }

    static std::shared_ptr<FFHeuristic<LiftedTask>> create(std::shared_ptr<LiftedTask> task) { return std::make_shared<FFHeuristic<LiftedTask>>(task); }

    float_t extract_cost_and_set_preferred_actions_impl(const State<LiftedTask>& state)
    {
        m_relaxed_plan.clear();
        m_preferred_actions.clear();
        for (auto& bitset : m_markings)
            bitset.reset();

        auto state_context = StateContext<LiftedTask>(*this->m_task, state.get_unpacked_state(), float_t(0));
        auto grounder_context = formalism::planning::GrounderContext { this->m_workspace.planning_builder, *this->m_task->get_repository(), m_binding };

        for (const auto atom : m_workspace.tp.get_atoms())
            extract_relaxed_plan_and_preferred_actions(atom, state_context, grounder_context);

        // std::cout << "Num pref: " << m_preferred_actions.size() << " h: " << m_relaxed_plan.size() << std::endl;
        // for (const auto& action : m_preferred_actions)
        // {
        //     std::cout << std::make_pair(make_view(action, *this->m_task->get_repository()), formalism::planning::PlanFormatting()) << std::endl;
        // }

        return m_relaxed_plan.size();
    }

    const UnorderedSet<Index<formalism::planning::GroundAction>>& get_preferred_actions() override { return m_preferred_actions; }

    bool mark_atom(Index<formalism::datalog::GroundAtom<formalism::FluentTag>> atom)
    {
        assert(uint_t(atom.group) < m_markings.size());
        if (tyr::test(atom.value, m_markings[uint_t(atom.group)]))
            return true;
        tyr::set(atom.value, true, m_markings[uint_t(atom.group)]);
        return false;
    }

private:
    void extract_relaxed_plan_and_preferred_actions(Index<formalism::datalog::GroundAtom<formalism::FluentTag>> atom,
                                                    const StateContext<LiftedTask>& state_context,
                                                    formalism::planning::GrounderContext& grounder_context)
    {
        // Base case 1: atom is already marked => do not recurse again
        if (mark_atom(atom))
            return;

        // Base case 2: atom has no witness, i.e., was true initially => do not recurse again
        const auto it = m_workspace.and_annot.find(atom);
        if (it == m_workspace.and_annot.end())
            return;

        const auto& witness = it->second;
        const auto& mapping = this->m_task->get_rpg_program().get_rule_to_action_mapping();
        // auto merge_context = formalism::planning::MergeDatalogContext { m_workspace.datalog_builder, m_workspace.repository };

        if (const auto it = mapping.find(witness.get_rule()); it != mapping.end())
        {
            const auto action = it->second;

            grounder_context.binding = witness.get_binding().get_data().objects;

            const auto ground_action_index = formalism::planning::ground(make_view(action, grounder_context.destination),
                                                                         grounder_context,
                                                                         m_task->get_parameter_domains_per_cond_effect_per_action()[uint_t(action)],
                                                                         m_assign,
                                                                         m_iter_workspace,
                                                                         *m_task->get_fdr_context())
                                                 .first;

            m_relaxed_plan.insert(ground_action_index);

            const auto ground_action = make_view(ground_action_index, grounder_context.destination);
            if (is_applicable(ground_action, state_context, m_effect_families))
                m_preferred_actions.insert(ground_action_index);

            /*
        for (const auto cond_effect : ground_action.get_effects())
        {
            if (!cond_effect.get_condition().get_facts<formalism::FluentTag>().empty())
                continue;

            for (const auto fact : cond_effect.get_effect().get_facts())
            {
                if (!fact.has_value())
                    continue;

                const auto merged_atom = formalism::planning::merge_p2d(fact.get_atom(), merge_context).first;

                mark_atom(merged_atom);
            }
        }
            */
        }

        // Divide case: recursively call for preconditions.
        for (const auto literal : witness.get_witness_condition().get_literals<formalism::FluentTag>())
        {
            extract_relaxed_plan_and_preferred_actions(literal.get_atom().get_index(), state_context, grounder_context);
        }
    }

private:
    std::vector<boost::dynamic_bitset<>> m_markings;

    /// For grounding actions
    IndexList<formalism::Object> m_binding;
    UnorderedMap<Index<formalism::planning::FDRVariable<formalism::FluentTag>>, formalism::planning::FDRValue> m_assign;
    itertools::cartesian_set::Workspace<Index<formalism::Object>> m_iter_workspace;
    formalism::planning::EffectFamilyList m_effect_families;

    UnorderedSet<Index<formalism::planning::GroundAction>> m_relaxed_plan;
    UnorderedSet<Index<formalism::planning::GroundAction>> m_preferred_actions;
};

}

#endif
