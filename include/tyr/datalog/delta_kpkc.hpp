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

#ifndef TYR_DATALOG_KPKC_HPP_
#define TYR_DATALOG_KPKC_HPP_

#include "tyr/common/formatter.hpp"
#include "tyr/datalog/declarations.hpp"

#include <boost/dynamic_bitset.hpp>
#include <cstddef>
#include <iostream>
#include <vector>

namespace tyr::datalog
{
namespace delta_kpkc
{
struct ConstGraph
{
    /// Meta
    size_t num_vertices;
    size_t k;
    /// Vertex partitioning
    std::vector<std::vector<uint_t>> partitions;  ///< Dimensions K x V
    std::vector<uint_t> vertex_to_partition;
};

struct Graph
{
    /// Vertices
    boost::dynamic_bitset<> vertices;  ///< Dimensions V
    /// Edges
    std::vector<boost::dynamic_bitset<>> adjacency_matrix;  ///< Dimensions V x V

    void reset() noexcept
    {
        vertices.reset();
        for (auto& bitset : adjacency_matrix)
            bitset.reset();
    }
};

/// @brief `Workspace` is preallocated memory for a rule.
struct Workspace
{
    std::vector<std::vector<boost::dynamic_bitset<>>> compatible_vertices;  ///< Dimensions K x K x V
    boost::dynamic_bitset<> partition_bits;                                 ///< Dimensions K
    std::vector<uint_t> partial_solution;                                   ///< Dimensions K
    uint_t anchor_edge_rank;
};
}

class DeltaKPKC
{
public:
    DeltaKPKC(const StaticConsistencyGraph& static_graph);

    /// @brief Complete member initialization (for testing)
    DeltaKPKC(delta_kpkc::ConstGraph const_graph, delta_kpkc::Graph delta_graph, delta_kpkc::Graph full_graph, delta_kpkc::Workspace workspace);

    /// @brief Set delta to current adjacency matrix, recompute current adjacency matrix, transform delta to contain the difference.
    /// @param assignment_sets
    void set_next_assignment_sets(const StaticConsistencyGraph& static_graph, const AssignmentSets& assignment_sets);

    /// @brief Reset should be called before solving for a program.
    void reset()
    {
        m_delta_graph.reset();
        m_full_graph.reset();
    }

    template<typename Callback>
    void for_each_new_k_clique(Callback&& callback)
    {
        assert(m_const_graph.k >= 2);

        for (uint_t i = 0; i < m_const_graph.num_vertices; ++i)
        {
            const auto& row = m_delta_graph.adjacency_matrix[i];

            for (auto j = row.find_next(i); j != boost::dynamic_bitset<>::npos; j = row.find_next(j))
            {
                assert(m_full_graph.vertices.test(i));
                assert(m_full_graph.vertices.test(j));

                // std::cout << "Anchor: " << i << " " << j << std::endl;

                // Special case
                if (m_const_graph.k == 2)
                {
                    m_workspace.partial_solution.clear();
                    m_workspace.partial_solution.push_back(i);
                    m_workspace.partial_solution.push_back(j);
                    callback(m_workspace.partial_solution);
                    continue;
                }

                seed_from_anchor(i, j);

                complete_from_seed(callback, 0);
            }
        }
    }

    const delta_kpkc::ConstGraph& get_const_graph() const noexcept { return m_const_graph; }
    const delta_kpkc::Graph& get_delta_graph() const noexcept { return m_delta_graph; }
    const delta_kpkc::Graph& get_full_graph() const noexcept { return m_full_graph; }

private:
    uint_t edge_rank(uint_t u, uint_t v) const
    {
        const uint_t a = std::min(u, v);
        const uint_t b = std::max(u, v);
        return a * m_const_graph.num_vertices + b;
    }

    bool is_delta_edge(uint_t u, uint_t v) const
    {
        const uint_t a = std::min(u, v);
        const uint_t b = std::max(u, v);
        return m_delta_graph.adjacency_matrix[a].test(b);
    }

    bool is_vertex_compatible_with_anchor(uint_t u, uint_t v, uint_t vertex, uint_t anchor_edge_rank) const
    {
        return (m_full_graph.vertices.test(vertex)                                                     //
                && m_full_graph.adjacency_matrix[u].test(vertex)                                       //
                && m_full_graph.adjacency_matrix[v].test(vertex)                                       //
                && (!is_delta_edge(u, vertex) || edge_rank(u, vertex) > m_workspace.anchor_edge_rank)  //
                && (!is_delta_edge(v, vertex) || edge_rank(v, vertex) > m_workspace.anchor_edge_rank));
    }

    void seed_from_anchor(uint_t i, uint_t j)
    {
        // Reset workspace state for this anchored run
        m_workspace.partial_solution.clear();
        m_workspace.partial_solution.push_back(i);
        m_workspace.partial_solution.push_back(j);
        m_workspace.anchor_edge_rank = edge_rank(i, j);

        const uint_t pi = m_const_graph.vertex_to_partition[i];
        const uint_t pj = m_const_graph.vertex_to_partition[j];
        assert(pi != pj);

        m_workspace.partition_bits.reset();
        m_workspace.partition_bits.set(pi);
        m_workspace.partition_bits.set(pj);

        // Seed candidates for each partition
        auto& compatible_vertices_0 = m_workspace.compatible_vertices[0];
        for (uint_t p = 0; p < m_const_graph.k; ++p)
        {
            auto& partition_compatible_vertices_0 = compatible_vertices_0[p];
            partition_compatible_vertices_0.reset();

            if (p == pi || p == pj)
                continue;

            const auto& part = m_const_graph.partitions[p];
            for (uint_t bit = 0; bit < part.size(); ++bit)
                if (is_vertex_compatible_with_anchor(i, j, part[bit], m_workspace.anchor_edge_rank))
                    partition_compatible_vertices_0.set(bit);
        }
    }

    uint_t choose_best_partition(size_t depth) const
    {
        const uint_t k = m_const_graph.k;
        const auto& compatible_vertices = m_workspace.compatible_vertices[depth];
        const auto& partition_bits = m_workspace.partition_bits;

        uint_t best_partition = std::numeric_limits<uint_t>::max();
        uint_t best_set_bits = std::numeric_limits<uint_t>::max();

        for (uint_t p = 0; p < k; ++p)
        {
            if (partition_bits.test(p))
                continue;

            auto num_set_bits = compatible_vertices[p].count();
            if (num_set_bits < best_set_bits)
            {
                best_set_bits = num_set_bits;
                best_partition = p;
            }
        }
        return best_partition;
    }

    void copy_current_compatible_vertices_to_next_depth(size_t depth)
    {
        const uint_t k = m_const_graph.k;
        auto& current = m_workspace.compatible_vertices[depth];
        auto& next = m_workspace.compatible_vertices[depth + 1];

        for (uint_t p = 0; p < k; ++p)
            next[p] = current[p];
    }

    void update_compatible_adjacent_vertices_at_next_depth(uint_t vertex, size_t depth)
    {
        const uint_t k = m_const_graph.k;
        auto& next = m_workspace.compatible_vertices[depth + 1];
        const auto& partition_bits = m_workspace.partition_bits;

        // Offset trick assumes that vertices in lower partition have lower indices.
        uint_t offset = 0;
        for (uint_t partition = 0; partition < k; ++partition)
        {
            auto& partition_compatible_vertices_next = next[partition];
            auto partition_size = partition_compatible_vertices_next.size();
            if (!partition_bits.test(partition))
            {
                for (uint_t bit = 0; bit < partition_size; ++bit)
                {
                    const auto target_vertex = bit + offset;

                    partition_compatible_vertices_next[bit] &= m_full_graph.adjacency_matrix[vertex].test(target_vertex);

                    // monotone delta-rank pruning
                    if (partition_compatible_vertices_next.test(bit) && is_delta_edge(vertex, target_vertex)
                        && edge_rank(vertex, target_vertex) < m_workspace.anchor_edge_rank)
                        partition_compatible_vertices_next.reset(bit);
                }
            }
            offset += partition_size;
        }
    }

    uint_t num_possible_additions_at_next_depth(size_t depth)
    {
        const uint_t k = m_const_graph.k;
        const auto& next = m_workspace.compatible_vertices[depth + 1];
        const auto& partition_bits = m_workspace.partition_bits;

        uint_t possible_additions = 0;
        for (uint_t partition = 0; partition < k; ++partition)
            if (!partition_bits.test(partition) && next[partition].any())
                ++possible_additions;

        return possible_additions;
    }

    template<class Callback>
    void complete_from_seed(Callback&& callback, size_t depth)
    {
        assert(depth < m_const_graph.k);

        uint_t p = choose_best_partition(depth);
        if (p == std::numeric_limits<uint_t>::max())
            return;  // dead branch: no unused partition has candidates

        uint_t k = m_const_graph.k;
        auto& best = m_workspace.compatible_vertices[depth][p];
        auto& partition_bits = m_workspace.partition_bits;
        auto& partial_solution = m_workspace.partial_solution;

        // Iterate through compatible vertices in the best partition
        for (auto bit = best.find_first(); bit != boost::dynamic_bitset<>::npos; bit = best.find_next(bit))
        {
            uint_t vertex = m_const_graph.partitions[p][bit];
            best.reset(bit);

            partial_solution.push_back(vertex);

            if (partial_solution.size() == k)
            {
                callback(partial_solution);
            }
            else
            {
                copy_current_compatible_vertices_to_next_depth(depth);

                update_compatible_adjacent_vertices_at_next_depth(vertex, depth);

                partition_bits.set(p);

                if ((partial_solution.size() + num_possible_additions_at_next_depth(depth)) == k)
                    complete_from_seed(callback, depth + 1);

                partition_bits.reset(p);
            }

            partial_solution.pop_back();
        }
    }

private:
    delta_kpkc::ConstGraph m_const_graph;
    delta_kpkc::Graph m_delta_graph;
    delta_kpkc::Graph m_full_graph;
    delta_kpkc::Workspace m_workspace;
};
}

#endif
