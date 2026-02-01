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

namespace tyr::datalog::kpkc
{

[[maybe_unused]] static bool verify_partitions(size_t nv, size_t k, const std::vector<std::vector<Vertex>>& partitions)
{
    if (partitions.size() != k)
        return false;

    auto expected = uint_t(0);

    for (uint_t pi = 0; pi < k; ++pi)
    {
        for (auto vertex : partitions[pi])
        {
            const auto v = vertex.index;

            // Must be in bounds
            if (v >= nv)
                return false;

            // Enforce *global* contiguity/order across partitions:
            // [[0,1,2],[3,4],...] => concatenation equals 0..nv-1
            if (v != expected)
                return false;

            ++expected;
        }
    }

    // Must cover exactly all vertices 0..nv-1 (no missing, no extras)
    return expected == nv;
}

[[maybe_unused]] static bool verify_vertex_to_partition(size_t nv, size_t k, const std::vector<uint_t>& vertex_to_partition)
{
    if (vertex_to_partition.size() != nv)
        return false;

    for (uint_t v = 0; v < nv; ++v)
        if (vertex_to_partition[v] >= k)
            return false;

    return true;
}

GraphLayout::GraphLayout(size_t nv_,
                         size_t k_,
                         std::vector<std::vector<Vertex>> partitions_,
                         std::vector<uint_t> vertex_to_partition_,
                         std::vector<uint_t> vertex_to_bit_) :
    nv(nv_),
    k(k_),
    partitions(std::move(partitions_)),
    vertex_to_partition(std::move(vertex_to_partition_)),
    vertex_to_bit(std::move(vertex_to_bit_))
{
    assert(verify_partitions(nv, k, partitions));
    assert(verify_vertex_to_partition(nv, k, vertex_to_partition));
}

Workspace::Workspace(const GraphLayout& graph) :
    compatible_vertices(graph.k, std::vector<boost::dynamic_bitset<>>(graph.k)),
    partition_bits(graph.k, false),
    partial_solution(),
    anchor_key(std::numeric_limits<uint_t>::max())
{
    for (uint_t pi = 0; pi < graph.k; ++pi)
        for (uint_t pj = 0; pj < graph.k; ++pj)
            compatible_vertices[pi][pj].resize(graph.partitions[pj].size());

    partial_solution.reserve(graph.k);
}

GraphLayout allocate_const_graph(const StaticConsistencyGraph& static_graph)
{
    // Fetch data
    const auto nv = static_graph.get_num_vertices();
    const auto k = static_graph.get_vertex_partitions().size();

    // Initialize partitions
    auto partitions = std::vector<std::vector<Vertex>>(k);
    auto vertex_to_partition = std::vector<uint_t>(nv);
    auto vertex_to_bit = std::vector<uint_t>(nv);
    for (size_t p = 0; p < k; ++p)
    {
        for (const auto& v : static_graph.get_vertex_partitions()[p])
        {
            vertex_to_bit[v] = partitions[p].size();
            partitions[p].push_back(Vertex(v));
            vertex_to_partition[v] = p;
        }
    }

    return GraphLayout(nv, k, std::move(partitions), std::move(vertex_to_partition), std::move(vertex_to_bit));
}

GraphActivityMasks allocate_activity_mask(const StaticConsistencyGraph& static_graph)
{
    return GraphActivityMasks { boost::dynamic_bitset<>(static_graph.get_num_vertices(), true), boost::dynamic_bitset<>(static_graph.get_num_edges(), true) };
}

Graph allocate_empty_graph(const GraphLayout& cg)
{
    auto graph = Graph();

    // Allocate
    graph.vertices.resize(cg.nv, false);

    graph.partition_vertices.resize(cg.k);
    for (uint_t p = 0; p < cg.k; ++p)
        graph.partition_vertices[p].resize(cg.partitions[p].size());

    // Allocate partition adjacency matrix
    graph.partition_adjacency_matrix.resize(cg.nv);
    for (uint_t i = 0; i < cg.nv; ++i)
    {
        graph.partition_adjacency_matrix[i].resize(cg.k);

        for (uint_t p = 0; p < cg.k; ++p)
            graph.partition_adjacency_matrix[i][p].resize(cg.partitions[p].size());
    }

    return graph;
}

DeltaKPKC::DeltaKPKC(const StaticConsistencyGraph& static_graph) :
    m_const_graph(allocate_const_graph(static_graph)),
    m_delta_graph(allocate_empty_graph(m_const_graph)),
    m_full_graph(allocate_empty_graph(m_const_graph)),
    m_read_masks(allocate_activity_mask(static_graph)),
    m_write_masks(allocate_activity_mask(static_graph)),
    m_iteration(0)
{
}

DeltaKPKC::DeltaKPKC(GraphLayout const_graph, Graph delta_graph, Graph full_graph) :
    m_const_graph(std::move(const_graph)),
    m_delta_graph(std::move(delta_graph)),
    m_full_graph(std::move(full_graph)),
    m_iteration(0)
{
}

void DeltaKPKC::set_next_assignment_sets(const StaticConsistencyGraph& static_graph, const AssignmentSets& assignment_sets)
{
    ++m_iteration;

    // Backup old graph
    std::swap(m_delta_graph, m_full_graph);

    /// 2. Initialize the full graph

    m_full_graph.reset();

    m_read_masks.vertices = m_write_masks.vertices;
    m_read_masks.edges = m_write_masks.edges;

    // Compute consistent vertices to speed up consistent edges computation
    static_graph.delta_consistent_vertices(assignment_sets,
                                           m_read_masks.vertices,
                                           [&](auto&& vertex)
                                           {
                                               // std::cout << "deactivate: " << vertex << std::endl;
                                               // Enforce delta update
                                               const auto index = vertex.get_index();
                                               const auto partition = m_const_graph.vertex_to_partition[index];
                                               const auto bit = m_const_graph.vertex_to_bit[index];

                                               // Enforce delta update
                                               assert(!m_delta_graph.vertices.test(index));

                                               m_full_graph.vertices.set(index);

                                               m_write_masks.vertices.reset(index);

                                               m_full_graph.partition_vertices[partition].set(bit);
                                           });

    std::swap(m_delta_graph.vertices, m_full_graph.vertices);
    m_full_graph.vertices |= m_delta_graph.vertices;

    std::swap(m_delta_graph.partition_vertices, m_full_graph.partition_vertices);
    for (uint_t p = 0; p < m_const_graph.k; ++p)
        m_full_graph.partition_vertices[p] |= m_delta_graph.partition_vertices[p];

    // Initialize adjacency matrix: Add consistent undirected edges to adj matrix.
    static_graph.delta_consistent_edges(assignment_sets,
                                        m_read_masks.edges,
                                        m_full_graph.vertices,
                                        [&](auto&& edge)
                                        {
                                            // std::cout << "deactivate: " << edge << std::endl;

                                            const auto first_index = edge.get_src().get_index();
                                            const auto second_index = edge.get_dst().get_index();
                                            // Enforce invariant of static consistency graph
                                            assert(first_index != second_index);

                                            m_write_masks.edges.reset(edge.get_index());

                                            const auto first_partition = m_const_graph.vertex_to_partition[first_index];
                                            const auto second_partition = m_const_graph.vertex_to_partition[second_index];
                                            const auto first_bit = m_const_graph.vertex_to_bit[first_index];
                                            const auto second_bit = m_const_graph.vertex_to_bit[second_index];
                                            assert(!m_delta_graph.partition_adjacency_matrix[first_index][second_partition].test(second_bit));
                                            assert(!m_delta_graph.partition_adjacency_matrix[second_index][first_partition].test(first_bit));
                                            m_full_graph.partition_adjacency_matrix[first_index][second_partition].set(second_bit);
                                            m_full_graph.partition_adjacency_matrix[second_index][first_partition].set(first_bit);

                                            // Enforce vertices adjacent to delta edges as delta vertices.
                                            // This wont affect the k = 1 case.
                                            m_delta_graph.vertices.set(first_index);
                                            m_delta_graph.vertices.set(second_index);

                                            m_delta_graph.partition_vertices[first_partition].set(first_bit);
                                            m_delta_graph.partition_vertices[second_partition].set(second_bit);
                                        });

    std::swap(m_delta_graph.partition_adjacency_matrix, m_full_graph.partition_adjacency_matrix);
    for (uint_t v = 0; v < m_const_graph.nv; ++v)
        for (uint_t p = 0; p < m_const_graph.k; ++p)
            m_full_graph.partition_adjacency_matrix[v][p] |= m_delta_graph.partition_adjacency_matrix[v][p];
}

void DeltaKPKC::reset()
{
    m_delta_graph.reset();
    m_full_graph.reset();
    m_read_masks.reset();
    m_write_masks.reset();
    m_iteration = 0;
}

void DeltaKPKC::for_each_new_k_clique(Cliques& cliques, Workspace& workspace) const
{
    cliques.clear();

    for_each_new_k_clique([&](auto&& clique) { cliques.append(std::span<Vertex>(clique)); }, workspace);
}

void DeltaKPKC::seed_without_anchor(Workspace& workspace) const
{
    workspace.partial_solution.clear();
    workspace.partition_bits.reset();
    workspace.anchor_key = std::numeric_limits<uint_t>::max();  // unused
    workspace.anchor_pi = std::numeric_limits<uint_t>::max();   // unused
    workspace.anchor_pj = std::numeric_limits<uint_t>::max();   // unused

    auto& cv_0 = workspace.compatible_vertices[0];

    for (uint_t p = 0; p < m_const_graph.k; ++p)
        cv_0[p] = m_full_graph.partition_vertices[p];
}

void DeltaKPKC::seed_from_anchor(const Edge& edge, Workspace& workspace) const
{
    workspace.partial_solution.clear();
    workspace.partial_solution.push_back(edge.src);
    workspace.partial_solution.push_back(edge.dst);

    if (m_const_graph.k == 2)
        return;

    const uint_t pi = m_const_graph.vertex_to_partition[edge.src.index];
    const uint_t pj = m_const_graph.vertex_to_partition[edge.dst.index];
    assert(pi < pj);

    workspace.anchor_key = edge.rank(m_const_graph.nv);
    workspace.anchor_pi = pi;
    workspace.anchor_pj = pj;
    workspace.partition_bits.reset();
    workspace.partition_bits.set(pi);
    workspace.partition_bits.set(pj);

    auto& cv_0 = workspace.compatible_vertices[0];

    for (uint_t p = 0; p < m_const_graph.k; ++p)
    {
        auto& cv_0_p = cv_0[p];
        cv_0_p.reset();

        if (p == pi || p == pj)
            continue;

        cv_0_p |= m_full_graph.partition_adjacency_matrix[edge.src.index][p];
        cv_0_p &= m_full_graph.partition_adjacency_matrix[edge.dst.index][p];

        if (p < pj)
            cv_0_p -= m_delta_graph.partition_adjacency_matrix[edge.src.index][p];

        if (p < pi)
            cv_0_p -= m_delta_graph.partition_adjacency_matrix[edge.dst.index][p];
    }
}

uint_t DeltaKPKC::choose_best_partition(size_t depth, const Workspace& workspace) const
{
    const uint_t k = m_const_graph.k;
    const auto& cv_d = workspace.compatible_vertices[depth];
    const auto& partition_bits = workspace.partition_bits;

    uint_t best_partition = std::numeric_limits<uint_t>::max();
    uint_t best_set_bits = std::numeric_limits<uint_t>::max();

    for (uint_t p = 0; p < k; ++p)
    {
        if (partition_bits.test(p))
            continue;

        auto num_set_bits = cv_d[p].count();
        if (num_set_bits < best_set_bits)
        {
            best_set_bits = num_set_bits;
            best_partition = p;
        }
    }
    return best_partition;
}

uint_t DeltaKPKC::num_possible_additions_at_next_depth(size_t depth, const Workspace& workspace) const
{
    const uint_t k = m_const_graph.k;
    const auto& cv_d_next = workspace.compatible_vertices[depth + 1];
    const auto& partition_bits = workspace.partition_bits;

    uint_t possible_additions = 0;
    for (uint_t partition = 0; partition < k; ++partition)
        if (!partition_bits.test(partition) && cv_d_next[partition].any())
            ++possible_additions;

    return possible_additions;
}
}
