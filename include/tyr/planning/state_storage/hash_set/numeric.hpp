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

#ifndef TYR_PLANNING_STATE_STORAGE_NUMERIC_HASH_SET_HPP_
#define TYR_PLANNING_STATE_STORAGE_NUMERIC_HASH_SET_HPP_

#include "tyr/common/config.hpp"
#include "tyr/common/raw_vector_set.hpp"
#include "tyr/planning/state_storage.hpp"
#include "tyr/planning/state_storage/tags.hpp"

#include <limits>

namespace tyr::planning
{
template<typename Task>
struct NumericPackedStorage<Task, HashSet>
{
    uint_t index;

    auto identifying_members() const noexcept { return std::tie(index); }
};

template<typename Task>
class NumericStorageBackend<Task, HashSet>
{
public:
    using Unpacked = NumericUnpackedStorage<Task>;
    using Packed = NumericPackedStorage<Task, HashSet>;

    explicit NumericStorageBackend(StateStorageContext<Task, HashSet>& ctx);

    Packed insert(const Unpacked& unpacked);

    void unpack(const Packed& packed, Unpacked& unpacked);

private:
    RawVectorSet<uint_t, float_t> m_vec_set;
};

}

#endif
