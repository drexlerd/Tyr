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

#include "tyr/planning/state_storage/hash_set/numeric.hpp"

#include "tyr/planning/ground_task/state_storage/hash_set/context.hpp"
#include "tyr/planning/lifted_task/state_storage/hash_set/context.hpp"
#include "tyr/planning/task.hpp"

namespace tyr::planning
{
template<TaskKind Kind>
NumericStorageBackend<Kind, HashSet>::NumericStorageBackend(StateStorageContext<Kind, HashSet>& ctx) : m_float_vec_set(ctx.float_vec_set)
{
}

template<TaskKind Kind>
typename NumericStorageBackend<Kind, HashSet>::Packed
NumericStorageBackend<Kind, HashSet>::insert(const typename NumericStorageBackend<Kind, HashSet>::Unpacked& unpacked)
{
    return NumericStorageBackend<Kind, HashSet>::Packed { m_float_vec_set.insert(unpacked.values) };
}

template<TaskKind Kind>
void NumericStorageBackend<Kind, HashSet>::unpack(const typename NumericStorageBackend<Kind, HashSet>::Packed& packed,
                                                  typename NumericStorageBackend<Kind, HashSet>::Unpacked& unpacked)
{
    const auto view = m_float_vec_set[packed.index];

    unpacked.values.resize(view.size());
    for (uint_t i = 0; i < view.size(); ++i)
        unpacked.values[i] = view[i];
}

template class NumericStorageBackend<LiftedTag, HashSet>;
template class NumericStorageBackend<GroundTag, HashSet>;

}
