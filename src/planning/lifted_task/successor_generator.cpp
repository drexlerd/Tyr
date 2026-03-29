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
#include "tyr/datalog/bottom_up.hpp"
#include "tyr/datalog/contexts/program.hpp"
#include "tyr/formalism/planning/grounder.hpp"
#include "tyr/formalism/planning/merge_datalog.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task/match_tree/match_tree.hpp"
#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/lifted_task/axiom_evaluator.hpp"
#include "tyr/planning/lifted_task/state_repository.hpp"
#include "tyr/planning/lifted_task/state_view.hpp"
#include "tyr/planning/programs/action.hpp"
#include "tyr/planning/successor_generator.hpp"
#include "tyr/planning/task_utils.hpp"

namespace d = tyr::datalog;
namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;
namespace df = tyr::formalism::datalog;

namespace tyr::planning
{
namespace
{
void insert_derived_atoms_to_fact_set(const UnpackedState<LiftedTag>& state,
                                      const formalism::planning::Repository& repository,
                                      fp::MergeDatalogContext& merge_context,
                                      datalog::TaggedFactSets<f::FluentTag>& fact_sets)
{
    for (const auto atom : state.get_derived_atoms_view(repository))
        fact_sets.predicate.insert(fp::merge_p2d<f::DerivedTag, f::FluentTag>(atom, merge_context).first);
}

void insert_numeric_variables_to_fact_set(const UnpackedState<LiftedTag>& state,
                                          const formalism::planning::Repository& repository,
                                          fp::MergeDatalogContext& merge_context,
                                          datalog::TaggedFactSets<f::FluentTag>& fact_sets)
{
    for (const auto& [fterm, value] : state.get_fluent_fterm_values_view(repository))
        fact_sets.function.insert(fp::merge_p2d(fterm, merge_context).first, value);
}

void insert_extended_state(const UnpackedState<LiftedTag>& unpacked_state,
                           const fp::Repository& atoms_context,
                           fp::MergeDatalogContext& merge_context,
                           datalog::TaggedFactSets<f::FluentTag>& fact_sets,
                           datalog::TaggedAssignmentSets<f::FluentTag>& assignment_sets)
{
    fact_sets.reset();
    assignment_sets.reset();

    insert_fluent_atoms_to_fact_set(unpacked_state, atoms_context, merge_context, fact_sets);
    insert_derived_atoms_to_fact_set(unpacked_state, atoms_context, merge_context, fact_sets);
    insert_numeric_variables_to_fact_set(unpacked_state, atoms_context, merge_context, fact_sets);

    assignment_sets.insert(fact_sets);
}
}

SuccessorGenerator<LiftedTag>::SuccessorGenerator(std::shared_ptr<Task<LiftedTag>> task, ExecutionContextPtr execution_context) :
    m_task(std::move(task)),
    m_execution_context(std::move(execution_context)),
    m_workspace(m_task->get_action_program().get_program_context(),
                m_task->get_action_program().get_const_program_workspace(),
                d::NoOrAnnotationPolicy(),
                d::NoAndAnnotationPolicy(),
                d::NoTerminationPolicy()),
    m_state_repository(std::make_shared<StateRepository<LiftedTag>>(m_task, m_execution_context)),
    m_executor()
{
}

std::shared_ptr<SuccessorGenerator<LiftedTag>> SuccessorGenerator<LiftedTag>::create(std::shared_ptr<Task<LiftedTag>> task,
                                                                                     ExecutionContextPtr execution_context)
{
    return std::make_shared<SuccessorGenerator<LiftedTag>>(std::move(task), std::move(execution_context));
}

Node<LiftedTag> SuccessorGenerator<LiftedTag>::get_initial_node()
{
    auto initial_state = m_state_repository->get_initial_state();

    const auto state_context = StateContext<LiftedTag>(*m_task, initial_state.get_unpacked_state(), 0);

    const auto state_metric = evaluate_metric(m_task->get_task().get_metric(), m_task->get_task().get_auxiliary_fterm_value(), state_context);

    return Node<LiftedTag>(std::move(initial_state), state_metric);
}

std::vector<LabeledNode<LiftedTag>> SuccessorGenerator<LiftedTag>::get_labeled_successor_nodes(const Node<LiftedTag>& node)
{
    auto result = std::vector<LabeledNode<LiftedTag>> {};

    get_labeled_successor_nodes(node, result);

    return result;
}

void SuccessorGenerator<LiftedTag>::get_labeled_successor_nodes(const Node<LiftedTag>& node, std::vector<LabeledNode<LiftedTag>>& out_nodes)
{
    out_nodes.clear();

    const auto state = node.get_state();

    auto merge_context = fp::MergeDatalogContext { m_workspace.datalog_builder, m_workspace.workspace_repository };

    insert_extended_state(state.get_unpacked_state(), *m_task->get_repository(), merge_context, m_workspace.facts.fact_sets, m_workspace.facts.assignment_sets);

    auto ctx = d::ProgramExecutionContext(m_workspace, m_task->get_action_program().get_const_program_workspace());
    ctx.clear();

    m_execution_context->arena().execute([&] { d::solve_bottom_up(ctx); });

    const auto state_context = StateContext<LiftedTag>(*m_task, state.get_unpacked_state(), node.get_metric());

    out_nodes.clear();

    auto fluent_assign = UnorderedMap<Index<fp::FDRVariable<f::FluentTag>>, fp::FDRValue> {};
    auto iter_workspace = itertools::cartesian_set::Workspace<Index<f::Object>> {};

    for (const auto& set : m_workspace.facts.fact_sets.predicate.get_sets())
    {
        for (const auto& binding : set.get_bindings())
        {
            const auto& mapping = m_task->get_action_program().get_predicate_to_actions_mapping();

            if (const auto it = mapping.find(binding.get_relation()); it != mapping.end())
            {
                const auto action = it->second;

                auto grounder_context = fp::GrounderContext { m_workspace.planning_builder, *m_task->get_repository(), m_workspace.d2p.binding };

                m_workspace.d2p.binding.clear();
                for (const auto object : binding.get_objects())
                    m_workspace.d2p.binding.push_back(object.get_index());

                const auto ground_action = fp::ground(action,
                                                      grounder_context,
                                                      m_task->get_parameter_domains_per_cond_effect_per_action()[uint_t(action.get_index())],
                                                      fluent_assign,
                                                      iter_workspace,
                                                      *m_task->get_fdr_context())
                                               .first;

                if (m_executor.is_applicable(ground_action, state_context))
                    out_nodes.emplace_back(ground_action, m_executor.apply_action(state_context, ground_action, *m_state_repository));
            }
        }
    }
}

Node<LiftedTag> SuccessorGenerator<LiftedTag>::get_successor_node(const Node<LiftedTag>& node, fp::GroundActionView action)
{
    const auto& state = node.get_state();
    const auto state_context = StateContext<LiftedTag>(*m_task, state.get_unpacked_state(), node.get_metric());

    return m_executor.apply_action(state_context, action, *m_state_repository);
}

Node<LiftedTag> SuccessorGenerator<LiftedTag>::get_node(Index<State<LiftedTag>> state_index)
{
    auto state = m_state_repository->get_registered_state(state_index);
    const auto state_context = StateContext<LiftedTag>(*m_task, state.get_unpacked_state(), 0);
    const auto state_metric = evaluate_metric(m_task->get_task().get_metric(), m_task->get_task().get_auxiliary_fterm_value(), state_context);

    return Node<LiftedTag>(std::move(state), state_metric);
}

static_assert(SuccessorGeneratorConcept<SuccessorGenerator<LiftedTag>, LiftedTag>);
}
