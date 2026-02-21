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

#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/common/formatter.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/datalog/declarations.hpp"
#include "tyr/datalog/delta_kpkc_graph.hpp"
#include "tyr/datalog/formatter.hpp"
#include "tyr/datalog/statistics/rule.hpp"

#include <boost/dynamic_bitset.hpp>
#include <cstddef>
#include <iostream>
#include <vector>

namespace tyr::datalog::kpkc
{

/// @brief `Workspace` is preallocated memory for a rule.
struct Workspace
{
    std::vector<uint64_t> compatible_vertices_data;
    MDSpan<uint64_t, 2> compatible_vertices_span;  ///< Dimensions K x K x O(V)

    boost::dynamic_bitset<> partition_bits;  ///< Dimensions K
    std::vector<Vertex> partial_solution;    ///< Dimensions K
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

    /// @brief Set new fact set to compute deltas.
    /// @param assignment_sets
    void set_next_assignment_sets(const StaticConsistencyGraph& static_graph,
                                  const TaggedFactSets<formalism::FluentTag>& delta_fact_sets,
                                  const AssignmentSets& assignment_sets);

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

    const GraphLayout& get_graph_layout() const noexcept { return m_layout; }
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
    bool seed_from_anchor(const Edge& edge, Workspace& workspace) const;

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
    bool update_compatible_adjacent_vertices_at_next_depth(Vertex src, size_t depth, Workspace& workspace) const;

private:
    GraphLayout m_layout;
    size_t m_iteration;

    Graph m_delta_graph;
    Graph m_full_graph;

    VertexPartitions m_fact_induced_candidates;

    DirtyPartitions m_dirty_partitions;
};

/**
 * Implementations
 */

template<typename Callback>
void DeltaKPKC::for_each_new_unary_clique(Callback&& callback, Workspace& workspace) const
{
    assert(m_layout.k == 1);

    m_delta_graph.matrix.for_each_vertex(
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
    assert(m_layout.k == 1);

    m_full_graph.matrix.for_each_vertex(
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
    assert(m_layout.k == 2);

    m_delta_graph.matrix.for_each_edge(
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
    assert(m_layout.k == 2);

    m_full_graph.matrix.for_each_edge(
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
    const auto k = m_layout.k;

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
        const auto k = m_layout.k;

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
            m_delta_graph.matrix.for_each_edge(
                [&](auto&& edge)
                {
                    if (seed_from_anchor(edge, workspace))
                        complete_from_seed<Edge>(callback, 0, workspace);
                });
        }
    }
}

template<typename AnchorType>
bool DeltaKPKC::update_compatible_adjacent_vertices_at_next_depth(Vertex src, size_t depth, Workspace& workspace) const
{
    const uint_t k = m_layout.k;

    const auto& partition_bits = workspace.partition_bits;

    const auto p_src = m_layout.vertex_to_partition[src.index];
    assert(p_src != workspace.anchor_pi);
    assert(p_src != workspace.anchor_pj);

    const auto cv_curr = workspace.compatible_vertices_span(depth);
    auto cv_next = workspace.compatible_vertices_span(depth + 1);

    for (uint_t p = 0; p < k; ++p)
    {
        if (partition_bits.test(p))
            continue;

        const auto& info = m_layout.info.infos[p];
        auto src_cur = BitsetSpan<const uint64_t>(cv_curr.data() + info.block_offset, info.num_bits);
        auto dst_next = BitsetSpan<uint64_t>(cv_next.data() + info.block_offset, info.num_bits);
        auto src_full = m_full_graph.matrix.get_bitset(src.index, p);

        dst_next.copy_from(src_cur);
        dst_next &= src_full;

        if (!dst_next.any())
            return false;

        if constexpr (std::is_same_v<AnchorType, Edge>)
        {
            // Remove illegal delta edges whose rank is less than anchor rank

            if (p_src < workspace.anchor_pi || p < workspace.anchor_pi)
            {
                auto src_delta = m_delta_graph.matrix.get_bitset(src.index, p);

                dst_next -= src_delta;

                if (!dst_next.any())
                    return false;
            }
        }
    }
    return true;
}

template<typename AnchorType, class Callback>
void DeltaKPKC::complete_from_seed(Callback&& callback, size_t depth, Workspace& workspace) const
{
    assert(depth < m_layout.k);

    const uint_t p = choose_best_partition(depth, workspace);
    if (p == std::numeric_limits<uint_t>::max())
        return;  // dead branch: no unused partition has candidates

    const uint_t k = m_layout.k;

    auto& partition_bits = workspace.partition_bits;
    auto& partial_solution = workspace.partial_solution;
    const auto& info = m_layout.info.infos[p];
    const auto cv_d_p = BitsetSpan<const uint64_t>(workspace.compatible_vertices_span(depth).data() + info.block_offset, info.num_bits);

    // Iterate through compatible vertices in the best partition
    for (auto bit = cv_d_p.find_first(); bit != BitsetSpan<const uint64_t>::npos; bit = cv_d_p.find_next(bit))
    {
        const auto vertex = Vertex(info.bit_offset + bit);

        partial_solution.push_back(vertex);

        // print(std::cout, partial_solution);
        // std::cout << std::endl;

        if (partial_solution.size() == k)
        {
            callback(partial_solution);
        }
        else
        {
            partition_bits.set(p);

            if (update_compatible_adjacent_vertices_at_next_depth<AnchorType>(vertex, depth, workspace))
                complete_from_seed<AnchorType>(callback, depth + 1, workspace);

            partition_bits.reset(p);
        }

        partial_solution.pop_back();
    }
}
}

#endif
