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

namespace tyr::planning
{

GroundTask::GroundTask(std::shared_ptr<Domain> domain,
                       std::shared_ptr<formalism::Repository> repository,
                       std::shared_ptr<formalism::ScopedRepository<formalism::Repository>> scoped_repository,
                       View<Index<formalism::planning::Task>, formalism::ScopedRepository<formalism::Repository>> task) :
    TaskMixin(std::move(domain), std::move(repository), std::move(scoped_repository), task)
{
}

std::vector<std::pair<View<Index<formalism::planning::GroundAction>, formalism::ScopedRepository<formalism::Repository>>, Node<GroundTask>>>
GroundTask::get_labeled_successor_nodes_impl(const Node<GroundTask>& node)
{
    auto result =
        std::vector<std::pair<View<Index<formalism::planning::GroundAction>, formalism::ScopedRepository<formalism::Repository>>, Node<GroundTask>>> {};
    return result;
}

void GroundTask::get_labeled_successor_nodes_impl(
    const Node<GroundTask>& node,
    std::vector<std::pair<View<Index<formalism::planning::GroundAction>, formalism::ScopedRepository<formalism::Repository>>, Node<GroundTask>>>& out_nodes)
{
}

}
