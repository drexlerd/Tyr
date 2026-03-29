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

#ifndef TYR_PLANNING_LIFTED_TASK_STATE_STORAGE_HASH_SET_ATOM_HPP_
#define TYR_PLANNING_LIFTED_TASK_STATE_STORAGE_HASH_SET_ATOM_HPP_

#include "tyr/common/config.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/lifted_task/state_storage.hpp"
#include "tyr/planning/lifted_task/state_storage/hash_set/context.hpp"
#include "tyr/planning/state_storage.hpp"
#include "tyr/planning/state_storage/tags.hpp"

namespace tyr::planning
{

template<>
struct AtomPackedStorage<LiftedTag, HashSet>
{
    uint_t index;

    auto identifying_members() const noexcept { return std::tie(index); }
};

template<>
class AtomStorageBackend<LiftedTag, HashSet>
{
public:
    using Unpacked = AtomUnpackedStorage<LiftedTag>;
    using Packed = AtomPackedStorage<LiftedTag, HashSet>;

    explicit AtomStorageBackend(StateStorageContext<LiftedTag, HashSet>& ctx);

    Packed insert(const Unpacked& unpacked);

    void unpack(const Packed& packed, Unpacked& unpacked);

private:
    RawVectorSet<uint_t, uint_t>& m_uint_vec_set;

    std::vector<uint_t> m_buffer;
};

}

#endif
