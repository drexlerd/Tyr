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

#include "tyr/formalism/formalism.hpp"
#include "tyr/planning/domain.hpp"
#include "tyr/planning/task_mixin.hpp"

namespace tyr::planning
{

class GroundTask : public TaskMixin<GroundTask>
{
public:
    // Eventually pass ground facts, actions, and axioms derived from delete relaxation in the constructor
    // and build a data structure to efficiently compute applicable actions.
    GroundTask(std::shared_ptr<Domain> domain,
               std::shared_ptr<formalism::Repository> repository,
               std::shared_ptr<formalism::ScopedRepository<formalism::Repository>> scoped_repository,
               View<Index<formalism::planning::Task>, formalism::ScopedRepository<formalism::Repository>> task);

    std::vector<std::pair<View<Index<formalism::planning::GroundAction>, formalism::ScopedRepository<formalism::Repository>>, Node<GroundTask>>>
    get_labeled_successor_nodes_impl(const Node<GroundTask>& node);

    void get_labeled_successor_nodes_impl(
        const Node<GroundTask>& node,
        std::vector<std::pair<View<Index<formalism::planning::GroundAction>, formalism::ScopedRepository<formalism::Repository>>, Node<GroundTask>>>&
            out_nodes);
};

}

#endif
