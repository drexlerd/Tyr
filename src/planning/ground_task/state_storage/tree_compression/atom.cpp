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

#include "tyr/planning/ground_task/state_storage/tree_compression/atom.hpp"

#include "tyr/common/bit.hpp"
#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/ground_task/state_storage/tree_compression/context.hpp"

namespace tyr::planning
{

AtomStorageBackend<GroundTag, TreeCompression>::AtomStorageBackend(StateStorageContext<GroundTag, TreeCompression>& ctx) :
    m_array_set(ctx.derived_array_set),
    m_num_bits(ctx.derived_num_bits),
    m_buffer(m_array_set.array_size())
{
}

typename AtomStorageBackend<GroundTag, TreeCompression>::Packed
AtomStorageBackend<GroundTag, TreeCompression>::insert(const typename AtomStorageBackend<GroundTag, TreeCompression>::Unpacked& unpacked)
{
    auto data = m_buffer.data();

    std::fill(m_buffer.begin(), m_buffer.end(), uint_t(0));
    for (uint_t i = 0; i < m_num_bits; ++i)
        bit::bit_reference(data, i) = unpacked.indices.test(i);
    return typename AtomStorageBackend<GroundTag, TreeCompression>::Packed { m_array_set.insert(m_buffer) };
}

void AtomStorageBackend<GroundTag, TreeCompression>::unpack(const typename AtomStorageBackend<GroundTag, TreeCompression>::Packed& packed,
                                                            typename AtomStorageBackend<GroundTag, TreeCompression>::Unpacked& unpacked)
{
    const auto data = m_array_set[packed.index];
    auto& indices = unpacked.indices;

    if (indices.size() != m_num_bits)
        indices.resize(m_num_bits);

    for (uint_t i = 0; i < m_num_bits; ++i)
        indices[i] = bool(bit::bit_reference(data, i));
}

}
