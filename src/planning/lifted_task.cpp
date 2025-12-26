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

#include "tyr/planning/lifted_task.hpp"

#include "metric.hpp"
#include "task_utils.hpp"
#include "tyr/analysis/domains.hpp"
#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/grounder_planning.hpp"
#include "tyr/formalism/merge_datalog.hpp"
#include "tyr/formalism/merge_planning.hpp"
#include "tyr/grounder/consistency_graph.hpp"
#include "tyr/grounder/execution_contexts.hpp"
#include "tyr/grounder/generator.hpp"
#include "tyr/planning/applicability.hpp"
#include "tyr/planning/domain.hpp"
#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/lifted_task/node.hpp"
#include "tyr/planning/lifted_task/packed_state.hpp"
#include "tyr/planning/lifted_task/state.hpp"
#include "tyr/planning/lifted_task/task_grounder.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"
#include "tyr/solver/bottom_up.hpp"

using namespace tyr::formalism;
using namespace tyr::grounder;
using namespace tyr::solver;

namespace tyr::planning
{

static void read_solution_and_instantiate_labeled_successor_nodes(const StateContext<LiftedTask>& state_context,
                                                                  OverlayRepository<Repository>& task_repository,
                                                                  ProgramExecutionContext& action_context,
                                                                  BinaryFDRContext<OverlayRepository<Repository>>& fdr_context,
                                                                  ActionExecutor& successor_generator,
                                                                  const ApplicableActionProgram& action_program,
                                                                  const std::vector<analysis::DomainListListList>& parameter_domains_per_cond_effect_per_action,
                                                                  std::vector<LabeledNode<LiftedTask>>& out_nodes)
{
    out_nodes.clear();

    auto& fluent_assign = action_context.planning_execution_context.fluent_assign;
    auto& iter_workspace = action_context.planning_execution_context.iter_workspace;

    /// TODO: store facts by predicate such that we can swap the iteration, i.e., first over predicate_to_actions_mapping, then facts of the predicate
    for (const auto fact : action_context.facts_execution_context.fact_sets.fluent_sets.predicate.get_facts())
    {
        if (action_program.get_predicate_to_actions_mapping().contains(fact.get_predicate().get_index()))
        {
            for (const auto action_index : action_program.get_predicate_to_actions_mapping().at(fact.get_predicate().get_index()))
            {
                auto grounder_context = GrounderContext { action_context.builder, task_repository, action_context.program_to_task_execution_context.binding };

                const auto action = make_view(action_index, grounder_context.destination);

                action_context.program_to_task_execution_context.binding = fact.get_binding().get_objects().get_data();

                const auto ground_action_index = ground_planning(action,
                                                                 grounder_context,
                                                                 parameter_domains_per_cond_effect_per_action[action_index.get_value()],
                                                                 fluent_assign,
                                                                 iter_workspace,
                                                                 fdr_context)
                                                     .first;

                const auto ground_action = make_view(ground_action_index, grounder_context.destination);

                if (successor_generator.is_applicable(ground_action, state_context))
                    out_nodes.emplace_back(ground_action, successor_generator.apply_action(state_context, ground_action));
            }
        }
    }
}

LiftedTask::LiftedTask(DomainPtr domain,
                       RepositoryPtr repository,
                       OverlayRepositoryPtr<Repository> overlay_repository,
                       View<Index<Task>, OverlayRepository<Repository>> task,
                       BinaryFDRContext<OverlayRepository<Repository>> fdr_context) :
    m_domain(std::move(domain)),
    m_repository(std::move(repository)),
    m_overlay_repository(std::move(overlay_repository)),
    m_task(task),
    m_state_repository(*this, std::move(fdr_context)),
    m_static_atoms_bitset(),
    m_static_numeric_variables(),
    m_successor_generator(),
    m_axiom_evaluator(m_task, m_overlay_repository),
    m_action_program(*this),
    m_action_context(m_action_program.get_program(),
                     m_action_program.get_repository(),
                     m_action_program.get_domains(),
                     m_action_program.get_strata(),
                     m_action_program.get_listeners()),
    m_parameter_domains_per_cond_effect_per_action(compute_parameter_domains_per_cond_effect_per_action(task))
{
    for (const auto atom : m_task.template get_atoms<StaticTag>())
        set(uint_t(atom.get_index()), true, m_static_atoms_bitset);

    for (const auto fterm_value : m_task.template get_fterm_values<StaticTag>())
        set(uint_t(fterm_value.get_fterm().get_index()), fterm_value.get_value(), m_static_numeric_variables, std::numeric_limits<float_t>::quiet_NaN());
}

State<LiftedTask> LiftedTask::get_state(StateIndex state_index) { return m_state_repository.get_registered_state(state_index); }

State<LiftedTask> LiftedTask::register_state(SharedObjectPoolPtr<UnpackedState<LiftedTask>> state) { return m_state_repository.register_state(state); }

void LiftedTask::compute_extended_state(UnpackedState<LiftedTask>& unpacked_state) { m_axiom_evaluator.compute_extended_state(unpacked_state); }

Node<LiftedTask> LiftedTask::get_initial_node()
{
    auto initial_state = m_state_repository.get_initial_state();

    const auto state_context = StateContext<LiftedTask>(*this, initial_state.get_unpacked_state(), 0);

    const auto state_metric = evaluate_metric(get_task().get_metric(), get_task().get_auxiliary_fterm_value(), state_context);

    return Node<LiftedTask>(std::move(initial_state), state_metric);
}

std::vector<LabeledNode<LiftedTask>> LiftedTask::get_labeled_successor_nodes(const Node<LiftedTask>& node)
{
    auto result = std::vector<LabeledNode<LiftedTask>> {};

    get_labeled_successor_nodes(node, result);

    return result;
}

void LiftedTask::get_labeled_successor_nodes(const Node<LiftedTask>& node, std::vector<LabeledNode<LiftedTask>>& out_nodes)
{
    out_nodes.clear();

    const auto state = node.get_state();

    insert_extended_state(state.get_unpacked_state(), *m_overlay_repository, m_action_context);

    solve_bottom_up(m_action_context);

    const auto state_context = StateContext<LiftedTask>(*this, state.get_unpacked_state(), node.get_metric());

    read_solution_and_instantiate_labeled_successor_nodes(state_context,
                                                          *m_overlay_repository,
                                                          m_action_context,
                                                          m_state_repository.get_fdr_context(),
                                                          m_successor_generator,
                                                          m_action_program,
                                                          m_parameter_domains_per_cond_effect_per_action,
                                                          out_nodes);
}

GroundTaskPtr LiftedTask::get_ground_task() { return ground_task(m_domain, m_task, *m_overlay_repository, m_state_repository, m_static_atoms_bitset); }

const ApplicableActionProgram& LiftedTask::get_action_program() const { return m_action_program; }

}
