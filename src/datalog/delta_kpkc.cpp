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

[[maybe_unused]] static bool verify_partitions(const std::vector<Vertex>& partitions)
{
    for (size_t i = 0; i < partitions.size(); ++i)
    {
        if (i != partitions[i].index)
            return false;
    }
    return true;
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

GraphLayout::GraphLayout(const StaticConsistencyGraph& static_graph) :
    nv(static_graph.get_num_vertices()),
    k(static_graph.get_vertex_partitions().size()),
    partitions(),
    vertex_to_partition(),
    vertex_to_bit(),
    info()
{
    partitions.reserve(nv);
    vertex_to_partition.resize(nv);
    vertex_to_bit.resize(nv);
    info.infos.reserve(k);

    uint_t block_offset = uint_t(0);

    for (size_t p = 0; p < k; ++p)
    {
        const auto& partition = static_graph.get_vertex_partitions()[p];

        const auto partition_size = static_cast<uint_t>(partition.size());
        const auto partition_blocks = static_cast<uint_t>(BitsetSpan<uint64_t>::num_blocks(partition_size));
        const auto bit_offset = static_cast<uint_t>(partitions.size());
        info.infos.push_back(GraphLayout::BitsetInfo { bit_offset, partition_size, block_offset, partition_blocks });
        block_offset += partition_blocks;

        uint_t bit = 0;
        for (const auto& v : partition)
        {
            vertex_to_bit[v] = bit++;
            partitions.push_back(Vertex(v));
            vertex_to_partition[v] = p;
        }
    }

    info.num_blocks = block_offset;

    assert(verify_partitions(partitions));
    assert(verify_vertex_to_partition(nv, k, vertex_to_partition));
}

Workspace::Workspace(const GraphLayout& graph) :
    compatible_vertices_data(graph.k * graph.info.num_blocks, 0),
    compatible_vertices_span(compatible_vertices_data.data(), std::array<size_t, 2> { graph.k, graph.info.num_blocks }),
    partition_bits(graph.k, false),
    partial_solution(),
    anchor_key(std::numeric_limits<uint_t>::max())
{
    partial_solution.reserve(graph.k);
}

GraphActivityMasks::GraphActivityMasks(const StaticConsistencyGraph& static_graph) :
    vertices(static_graph.get_num_vertices(), true),
    edges(static_graph.get_num_edges(), true)
{
}

Graph::Graph(const GraphLayout& cg) :
    cg(cg),
    vertices(cg.nv, false),
    partition_vertices_data(cg.info.num_blocks, 0),
    partition_adjacency_matrix_data(cg.nv * cg.info.num_blocks, 0),
    partition_adjacency_matrix_span(partition_adjacency_matrix_data.data(), std::array<size_t, 2> { cg.nv, cg.info.num_blocks })
{
}

DeltaKPKC::DeltaKPKC(const StaticConsistencyGraph& static_graph) :
    m_const_graph(GraphLayout(static_graph)),
    m_delta_graph(Graph(m_const_graph)),
    m_full_graph(Graph(m_const_graph)),
    m_read_masks(GraphActivityMasks(static_graph)),
    m_write_masks(GraphActivityMasks(static_graph)),
    m_iteration(0)
{
}

DeltaKPKC::DeltaKPKC(GraphLayout const_graph, Graph delta_graph, Graph full_graph, GraphActivityMasks read_masks, GraphActivityMasks write_masks) :
    m_const_graph(std::move(const_graph)),
    m_delta_graph(std::move(delta_graph)),
    m_full_graph(std::move(full_graph)),
    m_read_masks(std::move(read_masks)),
    m_write_masks(std::move(write_masks)),
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

                                               const auto& info = m_const_graph.info.infos[partition];
                                               auto& full_partition_row = m_full_graph.partition_vertices_data;
                                               auto full_partition = BitsetSpan<uint64_t>(full_partition_row.data() + info.block_offset, info.num_bits);
                                               full_partition.set(bit);
                                           });

    std::swap(m_delta_graph.vertices, m_full_graph.vertices);
    m_full_graph.vertices |= m_delta_graph.vertices;

    std::swap(m_delta_graph.partition_vertices_data, m_full_graph.partition_vertices_data);
    for (uint_t p = 0; p < m_const_graph.k; ++p)
    {
        const auto& info = m_const_graph.info.infos[p];
        auto& delta_partition_row = m_delta_graph.partition_vertices_data;
        auto delta_partition = BitsetSpan<const uint64_t>(delta_partition_row.data() + info.block_offset, info.num_bits);
        auto& full_partition_row = m_full_graph.partition_vertices_data;
        auto full_partition = BitsetSpan<uint64_t>(full_partition_row.data() + info.block_offset, info.num_bits);
        full_partition |= delta_partition;
    }

    // Initialize adjacency matrix: Add consistent undirected edges to adj matrix.
    static_graph.delta_consistent_edges(
        assignment_sets,
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
            const auto& first_info = m_const_graph.info.infos[first_partition];
            const auto& second_info = m_const_graph.info.infos[second_partition];

            {
                auto first_adj_list_row = m_full_graph.partition_adjacency_matrix_span(first_index);
                auto second_adj_list_row = m_full_graph.partition_adjacency_matrix_span(second_index);
                auto first_adj_list = BitsetSpan<uint64_t>(first_adj_list_row.data() + second_info.block_offset, second_info.num_bits);
                auto second_adj_list = BitsetSpan<uint64_t>(second_adj_list_row.data() + first_info.block_offset, first_info.num_bits);
                assert(!first_adj_list.test(second_bit));
                assert(!second_adj_list.test(first_bit));
                first_adj_list.set(second_bit);
                second_adj_list.set(first_bit);
            }

            {
                // Enforce vertices adjacent to delta edges as delta vertices.
                // This wont affect the k = 1 case.
                m_delta_graph.vertices.set(first_index);
                m_delta_graph.vertices.set(second_index);

                auto& first_partition_row = m_delta_graph.partition_vertices_data;
                auto& second_partition_row = m_delta_graph.partition_vertices_data;
                auto first_partition = BitsetSpan<uint64_t>(first_partition_row.data() + first_info.block_offset, first_info.num_bits);
                auto second_partition = BitsetSpan<uint64_t>(second_partition_row.data() + second_info.block_offset, second_info.num_bits);
                first_partition.set(first_bit);
                second_partition.set(second_bit);
            }
        });

    std::swap(m_delta_graph.partition_adjacency_matrix_data, m_full_graph.partition_adjacency_matrix_data);
    std::swap(m_delta_graph.partition_adjacency_matrix_span, m_full_graph.partition_adjacency_matrix_span);
    for (auto v = m_delta_graph.vertices.find_first(); v != boost::dynamic_bitset<>::npos; v = m_delta_graph.vertices.find_next(v))
    {
        const auto pv = m_const_graph.vertex_to_partition[v];
        const auto delta_v_adj_list_row = m_delta_graph.partition_adjacency_matrix_span(v);
        auto full_v_adj_list_row = m_full_graph.partition_adjacency_matrix_span(v);

        for (uint_t p = 0; p < m_const_graph.k; ++p)
        {
            if (p == pv)
                continue;  ///< no edges between vertices in the same partition

            const auto& info = m_const_graph.info.infos[p];
            auto delta_v_adj_list = BitsetSpan<const uint64_t>(delta_v_adj_list_row.data() + info.block_offset, info.num_bits);
            auto full_v_adj_list = BitsetSpan<uint64_t>(full_v_adj_list_row.data() + info.block_offset, info.num_bits);
            full_v_adj_list |= delta_v_adj_list;
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

    auto cv_0_row = workspace.compatible_vertices_span(0);

    for (uint_t p = 0; p < m_const_graph.k; ++p)
    {
        const auto& info = m_const_graph.info.infos[p];
        auto cv_0 = BitsetSpan<uint64_t>(cv_0_row.data() + info.block_offset, info.num_bits);

        auto partition = BitsetSpan<const uint64_t>(m_full_graph.partition_vertices_data.data() + info.block_offset, info.num_bits);
        cv_0.copy_from(partition);
    }
}

bool DeltaKPKC::seed_from_anchor(const Edge& edge, Workspace& workspace) const
{
    workspace.partial_solution.clear();
    workspace.partial_solution.push_back(edge.src);
    workspace.partial_solution.push_back(edge.dst);

    const uint_t pi = m_const_graph.vertex_to_partition[edge.src.index];
    const uint_t pj = m_const_graph.vertex_to_partition[edge.dst.index];
    assert(pi < pj);

    workspace.anchor_key = edge.rank(m_const_graph.nv);
    workspace.anchor_pi = pi;
    workspace.anchor_pj = pj;
    workspace.partition_bits.reset();
    workspace.partition_bits.set(pi);
    workspace.partition_bits.set(pj);

    auto cv_0_row = workspace.compatible_vertices_span(0);
    const auto full_src_adj_list_row = m_full_graph.partition_adjacency_matrix_span(edge.src.index);
    const auto full_dst_adj_list_row = m_full_graph.partition_adjacency_matrix_span(edge.dst.index);
    const auto delta_src_adj_list_row = m_delta_graph.partition_adjacency_matrix_span(edge.src.index);
    const auto delta_dst_adj_list_row = m_delta_graph.partition_adjacency_matrix_span(edge.dst.index);

    for (uint_t p = 0; p < m_const_graph.k; ++p)
    {
        if (p == pi || p == pj)
            continue;

        const auto& info = m_const_graph.info.infos[p];
        auto cv_0 = BitsetSpan<uint64_t>(cv_0_row.data() + info.block_offset, info.num_bits);
        auto full_src_adj_list = BitsetSpan<const uint64_t>(full_src_adj_list_row.data() + info.block_offset, info.num_bits);
        auto full_dst_adj_list = BitsetSpan<const uint64_t>(full_dst_adj_list_row.data() + info.block_offset, info.num_bits);
        auto delta_src_adj_list = BitsetSpan<const uint64_t>(delta_src_adj_list_row.data() + info.block_offset, info.num_bits);
        auto delta_dst_adj_list = BitsetSpan<const uint64_t>(delta_dst_adj_list_row.data() + info.block_offset, info.num_bits);

        cv_0.copy_from(full_src_adj_list);
        cv_0 &= full_dst_adj_list;

        if (p < pj)
            cv_0 -= delta_src_adj_list;

        if (p < pi)
            cv_0 -= delta_dst_adj_list;

        if (!cv_0.any())
            return false;  ///< triangle pruning
    }

    return true;
}

uint_t DeltaKPKC::choose_best_partition(size_t depth, const Workspace& workspace) const
{
    const uint_t k = m_const_graph.k;
    const auto& partition_bits = workspace.partition_bits;

    uint_t best_partition = std::numeric_limits<uint_t>::max();
    uint_t best_set_bits = std::numeric_limits<uint_t>::max();
    const auto cv_curr_row = workspace.compatible_vertices_span(depth);
    for (uint_t p = 0; p < k; ++p)
    {
        if (partition_bits.test(p))
            continue;

        const auto& info = m_const_graph.info.infos[p];
        auto cv_curr = BitsetSpan<const uint64_t>(cv_curr_row.data() + info.block_offset, info.num_bits);

        const auto num_set_bits = cv_curr.count();
        if (num_set_bits < best_set_bits)
        {
            best_set_bits = num_set_bits;
            best_partition = p;
        }
    }

    return best_partition;
}
}
