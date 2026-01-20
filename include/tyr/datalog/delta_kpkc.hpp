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

    /// @brief Constructor enforces invariants.
    /// @param nv
    /// @param k
    /// @param partitions
    /// @param vertex_to_partition
    GraphLayout(size_t nv, size_t k, std::vector<std::vector<Vertex>> partitions, std::vector<uint_t> vertex_to_partition);
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
    boost::dynamic_bitset<> vertices;  ///< Dimensions V
    /// Edges
    std::vector<boost::dynamic_bitset<>> adjacency_matrix;  ///< Dimensions V x V

    void reset() noexcept
    {
        vertices.reset();
        for (auto& bitset : adjacency_matrix)
            bitset.reset();
    }

    bool contains(Vertex vertex) const noexcept { return vertices.test(vertex.index); }
    bool contains(Edge edge) const noexcept { return adjacency_matrix[edge.src.index].test(edge.dst.index); }

    struct VertexIterator
    {
    private:
        const Graph* m_graph;
        boost::dynamic_bitset<>::size_type i;

    private:
        void advance_to_next() noexcept
        {
            if (i != boost::dynamic_bitset<>::npos)
                i = m_graph->vertices.find_next(i);
        }

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Vertex;
        using iterator_category = std::input_iterator_tag;
        using iterator_concept = std::input_iterator_tag;

        VertexIterator() noexcept : m_graph(nullptr), i(boost::dynamic_bitset<>::npos) {}
        VertexIterator(const Graph& graph, bool begin) noexcept : m_graph(&graph), i(begin ? graph.vertices.find_first() : boost::dynamic_bitset<>::npos) {}

        value_type operator*() const noexcept { return Vertex { uint_t(i) }; }

        VertexIterator& operator++() noexcept
        {
            advance_to_next();
            return *this;
        }

        VertexIterator operator++(int) noexcept
        {
            VertexIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const VertexIterator& lhs, const VertexIterator& rhs) noexcept { return lhs.m_graph == rhs.m_graph && lhs.i == rhs.i; }
        friend bool operator!=(const VertexIterator& lhs, const VertexIterator& rhs) noexcept { return !(lhs == rhs); }
    };

    struct EdgeIterator
    {
    private:
        const Graph* m_graph;
        uint_t i;
        boost::dynamic_bitset<>::size_type j;

    private:
        void advance_to_next() noexcept
        {
            const auto n = uint_t(m_graph->vertices.size());

            while (i < n)
            {
                const auto& row = m_graph->adjacency_matrix[i];

                if (j == boost::dynamic_bitset<>::npos)
                    j = row.find_next(i);  // find first from i; enforces i < j
                else
                    j = row.find_next(j);  // continue scanning row

                if (j != boost::dynamic_bitset<>::npos)
                    return;  // found edge (i,j)

                ++i;
                j = boost::dynamic_bitset<>::npos;
            }
        }

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Edge;
        using iterator_category = std::input_iterator_tag;
        using iterator_concept = std::input_iterator_tag;

        EdgeIterator() noexcept : m_graph(nullptr), i(0), j(boost::dynamic_bitset<>::npos) {}
        EdgeIterator(const Graph& graph, bool begin) noexcept : m_graph(&graph), i(begin ? 0 : uint_t(graph.vertices.size())), j(boost::dynamic_bitset<>::npos)
        {
            if (begin)
                advance_to_next();
        }

        value_type operator*() const noexcept { return Edge { Vertex { i }, Vertex { uint_t(j) } }; }

        EdgeIterator& operator++() noexcept
        {
            advance_to_next();
            return *this;
        }

        EdgeIterator operator++(int) noexcept
        {
            EdgeIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const EdgeIterator& lhs, const EdgeIterator& rhs) noexcept
        {
            return lhs.m_graph == rhs.m_graph && lhs.i == rhs.i && lhs.j == rhs.j;
        }
        friend bool operator!=(const EdgeIterator& lhs, const EdgeIterator& rhs) noexcept { return !(lhs == rhs); }
    };

    auto vertices_range() const noexcept { return std::ranges::subrange(VertexIterator(*this, true), VertexIterator(*this, false)); }
    auto edges_range() const noexcept { return std::ranges::subrange(EdgeIterator(*this, true), EdgeIterator(*this, false)); }
};

/// @brief `Workspace` is preallocated memory for a rule.
struct Workspace
{
    std::vector<std::vector<boost::dynamic_bitset<>>> compatible_vertices;  ///< Dimensions K x K x O(V)
    boost::dynamic_bitset<> partition_bits;                                 ///< Dimensions K
    std::vector<Vertex> partial_solution;                                   ///< Dimensions K
    uint_t anchor_edge_rank;

    /// @brief Allocate workspace memory layout for a given graph layout.
    /// @param graph
    explicit Workspace(const GraphLayout& graph);
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

    auto delta_vertices_range() const noexcept { return m_delta_graph.vertices_range(); }
    auto delta_edges_range() const noexcept { return m_delta_graph.edges_range(); }
    auto full_vertices_range() const noexcept { return m_full_graph.vertices_range(); }
    auto full_edges_range() const noexcept { return m_full_graph.edges_range(); }

    template<typename Callback>
    void for_each_new_unary_clique(Callback&& callback, Workspace& workspace) const;

    template<typename Callback>
    void for_each_unary_clique(Callback&& callback, Workspace& workspace) const;

    template<typename Callback>
    void for_each_new_binary_clique(Callback&& callback, Workspace& workspace) const;

    template<typename Callback>
    void for_each_binary_clique(Callback&& callback, Workspace& workspace) const;

    template<typename Callback>
    void for_each_k_clique(Callback&& callback, Workspace& workspace) const;

    template<typename Callback>
    void for_each_new_k_clique(Callback&& callback, Workspace& workspace) const;

    /**
     * Getters
     */

    const GraphLayout& get_graph_layout() const noexcept { return m_const_graph; }
    const Graph& get_delta_graph() const noexcept { return m_delta_graph; }
    const Graph& get_full_graph() const noexcept { return m_full_graph; }

private:
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

    /// @brief Find the pivot partition that greedily minimizes the number of recursive calls,
    /// i.e., the partition with the smallest number of candidate vertices.
    /// @param depth is the recursion depth.
    /// @return
    uint_t choose_best_partition(size_t depth, const Workspace& workspace) const;

    /// @brief Update the P part of BronKerbosch given the last selected vertex `src` at depth `depth`.
    /// @tparam Delta is a flag to enable monotonic delta constraint check.
    /// @param src is the last selected vertex.
    /// @param depth is the recursion depth.
    template<bool Delta>
    void update_compatible_adjacent_vertices_at_next_depth(Vertex src, size_t depth, Workspace& workspace) const;

    /// @brief Early termination helper
    /// @param depth is the recursion depth.
    /// @return
    uint_t num_possible_additions_at_next_depth(size_t depth, const Workspace& workspace) const;

    /// @brief Complete the k-clique recursively.
    /// @tparam Callback is called upon finding a k-clique.
    /// @tparam Delta is a flag to enable monotonic delta constraint check.
    /// @param callback is the callback function.
    /// @param depth is the recursion depth.
    template<bool Delta, class Callback>
    void complete_from_seed(Callback&& callback, size_t depth, Workspace& workspace) const;

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

    for (const auto& vertex : delta_vertices_range())
    {
        workspace.partial_solution.clear();
        workspace.partial_solution.push_back(vertex);

        callback(workspace.partial_solution);
    }
}

template<typename Callback>
void DeltaKPKC::for_each_unary_clique(Callback&& callback, Workspace& workspace) const
{
    assert(m_const_graph.k == 1);

    for (const auto& vertex : full_vertices_range())
    {
        workspace.partial_solution.clear();
        workspace.partial_solution.push_back(vertex);

        callback(workspace.partial_solution);
    }
}

template<typename Callback>
void DeltaKPKC::for_each_new_binary_clique(Callback&& callback, Workspace& workspace) const
{
    assert(m_const_graph.k == 2);

    for (const auto& edge : delta_edges_range())
    {
        workspace.partial_solution.clear();
        workspace.partial_solution.push_back(edge.src);
        workspace.partial_solution.push_back(edge.dst);

        callback(workspace.partial_solution);
    }
}

template<typename Callback>
void DeltaKPKC::for_each_binary_clique(Callback&& callback, Workspace& workspace) const
{
    assert(m_const_graph.k == 2);

    for (const auto& edge : full_edges_range())
    {
        workspace.partial_solution.clear();
        workspace.partial_solution.push_back(edge.src);
        workspace.partial_solution.push_back(edge.dst);

        callback(workspace.partial_solution);
    }
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

        complete_from_seed<false>(callback, 0, workspace);
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
            for (const auto& edge : delta_edges_range())
            {
                seed_from_anchor(edge, workspace);

                complete_from_seed<true>(callback, 0, workspace);
            }
        }
    }
}

template<bool Delta>
void DeltaKPKC::update_compatible_adjacent_vertices_at_next_depth(Vertex src, size_t depth, Workspace& workspace) const
{
    const uint_t k = m_const_graph.k;

    auto& cv_curr = workspace.compatible_vertices[depth];
    auto& cv_next = workspace.compatible_vertices[depth + 1];
    const auto& partition_bits = workspace.partition_bits;

    const auto& full_row = m_full_graph.adjacency_matrix[src.index];
    const auto& delta_row = m_delta_graph.adjacency_matrix[src.index];

    uint_t offset = 0;

    for (uint_t p = 0; p < k; ++p)
    {
        auto& cv_next_p = cv_next[p];

        // Restrict neighborhood
        auto partition_size = cv_next_p.size();

        if (partition_bits.test(p))
        {
            offset += partition_size;
            continue;
        }

        // Copy current into next
        cv_next_p = cv_curr[p];

        for (auto bit = cv_next_p.find_first(); bit != boost::dynamic_bitset<>::npos; bit = cv_next_p.find_next(bit))
        {
            uint_t dst = bit + offset;

            // Full Graph Check
            if (!full_row.test(dst))
            {
                cv_next_p.reset(bit);
                continue;
            }

            // Delta Graph Check
            if constexpr (Delta)
                if (cv_next_p.test(bit) && delta_row.test(dst) && (Edge(src, Vertex(dst)).rank(m_const_graph.nv) < workspace.anchor_edge_rank))
                    cv_next_p.reset(bit);
        }

        offset += partition_size;
    }
}

template<bool Delta, class Callback>
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

        if (partial_solution.size() == k)
        {
            callback(partial_solution);
        }
        else
        {
            update_compatible_adjacent_vertices_at_next_depth<Delta>(vertex, depth, workspace);

            partition_bits.set(p);

            if ((partial_solution.size() + num_possible_additions_at_next_depth(depth, workspace)) == k)
                complete_from_seed<Delta>(callback, depth + 1, workspace);

            partition_bits.reset(p);
        }

        partial_solution.pop_back();
    }
}
}

#endif
