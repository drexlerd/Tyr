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

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/domain.hpp"
#include "tyr/planning/ground_task/node.hpp"
#include "tyr/planning/ground_task/packed_state.hpp"
#include "tyr/planning/ground_task/state.hpp"
#include "tyr/planning/ground_task/unpacked_state.hpp"

namespace tyr::planning
{

class GroundTask
{
public:
    // Eventually pass ground facts, actions, and axioms derived from delete relaxation in the constructor
    // and build a data structure to efficiently compute applicable actions.
    GroundTask(DomainPtr domain,
               formalism::RepositoryPtr repository,
               formalism::OverlayRepositoryPtr<formalism::Repository> overlay_repository,
               View<Index<formalism::Task>, formalism::OverlayRepository<formalism::Repository>> task,
               IndexList<formalism::GroundAtom<formalism::FluentTag>> fluent_atoms,
               IndexList<formalism::GroundAtom<formalism::DerivedTag>> derived_atoms,
               IndexList<formalism::GroundAction> actions,
               IndexList<formalism::GroundAxiom> axioms);

    void compute_extended_state(UnpackedState<GroundTask>& unpacked_state);

    Node<GroundTask> get_initial_node();

    std::vector<std::pair<View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>>, Node<GroundTask>>>
    get_labeled_successor_nodes(const Node<GroundTask>& node);

    void get_labeled_successor_nodes(
        const Node<GroundTask>& node,
        std::vector<std::pair<View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>>, Node<GroundTask>>>& out_nodes);

    template<formalism::FactKind T>
    size_t get_num_atoms() const noexcept;
    size_t get_num_actions() const noexcept;
    size_t get_num_axioms() const noexcept;

private:
    size_t m_num_fluent_atoms;
    size_t m_num_derived_atoms;
    size_t m_num_actions;
    size_t m_num_axioms;

    IndexList<formalism::FDRVariable<formalism::FluentTag>> m_fluent_variables;
    IndexList<formalism::FDRVariable<formalism::DerivedTag>> m_derived_variables;

    DataList<formalism::FDRFact<formalism::FluentTag>> m_fluent_fact;
};

}

#endif
