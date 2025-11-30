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

#include "tyr/formalism/formalism.hpp"
#include "tyr/planning/domain.hpp"
#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/task_mixin.hpp"

namespace tyr::planning
{

class LiftedTask : public TaskMixin<LiftedTask>
{
public:
    LiftedTask(std::shared_ptr<Domain> domain,
               std::shared_ptr<formalism::Repository> repository,
               std::shared_ptr<formalism::ScopedRepository<formalism::Repository>> scoped_repository,
               View<Index<formalism::planning::Task>, formalism::ScopedRepository<formalism::Repository>> task);

    std::vector<std::pair<View<Index<formalism::planning::GroundAction>, formalism::ScopedRepository<formalism::Repository>>, Node<LiftedTask>>>
    get_labeled_successor_nodes_impl(const Node<LiftedTask>& node);

    void get_labeled_successor_nodes_impl(
        const Node<LiftedTask>& node,
        std::vector<std::pair<View<Index<formalism::planning::GroundAction>, formalism::ScopedRepository<formalism::Repository>>, Node<LiftedTask>>>&
            out_nodes);

    GroundTask get_ground_task();

private:
    std::shared_ptr<formalism::Repository> m_delete_free_program_repository;
    View<Index<formalism::Program>, formalism::Repository> m_delete_free_program;
};

}

#endif
