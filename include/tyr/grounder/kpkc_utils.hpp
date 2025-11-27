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

#ifndef TYR_GROUNDER_KPKC_UTILS_HPP_
#define TYR_GROUNDER_KPKC_UTILS_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/grounder/consistency_graph.hpp"
#include "tyr/grounder/declarations.hpp"
#include "tyr/grounder/kpkc_data.hpp"

#include <boost/dynamic_bitset.hpp>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>
#include <tyr/common/config.hpp>
#include <vector>

namespace tyr::grounder::kpkc
{

/// @brief Helper to allocate a DenseKPartiteGraph from a given StaticConsistencyGraph.
template<formalism::IsContext C>
inline DenseKPartiteGraph allocate_dense_graph(const StaticConsistencyGraph<C>& sparse_graph)
{
    auto graph = DenseKPartiteGraph();

    const auto num_vertices = sparse_graph.get_num_vertices();
    const auto k = sparse_graph.get_condition().get_arity();
    const auto& partitions = sparse_graph.get_partitions();

    // Allocate adjacency matrix (V x V)
    graph.adjacency_matrix.resize(num_vertices);
    for (uint_t i = 0; i < num_vertices; ++i)
    {
        graph.adjacency_matrix[i].resize(num_vertices);
    }

    // Initialize partitions
    graph.partitions = partitions;

    // Initialize metadata
    graph.k = k;
    graph.num_vertices = num_vertices;

    return graph;
}

/// @brief Helper to allocate a Workspace from a given StaticConsistencyGraph.
template<formalism::IsContext C>
inline Workspace allocate_workspace(const StaticConsistencyGraph<C>& sparse_graph)
{
    auto workspace = Workspace();

    const auto num_vertices = sparse_graph.get_num_vertices();
    const auto k = sparse_graph.get_condition().get_arity();
    const auto& partitions = sparse_graph.get_partitions();

    // Allocate compatible vertices
    workspace.compatible_vertices.resize(k);
    for (uint_t i = 0; i < k; ++i)
    {
        workspace.compatible_vertices[i].resize(k);
        for (uint_t j = 0; j < k; ++j)
        {
            workspace.compatible_vertices[i][j].resize(partitions[j].size());
        }
    }

    // Allocate partition bits
    workspace.partition_bits.resize(k);

    // Allocate consistent vertices
    workspace.consistent_vertices.resize(num_vertices);

    // Allocate partial solution: Dont change size because kpkc uses push_back() and pop_back() during recursion
    workspace.partial_solution.reserve(k);

    // Initialize metadata
    workspace.k = k;
    workspace.num_vertices = num_vertices;

    return workspace;
}

template<formalism::IsContext C>
inline void initialize_dense_graph_and_workspace(const StaticConsistencyGraph<C>& sparse_graph,
                                                 const AssignmentSets& assignment_sets,
                                                 DenseKPartiteGraph& ref_graph,
                                                 Workspace& ref_workspace)
{
    ref_workspace.consistent_vertices.reset();
    ref_workspace.consistent_vertices_vec.clear();

    // Compute consistent vertices to speed up consistent edges computation
    for (const auto& vertex : sparse_graph.consistent_vertices(assignment_sets))
    {
        ref_workspace.consistent_vertices.set(vertex.get_index());
        ref_workspace.consistent_vertices_vec.push_back(vertex.get_index());
    }

    // Clear the adj matrix
    for (auto& adj_list : ref_graph.adjacency_matrix)
    {
        adj_list.reset();
    }

    // Initialize adjacency matrix: Add consistent undirected edges to adj matrix.
    for (const auto& edge : sparse_graph.consistent_edges(assignment_sets, ref_workspace.consistent_vertices))
    {
        const auto first_index = edge.get_src().get_index();
        const auto second_index = edge.get_dst().get_index();
        auto& first_row = ref_graph.adjacency_matrix[first_index];
        auto& second_row = ref_graph.adjacency_matrix[second_index];
        first_row.set(second_index);
        second_row.set(first_index);
    }

    // Initialize compatible vertices: Set bits for depth = 0 because kpkc copies depth i into depth i + 1 before recursive call.
    for (uint_t i = 0; i < ref_workspace.k; ++i)
    {
        ref_workspace.compatible_vertices[0][i].set();
    }

    // Initialize partition bits: Reset the partition bits
    ref_workspace.partition_bits.reset();
}

}

#endif