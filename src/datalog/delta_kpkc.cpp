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

GraphLayout::GraphLayout(size_t nv_, size_t k_, std::vector<std::vector<Vertex>> partitions_, std::vector<uint_t> vertex_to_partition_) :
    nv(nv_),
    k(k_),
    partitions(std::move(partitions_)),
    vertex_to_partition(std::move(vertex_to_partition_))
{
    assert(verify_partitions(nv, k, partitions));
    assert(verify_vertex_to_partition(nv, k, vertex_to_partition));
}

Workspace::Workspace(const GraphLayout& graph) :
    compatible_vertices(graph.k, std::vector<boost::dynamic_bitset<>>(graph.k)),
    partition_bits(graph.k, false),
    partial_solution(),
    contains_delta_edge(),
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
    for (size_t p = 0; p < k; ++p)
    {
        for (const auto& v : static_graph.get_vertex_partitions()[p])
        {
            partitions[p].push_back(Vertex(v));
            vertex_to_partition[v] = p;
        }
    }

    return GraphLayout(nv, k, std::move(partitions), std::move(vertex_to_partition));
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

    // Backup old graph
    std::swap(m_delta_graph, m_full_graph);

    /// 2. Initialize the full graph

    m_full_graph.reset();

    // std::cout << "set_next_assignment_sets" << std::endl;

    m_read_masks.vertices = m_write_masks.vertices;
    m_read_masks.edges = m_write_masks.edges;

    // Compute consistent vertices to speed up consistent edges computation
    static_graph.delta_consistent_vertices(assignment_sets,
                                           m_read_masks.vertices,
                                           [&](auto&& vertex)
                                           {
                                               // std::cout << "deactivate: " << vertex << std::endl;
                                               // Enforce delta update
                                               assert(!m_delta_graph.vertices.test(vertex.get_index()));
                                               m_full_graph.vertices.set(vertex.get_index());
                                               m_write_masks.vertices.reset(vertex.get_index());
                                           });

    std::swap(m_delta_graph.vertices, m_full_graph.vertices);
    m_full_graph.vertices |= m_delta_graph.vertices;

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
                                            auto& first_row = m_full_graph.adjacency_matrix[first_index];
                                            auto& second_row = m_full_graph.adjacency_matrix[second_index];
                                            // Enforce delta update
                                            assert(!m_delta_graph.adjacency_matrix[first_index].test(second_index));
                                            assert(!m_delta_graph.adjacency_matrix[second_index].test(first_index));
                                            first_row.set(second_index);
                                            second_row.set(first_index);
                                            m_write_masks.edges.reset(edge.get_index());

                                            // Enforce vertices adjacent to delta edges as delta vertices.
                                            // This wont affect the k = 1 case.
                                            m_delta_graph.vertices.set(first_index);
                                            m_delta_graph.vertices.set(second_index);
                                        });

    std::swap(m_delta_graph.adjacency_matrix, m_full_graph.adjacency_matrix);
    for (uint v = 0; v < m_const_graph.nv; ++v)
        m_full_graph.adjacency_matrix[v] |= m_delta_graph.adjacency_matrix[v];

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

void DeltaKPKC::reset()
{
    m_delta_graph.reset();
    m_full_graph.reset();
    m_read_masks.reset();
    m_write_masks.reset();
    m_iteration = 0;
}

void DeltaKPKC::seed_without_anchor(Workspace& workspace) const
{
    workspace.partial_solution.clear();
    workspace.partition_bits.reset();
    workspace.anchor_key = std::numeric_limits<uint_t>::max();  // unused

    auto& cv_0 = workspace.compatible_vertices[0];

    for (uint_t p = 0; p < m_const_graph.k; ++p)
    {
        auto& cv_0_p = cv_0[p];
        cv_0_p.reset();

        const auto& partition = m_const_graph.partitions[p];
        for (uint_t bit = 0; bit < partition.size(); ++bit)
            if (m_full_graph.vertices.test(partition[bit].index))
                cv_0_p.set(bit);
    }
}

void DeltaKPKC::seed_from_anchor(const Vertex& vertex, Workspace& workspace) const
{
    assert(m_delta_graph.contains(vertex));

    workspace.partial_solution.clear();
    workspace.partial_solution.push_back(vertex);

    if (m_const_graph.k == 1)
        return;

    const uint_t pi = m_const_graph.vertex_to_partition[vertex.index];

    workspace.contains_delta_edge.push_back(false);
    workspace.anchor_key = vertex.index;
    workspace.partition_bits.reset();
    workspace.partition_bits.set(pi);

    const auto& full_row = m_full_graph.adjacency_matrix[vertex.index];
    auto& cv_0 = workspace.compatible_vertices[0];

    for (uint_t p = 0; p < m_const_graph.k; ++p)
    {
        auto& cv_0_p = cv_0[p];
        cv_0_p.reset();

        if (p == pi)
            continue;

        const auto& partition = m_const_graph.partitions[p];

        for (uint_t bit = 0; bit < partition.size(); ++bit)
        {
            const auto c = partition[bit];

            if (!full_row.test(c.index))
                continue;

            // ownership: anchor must be smallest delta vertex in clique
            if (m_delta_graph.vertices.test(c.index) && c.index < workspace.anchor_key)
                continue;

            cv_0_p.set(bit);
        }
    }
}

void DeltaKPKC::seed_from_anchor(const Edge& edge, Workspace& workspace) const
{
    assert(m_delta_graph.contains(edge));

    workspace.partial_solution.clear();
    workspace.partial_solution.push_back(edge.src);
    workspace.partial_solution.push_back(edge.dst);

    if (m_const_graph.k == 2)
        return;

    const uint_t pi = m_const_graph.vertex_to_partition[edge.src.index];
    const uint_t pj = m_const_graph.vertex_to_partition[edge.dst.index];
    assert(pi != pj);

    workspace.anchor_key = edge.rank(m_const_graph.nv);
    workspace.partition_bits.reset();
    workspace.partition_bits.set(pi);
    workspace.partition_bits.set(pj);

    auto is_legal = [&](const Edge& edge)
    {
        if (!m_full_graph.contains(edge))
            return false;
        if (!m_delta_graph.contains(edge))
            return true;  // Not a delta edge -> always legal if in full
        return edge.rank(m_const_graph.nv) > workspace.anchor_key;
    };

    auto& cv_0 = workspace.compatible_vertices[0];

    for (uint_t p = 0; p < m_const_graph.k; ++p)
    {
        auto& cv_0_p = cv_0[p];
        cv_0_p.reset();

        if (p == pi || p == pj)
            continue;

        const auto& partition = m_const_graph.partitions[p];

        for (uint_t bit = 0; bit < partition.size(); ++bit)
        {
            const auto candidate_vertex = partition[bit];

            if (is_legal(Edge(edge.src, candidate_vertex)) && is_legal(Edge(edge.dst, candidate_vertex)))
                cv_0_p.set(bit);
        }
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
