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

#include "tyr/datalog/delta_kpkc.hpp"

#include "tyr/datalog/consistency_graph.hpp"

namespace tyr::datalog
{
namespace delta_kpkc
{

ConstGraph allocate_const_graph(const StaticConsistencyGraph& static_graph)
{
    auto graph = ConstGraph();

    const auto num_vertices = static_graph.get_num_vertices();
    const auto k = static_graph.get_condition().get_arity();
    const auto& partitions = static_graph.get_partitions();

    // Initialize metadata
    graph.k = k;
    graph.num_vertices = num_vertices;

    // Initialize partitions
    graph.partitions = partitions;
    graph.vertex_to_partition.resize(num_vertices);
    for (size_t i = 0; i < partitions.size(); ++i)
        for (const auto& v : partitions[i])
            graph.vertex_to_partition[v] = i;

    return graph;
}

Graph allocate_empty_graph(const StaticConsistencyGraph& static_graph)
{
    auto graph = Graph();

    const auto num_vertices = static_graph.get_num_vertices();

    // Allocate
    graph.vertices.resize(num_vertices, false);

    // Allocate adjacency matrix (V x V)
    graph.adjacency_matrix.resize(num_vertices);
    for (uint_t i = 0; i < num_vertices; ++i)
        graph.adjacency_matrix[i].resize(num_vertices, false);

    return graph;
}

Workspace allocate_empty_workspace(const StaticConsistencyGraph& static_graph)
{
    auto workspace = Workspace();

    const auto k = static_graph.get_condition().get_arity();
    const auto& partitions = static_graph.get_partitions();

    // Allocate compatible vertices
    workspace.compatible_vertices.resize(k);
    for (uint_t i = 0; i < k; ++i)
    {
        workspace.compatible_vertices[i].resize(k);
        for (uint_t j = 0; j < k; ++j)
            workspace.compatible_vertices[i][j].resize(partitions[j].size());
    }

    // Allocate partition bits
    workspace.partition_bits.resize(k);

    // Allocate partial solution: Dont change size because kpkc uses push_back() and pop_back() during recursion
    workspace.partial_solution.reserve(k);

    return workspace;
}
}

DeltaKPKC::DeltaKPKC(const StaticConsistencyGraph& static_graph) :
    m_const_graph(delta_kpkc::allocate_const_graph(static_graph)),
    m_delta_graph(delta_kpkc::allocate_empty_graph(static_graph)),
    m_full_graph(delta_kpkc::allocate_empty_graph(static_graph)),
    m_workspace(delta_kpkc::allocate_empty_workspace(static_graph))
{
}

DeltaKPKC::DeltaKPKC(delta_kpkc::ConstGraph const_graph, delta_kpkc::Graph delta_graph, delta_kpkc::Graph full_graph, delta_kpkc::Workspace workspace) :
    m_const_graph(std::move(const_graph)),
    m_delta_graph(std::move(delta_graph)),
    m_full_graph(std::move(full_graph)),
    m_workspace(std::move(workspace))
{
}

void DeltaKPKC::set_next_assignment_sets(const StaticConsistencyGraph& static_graph, const AssignmentSets& assignment_sets)
{
    // std::cout << "m_delta_graph.vertices before:" << std::endl;
    // std::cout << m_delta_graph.vertices << std::endl;
    // std::cout << "m_delta_graph.adjacency_matrix  before:" << std::endl;
    // for (auto& bitset : m_delta_graph.adjacency_matrix)
    //     std::cout << bitset << std::endl;
    // std::cout << "m_full_graph.vertices before:" << std::endl;
    // std::cout << m_full_graph.vertices << std::endl;
    // std::cout << "m_full_graph.adjacency_matrix before:" << std::endl;
    // for (auto& bitset : m_full_graph.adjacency_matrix)
    //     std::cout << bitset << std::endl;

    /// 1. Set delta to the old graph.
    std::swap(m_delta_graph, m_full_graph);

    /// 2. Initialize the full graph

    m_full_graph.vertices.reset();

    // Compute consistent vertices to speed up consistent edges computation
    for (const auto& vertex : static_graph.consistent_vertices(assignment_sets))
        m_full_graph.vertices.set(vertex.get_index());

    // Clear the adj matrix
    for (auto& adj_list : m_full_graph.adjacency_matrix)
        adj_list.reset();

    // Initialize adjacency matrix: Add consistent undirected edges to adj matrix.
    for (const auto& edge : static_graph.consistent_edges(assignment_sets, m_full_graph.vertices))
    {
        const auto first_index = edge.get_src().get_index();
        const auto second_index = edge.get_dst().get_index();
        // Enforce invariant of static consistency graph
        assert(first_index != second_index);
        auto& first_row = m_full_graph.adjacency_matrix[first_index];
        auto& second_row = m_full_graph.adjacency_matrix[second_index];
        first_row.set(second_index);
        second_row.set(first_index);
    }

    // Initialize compatible vertices: Set bits for depth = 0 because kpkc copies depth i into depth i + 1 before recursive call.
    for (uint_t i = 0; i < m_const_graph.k; ++i)
    {
        m_workspace.compatible_vertices[0][i].set();
    }

    // Initialize partition bits: Reset the partition bits
    m_workspace.partition_bits.reset();

    /// 3. Set delta graph vertices to those that were added
    m_delta_graph.vertices ^= m_full_graph.vertices;  // OLD ⊕ NEW
    m_delta_graph.vertices &= m_full_graph.vertices;  // (OLD ⊕ NEW) ∧ NEW = NEW ∧ ~OLD
    // m_delta_graph.vertices = m_full_graph.vertices;

    /// 4. Set delta graph edges to those that were added
    for (uint i = 0; i < m_const_graph.num_vertices; ++i)
    {
        m_delta_graph.adjacency_matrix[i] ^= m_full_graph.adjacency_matrix[i];  // OLD ⊕ NEW
        m_delta_graph.adjacency_matrix[i] &= m_full_graph.adjacency_matrix[i];  // (OLD ⊕ NEW) ∧ NEW = NEW ∧ ~OLD
        // m_delta_graph.adjacency_matrix[i] = m_full_graph.adjacency_matrix[i];
    }

    // std::cout << "m_full_graph.vertices after:" << std::endl;
    // std::cout << m_full_graph.vertices << std::endl;
    // std::cout << "m_full_graph.adjacency_matrix after:" << std::endl;
    // for (auto& bitset : m_full_graph.adjacency_matrix)
    //     std::cout << bitset << std::endl;
    // std::cout << "m_delta_graph.vertices after:" << std::endl;
    // std::cout << m_delta_graph.vertices << std::endl;
    // std::cout << "m_delta_graph.adjacency_matrix after:" << std::endl;
    // for (auto& bitset : m_delta_graph.adjacency_matrix)
    //     std::cout << bitset << std::endl;
}
}
