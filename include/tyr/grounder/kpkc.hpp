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

#ifndef TYR_GROUNDER_KPKC_HPP_
#define TYR_GROUNDER_KPKC_HPP_

#include <boost/dynamic_bitset.hpp>
#include <cstddef>
#include <cstdint>
#include <span>
#include <tyr/common/config.hpp>
#include <vector>

namespace tyr::grounder::kpkc
{
/**
 * V = num vertices
 * K = num partitions
 */

/// @brief `DenseConsistencyGraph` is a dense representation of the consistency graph for a rule and a set of facts.
struct DenseConsistencyGraph
{
    std::vector<boost::dynamic_bitset<>> adjacency_matrix;  ///< Dimensions V x V
    std::vector<std::vector<uint_t>> partitions;            ///< Dimensions K x V
    size_t num_vertices;
    size_t k;
};

/// @brief Helper to allocate a DenseConsistencyGraph from a given StaticConsistencyGraph.
inline DenseConsistencyGraph allocate_dense_graph(const StaticConsistencyGraph& sparse_graph);

/// @brief `Workspace` is preallocated memory for a rule.
struct Workspace
{
    std::vector<std::vector<boost::dynamic_bitset<>>> compatible_vertices;  ///< Dimensions K x K x V
    boost::dynamic_bitset<> partition_bits;                                 ///< Dimensions K
    std::vector<uint_t> partial_solution;                                   ///< Dimensions K
    size_t k;
};

/// @brief Helper to allocate a Workspace from a given StaticConsistencyGraph.
inline Workspace allocate_workspace(const StaticConsistencyGraph& sparse_graph);

template<typename Callback>
void create_k_clique_in_k_partite_graph_generator_recursively(const DenseConsistencyGraph& graph, Workspace& workspace, Callback&& callback, size_t depth)
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
            assert(worspace.partial_solution.size() - 1 == depth);

            // Update compatible vertices for the next recursion
            auto& compatible_vertices_next = compatible_vertices[depth + 1];
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

            partition_bits[best_partition] = 1;

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
                find_all_k_cliques_in_k_partite_graph_helper(graph, workspace, callback, depth + 1);
            }

            partition_bits[best_partition] = 0;
        }

        partial_solution.pop_back();

        adjacent_index = best_partition_compatible_vertices.find_next(adjacent_index);
    }
}

template<typename Callback>
void create_k_clique_in_k_partite_graph_generator(const DenseConsistencyGraph& graph, Workspace& workspace, Callback&& callback)
{
    create_k_clique_in_k_partite_graph_generator_recursively(graph, workspace, callback, 0);
}
}

#endif