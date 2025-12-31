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

#ifndef TYR_DATALOG_KPKC_HPP_
#define TYR_DATALOG_KPKC_HPP_

#include "tyr/datalog/kpkc_data.hpp"

#include <boost/dynamic_bitset.hpp>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>
#include <tyr/common/config.hpp>
#include <vector>

namespace tyr::datalog::kpkc
{

template<typename Callback>
void for_each_k_clique_recursively(const DenseKPartiteGraph& graph, Workspace& workspace, Callback&& callback, size_t depth)
{
    assert(depth < graph.partitions.size());

    uint_t k = graph.partitions.size();
    uint_t best_set_bits = std::numeric_limits<uint_t>::max();
    uint_t best_partition = std::numeric_limits<uint_t>::max();

    auto& compatible_vertices = workspace.compatible_vertices[depth];
    auto& partition_bits = workspace.partition_bits;
    auto& partial_solution = workspace.partial_solution;

    // Find the best partition to work with
    for (uint_t partition = 0; partition < k; ++partition)
    {
        auto num_set_bits = compatible_vertices[partition].count();
        if (!partition_bits[partition] && (num_set_bits < best_set_bits))
        {
            best_set_bits = num_set_bits;
            best_partition = partition;
        }
    }

    if (best_partition == std::numeric_limits<uint_t>::max())
        return;  // dead branch: no unused partition has candidates

    // Iterate through compatible vertices in the best partition
    auto& best_partition_compatible_vertices = compatible_vertices[best_partition];
    uint_t adjacent_index = best_partition_compatible_vertices.find_first();
    while (adjacent_index < best_partition_compatible_vertices.size())
    {
        uint_t vertex = graph.partitions[best_partition][adjacent_index];
        best_partition_compatible_vertices[adjacent_index] = 0;

        partial_solution.push_back(vertex);

        if (partial_solution.size() == k)
        {
            callback(partial_solution);
        }
        else
        {
            assert(workspace.partial_solution.size() - 1 == depth);

            // Update compatible vertices for the next recursion
            auto& compatible_vertices_next = workspace.compatible_vertices[depth + 1];
            for (uint_t partition = 0; partition < k; ++partition)
            {
                auto& partition_compatible_vertices_next = compatible_vertices_next[partition];
                auto& partition_compatible_vertices = compatible_vertices[partition];
                partition_compatible_vertices_next = partition_compatible_vertices;  // copy bits from current to next iteration
            }

            uint_t offset = 0;
            for (uint_t partition = 0; partition < k; ++partition)
            {
                auto& partition_compatible_vertices_next = compatible_vertices_next[partition];
                auto partition_size = partition_compatible_vertices_next.size();
                if (!partition_bits[partition])
                {
                    for (uint_t index = 0; index < partition_size; ++index)
                    {
                        partition_compatible_vertices_next[index] &= graph.adjacency_matrix[vertex][index + offset];
                    }
                }
                offset += partition_size;
            }

            partition_bits.set(best_partition);

            uint_t possible_additions = 0;
            for (uint_t partition = 0; partition < k; ++partition)
            {
                auto& partition_compatible_vertices = compatible_vertices[partition];
                if (!partition_bits[partition] && partition_compatible_vertices.any())
                {
                    ++possible_additions;
                }
            }

            if ((partial_solution.size() + possible_additions) == k)
            {
                for_each_k_clique_recursively(graph, workspace, callback, depth + 1);
            }

            partition_bits.reset(best_partition);
        }

        partial_solution.pop_back();

        adjacent_index = best_partition_compatible_vertices.find_next(adjacent_index);
    }
}

template<typename Callback>
void for_each_k_clique(const DenseKPartiteGraph& graph, Workspace& workspace, Callback&& callback)
{
    for_each_k_clique_recursively(graph, workspace, callback, 0);
}
}

#endif