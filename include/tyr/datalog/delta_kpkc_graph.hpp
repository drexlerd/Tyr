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

#ifndef TYR_DATALOG_DELTA_KPKC_GRAPH_HPP_
#define TYR_DATALOG_DELTA_KPKC_GRAPH_HPP_

#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/common/formatter.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/datalog/declarations.hpp"
#include "tyr/datalog/formatter.hpp"

#include <boost/dynamic_bitset.hpp>
#include <cstddef>
#include <iostream>
#include <vector>

namespace tyr::datalog::kpkc
{
struct Vertex
{
    uint_t index;

    constexpr Vertex() noexcept : index(std::numeric_limits<uint_t>::max()) {}
    constexpr explicit Vertex(uint_t i) noexcept : index(i) {}

    friend constexpr bool operator==(Vertex lhs, Vertex rhs) noexcept { return lhs.index == rhs.index; }
};

struct Edge
{
    Vertex src;
    Vertex dst;

    constexpr Edge() noexcept : src(), dst() {}
    constexpr Edge(Vertex u, Vertex v) noexcept : src(u.index < v.index ? u : v), dst(u.index < v.index ? v : u) {}

    friend constexpr bool operator==(Edge lhs, Edge rhs) noexcept { return lhs.src == rhs.src && lhs.dst == rhs.dst; }

    /// @brief Get the rank relative to a given number of vertices.
    /// @param nv is the total number of vertices.
    /// @return is the rank of the edge.
    uint_t rank(uint_t nv) const noexcept { return src.index * nv + dst.index; }
};

struct GraphLayout
{
    /// Meta
    size_t nv;
    size_t k;
    /// Vertex partitioning with continuous vertex indices [[0,1,2],[3,4],[5,6]]
    std::vector<Vertex> partitions;
    std::vector<uint_t> vertex_to_partition;
    std::vector<uint_t> vertex_to_bit;

    struct BitsetInfo
    {
        uint_t bit_offset;  // bit offset ignoring unused bits
        uint_t num_bits;

        uint_t block_offset;
        uint_t num_blocks;
    };

    struct PartitionInfo
    {
        std::vector<BitsetInfo> infos;
        size_t num_blocks;
    };

    PartitionInfo info;

    GraphLayout(const StaticConsistencyGraph& static_graph);
};

struct GraphActivityMasks
{
    boost::dynamic_bitset<> vertices;
    boost::dynamic_bitset<> edges;

    explicit GraphActivityMasks(const StaticConsistencyGraph& static_graph);

    void reset() noexcept
    {
        vertices.set();
        edges.set();
    }
};

struct Graph
{
    std::reference_wrapper<const GraphLayout> cg;

    boost::dynamic_bitset<> vertices;

    std::vector<uint64_t> partition_vertices_data;

    std::vector<uint64_t> partition_adjacency_matrix_data;
    MDSpan<uint64_t, 2> partition_adjacency_matrix_span;

    explicit Graph(const GraphLayout& cg);

    void reset_vertices() noexcept
    {
        vertices.reset();
        partition_vertices_data.assign(partition_vertices_data.size(), 0);
    }

    void reset_adjacency_row(uint_t v) noexcept
    {
        auto adj_list_row = partition_adjacency_matrix_span(v);
        std::fill(adj_list_row.data(), adj_list_row.data() + adj_list_row.size(), uint64_t { 0 });
    }

    void reset() noexcept
    {
        vertices.reset();
        partition_vertices_data.assign(partition_vertices_data.size(), 0);
        partition_adjacency_matrix_data.assign(partition_adjacency_matrix_data.size(), 0);
    }

    template<typename Callback>
    void for_each_vertex(Callback&& callback) const
    {
        auto offset = uint_t(0);
        for (uint_t p = 0; p < cg.get().k; ++p)
        {
            const auto& info = cg.get().info.infos[p];
            auto bits = BitsetSpan<const uint64_t>(partition_vertices_data.data() + info.block_offset, info.num_bits);

            for (auto bit = bits.find_first(); bit != BitsetSpan<const uint64_t>::npos; bit = bits.find_next(bit))
            {
                assert(vertices.test(offset + bit));
                callback(Vertex(offset + bit));
            }

            offset += info.num_bits;
        }
    }

    template<typename Callback>
    void for_each_edge(Callback&& callback) const
    {
        auto src_offset = uint_t(0);
        for (uint_t src_p = 0; src_p < cg.get().k; ++src_p)
        {
            const auto& src_info = cg.get().info.infos[src_p];
            auto src_bits = BitsetSpan<const uint64_t>(partition_vertices_data.data() + src_info.block_offset, src_info.num_bits);

            for (auto src_bit = src_bits.find_first(); src_bit != BitsetSpan<const uint64_t>::npos; src_bit = src_bits.find_next(src_bit))
            {
                const auto src_index = src_offset + src_bit;
                assert(vertices.test(src_index));
                const auto src = Vertex(src_index);

                const auto adjacency_list = partition_adjacency_matrix_span(src_index);
                auto dst_offset = src_offset + src_info.num_bits;

                for (uint_t dst_p = src_p + 1; dst_p < cg.get().k; ++dst_p)
                {
                    const auto& dst_info = cg.get().info.infos[dst_p];
                    auto dst_bits = BitsetSpan<const uint64_t>(partition_vertices_data.data() + dst_info.block_offset, dst_info.num_bits);
                    auto adj_bits = BitsetSpan<const uint64_t>(adjacency_list.data() + dst_info.block_offset, dst_info.num_bits);

                    for (auto dst_bit = adj_bits.find_first(); dst_bit != BitsetSpan<const uint64_t>::npos; dst_bit = adj_bits.find_next(dst_bit))
                    {
                        if (dst_bits.test(dst_bit))
                        {
                            assert(vertices.test(dst_offset + dst_bit));
                            callback(Edge(src, Vertex(dst_offset + dst_bit)));
                        }
                    }

                    dst_offset += dst_info.num_bits;
                }
            }

            src_offset += src_info.num_bits;
        }
    }
};

}

#endif
