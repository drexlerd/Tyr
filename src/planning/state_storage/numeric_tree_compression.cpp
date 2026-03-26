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

#include "tyr/planning/state_storage/numeric_tree_compression.hpp"

#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/lifted_task.hpp"

namespace tyr::planning
{

template<typename Task>
NumericStorageBackend<Task, TreeCompression>::NumericStorageBackend()
{
}

template<typename Task>
std::pair<typename NumericStorageBackend<Task, TreeCompression>::Packed, bool>
NumericStorageBackend<Task, TreeCompression>::insert(const typename NumericStorageBackend<Task, TreeCompression>::Unpacked& unpacked)
{
}

template<typename Task>
void NumericStorageBackend<Task, TreeCompression>::unpack(const typename NumericStorageBackend<Task, TreeCompression>::Packed& packed,
                                                          typename NumericStorageBackend<Task, TreeCompression>::Unpacked& unpacked) const
{
}

template class NumericStorageBackend<LiftedTask, TreeCompression>;
template class NumericStorageBackend<GroundTask, TreeCompression>;

}
