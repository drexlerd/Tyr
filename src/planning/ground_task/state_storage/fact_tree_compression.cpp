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

#include "tyr/planning/ground_task/state_storage/fact_tree_compression.hpp"

#include "tyr/common/bit.hpp"
#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/ground_task/state_storage/context.hpp"

namespace tyr::planning
{

FactStorageBackend<GroundTask, TreeCompression>::FactStorageBackend(StateStorageContext<GroundTask, TreeCompression>& ctx) :
    m_array_set(ctx.fluent_array_set),
    m_infos(ctx.fluent_infos),
    m_buffer(m_array_set.array_size())
{
}

typename FactStorageBackend<GroundTask, TreeCompression>::Packed
FactStorageBackend<GroundTask, TreeCompression>::insert(const typename FactStorageBackend<GroundTask, TreeCompression>::Unpacked& unpacked)
{
    std::fill(m_buffer.begin(), m_buffer.end(), uint_t(0));
    for (uint_t i = 0; i < m_infos.size(); ++i)
    {
        const auto& info = m_infos[i];

        bit::int_reference(m_buffer.data() + info.begin, info.offset, info.length) = unpacked.values[i];
    }

    return typename FactStorageBackend<GroundTask, TreeCompression>::Packed { m_array_set.insert(m_buffer) };
}

void FactStorageBackend<GroundTask, TreeCompression>::unpack(const typename FactStorageBackend<GroundTask, TreeCompression>::Packed& packed,
                                                             typename FactStorageBackend<GroundTask, TreeCompression>::Unpacked& unpacked)
{
    const auto fluent_ptr = m_array_set[packed.index];
    auto& fluent_values = unpacked.values;

    for (uint_t i = 0; i < m_infos.size(); ++i)
    {
        const auto& info = m_infos[i];
        fluent_values[i] = uint_t(bit::int_reference<uint_t>(fluent_ptr + info.begin, info.offset, info.length));
    }
}

}
