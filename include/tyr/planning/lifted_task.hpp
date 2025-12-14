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

#include "tyr/analysis/domains.hpp"
#include "tyr/common/common.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/grounder/execution_contexts.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/lifted_task/node.hpp"
#include "tyr/planning/lifted_task/packed_state.hpp"
#include "tyr/planning/lifted_task/state.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"
#include "tyr/planning/programs/action.hpp"
#include "tyr/planning/programs/axiom.hpp"
#include "tyr/planning/programs/ground.hpp"
#include "tyr/planning/state_index.hpp"

#include <valla/valla.hpp>

namespace tyr::planning
{

class LiftedTask
{
public:
    LiftedTask(DomainPtr domain,
               formalism::RepositoryPtr repository,
               formalism::OverlayRepositoryPtr<formalism::Repository> overlay_repository,
               View<Index<formalism::Task>, formalism::OverlayRepository<formalism::Repository>> task);

    State<LiftedTask> get_state(StateIndex state_index);

    StateIndex register_state(const UnpackedState<LiftedTask>& state);

    void compute_extended_state(UnpackedState<LiftedTask>& unpacked_state);

    std::vector<std::pair<View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>>, Node<LiftedTask>>>
    get_labeled_successor_nodes(const Node<LiftedTask>& node);

    Node<LiftedTask> get_initial_node();

    void get_labeled_successor_nodes(
        const Node<LiftedTask>& node,
        std::vector<std::pair<View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>>, Node<LiftedTask>>>& out_nodes);

    GroundTaskPtr get_ground_task();

    const ApplicableActionProgram& get_action_program() const;
    const AxiomEvaluatorProgram& get_axiom_program() const;
    const GroundTaskProgram& get_ground_program() const;

    /**
     * Getters
     */

    const auto& get_domain() const noexcept { return m_domain; }

    auto get_task() const noexcept { return m_task; }

    auto& get_repository() noexcept { return m_overlay_repository; }
    const auto& get_repository() const noexcept { return m_overlay_repository; }

    auto& get_unpacked_state_pool() noexcept { return m_unpacked_state_pool; }

    const auto& get_static_atoms_bitset() const noexcept { return m_static_atoms_bitset; }
    const auto& get_static_numeric_variables() const noexcept { return m_static_numeric_variables; }
    bool test(Index<formalism::GroundAtom<formalism::StaticTag>> index) const
    {
        if (index.get_value() >= m_static_atoms_bitset.size())
            return false;
        return m_static_atoms_bitset.test(index.get_value());
    }
    float_t get(Index<formalism::GroundFunctionTerm<formalism::StaticTag>> index) const
    {
        if (index.get_value() >= m_static_numeric_variables.size())
            return std::numeric_limits<float_t>::quiet_NaN();
        return m_static_numeric_variables[index.get_value()];
    }

private:
    // Inputs
    DomainPtr m_domain;
    formalism::RepositoryPtr m_repository;
    formalism::OverlayRepositoryPtr<formalism::Repository> m_overlay_repository;
    View<Index<formalism::Task>, formalism::OverlayRepository<formalism::Repository>> m_task;

    // States
    valla::IndexedHashSet<valla::Slot<uint_t>, uint_t> m_uint_nodes;
    valla::IndexedHashSet<float_t, uint_t> m_float_nodes;
    IndexedHashSet<PackedState<LiftedTask>, StateIndex> m_packed_states;
    SharedObjectPool<UnpackedState<LiftedTask>> m_unpacked_state_pool;
    boost::dynamic_bitset<> m_static_atoms_bitset;
    std::vector<float_t> m_static_numeric_variables;

    // Programs
    ApplicableActionProgram m_action_program;
    AxiomEvaluatorProgram m_axiom_program;
    GroundTaskProgram m_ground_program;

    // Execution contexts
    grounder::ProgramExecutionContext m_action_context;
    grounder::ProgramExecutionContext m_axiom_context;

    std::vector<analysis::DomainListListList> m_parameter_domains_per_cond_effect_per_action;
};

}

#endif
