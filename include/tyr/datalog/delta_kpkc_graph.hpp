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
#include "tyr/formalism/datalog/variable_dependency_graph.hpp"

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

/// @brief A compact adjacency matrix representation for k partite graphs.
///
/// Row-major adjacency lists with targets grouped by partition.
struct PartitionedAdjacencyLists
{
    PartitionedAdjacencyLists() : m_vertex_partitions(), m_data(), m_row_offsets(), m_num_edges(0), m_k(0) {}
    PartitionedAdjacencyLists(std::vector<std::vector<uint_t>> vertex_partitions) :
        m_vertex_partitions(std::move(vertex_partitions)),
        m_data(),
        m_row_offsets(),
        m_num_edges(),
        m_k(m_vertex_partitions.size())
    {
        clear();
    }

    /**
     * Construction
     */

    void clear() noexcept
    {
        m_data.clear();
        m_row_offsets.push_back(m_data.size());
        m_num_edges = 0;
    }

    void start_row(uint_t v, uint_t p) noexcept
    {
        m_row_len_pos = m_data.size();
        m_row_len = 0;
        m_data.push_back(0);
        m_data.push_back(v);
        m_data.push_back(p);
    }

    void start_partition() noexcept
    {
        m_partition_len_pos = m_data.size();
        m_data.push_back(0);
        m_partition_data_start_pos = m_data.size();
    }

    void add_target(uint_t target) noexcept
    {
        m_data.push_back(target);
        ++m_num_edges;
    }

    void finish_partition_without_edge(uint_t p)
    {
        m_data[m_partition_len_pos] = kpkc::PartitionedAdjacencyLists::RowView::FULL;
        const auto num_targets = m_vertex_partitions[p].size();
        m_row_len += num_targets;
        m_num_edges += num_targets;
    }

    void finish_partition_with_edge(uint_t p)
    {
        uint_t partition_data_end_pos = m_data.size();
        const auto num_targets = partition_data_end_pos - m_partition_data_start_pos;
        if (num_targets == m_vertex_partitions[p].size())
        {
            // Use partition reference mechanism for dense regions
            m_data[m_partition_len_pos] = kpkc::PartitionedAdjacencyLists::RowView::FULL;
            m_data.resize(m_partition_len_pos + 1);
        }
        else
        {
            m_data[m_partition_len_pos] = num_targets;
        }
        m_row_len += num_targets;
    }

    void finish_row()
    {
        m_data[m_row_len_pos] = m_row_len;
        m_row_offsets.push_back(m_data.size());
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

    class RowView
    {
    public:
        RowView(const PartitionedAdjacencyLists& matrix, uint_t row, uint_t len, uint_t v, uint_t p, std::span<const uint_t> data) noexcept :
            m_matrix(matrix),
            m_row(row),
            m_len(len),
            m_v(v),
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
        uint_t len() const noexcept { return m_len; }
        uint_t v() const noexcept { return m_v; }
        uint_t p() const noexcept { return m_p; }

    private:
        const PartitionedAdjacencyLists& m_matrix;
        uint_t m_row;
        uint_t m_len;
        uint_t m_v;
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

            const uint_t len = m_data[start];
            const uint_t v = m_data[start + 1];
            const uint_t p = m_data[start + 2];
            const uint_t data_start = start + 3;

            assert(data_start <= end);  ///< start must be equal to end if there is no target

            callback(RowView(*this, row, len, v, p, std::span<const uint_t>(m_data.data() + data_start, end - data_start)));
        }
    }

    const auto& vertex_partitions() const noexcept { return m_vertex_partitions; }
    const auto& data() const noexcept { return m_data; }
    const auto& row_offsets() const noexcept { return m_row_offsets; }
    auto num_edges() const noexcept { return m_num_edges; }
    auto k() const noexcept { return m_k; }

private:
    std::vector<std::vector<uint_t>> m_vertex_partitions;
    std::vector<uint_t> m_data;
    std::vector<uint_t> m_row_offsets;
    uint_t m_num_edges;
    uint_t m_k;

    uint_t m_row_len_pos;
    uint_t m_row_len;
    uint_t m_partition_len_pos;
    uint_t m_partition_data_start_pos;
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

class VertexPartitions
{
public:
    explicit VertexPartitions(const GraphLayout& layout) : m_layout(layout), m_data(layout.info.num_blocks, 0) {}

    void reset() noexcept { std::memset(m_data.data(), 0, m_data.size() * sizeof(uint64_t)); }

    auto& data() noexcept { return m_data; }
    const auto& data() const noexcept { return m_data; }

private:
    const GraphLayout& m_layout;

    /// Implicit storage: the active vertices in the partition
    std::vector<uint64_t> m_data;
};

class PartitionedAdjacencyMatrix
{
public:
    PartitionedAdjacencyMatrix(const GraphLayout& layout,
                               const VertexPartitions& partitions,
                               const VertexPartitions& consistent,
                               const std::vector<std::vector<uint_t>>& static_partitions,
                               const formalism::datalog::VariableDependencyGraph& dependency_graph) :
        m_layout(layout),
        m_partitions(partitions),
        m_consistent(consistent),
        m_adj_data(m_layout.get().nv * m_layout.get().k, Cell { Cell::Mode::IMPLICIT, std::numeric_limits<uint_t>::max() }),
        m_adj_span(m_adj_data.data(), std::array<size_t, 2> { m_layout.get().nv, m_layout.get().k }),
        m_bitset_data()
    {
        for (uint_t pi = 0; pi < m_layout.get().k; ++pi)
        {
            for (uint_t v : static_partitions[pi])
            {
                for (uint_t pj = 0; pj < m_layout.get().k; ++pj)
                {
                    auto& cell = m_adj_span(v, pj);

                    // ADJ represents upper triangle only
                    auto ppi = pi;
                    auto ppj = pj;
                    if (ppi > ppj)
                        std::swap(ppi, ppj);

                    if (ppi < ppj && dependency_graph.get_adj_matrix().get_cell(formalism::ParameterIndex(ppi), formalism::ParameterIndex(ppj)).empty())
                    {
                        cell.mode = Cell::Mode::IMPLICIT;
                        cell.offset = std::numeric_limits<uint_t>::max();  // unused
                    }
                    else
                    {
                        cell.mode = Cell::Mode::EXPLICIT;
                        cell.offset = m_bitset_data.size();

                        const auto& info = m_layout.get().info.infos[pj];
                        m_bitset_data.resize(m_bitset_data.size() + info.num_blocks);
                    }
                }
            }
        }
    }

    auto get_bitset(uint_t v, uint_t p) noexcept
    {
        assert(m_adj_span(v, p).mode == Cell::Mode::EXPLICIT);

        const auto& cell = m_adj_span(v, p);
        const auto& info = m_layout.get().info.infos[p];

        return BitsetSpan<uint64_t>(m_bitset_data.data() + cell.offset, info.num_bits);
    }

    auto get_bitset(uint_t v, uint_t p) const noexcept
    {
        const auto& cell = m_adj_span(v, p);
        const auto& info = m_layout.get().info.infos[p];

        if (cell.mode == Cell::Mode::EXPLICIT)
        {
            return BitsetSpan<const uint64_t>(m_bitset_data.data() + m_adj_span(v, p).offset, info.num_bits);
        }
        else
        {
            assert(cell.mode == Cell::Mode::IMPLICIT);
            // TODO: if v is new, then return
            // return BitsetSpan<const uint64_t>(m_partitions.get().data().data() + info.block_offset, info.num_bits);
            // else
            return BitsetSpan<const uint64_t>(m_partitions.get().data().data() + info.block_offset, info.num_bits);
        }
    }

    struct Cell
    {
        enum class Mode
        {
            EXPLICIT = 0,
            IMPLICIT = 1,
        };

        Mode mode;
        uint_t offset;
    };

    void copy_from(const PartitionedAdjacencyMatrix& other) noexcept
    {
        for (uint_t v = 0; v < m_layout.get().nv; ++v)
        {
            for (uint_t p = 0; p < m_layout.get().k; ++p)
            {
                auto& cell = m_adj_span(v, p);

                assert(cell.mode == other.m_adj_span(v, p).mode);

                if (cell.mode == PartitionedAdjacencyMatrix::Cell::Mode::IMPLICIT)
                    continue;

                get_bitset(v, p).copy_from(other.get_bitset(v, p));
            }
        }
    }

    void diff_from(const PartitionedAdjacencyMatrix& other) noexcept
    {
        for (uint_t v = 0; v < m_layout.get().nv; ++v)
        {
            for (uint_t p = 0; p < m_layout.get().k; ++p)
            {
                auto& cell = m_adj_span(v, p);

                assert(cell.mode == other.m_adj_span(v, p).mode);

                if (cell.mode == PartitionedAdjacencyMatrix::Cell::Mode::IMPLICIT)
                    continue;

                get_bitset(v, p).diff_from(other.get_bitset(v, p));
            }
        }
    }

    template<typename Callback>
    void for_each_vertex(Callback&& callback) const noexcept
    {
        auto offset = uint_t(0);

        for (uint_t p = 0; p < m_layout.get().k; ++p)
        {
            const auto& info = m_layout.get().info.infos[p];
            auto partition = BitsetSpan<const uint64_t>(m_partitions.get().data().data() + info.block_offset, info.num_bits);

            for (auto bit = partition.find_first(); bit != BitsetSpan<const uint64_t>::npos; bit = partition.find_next(bit))
            {
                const uint_t v = offset + static_cast<uint_t>(bit);

                callback(Vertex(v));
            }

            offset += info.num_bits;
        }
    }

    template<typename Callback>
    void for_each_edge(Callback&& callback) const noexcept
    {
        uint_t src_offset = 0;

        for (uint_t pi = 0; pi < m_layout.get().k; ++pi)
        {
            const auto& info_i = m_layout.get().info.infos[pi];
            auto src_bits = BitsetSpan<const uint64_t>(m_partitions.get().data().data() + info_i.block_offset, info_i.num_bits);

            for (auto bi = src_bits.find_first(); bi != BitsetSpan<const uint64_t>::npos; bi = src_bits.find_next(bi))
            {
                const uint_t vi = src_offset + static_cast<uint_t>(bi);

                uint_t dst_offset = src_offset + info_i.num_bits;

                for (uint_t pj = pi + 1; pj < m_layout.get().k; ++pj)
                {
                    const auto& info_j = m_layout.get().info.infos[pj];

                    auto dst_active = BitsetSpan<const uint64_t>(m_partitions.get().data().data() + info_j.block_offset, info_j.num_bits);

                    auto adj = get_bitset(vi, pj);

                    for (auto bj = adj.find_first(); bj != BitsetSpan<const uint64_t>::npos; bj = adj.find_next(bj))
                    {
                        if (!dst_active.test(bj))
                            continue;

                        const uint_t vj = dst_offset + static_cast<uint_t>(bj);

                        callback(Edge(Vertex(vi), Vertex(vj)));
                    }

                    dst_offset += info_j.num_bits;
                }
            }

            src_offset += info_i.num_bits;
        }
    }

    void reset() noexcept { std::memset(m_bitset_data.data(), 0, m_bitset_data.size() * sizeof(uint64_t)); }

    const auto& get_cell(uint_t v, uint_t p) const noexcept { return m_adj_span(v, p); }

    const auto& layout() const noexcept { return m_layout.get(); }
    const auto& partitions() const noexcept { return m_partitions.get(); }
    const auto& adj_data() const noexcept { return m_adj_data; }
    auto adj_span() const noexcept { return m_adj_span; }

    const auto& bitset_data() const noexcept { return m_bitset_data; }

private:
    std::reference_wrapper<const GraphLayout> m_layout;
    std::reference_wrapper<const VertexPartitions> m_partitions;
    std::reference_wrapper<const VertexPartitions> m_consistent;

    /// k x k matrix where each cell refers to a bitset either stored explicitly or referring implicitly to a vertex partition.
    std::vector<Cell> m_adj_data;
    MDSpan<Cell, 2> m_adj_span;

    /// Explicit storage
    std::vector<uint64_t> m_bitset_data;
};

struct Graph2
{
    Graph2(const GraphLayout& layout,
           const std::vector<std::vector<uint_t>>& static_partitions,
           const formalism::datalog::VariableDependencyGraph& dependency_graph) :
        partitions(layout),
        consistent(layout),
        matrix(layout, partitions, consistent, static_partitions, dependency_graph)
    {
    }

    void reset() noexcept
    {
        partitions.reset();
        consistent.reset();
        matrix.reset();
    }

    VertexPartitions partitions;
    VertexPartitions consistent;
    PartitionedAdjacencyMatrix matrix;
};

}

#endif
