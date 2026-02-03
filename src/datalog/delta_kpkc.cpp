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
                         std::vector<uint_t> vertex_to_bit_,
                         std::vector<BitsetInfo> partition_info_,
                         size_t num_blocks_for_partitions_) :
    nv(nv_),
    k(k_),
    partitions(std::move(partitions_)),
    vertex_to_partition(std::move(vertex_to_partition_)),
    vertex_to_bit(std::move(vertex_to_bit_)),
    partition_info(std::move(partition_info_)),
    num_blocks_for_partitions(num_blocks_for_partitions_)
{
    assert(verify_partitions(nv, k, partitions));
    assert(verify_vertex_to_partition(nv, k, vertex_to_partition));
}

Workspace::Workspace(const GraphLayout& graph) :
    compatible_vertices_span_data(graph.k * graph.num_blocks_for_partitions, 0),
    compatible_vertices_span(compatible_vertices_span_data.data(), std::array<size_t, 2> { graph.k, graph.num_blocks_for_partitions }),
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
    auto partition_info = std::vector<GraphLayout::BitsetInfo>();

    uint_t bitset_offset_blocks = uint_t(0);

    for (size_t p = 0; p < k; ++p)
    {
        const auto& partition = static_graph.get_vertex_partitions()[p];

        const auto partition_size = static_cast<uint_t>(partition.size());
        const auto partition_blocks = static_cast<uint_t>(BitsetSpan<uint64_t>::num_blocks(partition_size));
        partition_info.push_back(GraphLayout::BitsetInfo { bitset_offset_blocks, partition_size, partition_blocks });
        bitset_offset_blocks += partition_blocks;

        for (const auto& v : partition)
        {
            vertex_to_bit[v] = partitions[p].size();
            partitions[p].push_back(Vertex(v));
            vertex_to_partition[v] = p;
        }
    }

    return GraphLayout(nv, k, std::move(partitions), std::move(vertex_to_partition), std::move(vertex_to_bit), std::move(partition_info), bitset_offset_blocks);
}

GraphActivityMasks allocate_activity_mask(const StaticConsistencyGraph& static_graph)
{
    return GraphActivityMasks { boost::dynamic_bitset<>(static_graph.get_num_vertices(), true), boost::dynamic_bitset<>(static_graph.get_num_edges(), true) };
}

Graph::Graph(const GraphLayout& cg) :
    cg(cg),
    vertices(),
    partition_vertices(),
    partition_vertices_span_data(),
    partition_adjacency_matrix(),
    partition_adjacency_matrix_span_data(),
    partition_adjacency_matrix_span()
{
    partition_vertices_span_data.resize(cg.num_blocks_for_partitions, 0);

    partition_adjacency_matrix_span_data.resize(cg.nv * cg.num_blocks_for_partitions, 0);
    partition_adjacency_matrix_span =
        MDSpan<uint64_t, 2>(partition_adjacency_matrix_span_data.data(), std::array<size_t, 2> { cg.nv, cg.num_blocks_for_partitions });

    // Allocate
    vertices.resize(cg.nv, false);

    partition_vertices.resize(cg.k);
    for (uint_t p = 0; p < cg.k; ++p)
        partition_vertices[p].resize(cg.partitions[p].size());

    // Allocate partition adjacency matrix
    partition_adjacency_matrix.resize(cg.nv);
    for (uint_t i = 0; i < cg.nv; ++i)
    {
        partition_adjacency_matrix[i].resize(cg.k);

        for (uint_t p = 0; p < cg.k; ++p)
            partition_adjacency_matrix[i][p].resize(cg.partitions[p].size());
    }
}

DeltaKPKC::DeltaKPKC(const StaticConsistencyGraph& static_graph) :
    m_const_graph(allocate_const_graph(static_graph)),
    m_delta_graph(Graph(m_const_graph)),
    m_full_graph(Graph(m_const_graph)),
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

                                               const auto& info = m_const_graph.partition_info[partition];
                                               auto dst = BitsetSpan<uint64_t>(m_full_graph.partition_vertices_span_data.data() + info.offset, info.num_bits);
                                               dst.set(bit);
                                           });

    std::swap(m_delta_graph.vertices, m_full_graph.vertices);
    m_full_graph.vertices |= m_delta_graph.vertices;

    std::swap(m_delta_graph.partition_vertices, m_full_graph.partition_vertices);
    for (uint_t p = 0; p < m_const_graph.k; ++p)
        m_full_graph.partition_vertices[p] |= m_delta_graph.partition_vertices[p];

    std::swap(m_delta_graph.partition_vertices_span_data, m_full_graph.partition_vertices_span_data);
    for (uint_t p = 0; p < m_const_graph.k; ++p)
    {
        const auto& info = m_const_graph.partition_info[p];
        auto src = BitsetSpan<const uint64_t>(m_delta_graph.partition_vertices_span_data.data() + info.offset, info.num_bits);
        auto dst = BitsetSpan<uint64_t>(m_full_graph.partition_vertices_span_data.data() + info.offset, info.num_bits);
        dst |= src;
    }

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

                                            const auto& first_info = m_const_graph.partition_info[first_partition];
                                            const auto& second_info = m_const_graph.partition_info[second_partition];

                                            {
                                                auto* first_data = m_full_graph.partition_adjacency_matrix_span(first_index).data();
                                                auto* second_data = m_full_graph.partition_adjacency_matrix_span(second_index).data();
                                                auto first_dst = BitsetSpan<uint64_t>(first_data + second_info.offset, second_info.num_bits);
                                                auto second_dst = BitsetSpan<uint64_t>(second_data + first_info.offset, first_info.num_bits);
                                                first_dst.set(second_bit);
                                                second_dst.set(first_bit);
                                            }

                                            // Enforce vertices adjacent to delta edges as delta vertices.
                                            // This wont affect the k = 1 case.
                                            m_delta_graph.vertices.set(first_index);
                                            m_delta_graph.vertices.set(second_index);

                                            m_delta_graph.partition_vertices[first_partition].set(first_bit);
                                            m_delta_graph.partition_vertices[second_partition].set(second_bit);

                                            {
                                                auto* data = m_delta_graph.partition_vertices_span_data.data();
                                                auto first_dst = BitsetSpan<uint64_t>(data + first_info.offset, first_info.num_bits);
                                                auto second_dst = BitsetSpan<uint64_t>(data + second_info.offset, second_info.num_bits);
                                                first_dst.set(first_bit);
                                                second_dst.set(second_bit);
                                            }
                                        });

    std::swap(m_delta_graph.partition_adjacency_matrix, m_full_graph.partition_adjacency_matrix);
    for (uint_t v = 0; v < m_const_graph.nv; ++v)
        for (uint_t p = 0; p < m_const_graph.k; ++p)
            m_full_graph.partition_adjacency_matrix[v][p] |= m_delta_graph.partition_adjacency_matrix[v][p];

    std::swap(m_delta_graph.partition_adjacency_matrix_span_data, m_full_graph.partition_adjacency_matrix_span_data);
    std::swap(m_delta_graph.partition_adjacency_matrix_span, m_full_graph.partition_adjacency_matrix_span);
    for (uint_t v = 0; v < m_const_graph.nv; ++v)
    {
        const auto* src_row = m_delta_graph.partition_adjacency_matrix_span(v).data();
        auto* dst_row = m_full_graph.partition_adjacency_matrix_span(v).data();

        for (uint_t p = 0; p < m_const_graph.k; ++p)
        {
            const auto& info = m_const_graph.partition_info[p];
            auto src = BitsetSpan<const uint64_t>(src_row + info.offset, info.num_bits);
            auto dst = BitsetSpan<uint64_t>(dst_row + info.offset, info.num_bits);
            dst |= src;
        }
    }
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

    {
        auto& cv_0 = workspace.compatible_vertices[0];

        for (uint_t p = 0; p < m_const_graph.k; ++p)
            cv_0[p] = m_full_graph.partition_vertices[p];
    }

    {
        auto* dst_row = workspace.compatible_vertices_span(0).data();
        const auto* src_row = m_full_graph.partition_vertices_span_data.data();
        for (uint_t p = 0; p < m_const_graph.k; ++p)
        {
            const auto& info = m_const_graph.partition_info[p];
            auto dst = BitsetSpan<uint64_t>(dst_row + info.offset, info.num_bits);
            auto src = BitsetSpan<const uint64_t>(src_row + info.offset, info.num_bits);
            dst.copy_from(src);
        }
    }
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

    {
        auto* dst_row = workspace.compatible_vertices_span(0).data();
        const auto* src_full_src_row = m_full_graph.partition_adjacency_matrix_span(edge.src.index).data();
        const auto* src_full_dst_row = m_full_graph.partition_adjacency_matrix_span(edge.dst.index).data();
        const auto* src_delta_src_row = m_delta_graph.partition_adjacency_matrix_span(edge.src.index).data();
        const auto* src_delta_dst_row = m_delta_graph.partition_adjacency_matrix_span(edge.dst.index).data();
        for (uint_t p = 0; p < m_const_graph.k; ++p)
        {
            const auto& info = m_const_graph.partition_info[p];
            auto dst = BitsetSpan<uint64_t>(dst_row + info.offset, info.num_bits);
            auto src_full_src = BitsetSpan<const uint64_t>(src_full_src_row + info.offset, info.num_bits);
            auto src_full_dst = BitsetSpan<const uint64_t>(src_full_dst_row + info.offset, info.num_bits);
            auto src_delta_src = BitsetSpan<const uint64_t>(src_delta_src_row + info.offset, info.num_bits);
            auto src_delta_dst = BitsetSpan<const uint64_t>(src_delta_dst_row + info.offset, info.num_bits);

            dst.reset();

            dst |= src_full_src;
            dst &= src_full_dst;

            if (p < pj)
                dst -= src_delta_src;

            if (p < pi)
                dst -= src_delta_dst;
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

        const auto num_set_bits = cv_d[p].count();
        if (num_set_bits < best_set_bits)
        {
            best_set_bits = num_set_bits;
            best_partition = p;
        }
    }

    uint_t best_partition2 = std::numeric_limits<uint_t>::max();
    uint_t best_set_bits2 = std::numeric_limits<uint_t>::max();
    const auto* row = workspace.compatible_vertices_span(depth).data();
    for (uint_t p = 0; p < k; ++p)
    {
        if (partition_bits.test(p))
            continue;

        const auto& info = m_const_graph.partition_info[p];
        auto dst = BitsetSpan<const uint64_t>(row + info.offset, info.num_bits);

        const auto num_set_bits = dst.count();
        if (num_set_bits < best_set_bits2)
        {
            best_set_bits2 = num_set_bits;
            best_partition2 = p;
        }
    }

    assert(best_partition == best_partition2);

    return best_partition;
}

uint_t DeltaKPKC::num_possible_additions_at_next_depth(size_t depth, const Workspace& workspace) const
{
    const uint_t k = m_const_graph.k;
    const auto& cv_d_next = workspace.compatible_vertices[depth + 1];
    const auto& partition_bits = workspace.partition_bits;

    uint_t possible_additions = 0;
    for (uint_t p = 0; p < k; ++p)
        if (!partition_bits.test(p) && cv_d_next[p].any())
            ++possible_additions;

    uint_t possible_additions2 = 0;
    const auto* next_row = workspace.compatible_vertices_span(depth + 1).data();

    for (uint_t p = 0; p < k; ++p)
    {
        if (!partition_bits.test(p))
        {
            const auto& info = m_const_graph.partition_info[p];
            auto dst = BitsetSpan<const uint64_t>(next_row + info.offset, info.num_bits);

            if (dst.any())
                ++possible_additions2;
        }
    }

    assert(possible_additions == possible_additions2);

    return possible_additions;
}
}
