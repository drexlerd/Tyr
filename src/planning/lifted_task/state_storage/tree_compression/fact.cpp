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

#include "tyr/planning/lifted_task/state_storage/tree_compression/fact.hpp"

#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/lifted_task/state_storage/tree_compression/context.hpp"

namespace tyr::planning
{

FactStorageBackend<LiftedTag, TreeCompression>::FactStorageBackend(StateStorageContext<LiftedTag, TreeCompression>& ctx) : m_uint_nodes(ctx.uint_nodes) {}

typename FactStorageBackend<LiftedTag, TreeCompression>::Packed
FactStorageBackend<LiftedTag, TreeCompression>::insert(const typename FactStorageBackend<LiftedTag, TreeCompression>::Unpacked& unpacked)
{
    m_uint_node_buffer.clear();
    const auto& bits = unpacked.indices;
    for (auto i = bits.find_first(); i != boost::dynamic_bitset<>::npos; i = bits.find_next(i))
        m_uint_node_buffer.push_back(i);

    const auto slot = valla::insert_sequence(m_uint_node_buffer, m_uint_nodes);
    return FactStorageBackend<LiftedTag, TreeCompression>::Packed { slot };
}

void FactStorageBackend<LiftedTag, TreeCompression>::unpack(const typename FactStorageBackend<LiftedTag, TreeCompression>::Packed& packed,
                                                            typename FactStorageBackend<LiftedTag, TreeCompression>::Unpacked& unpacked)
{
    auto& indices = unpacked.indices;

    m_uint_node_buffer.clear();
    indices.clear();

    valla::read_sequence(packed.slot, m_uint_nodes, std::back_inserter(m_uint_node_buffer));

    for (const auto i : m_uint_node_buffer)
        tyr::set(i, true, indices);
}

}
