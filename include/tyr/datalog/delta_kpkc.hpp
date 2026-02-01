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

#ifndef TYR_DATALOG_DELTA_KPKC_HPP_
#define TYR_DATALOG_DELTA_KPKC_HPP_

#include "tyr/common/formatter.hpp"
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
    std::vector<std::vector<Vertex>> partitions;  ///< Dimensions K x V
    std::vector<uint_t> vertex_to_partition;
    std::vector<uint_t> vertex_to_bit;

    /// @brief Constructor enforces invariants.
    /// @param nv
    /// @param k
    /// @param partitions
    /// @param vertex_to_partition
    GraphLayout(size_t nv, size_t k, std::vector<std::vector<Vertex>> partitions, std::vector<uint_t> vertex_to_partition, std::vector<uint_t> vertex_to_bit);
};

struct GraphActivityMasks
{
    boost::dynamic_bitset<> vertices;
    boost::dynamic_bitset<> edges;

    void reset() noexcept
    {
        vertices.set();
        edges.set();
    }
};

struct Graph
{
    /// Vertices
    boost::dynamic_bitset<> vertices;
    std::vector<boost::dynamic_bitset<>> partition_vertices;
    /// Edges
    std::vector<std::vector<boost::dynamic_bitset<>>> partition_adjacency_matrix;

    void reset() noexcept
    {
        vertices.reset();
        for (auto& bitset : partition_vertices)
            bitset.reset();
        for (auto& vec : partition_adjacency_matrix)
            for (auto& bitset : vec)
                bitset.reset();
    }

    template<typename Callback>
    void for_each_vertex(Callback&& callback) const
    {
        auto offset = uint_t(0);

        for (uint_t p = 0; p < partition_vertices.size(); ++p)
        {
            const auto& partition = partition_vertices[p];

            for (auto bit = partition.find_first(); bit != boost::dynamic_bitset<>::npos; bit = partition.find_next(bit))
                callback(Vertex(offset + bit));

            offset += partition.size();
        }
    }

    template<typename Callback>
    void for_each_edge(Callback&& callback) const
    {
        auto src_offset = uint_t(0);

        for (uint_t src_p = 0; src_p < partition_vertices.size(); ++src_p)
        {
            const auto& src_partition = partition_vertices[src_p];

            for (auto src_bit = src_partition.find_first(); src_bit != boost::dynamic_bitset<>::npos; src_bit = src_partition.find_next(src_bit))
            {
                const auto src_index = src_offset + src_bit;
                const auto src = Vertex(src_index);

                const auto& row = partition_adjacency_matrix[src_index];
                auto dst_offset = src_offset + src_partition.size();

                for (uint_t dst_p = src_p + 1; dst_p < row.size(); ++dst_p)
                {
                    const auto& dst_partition = row[dst_p];

                    for (auto dst_bit = dst_partition.find_first(); dst_bit != boost::dynamic_bitset<>::npos; dst_bit = dst_partition.find_next(dst_bit))
                    {
                        if (partition_vertices[dst_p].test(dst_bit))
                            callback(Edge(src, Vertex(dst_offset + dst_bit)));
                    }

                    dst_offset += dst_partition.size();
                }
            }

            src_offset += src_partition.size();
        }
    }
};

/// @brief `Workspace` is preallocated memory for a rule.
struct Workspace
{
    std::vector<std::vector<boost::dynamic_bitset<>>> compatible_vertices;  ///< Dimensions K x K x O(V)
    boost::dynamic_bitset<> partition_bits;                                 ///< Dimensions K
    std::vector<Vertex> partial_solution;                                   ///< Dimensions K
    uint_t anchor_key;
    uint_t anchor_pi;
    uint_t anchor_pj;

    /// @brief Allocate workspace memory layout for a given graph layout.
    /// @param graph
    explicit Workspace(const GraphLayout& graph);
};

struct Cliques
{
    explicit Cliques(size_t k) : m_k(k), m_size(0), m_data() {}

    void append(std::span<const Vertex> clique)
    {
        assert(clique.size() == m_k);

        m_data.insert(m_data.end(), clique.begin(), clique.end());
        ++m_size;
    }

    std::span<const Vertex> operator[](size_t index) const noexcept
    {
        assert(index < m_size);

        return std::span<const Vertex>(m_data.data() + m_k * index, m_k);
    }

    void clear() noexcept
    {
        m_data.clear();
        m_size = 0;
    }

    size_t size() const noexcept { return m_size; }

    size_t m_k;
    size_t m_size;
    std::vector<Vertex> m_data;
};

class DeltaKPKC
{
public:
    /// @brief Allocate graph structures.
    /// @param static_graph is the static precondition consistency graph.
    explicit DeltaKPKC(const StaticConsistencyGraph& static_graph);

    /// @brief Complete member initialization (for testing purposes)
    DeltaKPKC(GraphLayout const_graph, Graph delta_graph, Graph full_graph);

    /// @brief Set new fact set to compute deltas.
    /// @param assignment_sets
    void set_next_assignment_sets(const StaticConsistencyGraph& static_graph, const AssignmentSets& assignment_sets);

    /// @brief Reset should be called before first iteration.
    void reset();

    /**
     * Sequential API
     */

    template<typename Callback>
    void for_each_k_clique(Callback&& callback, Workspace& workspace) const;

    template<typename Callback>
    void for_each_new_k_clique(Callback&& callback, Workspace& workspace) const;

    /**
     * Parallel API
     */

    void for_each_new_k_clique(Cliques& cliques, Workspace& workspace) const;

    /**
     * Getters
     */

    const GraphLayout& get_graph_layout() const noexcept { return m_const_graph; }
    const Graph& get_delta_graph() const noexcept { return m_delta_graph; }
    const Graph& get_full_graph() const noexcept { return m_full_graph; }
    size_t get_iteration() const noexcept { return m_iteration; }

private:
    template<typename Callback>
    void for_each_new_unary_clique(Callback&& callback, Workspace& workspace) const;

    template<typename Callback>
    void for_each_unary_clique(Callback&& callback, Workspace& workspace) const;

    template<typename Callback>
    void for_each_new_binary_clique(Callback&& callback, Workspace& workspace) const;

    template<typename Callback>
    void for_each_binary_clique(Callback&& callback, Workspace& workspace) const;

    /// @brief Seed the P part of BronKerbosch.
    ///
    /// Initialize compatible vertices at depth 0 with empty solution
    /// for each partition with the vertices that are active in the full graph.
    void seed_without_anchor(Workspace& workspace) const;

    /// @brief Seed the P part of BronKerbosch based on an anchor edge.
    ///
    /// Initialize compatible vertices at depth 0 with solution of size 2, i.e., the vertices adjacent to the anchor,
    /// for each remaining partition with the vertices that are connected to vertices adjacent to the anchor
    /// through edges that satisfy the delta constraint, i.e., such edges must have higher delta rank.
    /// @param edge is the anchor edge.
    void seed_from_anchor(const Edge& edge, Workspace& workspace) const;

    /// @brief Complete the k-clique recursively.
    /// @tparam Callback is called upon finding a k-clique.
    /// @tparam AnchorType is the type of the anchor.
    /// @param callback is the callback function.
    /// @param depth is the recursion depth.
    template<typename AnchorType, class Callback>
    void complete_from_seed(Callback&& callback, size_t depth, Workspace& workspace) const;

    /// @brief Find the pivot partition that greedily minimizes the number of recursive calls,
    /// i.e., the partition with the smallest number of candidate vertices.
    /// @param depth is the recursion depth.
    /// @return
    uint_t choose_best_partition(size_t depth, const Workspace& workspace) const;

    /// @brief Update the P part of BronKerbosch given the last selected vertex `src` at depth `depth`.
    /// @tparam AnchorType is the type of the anchor.
    /// @param src is the last selected vertex.
    /// @param depth is the recursion depth.
    template<typename AnchorType>
    void update_compatible_adjacent_vertices_at_next_depth(Vertex src, size_t depth, Workspace& workspace) const;

    /// @brief Early termination helper
    /// @param depth is the recursion depth.
    /// @return
    uint_t num_possible_additions_at_next_depth(size_t depth, const Workspace& workspace) const;

private:
    GraphLayout m_const_graph;
    Graph m_delta_graph;
    Graph m_full_graph;
    GraphActivityMasks m_read_masks;
    GraphActivityMasks m_write_masks;
    size_t m_iteration;
};

/**
 * Implementations
 */

template<typename Callback>
void DeltaKPKC::for_each_new_unary_clique(Callback&& callback, Workspace& workspace) const
{
    assert(m_const_graph.k == 1);

    m_delta_graph.for_each_vertex(
        [&](auto&& vertex)
        {
            workspace.partial_solution.clear();
            workspace.partial_solution.push_back(vertex);

            callback(workspace.partial_solution);
        });
}

template<typename Callback>
void DeltaKPKC::for_each_unary_clique(Callback&& callback, Workspace& workspace) const
{
    assert(m_const_graph.k == 1);

    m_full_graph.for_each_vertex(
        [&](auto&& vertex)
        {
            workspace.partial_solution.clear();
            workspace.partial_solution.push_back(vertex);

            callback(workspace.partial_solution);
        });
}

template<typename Callback>
void DeltaKPKC::for_each_new_binary_clique(Callback&& callback, Workspace& workspace) const
{
    assert(m_const_graph.k == 2);

    m_delta_graph.for_each_edge(
        [&](auto&& edge)
        {
            workspace.partial_solution.clear();
            workspace.partial_solution.push_back(edge.src);
            workspace.partial_solution.push_back(edge.dst);

            callback(workspace.partial_solution);
        });
}

template<typename Callback>
void DeltaKPKC::for_each_binary_clique(Callback&& callback, Workspace& workspace) const
{
    assert(m_const_graph.k == 2);

    m_full_graph.for_each_edge(
        [&](auto&& edge)
        {
            workspace.partial_solution.clear();
            workspace.partial_solution.push_back(edge.src);
            workspace.partial_solution.push_back(edge.dst);

            callback(workspace.partial_solution);
        });
}

template<typename Callback>
void DeltaKPKC::for_each_k_clique(Callback&& callback, Workspace& workspace) const
{
    const auto k = m_const_graph.k;

    if (k == 0)
    {
        workspace.partial_solution.clear();
        callback(workspace.partial_solution);
        return;
    }
    else if (k == 1)
    {
        for_each_unary_clique(callback, workspace);
    }
    else if (k == 2)
    {
        for_each_binary_clique(callback, workspace);
    }
    else
    {
        seed_without_anchor(workspace);

        complete_from_seed<void>(callback, 0, workspace);
    }
}

template<typename Callback>
void DeltaKPKC::for_each_new_k_clique(Callback&& callback, Workspace& workspace) const
{
    if (m_iteration == 1)
    {
        for_each_k_clique(callback, workspace);
    }
    else
    {
        const auto k = m_const_graph.k;

        if (k == 0)
        {
            return;
        }
        else if (k == 1)
        {
            for_each_new_unary_clique(callback, workspace);
        }
        else if (k == 2)
        {
            for_each_new_binary_clique(callback, workspace);
        }
        else
        {
            m_delta_graph.for_each_edge(
                [&](auto&& edge)
                {
                    seed_from_anchor(edge, workspace);

                    complete_from_seed<Edge>(callback, 0, workspace);
                });
        }
    }
}

template<typename AnchorType>
void DeltaKPKC::update_compatible_adjacent_vertices_at_next_depth(Vertex src, size_t depth, Workspace& workspace) const
{
    const uint_t k = m_const_graph.k;

    auto& cv_curr = workspace.compatible_vertices[depth];
    auto& cv_next = workspace.compatible_vertices[depth + 1];
    const auto& partition_bits = workspace.partition_bits;

    const auto p_src = m_const_graph.vertex_to_partition[src.index];
    assert(p_src != workspace.anchor_pi);
    assert(p_src != workspace.anchor_pj);

    for (uint_t p = 0; p < k; ++p)
    {
        auto& cv_next_p = cv_next[p];

        if (partition_bits.test(p))
            continue;

        // Copy current into next
        cv_next_p = cv_curr[p];
        // Restrict neighborhood
        cv_next_p &= m_full_graph.partition_adjacency_matrix[src.index][p];

        if constexpr (std::is_same_v<AnchorType, Edge>)
        {
            // Remove illegal delta edges whose rank is less than anchor rank
            if (p_src < workspace.anchor_pi || p < workspace.anchor_pi)
                cv_next_p -= m_delta_graph.partition_adjacency_matrix[src.index][p];
        }
    }
}

template<typename AnchorType, class Callback>
void DeltaKPKC::complete_from_seed(Callback&& callback, size_t depth, Workspace& workspace) const
{
    assert(depth < m_const_graph.k);

    const uint_t p = choose_best_partition(depth, workspace);
    if (p == std::numeric_limits<uint_t>::max())
        return;  // dead branch: no unused partition has candidates

    const uint_t k = m_const_graph.k;
    auto& cv_d_p = workspace.compatible_vertices[depth][p];
    auto& partition_bits = workspace.partition_bits;
    auto& partial_solution = workspace.partial_solution;

    // Iterate through compatible vertices in the best partition
    for (auto bit = cv_d_p.find_first(); bit != boost::dynamic_bitset<>::npos; bit = cv_d_p.find_next(bit))
    {
        const auto vertex = m_const_graph.partitions[p][bit];

        partial_solution.push_back(vertex);

        // print(std::cout, partial_solution);
        // std::cout << std::endl;

        if (partial_solution.size() == k)
        {
            callback(partial_solution);
        }
        else
        {
            update_compatible_adjacent_vertices_at_next_depth<AnchorType>(vertex, depth, workspace);

            partition_bits.set(p);

            if ((partial_solution.size() + num_possible_additions_at_next_depth(depth, workspace)) == k)
                complete_from_seed<AnchorType>(callback, depth + 1, workspace);

            partition_bits.reset(p);
        }

        partial_solution.pop_back();
    }
}
}

#endif
