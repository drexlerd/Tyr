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

#ifndef TYR_DATALOG_CONSISTENCY_GRAPH_HPP_
#define TYR_DATALOG_CONSISTENCY_GRAPH_HPP_

#include "tyr/analysis/domains.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/datalog/assignment_sets.hpp"
#include "tyr/datalog/declarations.hpp"
#include "tyr/datalog/delta_kpkc_graph.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/overlay_repository.hpp"

#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <optional>
#include <ranges>
#include <sstream>
#include <vector>

namespace tyr::datalog
{
class StaticConsistencyGraph;

namespace details
{
struct InfoMappings
{
    // For building vertex assignments (p/o)
    std::vector<std::vector<uint_t>> parameter_to_infos;

    // For building edge assignments (p/o,q/c)
    std::vector<std::vector<std::vector<uint_t>>> parameter_pairs_to_infos;
    std::vector<std::vector<uint_t>> parameter_to_infos_with_constants;

    // For global vertex assignments (c) for constant c
    std::vector<uint_t> infos_with_constants;
    // For global edge assignments (c,c') for constants c,c'
    std::vector<uint_t> infos_with_constant_pairs;
};

struct PositionMappings
{
    std::vector<std::pair<uint_t, Index<formalism::Object>>> constant_positions;
    std::vector<std::vector<uint_t>> parameter_to_positions;
};

template<formalism::FactKind T>
struct LiteralInfo
{
    Index<formalism::Predicate<T>> predicate;
    bool polarity;
    size_t kpkc_arity;
    size_t num_parameters;
    size_t num_constants;

    PositionMappings position_mappings;
};

template<formalism::FactKind T>
struct TaggedIndexedLiterals
{
    std::vector<LiteralInfo<T>> infos;

    InfoMappings info_mappings;
};

struct IndexedLiterals
{
    details::TaggedIndexedLiterals<formalism::StaticTag> static_indexed;
    details::TaggedIndexedLiterals<formalism::FluentTag> fluent_indexed;

    template<formalism::FactKind T>
    const auto& get() const noexcept
    {
        if constexpr (std::is_same_v<T, formalism::StaticTag>)
            return static_indexed;
        else if constexpr (std::is_same_v<T, formalism::FluentTag>)
            return fluent_indexed;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }
};

template<formalism::FactKind T>
struct FunctionTermInfo
{
    Index<formalism::Function<T>> function;
    size_t kpkc_arity;
    size_t num_parameters;
    size_t num_constants;

    PositionMappings position_mappings;
};

template<formalism::FactKind T>
struct TaggedIndexedFunctionTerms
{
    UnorderedMap<Index<formalism::datalog::FunctionTerm<T>>, FunctionTermInfo<T>> infos;

    InfoMappings info_mappings;
};

struct ConstraintInfo
{
    TaggedIndexedFunctionTerms<formalism::StaticTag> static_infos;
    TaggedIndexedFunctionTerms<formalism::FluentTag> fluent_infos;

    size_t kpkc_arity;

    template<formalism::FactKind T>
    const auto& get() const noexcept
    {
        if constexpr (std::is_same_v<T, formalism::StaticTag>)
            return static_infos;
        else if constexpr (std::is_same_v<T, formalism::FluentTag>)
            return fluent_infos;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }
};

struct IndexedConstraints
{
    std::vector<ConstraintInfo> infos;
};

struct ParameterMappings
{
    static constexpr uint_t NoParam = std::numeric_limits<uint_t>::max();

    std::vector<uint_t> position_to_parameter;
};

struct LiteralAnchorInfo
{
    ParameterMappings parameter_mappings;
};

struct IndexedAnchors
{
    std::vector<std::vector<LiteralAnchorInfo>> predicate_to_infos;
};

/**
 * Vertex
 */

/// @brief A vertex [parameter_index/object_index] in the consistency graph.
class Vertex
{
private:
    uint_t m_index;
    formalism::ParameterIndex m_parameter_index;
    Index<formalism::Object> m_object_index;

public:
    Vertex(uint_t index, formalism::ParameterIndex parameter_index, Index<formalism::Object> object_index) noexcept :
        m_index(index),
        m_parameter_index(parameter_index),
        m_object_index(object_index)
    {
    }

    /**
     * Classical
     */

    template<formalism::FactKind T>
    bool consistent_literals(const TaggedIndexedLiterals<T>& indexed_literals, const PredicateAssignmentSets<T>& predicate_assignment_sets) const noexcept;

    /**
     * Numeric
     */

    bool consistent_numeric_constraints(
        View<DataList<formalism::datalog::BooleanOperator<Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> numeric_constraints,
        const IndexedConstraints& indexed_constraints,
        const AssignmentSets& assignment_sets) const noexcept;

    uint_t get_index() const noexcept { return m_index; }
    formalism::ParameterIndex get_parameter_index() const noexcept { return m_parameter_index; }
    Index<formalism::Object> get_object_index() const noexcept { return m_object_index; }
};

/**
 * Edge
 */

/// @brief An undirected edge {src,dst} in the consistency graph.
class Edge
{
private:
    uint_t m_index;
    Vertex m_src;
    Vertex m_dst;

public:
    Edge(uint_t index, Vertex src, Vertex dst) noexcept : m_index(index), m_src(std::move(src)), m_dst(std::move(dst)) {}

    /**
     * Classical
     */

    template<formalism::FactKind T>
    bool consistent_literals(const TaggedIndexedLiterals<T>& indexed_literals, const PredicateAssignmentSets<T>& predicate_assignment_sets) const noexcept;

    /**
     * Numeric
     */

    bool consistent_numeric_constraints(
        View<DataList<formalism::datalog::BooleanOperator<Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> numeric_constraints,
        const IndexedConstraints& indexed_constraints,
        const AssignmentSets& assignment_sets) const noexcept;

    uint_t get_index() const noexcept { return m_index; }
    const Vertex& get_src() const noexcept { return m_src; }
    const Vertex& get_dst() const noexcept { return m_dst; }
};

using Vertices = std::vector<Vertex>;

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

class StaticConsistencyGraph
{
private:
    /// @brief Helper to initialize vertices.
    std::tuple<details::Vertices, std::vector<std::vector<uint_t>>, std::vector<std::vector<uint_t>>>
    compute_vertices(const details::TaggedIndexedLiterals<formalism::StaticTag>& indexed_literals,
                     const analysis::DomainListList& parameter_domains,
                     size_t num_objects,
                     uint_t begin_parameter_index,
                     uint_t end_parameter_index,
                     const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets);

    /// @brief Helper to initialize edges.
    std::tuple<std::vector<uint_t>, std::vector<uint_t>, std::vector<uint_t>, details::PartitionedAdjacencyMatrix>
    compute_edges(const details::TaggedIndexedLiterals<formalism::StaticTag>& indexed_literals,
                  const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets,
                  const details::Vertices& vertices,
                  const std::vector<std::vector<uint_t>>& vertex_partitions);

    template<formalism::FactKind T>
    bool constant_consistent_literals(const details::TaggedIndexedLiterals<T>& indexed_literals,
                                      const PredicateAssignmentSets<T>& predicate_assignment_sets) const noexcept;

    template<formalism::FactKind T>
    bool constant_pair_consistent_literals(const details::TaggedIndexedLiterals<T>& indexed_literals,
                                           const PredicateAssignmentSets<T>& predicate_assignment_sets) const noexcept;

public:
    StaticConsistencyGraph(View<Index<formalism::datalog::Rule>, formalism::datalog::Repository> rule,
                           View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> condition,
                           View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> unary_overapproximation_condition,
                           View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> binary_overapproximation_condition,
                           const analysis::DomainListList& parameter_domains,
                           size_t num_objects,
                           size_t num_fluent_predicates,
                           uint_t begin_parameter_index,
                           uint_t end_parameter_index,
                           const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets);

    class EdgeIterator
    {
    private:
        const StaticConsistencyGraph* m_graph;
        uint_t m_index;
        size_t m_sources_pos;
        size_t m_targets_pos;

        const StaticConsistencyGraph& get_graph() const noexcept;

        void advance() noexcept;

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = details::Edge;
        using pointer = value_type*;
        using reference = const value_type&;
        using iterator_category = std::forward_iterator_tag;
        using iterator_concept = std::forward_iterator_tag;

        EdgeIterator() noexcept;
        EdgeIterator(const StaticConsistencyGraph& graph, bool begin) noexcept;
        value_type operator*() const noexcept;
        EdgeIterator& operator++() noexcept;
        EdgeIterator operator++(int) noexcept;
        bool operator==(const EdgeIterator& other) const noexcept;
        bool operator!=(const EdgeIterator& other) const noexcept;
    };

    auto get_vertices() const noexcept { return std::ranges::subrange(m_vertices.cbegin(), m_vertices.cend()); }
    auto get_edges() const noexcept { return std::ranges::subrange(EdgeIterator(*this, true), EdgeIterator(*this, false)); }

    /// @brief Efficient iteration over delta-consistent vertices.
    /// @tparam Callback
    /// @param assignment_sets
    /// @param active_vertices
    /// @param callback
    template<typename Callback>
    void delta_consistent_vertices(const AssignmentSets& assignment_sets, const boost::dynamic_bitset<>& active_vertices, Callback&& callback) const
    {
        assert(active_vertices.size() == get_num_vertices());

        const auto constraints = m_unary_overapproximation_condition.get_numeric_constraints();

        if (constant_consistent_literals(m_unary_overapproximation_indexed_literals.fluent_indexed, assignment_sets.fluent_sets.predicate))
        {
            for (auto index = active_vertices.find_first(); index != boost::dynamic_bitset<>::npos; index = active_vertices.find_next(index))
            {
                const auto vertex = get_vertex(index);

                if (vertex.consistent_literals(m_unary_overapproximation_indexed_literals.fluent_indexed, assignment_sets.fluent_sets.predicate)
                    && vertex.consistent_numeric_constraints(constraints, m_unary_overapproximation_indexed_constraints, assignment_sets))
                {
                    callback(vertex);
                }
            }
        }
    }

    /// @brief Efficient iteration over delta-consistent edges.
    /// @tparam Callback
    /// @param assignment_sets
    /// @param active_edges
    /// @param consistent_vertices
    /// @param callback
    template<typename Callback>
    void delta_consistent_edges(const AssignmentSets& assignment_sets,
                                const boost::dynamic_bitset<>& active_edges,
                                const boost::dynamic_bitset<>& consistent_vertices,
                                Callback&& callback) const
    {
        assert(m_target_offsets.size() == m_sources.size() + 1);
        assert(m_target_offsets.back() == m_targets.size());
        assert(m_targets.size() == active_edges.size());
        assert(consistent_vertices.size() == get_num_vertices());

        const auto constraints = m_binary_overapproximation_condition.get_numeric_constraints();

        if (constant_pair_consistent_literals(m_binary_overapproximation_indexed_literals.fluent_indexed, assignment_sets.fluent_sets.predicate))
        {
            for (uint_t src_pos = 0; src_pos < m_sources.size(); ++src_pos)
            {
                const auto src = m_sources[src_pos];

                if (!consistent_vertices.test(src))
                    continue;

                for (uint_t index = m_target_offsets[src_pos]; index < m_target_offsets[src_pos + 1]; ++index)
                {
                    if (!active_edges.test(index))
                        continue;

                    const auto dst = m_targets[index];

                    if (!consistent_vertices.test(dst))
                        continue;

                    const auto edge = details::Edge(index, get_vertex(src), get_vertex(dst));

                    if (edge.consistent_literals(m_binary_overapproximation_indexed_literals.fluent_indexed, assignment_sets.fluent_sets.predicate)
                        && edge.consistent_numeric_constraints(constraints, m_binary_overapproximation_indexed_constraints, assignment_sets))
                    {
                        callback(edge);
                    }
                }
            }
        }
    }

    void initialize_graphs(const AssignmentSets& assignment_sets,
                           kpkc::Graph& delta_graph,
                           kpkc::Graph& full_graph,
                           kpkc::GraphActivityMasks& read_masks,
                           kpkc::GraphActivityMasks& write_masks);

    const details::Vertex& get_vertex(uint_t index) const;
    const details::Vertex& get_vertex(formalism::ParameterIndex parameter, Index<formalism::Object> object) const;

    size_t get_num_vertices() const noexcept;
    size_t get_num_edges() const noexcept;

    View<Index<formalism::datalog::Rule>, formalism::datalog::Repository> get_rule() const noexcept;
    View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> get_condition() const noexcept;
    const std::vector<std::vector<uint_t>>& get_vertex_partitions() const noexcept;
    const std::vector<std::vector<uint_t>>& get_object_to_vertex_partitions() const noexcept;
    const details::IndexedAnchors& get_predicate_to_anchors() const noexcept;
    const details::PartitionedAdjacencyMatrix& get_adjacency_matrix() const noexcept;

private:
    View<Index<formalism::datalog::Rule>, formalism::datalog::Repository> m_rule;
    View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> m_condition;
    View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> m_unary_overapproximation_condition;
    View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> m_binary_overapproximation_condition;

    /* The data member of the consistency graph. */
    details::Vertices m_vertices;

    // Adjacency list of edges.
    std::vector<uint_t> m_sources;  ///< sources with non-zero out-degree
    std::vector<uint_t> m_target_offsets;
    std::vector<uint_t> m_targets;
    std::vector<std::vector<uint_t>> m_vertex_partitions;
    std::vector<std::vector<uint_t>> m_object_to_vertex_partitions;

    details::PartitionedAdjacencyMatrix m_adj_matrix;

    details::IndexedLiterals m_unary_overapproximation_indexed_literals;
    details::IndexedLiterals m_binary_overapproximation_indexed_literals;

    details::IndexedConstraints m_unary_overapproximation_indexed_constraints;
    details::IndexedConstraints m_binary_overapproximation_indexed_constraints;

    details::IndexedAnchors m_predicate_to_anchors;
};

extern std::pair<Index<formalism::datalog::GroundConjunctiveCondition>, bool>
create_ground_nullary_condition(View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> condition,
                                formalism::datalog::Repository& context);

extern std::pair<Index<formalism::datalog::ConjunctiveCondition>, bool>
create_overapproximation_conjunctive_condition(size_t k,
                                               View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> condition,
                                               formalism::datalog::Repository& context);

extern std::pair<Index<formalism::datalog::ConjunctiveCondition>, bool>
create_overapproximation_conflicting_conjunctive_condition(size_t k,
                                                           View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> condition,
                                                           formalism::datalog::Repository& context);

}

#endif
