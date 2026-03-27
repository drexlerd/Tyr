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

#include "tyr/planning/lifted_task/state_storage/atom_tree_compression.hpp"

#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/lifted_task/state_storage/context.hpp"

namespace tyr::planning
{

AtomStorageBackend<LiftedTask, TreeCompression>::AtomStorageBackend(StateStorageContext<LiftedTask, TreeCompression>& ctx) : m_uint_nodes(ctx.uint_nodes) {}

typename AtomStorageBackend<LiftedTask, TreeCompression>::Packed
AtomStorageBackend<LiftedTask, TreeCompression>::insert(const typename AtomStorageBackend<LiftedTask, TreeCompression>::Unpacked& unpacked)
{
    m_uint_node_buffer.clear();
    const auto& bits = unpacked.indices;
    for (auto i = bits.find_first(); i != boost::dynamic_bitset<>::npos; i = bits.find_next(i))
        m_uint_node_buffer.push_back(i);

    const auto slot = valla::insert_sequence(m_uint_node_buffer, m_uint_nodes);
    return AtomStorageBackend<LiftedTask, TreeCompression>::Packed { slot };
}

void AtomStorageBackend<LiftedTask, TreeCompression>::unpack(const typename AtomStorageBackend<LiftedTask, TreeCompression>::Packed& packed,
                                                             typename AtomStorageBackend<LiftedTask, TreeCompression>::Unpacked& unpacked)
{
    auto& indices = unpacked.indices;

    m_uint_node_buffer.clear();
    indices.clear();

    valla::read_sequence(packed.slot, m_uint_nodes, std::back_inserter(m_uint_node_buffer));

    for (const auto i : m_uint_node_buffer)
        tyr::set(i, true, indices);
}

}
