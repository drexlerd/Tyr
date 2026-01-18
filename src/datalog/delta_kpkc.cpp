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

namespace tyr::datalog::delta_kpkc
{

ConstGraph::ConstGraph(size_t nv_, size_t k_, std::vector<std::vector<Vertex>> partitions_, std::vector<uint_t> vertex_to_partition_) :
    nv(nv_),
    k(k_),
    partitions(std::move(partitions_)),
    vertex_to_partition(std::move(vertex_to_partition_))
{
}

Workspace::Workspace(const ConstGraph& graph) :
    compatible_vertices(graph.k, std::vector<boost::dynamic_bitset<>>(graph.k)),
    partition_bits(graph.k, false),
    partial_solution()
{
    for (uint_t pi = 0; pi < graph.k; ++pi)
        for (uint_t pj = 0; pj < graph.k; ++pj)
            compatible_vertices[pi][pj].resize(graph.partitions[pj].size());

    partial_solution.reserve(graph.k);
}

ConstGraph allocate_const_graph(const StaticConsistencyGraph& static_graph)
{
    // Fetch data
    const auto nv = static_graph.get_num_vertices();
    const auto k = static_graph.get_partitions().size();

    // Initialize partitions
    auto partitions = std::vector<std::vector<Vertex>>(k);
    auto vertex_to_partition = std::vector<uint_t>(nv);
    for (size_t p = 0; p < k; ++p)
    {
        for (const auto& v : static_graph.get_partitions()[p])
        {
            partitions[p].push_back(Vertex(v));
            vertex_to_partition[v] = p;
        }
    }

    return ConstGraph(nv, k, std::move(partitions), std::move(vertex_to_partition));
}

Graph allocate_empty_graph(const ConstGraph& cg)
{
    auto graph = Graph();

    // Allocate
    graph.vertices.resize(cg.nv, false);

    // Allocate adjacency matrix (V x V)
    graph.adjacency_matrix.resize(cg.nv);
    for (uint_t i = 0; i < cg.nv; ++i)
        graph.adjacency_matrix[i].resize(cg.nv, false);

    return graph;
}

DeltaKPKC::DeltaKPKC(const StaticConsistencyGraph& static_graph) :
    m_const_graph(allocate_const_graph(static_graph)),
    m_delta_graph(allocate_empty_graph(m_const_graph)),
    m_full_graph(allocate_empty_graph(m_const_graph)),
    m_workspace(m_const_graph),
    m_iteration(0)
{
}

DeltaKPKC::DeltaKPKC(ConstGraph const_graph, Graph delta_graph, Graph full_graph, Workspace workspace) :
    m_const_graph(std::move(const_graph)),
    m_delta_graph(std::move(delta_graph)),
    m_full_graph(std::move(full_graph)),
    m_workspace(std::move(workspace)),
    m_iteration(0)
{
}

void DeltaKPKC::set_next_assignment_sets(const StaticConsistencyGraph& static_graph, const AssignmentSets& assignment_sets)
{
    ++m_iteration;

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
    for (uint_t p = 0; p < m_const_graph.k; ++p)
        m_workspace.compatible_vertices[0][p].set();

    // Initialize partition bits: Reset the partition bits
    m_workspace.partition_bits.reset();

    /// 3. Set delta graph vertices to those that were added
    m_delta_graph.vertices ^= m_full_graph.vertices;  // OLD ⊕ NEW
    m_delta_graph.vertices &= m_full_graph.vertices;  // (OLD ⊕ NEW) ∧ NEW = NEW ∧ ~OLD

    /// 4. Set delta graph edges to those that were added
    for (uint v = 0; v < m_const_graph.nv; ++v)
    {
        m_delta_graph.adjacency_matrix[v] ^= m_full_graph.adjacency_matrix[v];  // OLD ⊕ NEW
        m_delta_graph.adjacency_matrix[v] &= m_full_graph.adjacency_matrix[v];  // (OLD ⊕ NEW) ∧ NEW = NEW ∧ ~OLD
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
