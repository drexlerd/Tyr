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

/// @brief A compact adjacency matrix representation for k partite graphs.
///
/// Row-major adjacency lists with targets grouped by partition.
struct PartitionedAdjacencyMatrix
{
    PartitionedAdjacencyMatrix() : m_vertex_partitions(), m_data(), m_row_offsets(), m_num_edges(0), m_k(0) {}
    PartitionedAdjacencyMatrix(std::vector<std::vector<uint_t>> vertex_partitions,
                               std::vector<uint_t> data,
                               std::vector<uint_t> row_offsets,
                               uint_t num_edges,
                               uint_t k) :
        m_vertex_partitions(std::move(vertex_partitions)),
        m_data(std::move(data)),
        m_row_offsets(std::move(row_offsets)),
        m_num_edges(num_edges),
        m_k(k)
    {
    }

    class PartitionView
    {
    public:
        PartitionView(uint_t p, std::span<const uint_t> data) noexcept : m_p(p), m_data(data) {}

        template<typename Callback>
        void for_each_target(Callback&& callback) const
        {
            for (const auto target : m_data)
                callback(target);
        }

        uint_t p() const noexcept { return m_p; }

    private:
        uint_t m_p;
        std::span<const uint_t> m_data;
    };

    struct RowView
    {
    public:
        RowView(const PartitionedAdjacencyMatrix& matrix, uint_t row, uint_t p, std::span<const uint_t> data) noexcept :
            m_matrix(matrix),
            m_row(row),
            m_p(p),
            m_data(data)
        {
        }

        static constexpr uint_t FULL = std::numeric_limits<uint_t>::max();

        template<typename Callback>
        void for_each_partition(Callback&& callback) const
        {
            uint_t i = 0;
            uint_t p = m_p;

            while (i < m_data.size())
            {
                const uint_t len = m_data[i++];
                if (len == FULL)
                {
                    const auto& vertices = m_matrix.vertex_partitions()[p];
                    callback(PartitionView(p, std::span<const uint_t>(vertices.data(), vertices.size())));
                }
                else
                {
                    const uint_t start = i;
                    const uint_t end = i + len;

                    assert(end <= m_data.size());

                    callback(PartitionView(p, std::span<const uint_t>(m_data.data() + start, len)));

                    i = end;
                }
                ++p;
            }

            assert(p == m_matrix.k());
        }

        uint_t row() const noexcept { return m_row; }

    private:
        const PartitionedAdjacencyMatrix& m_matrix;
        uint_t m_row;
        uint_t m_p;
        std::span<const uint_t> m_data;
    };

    template<typename Callback>
    void for_each_row(Callback&& callback) const
    {
        assert(!m_row_offsets.empty());
        assert(m_row_offsets.back() == m_data.size());

        const uint_t nv = static_cast<uint_t>(m_row_offsets.size() - 1);

        for (uint_t row = 0; row < nv; ++row)
        {
            const uint_t start = m_row_offsets[row];
            const uint_t end = m_row_offsets[row + 1];

            assert(start <= end);
            assert(end <= m_data.size());

            if (start == end)
                continue;

            const uint_t p = m_data[start];
            const uint_t data_start = start + 1;

            assert(data_start <= end);  ///< start must be equal to end if there is no target

            callback(RowView(*this, row, p, std::span<const uint_t>(m_data.data() + data_start, end - data_start)));
        }
    }

    const std::vector<std::vector<uint_t>>& vertex_partitions() const noexcept { return m_vertex_partitions; }
    uint_t num_edges() const noexcept { return m_num_edges; }
    uint_t k() const noexcept { return m_k; }

private:
    std::vector<std::vector<uint_t>> m_vertex_partitions;
    std::vector<uint_t> m_data;
    std::vector<uint_t> m_row_offsets;
    uint_t m_num_edges;
    uint_t m_k;
};

}

#endif
