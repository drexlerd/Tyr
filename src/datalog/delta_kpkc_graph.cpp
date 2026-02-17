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

#include "tyr/datalog/consistency_graph.hpp"
#include "tyr/datalog/delta_kpkc.hpp"

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

}
