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

struct GraphLayout
{
    /// Meta
    size_t nv;
    size_t k;

    /// Vertex partitioning
    std::vector<std::vector<uint_t>> vertex_partitions;
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

    GraphLayout() = default;
    GraphLayout(size_t nv, const std::vector<std::vector<uint_t>>& vertex_partitions);
};

class VertexPartitions
{
public:
    explicit VertexPartitions(const GraphLayout& layout) : m_layout(layout), m_data(layout.info.num_blocks, 0) {}

    void reset() noexcept { std::memset(m_data.data(), 0, m_data.size() * sizeof(uint64_t)); }

    auto get_bitset(const GraphLayout::BitsetInfo& info) noexcept { return BitsetSpan<uint64_t>(m_data.data() + info.block_offset, info.num_bits); }

    auto get_bitset(const GraphLayout::BitsetInfo& info) const noexcept { return BitsetSpan<const uint64_t>(m_data.data() + info.block_offset, info.num_bits); }

    auto get_bitset(uint_t p) noexcept { return get_bitset(m_layout.info.infos[p]); }

    auto get_bitset(uint_t p) const noexcept { return get_bitset(m_layout.info.infos[p]); }

    auto& data() noexcept { return m_data; }
    const auto& data() const noexcept { return m_data; }
    const auto& layout() const noexcept { return m_layout; }

private:
    const GraphLayout& m_layout;

    /// Implicit storage: the active vertices in the partition
    std::vector<uint64_t> m_data;
};

class AdjacencyMatrix
{
public:
    AdjacencyMatrix(const GraphLayout& layout) : m_layout(layout), m_bitset_data(layout.nv * layout.k * layout.info.num_blocks) {}

    auto get_row(uint_t v) const noexcept
    {
        const auto& info = m_layout.info;
        const auto row_offset = info.num_blocks * v;
        return std::span<const uint64_t>(m_bitset_data.data() + row_offset, info.num_blocks);
    }

    auto get_bitset(uint_t v, uint_t p) noexcept
    {
        const auto row_offset = m_layout.info.num_blocks * v;
        const auto& info = m_layout.info.infos[p];
        return BitsetSpan<uint64_t>(m_bitset_data.data() + row_offset + info.block_offset, info.num_bits);
    }

    auto get_bitset(uint_t v, uint_t p) const noexcept
    {
        const auto row_offset = m_layout.info.num_blocks * v;
        const auto& info = m_layout.info.infos[p];
        return BitsetSpan<const uint64_t>(m_bitset_data.data() + row_offset + info.block_offset, info.num_bits);
    }

    const auto& layout() const noexcept { return m_layout; }

    const auto& bitset_data() const noexcept { return m_bitset_data; }

private:
    GraphLayout m_layout;

    std::vector<uint64_t> m_bitset_data;
};

class DeduplicatedAdjacencyMatrix
{
public:
    DeduplicatedAdjacencyMatrix(const GraphLayout& layout) : m_layout(layout), m_row_offset(), m_row_data(), m_bitset_data() {}

    DeduplicatedAdjacencyMatrix(const AdjacencyMatrix& m) : m_layout(m.layout())
    {
        auto row_to_offset = UnorderedMap<std::span<const uint64_t>, uint_t> {};
        auto partition_to_offset = UnorderedMap<BitsetSpan<const uint64_t>, uint_t> {};

        for (uint_t v = 0; v < m.layout().nv; ++v)
        {
            const auto [it1, success1] = row_to_offset.emplace(m.get_row(v), m_row_data.size());

            if (!success1)
            {
                m_row_offset.push_back(it1->second);
                continue;  ///< succeeded row deduplication
            }
            m_row_offset.push_back(it1->second);  /// Build new row

            for (uint_t k = 0; k < m.layout().k; ++k)
            {
                const auto b = m.get_bitset(v, k);
                const auto [it2, success2] = partition_to_offset.emplace(b, m_bitset_data.size());

                if (!success2)
                {
                    m_row_data.push_back(it2->second);
                    continue;  ///< succeeded partition deduplication
                }

                m_row_data.push_back(it2->second);  /// Build new bitset

                /// New bitset encountered
                m_bitset_data.insert(m_bitset_data.end(), b.blocks().begin(), b.blocks().end());
            }
        }
    }

    auto get_bitset(uint_t v, uint_t p) const noexcept
    {
        const auto& info = m_layout.info.infos[p];

        assert(v < m_row_offset.size());
        assert(m_row_offset[v] + p < m_row_data.size());
        const auto start = m_row_data[m_row_offset[v] + p];

        return BitsetSpan<const uint64_t>(m_bitset_data.data() + start, info.num_bits);
    }

    const auto& layout() const noexcept { return m_layout; }
    const auto& row_offset() const noexcept { return m_row_offset; }
    const auto& row_data() const noexcept { return m_row_data; }
    const auto& bitset_data() const noexcept { return m_bitset_data; }

private:
    GraphLayout m_layout;

    std::vector<uint_t> m_row_offset;  // m_row_offset[v] is the offset into m_row_data
    std::vector<uint_t> m_row_data;

    std::vector<uint64_t> m_bitset_data;
};

class PartitionedAdjacencyMatrix
{
public:
    PartitionedAdjacencyMatrix(const GraphLayout& layout,
                               const VertexPartitions& affected_partitions,
                               const VertexPartitions& delta_partitions,
                               const std::vector<std::vector<uint_t>>& static_partitions,
                               const formalism::datalog::VariableDependencyGraph& dependency_graph) :
        m_layout(layout),
        m_affected_partitions(affected_partitions),
        m_delta_partitions(delta_partitions),
        m_adj_data(m_layout.get().nv * m_layout.get().k, Cell { Cell::Mode::IMPLICIT, std::numeric_limits<uint_t>::max() }),
        m_adj_span(m_adj_data.data(), std::array<size_t, 2> { m_layout.get().nv, m_layout.get().k }),
        m_touched_partitions(m_layout.get().nv * m_layout.get().k, false),
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

        if (cell.is_explicit())
        {
            return BitsetSpan<const uint64_t>(m_bitset_data.data() + cell.offset, info.num_bits);
        }
        else
        {
            assert(cell.mode == Cell::Mode::IMPLICIT);
            const auto pv = m_layout.get().vertex_to_partition[v];
            const auto bit = m_layout.get().vertex_to_bit[v];
            const auto& info_v = m_layout.get().info.infos[pv];
            const auto consistent_v = m_delta_partitions.get().get_bitset(info_v);

            return consistent_v.test(bit) ? m_affected_partitions.get().get_bitset(info) : m_delta_partitions.get().get_bitset(info);
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

        bool is_explicit() const noexcept { return mode == Mode::EXPLICIT; }
        bool is_implicit() const noexcept { return mode == Mode::IMPLICIT; }
    };

    template<typename Callback>
    void for_each_vertex(Callback&& callback) const noexcept
    {
        auto offset = uint_t(0);

        for (uint_t p = 0; p < m_layout.get().k; ++p)
        {
            const auto& info = m_layout.get().info.infos[p];
            auto partition = m_affected_partitions.get().get_bitset(info);

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
            auto src_bits = m_affected_partitions.get().get_bitset(info_i);

            for (auto bi = src_bits.find_first(); bi != BitsetSpan<const uint64_t>::npos; bi = src_bits.find_next(bi))
            {
                const uint_t vi = src_offset + static_cast<uint_t>(bi);

                uint_t dst_offset = src_offset + info_i.num_bits;

                for (uint_t pj = pi + 1; pj < m_layout.get().k; ++pj)
                {
                    const auto& info_j = m_layout.get().info.infos[pj];

                    auto dst_active = m_affected_partitions.get().get_bitset(info_j);

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

    void reset() noexcept
    {
        for (auto t = m_touched_partitions.find_first(); t != boost::dynamic_bitset<>::npos; t = m_touched_partitions.find_next(t))
        {
            const auto v = t / m_layout.get().k;
            const auto p = t % m_layout.get().k;

            get_bitset(v, p).reset();
        }

        m_touched_partitions.reset();
    }

    const auto& get_cell(uint_t v, uint_t p) const noexcept { return m_adj_span(v, p); }

    const auto& layout() const noexcept { return m_layout.get(); }
    const auto& affected_partitions() const noexcept { return m_affected_partitions.get(); }
    const auto& delta_partitions() const noexcept { return m_delta_partitions.get(); }
    const auto& touched_partitions() const noexcept { return m_touched_partitions; }
    auto touched_partitions(uint_t v, uint_t p) noexcept { return m_touched_partitions[v * m_layout.get().k + p]; }
    const auto& adj_data() const noexcept { return m_adj_data; }
    auto adj_span() const noexcept { return m_adj_span; }

    const auto& bitset_data() const noexcept { return m_bitset_data; }

private:
    std::reference_wrapper<const GraphLayout> m_layout;
    std::reference_wrapper<const VertexPartitions> m_affected_partitions;
    std::reference_wrapper<const VertexPartitions> m_delta_partitions;

    /// v x k matrix where each cell refers to a bitset either stored explicitly or referring implicitly to a vertex partition.
    std::vector<Cell> m_adj_data;
    MDSpan<Cell, 2> m_adj_span;

    /// v x k bitset to track touched cell bitsets
    boost::dynamic_bitset<> m_touched_partitions;

    /// Explicit storage
    std::vector<uint64_t> m_bitset_data;
};

struct Graph
{
    Graph(const GraphLayout& layout,
          const std::vector<std::vector<uint_t>>& static_partitions,
          const formalism::datalog::VariableDependencyGraph& dependency_graph) :
        affected_partitions(layout),
        delta_partitions(layout),
        matrix(layout, affected_partitions, delta_partitions, static_partitions, dependency_graph)
    {
    }

    void reset() noexcept
    {
        affected_partitions.reset();
        delta_partitions.reset();
        matrix.reset();
    }

    VertexPartitions affected_partitions;
    VertexPartitions delta_partitions;
    PartitionedAdjacencyMatrix matrix;
};

}

#endif
