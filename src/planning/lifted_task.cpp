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

namespace tyr::planning
{

LiftedTask::LiftedTask(DomainPtr domain,
                       formalism::RepositoryPtr repository,
                       formalism::OverlayRepositoryPtr<formalism::Repository> overlay_repository,
                       View<Index<formalism::Task>, formalism::OverlayRepository<formalism::Repository>> task) :
    TaskMixin(std::move(domain), std::move(repository), std::move(overlay_repository), task),
    m_action_program(*this),
    m_axiom_program(*this),
    m_ground_program(*this)
{
}

std::vector<std::pair<View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>>, Node<LiftedTask>>>
LiftedTask::get_labeled_successor_nodes_impl(const Node<LiftedTask>& node)
{
    auto result = std::vector<std::pair<View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>>, Node<LiftedTask>>> {};
    return result;
}

void LiftedTask::get_labeled_successor_nodes_impl(
    const Node<LiftedTask>& node,
    std::vector<std::pair<View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>>, Node<LiftedTask>>>& out_nodes)
{
}

GroundTask LiftedTask::get_ground_task() { return GroundTask(this->m_domain, this->m_repository, this->m_overlay_repository, this->m_task); }

}
