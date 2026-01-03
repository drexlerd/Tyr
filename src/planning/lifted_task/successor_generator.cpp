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

#include "tyr/planning/lifted_task/successor_generator.hpp"

#include "../metric.hpp"
#include "../task_utils.hpp"
#include "tyr/datalog/bottom_up.hpp"
#include "tyr/formalism/planning/grounder.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task/match_tree/match_tree.hpp"
#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/lifted_task/axiom_evaluator.hpp"
#include "tyr/planning/lifted_task/state_repository.hpp"
#include "tyr/planning/programs/action.hpp"
#include "tyr/planning/successor_generator.hpp"

namespace d = tyr::datalog;
namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;

namespace tyr::planning
{

SuccessorGenerator<LiftedTask>::SuccessorGenerator(std::shared_ptr<LiftedTask> task) :
    m_task(std::move(task)),
    m_workspace(m_task->get_action_program().get_program_context(), m_task->get_action_program().get_const_program_workspace()),
    m_aps(d::NoOrAnnotationPolicy(),
          std::vector<d::NoAndAnnotationPolicy>(m_workspace.rule_deltas.size()),
          d::OrAnnotationsList(),
          std::vector<d::AndAnnotationsMap>(m_workspace.rule_deltas.size()),
          std::vector<d::HeadToWitness>(m_workspace.rule_deltas.size())),
    m_tp(d::NoTerminationPolicy()),
    m_state_repository(std::make_shared<StateRepository<LiftedTask>>(m_task)),
    m_executor()
{
}

Node<LiftedTask> SuccessorGenerator<LiftedTask>::get_initial_node()
{
    auto initial_state = m_state_repository->get_initial_state();

    const auto state_context = StateContext<LiftedTask>(*m_task, initial_state.get_unpacked_state(), 0);

    const auto state_metric = evaluate_metric(m_task->get_task().get_metric(), m_task->get_task().get_auxiliary_fterm_value(), state_context);

    return Node<LiftedTask>(std::move(initial_state), state_metric);
}

std::vector<LabeledNode<LiftedTask>> SuccessorGenerator<LiftedTask>::get_labeled_successor_nodes(const Node<LiftedTask>& node)
{
    auto result = std::vector<LabeledNode<LiftedTask>> {};

    get_labeled_successor_nodes(node, result);

    return result;
}

void SuccessorGenerator<LiftedTask>::get_labeled_successor_nodes(const Node<LiftedTask>& node, std::vector<LabeledNode<LiftedTask>>& out_nodes)
{
    out_nodes.clear();

    const auto state = node.get_state();

    insert_extended_state(state.get_unpacked_state(), *m_task->get_repository(), m_workspace, m_task->get_action_program().get_const_program_workspace());

    d::solve_bottom_up(m_workspace, m_task->get_action_program().get_const_program_workspace(), m_aps, m_tp);

    const auto state_context = StateContext<LiftedTask>(*m_task, state.get_unpacked_state(), node.get_metric());

    out_nodes.clear();

    auto fluent_assign = UnorderedMap<Index<fp::FDRVariable<f::FluentTag>>, fp::FDRValue> {};
    auto iter_workspace = itertools::cartesian_set::Workspace<Index<f::Object>> {};

    /// TODO: store facts by predicate such that we can swap the iteration, i.e., first over predicate_to_actions_mapping, then facts of the predicate
    for (const auto& set : m_workspace.facts.fact_sets.predicate.get_sets())
    {
        for (const auto& fact : set.get_facts())
        {
            if (m_task->get_action_program().get_predicate_to_actions_mapping().contains(fact.get_predicate().get_index()))
            {
                for (const auto action_index : m_task->get_action_program().get_predicate_to_actions_mapping().at(fact.get_predicate().get_index()))
                {
                    auto grounder_context = fp::GrounderContext { m_workspace.planning_builder, *m_task->get_repository(), m_workspace.d2p.binding };

                    const auto action = make_view(action_index, grounder_context.destination);

                    m_workspace.d2p.binding = fact.get_objects().get_data();

                    const auto ground_action_index = ground(action,
                                                            grounder_context,
                                                            m_task->get_parameter_domains_per_cond_effect_per_action()[action_index.get_value()],
                                                            fluent_assign,
                                                            iter_workspace,
                                                            *m_task->get_fdr_context())
                                                         .first;

                    const auto ground_action = make_view(ground_action_index, grounder_context.destination);

                    if (m_executor.is_applicable(ground_action, state_context))
                        out_nodes.emplace_back(ground_action, m_executor.apply_action(state_context, ground_action, *m_state_repository));
                }
            }
        }
    }
}

State<LiftedTask> SuccessorGenerator<LiftedTask>::get_state(StateIndex state_index) { return m_state_repository->get_registered_state(state_index); }

}
