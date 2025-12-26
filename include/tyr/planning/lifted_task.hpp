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

#ifndef TYR_PLANNING_LIFTED_TASK_HPP_
#define TYR_PLANNING_LIFTED_TASK_HPP_

#include "tyr/planning/lifted_task/node.hpp"
#include "tyr/planning/lifted_task/packed_state.hpp"
#include "tyr/planning/lifted_task/state.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"
//
#include "tyr/analysis/domains.hpp"
#include "tyr/common/common.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/grounder/execution_contexts.hpp"
#include "tyr/planning/action_executor.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/lifted_task/axiom_evaluator.hpp"
#include "tyr/planning/lifted_task/state_repository.hpp"
#include "tyr/planning/programs/action.hpp"
#include "tyr/planning/programs/axiom.hpp"
#include "tyr/planning/state_index.hpp"

namespace tyr::planning
{

class LiftedTask
{
public:
    LiftedTask(DomainPtr domain,
               formalism::RepositoryPtr repository,
               formalism::OverlayRepositoryPtr<formalism::Repository> overlay_repository,
               View<Index<formalism::Task>, formalism::OverlayRepository<formalism::Repository>> task,
               formalism::BinaryFDRContext<formalism::OverlayRepository<formalism::Repository>> fdr_context);

    State<LiftedTask> get_state(StateIndex state_index);

    State<LiftedTask> register_state(SharedObjectPoolPtr<UnpackedState<LiftedTask>> state);

    void compute_extended_state(UnpackedState<LiftedTask>& unpacked_state);

    std::vector<LabeledNode<LiftedTask>> get_labeled_successor_nodes(const Node<LiftedTask>& node);

    Node<LiftedTask> get_initial_node();

    void get_labeled_successor_nodes(const Node<LiftedTask>& node, std::vector<LabeledNode<LiftedTask>>& out_nodes);

    GroundTaskPtr get_ground_task();

    const ApplicableActionProgram& get_action_program() const;

    /**
     * Getters
     */

    const auto& get_domain() const noexcept { return m_domain; }

    auto get_task() const noexcept { return m_task; }

    auto& get_repository() noexcept { return m_overlay_repository; }
    const auto& get_repository() const noexcept { return m_overlay_repository; }

    auto& get_state_repository() noexcept { return m_state_repository; }
    const auto& get_state_repository() const noexcept { return m_state_repository; }

    const auto& get_static_atoms_bitset() const noexcept { return m_static_atoms_bitset; }
    const auto& get_static_numeric_variables() const noexcept { return m_static_numeric_variables; }
    bool test(Index<formalism::GroundAtom<formalism::StaticTag>> index) const { return tyr::test(uint_t(index), m_static_atoms_bitset); }
    float_t get(Index<formalism::GroundFunctionTerm<formalism::StaticTag>> index) const
    {
        return tyr::get(uint_t(index), m_static_numeric_variables, std::numeric_limits<float_t>::quiet_NaN());
    }

private:
    // Inputs
    DomainPtr m_domain;
    formalism::RepositoryPtr m_repository;
    formalism::OverlayRepositoryPtr<formalism::Repository> m_overlay_repository;
    View<Index<formalism::Task>, formalism::OverlayRepository<formalism::Repository>> m_task;

    // States
    StateRepository<LiftedTask> m_state_repository;
    boost::dynamic_bitset<> m_static_atoms_bitset;
    std::vector<float_t> m_static_numeric_variables;

    // Transition
    ActionExecutor m_successor_generator;

    AxiomEvaluator<LiftedTask> m_axiom_evaluator;

    // Programs
    ApplicableActionProgram m_action_program;

    // Execution contexts
    grounder::ProgramExecutionContext m_action_context;

    std::vector<analysis::DomainListListList> m_parameter_domains_per_cond_effect_per_action;
};

}

#endif
