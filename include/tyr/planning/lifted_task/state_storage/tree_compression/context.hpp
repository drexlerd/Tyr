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

#ifndef TYR_PLANNING_LIFTED_TASK_STATE_STORAGE_TREE_COMPRESSION_CONTEXT_HPP_
#define TYR_PLANNING_LIFTED_TASK_STATE_STORAGE_TREE_COMPRESSION_CONTEXT_HPP_

#include "tyr/common/config.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/state_storage.hpp"
#include "tyr/planning/state_storage/tags.hpp"

#include <concepts>
#include <valla/valla.hpp>
#include <vector>

namespace tyr::planning
{

/**
 * Context
 */

template<>
struct StateStorageContext<LiftedTag, TreeCompression>
{
    valla::IndexedHashSet<valla::Slot<uint_t>, uint_t> uint_nodes;
    valla::IndexedHashSet<float_t, uint_t> float_nodes;

    size_t memory_usage() const noexcept
    {
        size_t bytes = 0;
        bytes += uint_nodes.mem_usage();
        bytes += float_nodes.mem_usage();
        return bytes;
    }
};

}

#endif