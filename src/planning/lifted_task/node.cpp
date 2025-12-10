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

#include "tyr/planning/lifted_task/node.hpp"

#include "tyr/planning/lifted_task/state.hpp"
//
#include "tyr/common/config.hpp"
#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/state_index.hpp"

namespace tyr::planning
{

State<LiftedTask> Node<LiftedTask>::get_state() const { return m_task->get_state(m_state_index); }

std::vector<std::pair<View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>>, Node<LiftedTask>>>
Node<LiftedTask>::get_labeled_successor_nodes()
{
    return m_task->get_labeled_successor_nodes(*this);
}

void Node<LiftedTask>::get_labeled_successor_nodes(
    std::vector<std::pair<View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>>, Node<LiftedTask>>>& out_nodes)
{
    m_task->get_labeled_successor_nodes(*this, out_nodes);
}

static_assert(NodeConcept<Node<LiftedTask>>);

}
