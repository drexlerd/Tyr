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

#ifndef TYR_PLANNING_GROUND_TASK_HPP_
#define TYR_PLANNING_GROUND_TASK_HPP_

#include "tyr/common/common.hpp"
#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/planning/fdr_context.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/domain.hpp"
#include "tyr/planning/ground_task/layout.hpp"
#include "tyr/planning/ground_task/match_tree/match_tree.hpp"
#include "tyr/planning/ground_task/node.hpp"
#include "tyr/planning/ground_task/packed_state.hpp"
#include "tyr/planning/ground_task/state.hpp"
#include "tyr/planning/ground_task/unpacked_state.hpp"

#include <valla/valla.hpp>

namespace tyr::planning
{

class GroundTask
{
public:
    GroundTask(DomainPtr domain,
               formalism::RepositoryPtr m_repository,
               formalism::OverlayRepositoryPtr<formalism::Repository> overlay_repository,
               View<Index<formalism::FDRTask>, formalism::OverlayRepository<formalism::Repository>> fdr_task,
               formalism::GeneralFDRContext<formalism::OverlayRepository<formalism::Repository>> fdr_context,
               FDRVariablesLayout<formalism::FluentTag, uint_t> fluent_layout,
               match_tree::MatchTreePtr<formalism::GroundAction> action_match_tree,
               std::vector<match_tree::MatchTreePtr<formalism::GroundAxiom>>&& axiom_match_tree_strata);

    static std::shared_ptr<GroundTask> create(DomainPtr domain,
                                              formalism::RepositoryPtr repository,
                                              formalism::OverlayRepositoryPtr<formalism::Repository> overlay_repository,
                                              View<Index<formalism::Task>, formalism::OverlayRepository<formalism::Repository>> task,
                                              IndexList<formalism::GroundAtom<formalism::FluentTag>> fluent_atoms,
                                              IndexList<formalism::GroundAtom<formalism::DerivedTag>> derived_atoms,
                                              IndexList<formalism::GroundFunctionTerm<formalism::FluentTag>> fluent_fterms,
                                              IndexList<formalism::GroundAction> actions,
                                              IndexList<formalism::GroundAxiom> axioms);

    State<GroundTask> get_state(StateIndex state_index);

    StateIndex register_state(const UnpackedState<GroundTask>& state);

    void compute_extended_state(UnpackedState<GroundTask>& unpacked_state);

    Node<GroundTask> get_initial_node();

    std::vector<LabeledNode<GroundTask>> get_labeled_successor_nodes(const Node<GroundTask>& node);

    void get_labeled_successor_nodes(const Node<GroundTask>& node, std::vector<LabeledNode<GroundTask>>& out_nodes);

    template<formalism::FactKind T>
    size_t get_num_atoms() const noexcept;
    size_t get_num_actions() const noexcept;
    size_t get_num_axioms() const noexcept;

    const auto& get_static_atoms_bitset() const noexcept { return m_static_atoms_bitset; }
    const auto& get_static_numeric_variables() const noexcept { return m_static_numeric_variables; }
    bool test(Index<formalism::GroundAtom<formalism::StaticTag>> index) const { return tyr::test(uint_t(index), m_static_atoms_bitset); }
    float_t get(Index<formalism::GroundFunctionTerm<formalism::StaticTag>> index) const
    {
        return tyr::get(uint_t(index), m_static_numeric_variables, std::numeric_limits<float_t>::quiet_NaN());
    }

    const auto& get_domain() const noexcept { return m_domain; }

    auto get_task() const noexcept { return m_fdr_task; }

    auto& get_repository() noexcept { return m_overlay_repository; }
    const auto& get_repository() const noexcept { return m_overlay_repository; }

    auto& get_unpacked_state_pool() noexcept { return m_unpacked_state_pool; }

    const auto& get_axiom_match_tree_strata() const noexcept { return m_axiom_match_tree_strata; }

private:
    DomainPtr m_domain;

    formalism::RepositoryPtr m_repository;
    formalism::OverlayRepositoryPtr<formalism::Repository> m_overlay_repository;
    View<Index<formalism::FDRTask>, formalism::OverlayRepository<formalism::Repository>> m_fdr_task;
    formalism::GeneralFDRContext<formalism::OverlayRepository<formalism::Repository>> m_fdr_context;
    FDRVariablesLayout<formalism::FluentTag, uint_t> m_fluent_layout;
    match_tree::MatchTreePtr<formalism::GroundAction> m_action_match_tree;
    std::vector<match_tree::MatchTreePtr<formalism::GroundAxiom>> m_axiom_match_tree_strata;

    /**
     * States
     */
    valla::IndexedHashSet<valla::Slot<uint_t>, uint_t> m_uint_nodes;
    valla::IndexedHashSet<float_t, uint_t> m_float_nodes;
    IndexedHashSet<PackedState<GroundTask>, StateIndex> m_packed_states;
    SharedObjectPool<UnpackedState<GroundTask>> m_unpacked_state_pool;
    boost::dynamic_bitset<> m_static_atoms_bitset;
    std::vector<float_t> m_static_numeric_variables;

    IndexList<formalism::GroundAction> m_applicable_actions;
    IndexList<formalism::GroundAxiom> m_applicable_axioms;
    formalism::EffectFamilyList m_effect_families;
};

}

#endif
