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
#include "tyr/datalog/assignment_sets.hpp"
#include "tyr/datalog/declarations.hpp"
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

    PositionMappings mappings;
};

template<formalism::FactKind T>
struct TaggedIndexedLiterals
{
    std::vector<LiteralInfo<T>> infos;

    InfoMappings mappings;
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

    PositionMappings mappings;
};

template<formalism::FactKind T>
struct TaggedIndexedFunctionTerms
{
    UnorderedMap<Index<formalism::datalog::FunctionTerm<T>>, FunctionTermInfo<T>> infos;

    InfoMappings mappings;
};

struct ConstraintInfo
{
    TaggedIndexedFunctionTerms<formalism::StaticTag> static_infos;
    TaggedIndexedFunctionTerms<formalism::FluentTag> fluent_infos;

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
    Vertex(uint_t index, formalism::ParameterIndex parameter_index, Index<formalism::Object> object_index) noexcept;

    template<formalism::FactKind T>
    bool consistent_literals(View<IndexList<formalism::datalog::Literal<T>>, formalism::datalog::Repository> literals,
                             const PredicateAssignmentSets<T>& predicate_assignment_sets) const noexcept;
    template<formalism::FactKind T>
    bool consistent_literals(const TaggedIndexedLiterals<T>& indexed_literals, const PredicateAssignmentSets<T>& predicate_assignment_sets) const noexcept;

    bool consistent_numeric_constraints(
        View<DataList<formalism::datalog::BooleanOperator<Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> numeric_constraints,
        const AssignmentSets& assignment_sets) const noexcept;

    Index<formalism::Object> get_object_if_overlap(View<Data<formalism::Term>, formalism::datalog::Repository> term) const noexcept;

    uint_t get_index() const noexcept;
    formalism::ParameterIndex get_parameter_index() const noexcept;
    Index<formalism::Object> get_object_index() const noexcept;
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
    Edge(uint_t index, Vertex src, Vertex dst) noexcept;

    template<formalism::FactKind T>
    bool consistent_literals(View<IndexList<formalism::datalog::Literal<T>>, formalism::datalog::Repository> literals,
                             const PredicateAssignmentSets<T>& predicate_assignment_sets) const noexcept;

    template<formalism::FactKind T>
    bool consistent_literals(const TaggedIndexedLiterals<T>& indexed_literals, const PredicateAssignmentSets<T>& predicate_assignment_sets) const noexcept;

    bool consistent_numeric_constraints(
        View<DataList<formalism::datalog::BooleanOperator<Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> numeric_constraints,
        const AssignmentSets& assignment_sets) const noexcept;

    Index<formalism::Object> get_object_if_overlap(View<Data<formalism::Term>, formalism::datalog::Repository> term) const noexcept;

    uint_t get_index() const noexcept;
    const Vertex& get_src() const noexcept;
    const Vertex& get_dst() const noexcept;
};

using Vertices = std::vector<Vertex>;
}

class StaticConsistencyGraph
{
private:
    /// @brief Helper to initialize vertices.
    std::pair<details::Vertices, std::vector<std::vector<uint_t>>>
    compute_vertices(const details::TaggedIndexedLiterals<formalism::StaticTag>& indexed_literals,
                     const analysis::DomainListList& parameter_domains,
                     uint_t begin_parameter_index,
                     uint_t end_parameter_index,
                     const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets);

    /// @brief Helper to initialize edges.
    std::tuple<std::vector<uint_t>, std::vector<uint_t>, std::vector<uint_t>>
    compute_edges(const details::TaggedIndexedLiterals<formalism::StaticTag>& indexed_literals,
                  const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets,
                  const details::Vertices& vertices);

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
                    && vertex.consistent_numeric_constraints(constraints, assignment_sets))
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
                        && edge.consistent_numeric_constraints(constraints, assignment_sets))
                    {
                        callback(edge);
                    }
                }
            }
        }
    }

    const details::Vertex& get_vertex(uint_t index) const noexcept;

    size_t get_num_vertices() const noexcept;
    size_t get_num_edges() const noexcept;

    View<Index<formalism::datalog::Rule>, formalism::datalog::Repository> get_rule() const noexcept;
    View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> get_condition() const noexcept;
    const std::vector<std::vector<uint_t>>& get_partitions() const noexcept;

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
    std::vector<std::vector<uint_t>> m_partitions;

    details::IndexedLiterals m_unary_overapproximation_indexed_literals;
    details::IndexedLiterals m_binary_overapproximation_indexed_literals;

    details::IndexedConstraints m_unary_overapproximation_indexed_constraints;
    details::IndexedConstraints m_binary_overapproximation_indexed_constraints;
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
