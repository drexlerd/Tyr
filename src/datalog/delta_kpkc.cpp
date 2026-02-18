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

DeltaKPKC::DeltaKPKC(const StaticConsistencyGraph& static_graph) :
    m_const_graph(GraphLayout(static_graph)),
    m_iteration(0),
    m_delta_graph2(m_const_graph, static_graph.get_vertex_partitions(), static_graph.get_variable_dependeny_graph()),
    m_full_graph2(m_const_graph, static_graph.get_vertex_partitions(), static_graph.get_variable_dependeny_graph())
{
}

DeltaKPKC::DeltaKPKC(GraphLayout const_graph, Graph2 delta_graph2, Graph2 full_graph2) :
    m_const_graph(std::move(const_graph)),
    m_iteration(0),
    m_delta_graph2(std::move(delta_graph2)),
    m_full_graph2(std::move(full_graph2))
{
}

SetNewAssignmentSetsStatistics DeltaKPKC::set_next_assignment_sets(const StaticConsistencyGraph& static_graph,
                                                                   const TaggedFactSets<formalism::FluentTag>& delta_fact_sets,
                                                                   const AssignmentSets& assignment_sets)
{
    std::chrono::steady_clock::time_point start_time2 = std::chrono::steady_clock::now();

    static_graph.initialize_dynamic_consistency_graphs(assignment_sets, delta_fact_sets, m_const_graph, m_delta_graph2, m_full_graph2);

    std::chrono::steady_clock::time_point end_time2 = std::chrono::steady_clock::now();

    ++m_iteration;

    return SetNewAssignmentSetsStatistics();
}

void DeltaKPKC::reset()
{
    m_delta_graph2.reset();
    m_full_graph2.reset();
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

        auto partition = m_full_graph2.matrix.affected_partitions().get_bitset(info);
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

    for (uint_t p = 0; p < m_const_graph.k; ++p)
    {
        if (p == pi || p == pj)
            continue;

        const auto& info = m_const_graph.info.infos[p];
        auto cv_0 = BitsetSpan<uint64_t>(cv_0_row.data() + info.block_offset, info.num_bits);
        auto full_src_adj_list = m_full_graph2.matrix.get_bitset(edge.src.index, p);
        auto full_dst_adj_list = m_full_graph2.matrix.get_bitset(edge.dst.index, p);

        cv_0.copy_from(full_src_adj_list);
        cv_0 &= full_dst_adj_list;

        if (!cv_0.any())
            return false;  ///< triangle pruning

        auto delta_src_adj_list = m_delta_graph2.matrix.get_bitset(edge.src.index, p);

        if (p < pj)
            cv_0 -= delta_src_adj_list;

        if (!cv_0.any())
            return false;  ///< triangle pruning

        auto delta_dst_adj_list = m_delta_graph2.matrix.get_bitset(edge.dst.index, p);

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
