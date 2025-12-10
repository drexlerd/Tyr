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

#include "tyr/formalism/formatter.hpp"

using namespace tyr::formalism;

namespace tyr::planning
{

GroundTask::GroundTask(DomainPtr domain,
                       RepositoryPtr repository,
                       OverlayRepositoryPtr<Repository> overlay_repository,
                       View<Index<Task>, OverlayRepository<Repository>> task,
                       IndexList<GroundAtom<FluentTag>> fluent_atoms,
                       IndexList<formalism::GroundAtom<formalism::DerivedTag>> derived_atoms,
                       IndexList<GroundAction> ground_actions,
                       IndexList<GroundAxiom> ground_axioms) :
    TaskMixin(std::move(domain), std::move(repository), std::move(overlay_repository), task)
{
    // std::cout << make_view(fluent_atoms, *this->m_overlay_repository) << std::endl;
    // std::cout << make_view(derived_atoms, *this->m_overlay_repository) << std::endl;
    // std::cout << make_view(ground_actions, *this->m_overlay_repository) << std::endl;
    // std::cout << make_view(ground_axioms, *this->m_overlay_repository) << std::endl;

    std::cout << "Num fluent atoms: " << fluent_atoms.size() << std::endl;
    std::cout << "Num derived atoms: " << derived_atoms.size() << std::endl;
    std::cout << "Num ground actions: " << ground_actions.size() << std::endl;
    std::cout << "Num ground axioms: " << ground_axioms.size() << std::endl;
}

void GroundTask::compute_extended_state(UnpackedState<GroundTask>& unpacked_state) {}

std::vector<std::pair<View<Index<GroundAction>, OverlayRepository<Repository>>, Node<GroundTask>>>
GroundTask::get_labeled_successor_nodes_impl(const Node<GroundTask>& node)
{
    auto result = std::vector<std::pair<View<Index<GroundAction>, OverlayRepository<Repository>>, Node<GroundTask>>> {};
    return result;
}

void GroundTask::get_labeled_successor_nodes_impl(const Node<GroundTask>& node,
                                                  std::vector<std::pair<View<Index<GroundAction>, OverlayRepository<Repository>>, Node<GroundTask>>>& out_nodes)
{
}

}
