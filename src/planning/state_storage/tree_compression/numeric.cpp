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

#include "tyr/planning/state_storage/tree_compression/numeric.hpp"

#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/ground_task/state_storage/tree_compression/context.hpp"
#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/lifted_task/state_storage/tree_compression/context.hpp"

namespace tyr::planning
{

template<TaskKind Kind>
NumericStorageBackend<Kind, TreeCompression>::NumericStorageBackend(StateStorageContext<Kind, TreeCompression>& ctx) :
    m_uint_nodes(ctx.uint_nodes),
    m_float_nodes(ctx.float_nodes),
    m_uint_node_buffer()
{
}

template<TaskKind Kind>
typename NumericStorageBackend<Kind, TreeCompression>::Packed
NumericStorageBackend<Kind, TreeCompression>::insert(const typename NumericStorageBackend<Kind, TreeCompression>::Unpacked& unpacked)
{
    m_uint_node_buffer.clear();
    valla::encode_as_unsigned_integrals(unpacked.values, m_float_nodes, std::back_inserter(m_uint_node_buffer));
    const auto slot = valla::insert_sequence(m_uint_node_buffer, m_uint_nodes);
    return NumericStorageBackend<Kind, TreeCompression>::Packed { slot };
}

template<TaskKind Kind>
void NumericStorageBackend<Kind, TreeCompression>::unpack(const typename NumericStorageBackend<Kind, TreeCompression>::Packed& packed,
                                                          typename NumericStorageBackend<Kind, TreeCompression>::Unpacked& unpacked)
{
    m_uint_node_buffer.clear();
    unpacked.values.clear();
    valla::read_sequence(packed.slot, m_uint_nodes, std::back_inserter(m_uint_node_buffer));
    valla::decode_from_unsigned_integrals(m_uint_node_buffer, m_float_nodes, std::back_inserter(unpacked.values));
}

template class NumericStorageBackend<LiftedTag, TreeCompression>;
template class NumericStorageBackend<GroundTag, TreeCompression>;

}
