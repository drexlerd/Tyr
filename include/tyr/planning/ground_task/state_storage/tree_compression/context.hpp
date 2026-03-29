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

#ifndef TYR_PLANNING_GROUND_TASK_STATE_STORAGE_TREE_COMPRESSION_CONTEXT_HPP_
#define TYR_PLANNING_GROUND_TASK_STATE_STORAGE_TREE_COMPRESSION_CONTEXT_HPP_

#include "tyr/common/config.hpp"
#include "tyr/common/raw_array_set.hpp"
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
struct StateStorageContext<GroundTag, TreeCompression>
{
    explicit StateStorageContext(const Task<GroundTag>& task);

    struct VariableInfo
    {
        uint_t begin;
        uint8_t offset;
        uint8_t length;
    };

    struct LayoutData
    {
        std::vector<VariableInfo> fluent_infos;
        uint_t fluent_array_size;
        uint_t derived_num_bits;
        uint_t derived_array_size;
    };

    std::vector<VariableInfo> fluent_infos;
    RawArraySet<uint_t> fluent_array_set;

    uint_t derived_num_bits;
    RawArraySet<uint_t> derived_array_set;

    valla::IndexedHashSet<valla::Slot<uint_t>, uint_t> uint_nodes;
    valla::IndexedHashSet<float_t, uint_t> float_nodes;

    size_t memory_usage() const noexcept
    {
        size_t bytes = 0;
        bytes += fluent_array_set.memory_usage();
        bytes += derived_array_set.memory_usage();
        bytes += uint_nodes.mem_usage();
        bytes += float_nodes.mem_usage();
        return bytes;
    }

private:
    explicit StateStorageContext(LayoutData&& layout_data);
};

}

#endif