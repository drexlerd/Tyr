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

#include "tyr/planning/ground_task.hpp"

#include "metric.hpp"
#include "task_utils.hpp"
#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/builder.hpp"
#include "tyr/formalism/canonicalization.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/merge_planning.hpp"
#include "tyr/planning/domain.hpp"
#include "tyr/planning/ground_task/axiom_evaluator.hpp"
#include "tyr/planning/ground_task/axiom_stratification.hpp"
#include "tyr/planning/ground_task/unpacked_state.hpp"

using namespace tyr::formalism;

namespace tyr::planning
{

GroundTask::GroundTask(DomainPtr domain,
                       RepositoryPtr m_repository,
                       OverlayRepositoryPtr<Repository> overlay_repository,
                       View<Index<FDRTask>, OverlayRepository<Repository>> fdr_task,
                       GeneralFDRContext<OverlayRepository<Repository>> fdr_context,
                       match_tree::MatchTreePtr<GroundAction> action_match_tree,
                       std::vector<match_tree::MatchTreePtr<GroundAxiom>>&& axiom_match_tree_strata) :
    m_domain(std::move(domain)),
    m_repository(std::move(m_repository)),
    m_overlay_repository(std::move(overlay_repository)),
    m_fdr_task(fdr_task),
    m_action_match_tree(std::move(action_match_tree)),
    m_axiom_match_tree_strata(std::move(axiom_match_tree_strata)),
    m_state_repository(*this, std::move(fdr_context)),
    m_static_atoms_bitset(),
    m_static_numeric_variables(),
    m_successor_generator(),
    m_applicable_actions(),
    m_applicable_axioms()
{
    // std::cout << m_fdr_task << std::endl;

    for (const auto atom : m_fdr_task.template get_atoms<StaticTag>())
        set(uint_t(atom.get_index()), true, m_static_atoms_bitset);

    for (const auto fterm_value : m_fdr_task.template get_fterm_values<StaticTag>())
        set(uint_t(fterm_value.get_fterm().get_index()), fterm_value.get_value(), m_static_numeric_variables, std::numeric_limits<float_t>::quiet_NaN());
}

Node<GroundTask> GroundTask::get_initial_node()
{
    auto initial_state = m_state_repository.get_initial_state();

    const auto state_context = StateContext<GroundTask>(*this, initial_state.get_unpacked_state(), 0);

    const auto state_metric = evaluate_metric(m_fdr_task.get_metric(), m_fdr_task.get_auxiliary_fterm_value(), state_context);

    return Node<GroundTask>(std::move(initial_state), state_metric);
}

State<GroundTask> GroundTask::get_state(StateIndex state_index) { return m_state_repository.get_registered_state(state_index); }

State<GroundTask> GroundTask::register_state(SharedObjectPoolPtr<UnpackedState<GroundTask>> state) { return m_state_repository.register_state(state); }

void GroundTask::compute_extended_state(UnpackedState<GroundTask>& unpacked_state) { evaluate_axioms_bottomup(unpacked_state, *this, m_applicable_axioms); }

std::vector<LabeledNode<GroundTask>> GroundTask::get_labeled_successor_nodes(const Node<GroundTask>& node)
{
    auto result = std::vector<LabeledNode<GroundTask>> {};

    get_labeled_successor_nodes(node, result);

    return result;
}

void GroundTask::get_labeled_successor_nodes(const Node<GroundTask>& node, std::vector<LabeledNode<GroundTask>>& out_nodes)
{
    out_nodes.clear();

    const auto state = node.get_state();

    const auto state_context = StateContext<GroundTask>(*this, state.get_unpacked_state(), node.get_metric());

    m_action_match_tree->generate(state_context, m_applicable_actions);

    for (const auto ground_action : make_view(m_applicable_actions, *m_overlay_repository))
    {
        if (m_successor_generator.is_applicable(ground_action, state_context))
            out_nodes.emplace_back(ground_action, m_successor_generator.apply_action(state_context, ground_action));
    }
}

template<FactKind T>
size_t GroundTask::get_num_atoms() const noexcept
{
    return m_fdr_task.template get_atoms<T>().size();
}

template size_t GroundTask::get_num_atoms<FluentTag>() const noexcept;
template size_t GroundTask::get_num_atoms<DerivedTag>() const noexcept;

size_t GroundTask::get_num_actions() const noexcept { return m_fdr_task.get_ground_actions().size(); }

size_t GroundTask::get_num_axioms() const noexcept { return m_fdr_task.get_ground_axioms().size(); }
}
