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

#ifndef TYR_PLANNING_STATE_STORAGE_NUMERIC_TREE_COMPRESSION_HPP_
#define TYR_PLANNING_STATE_STORAGE_NUMERIC_TREE_COMPRESSION_HPP_

#include "tyr/common/config.hpp"
#include "tyr/planning/state_storage.hpp"
#include "tyr/planning/state_storage/tags.hpp"

#include <limits>
#include <valla/valla.hpp>

namespace tyr::planning
{

template<typename Task>
struct NumericPackedStorage<Task, TreeCompression>
{
    valla::Slot<uint_t> slot;
};

template<typename Task>
class NumericStorageBackend<Task, TreeCompression>
{
public:
    using Unpacked = NumericUnpackedStorage<Task>;
    using Packed = NumericPackedStorage<Task, TreeCompression>;

    NumericStorageBackend();

    std::pair<Packed, bool> insert(const Unpacked& unpacked);

    void unpack(const Packed& packed, Unpacked& unpacked) const;

private:
    valla::IndexedHashSet<valla::Slot<uint_t>, uint_t> m_uint_nodes;
    valla::IndexedHashSet<float_t, uint_t> m_float_nodes;
};

}

#endif
