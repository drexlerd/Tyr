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

#include "tyr/planning/lifted_task/heuristics/rpg_ff.hpp"

#include "tyr/datalog/policies/annotation.hpp"
#include "tyr/datalog/policies/termination.hpp"
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/datalog/grounder.hpp"
#include "tyr/formalism/planning/formatter.hpp"
#include "tyr/formalism/planning/grounder.hpp"
#include "tyr/formalism/planning/merge_datalog.hpp"
#include "tyr/planning/applicability.hpp"
#include "tyr/planning/lifted_task/heuristics/rpg.hpp"

#include <boost/dynamic_bitset.hpp>
#include <cassert>

namespace tyr::planning
{

FFRPGHeuristic<LiftedTag>::FFRPGHeuristic(std::shared_ptr<Task<LiftedTag>> task, ExecutionContextPtr execution_context) :
    RPGBase<FFRPGHeuristic<LiftedTag>,
            datalog::OrAnnotationPolicy,
            datalog::AndAnnotationPolicy<datalog::SumAggregation>,
            datalog::TerminationPolicy<datalog::SumAggregation>>(
        task,
        std::move(execution_context),
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
    m_preferred_actions(),
    m_preferred_action_views(),
    m_preferred_action_views_dirty(true)
{
}

std::shared_ptr<FFRPGHeuristic<LiftedTag>> FFRPGHeuristic<LiftedTag>::create(std::shared_ptr<Task<LiftedTag>> task, ExecutionContextPtr execution_context)
{
    return std::make_shared<FFRPGHeuristic<LiftedTag>>(std::move(task), std::move(execution_context));
}

float_t FFRPGHeuristic<LiftedTag>::extract_cost_and_set_preferred_actions_impl(const StateView<LiftedTag>& state)
{
    m_preferred_action_views_dirty = true;
    m_relaxed_plan.clear();
    m_preferred_actions.clear();
    for (auto& bitset : m_markings)
        bitset.reset();

    auto state_context = StateContext<LiftedTag>(*this->m_task, state.get_unpacked_state(), float_t(0));
    auto grounder_context = formalism::planning::GrounderContext { this->m_workspace.planning_builder, *this->m_task->get_repository(), m_binding };

    for (const auto atom : m_workspace.tp.get_bindings())
        extract_relaxed_plan_and_preferred_actions(atom, state_context, grounder_context);

    return m_relaxed_plan.size();
}

const UnorderedSet<Index<formalism::planning::GroundAction>>& FFRPGHeuristic<LiftedTag>::get_preferred_actions() { return m_preferred_actions; }

const UnorderedSet<formalism::planning::GroundActionView>& FFRPGHeuristic<LiftedTag>::get_preferred_action_views()
{
    if (m_preferred_action_views_dirty)
    {
        m_preferred_action_views_dirty = false;
        m_preferred_action_views.clear();
        const auto& repository = *this->m_task->get_repository();
        for (const auto action_index : m_preferred_actions)
            m_preferred_action_views.insert(make_view(action_index, repository));
    }

    return m_preferred_action_views;
}

bool FFRPGHeuristic<LiftedTag>::mark_atom(formalism::datalog::PredicateBindingView<formalism::FluentTag> binding)
{
    const auto g = uint_t(binding.get_index().relation);
    const auto i = uint_t(binding.get_index().row);

    assert(g < m_markings.size());
    if (tyr::test(i, m_markings[g]))
        return true;
    tyr::set(i, true, m_markings[g]);
    return false;
}

void FFRPGHeuristic<LiftedTag>::extract_relaxed_plan_and_preferred_actions(formalism::datalog::PredicateBindingView<formalism::FluentTag> binding,
                                                                           const StateContext<LiftedTag>& state_context,
                                                                           formalism::planning::GrounderContext& grounder_context)
{
    // Base case 1: atom is already marked => do not recurse again
    if (mark_atom(binding))
        return;

    // Base case 2: atom has no witness, i.e., was true initially => do not recurse again
    const auto it = m_workspace.and_annot.find(binding);
    if (it == m_workspace.and_annot.end())
        return;

    const auto& witness = it->second;
    const auto& mapping = this->m_task->get_rpg_program().get_rule_to_action_mapping();

    const auto rule_row = witness.get_rule_row();
    const auto rule = rule_row.get_relation();
    const auto row = rule_row.get_objects();

    if (const auto it = mapping.find(rule); it != mapping.end())
    {
        const auto action = it->second;

        grounder_context.binding.clear();
        for (const auto object : row)
            grounder_context.binding.push_back(object.get_index());

        const auto ground_action = formalism::planning::ground(action,
                                                               grounder_context,
                                                               m_task->get_parameter_domains_per_cond_effect_per_action()[uint_t(action.get_index())],
                                                               m_assign,
                                                               m_iter_workspace,
                                                               *m_task->get_fdr_context())
                                       .first;

        const auto ground_action_index = ground_action.get_index();

        m_relaxed_plan.insert(ground_action_index);

        if (is_applicable(ground_action, state_context, m_effect_families))
            m_preferred_actions.insert(ground_action_index);
    }

    // Divide case: recursively call for preconditions.

    auto datalog_grounder_context = formalism::datalog::GrounderContext { m_workspace.datalog_builder, m_workspace.workspace_repository, m_workspace.binding };

    for (const auto literal : m_task->get_rpg_program()
                                  .get_const_program_workspace()
                                  .rules[uint_t(rule.get_index())]
                                  .get_witness_rule()
                                  .get_body()
                                  .get_literals<formalism::FluentTag>())
    {
        // Cannot do this before the loop because of overwrites during recursion; we could binding from a builder and place it into the grounder context.
        datalog_grounder_context.binding.clear();
        for (const auto object : row)
            datalog_grounder_context.binding.push_back(object.get_index());

        const auto witness_atom = formalism::datalog::ground(literal.get_atom(), datalog_grounder_context).first;

        extract_relaxed_plan_and_preferred_actions(witness_atom.get_row(), state_context, grounder_context);
    }
}

}
