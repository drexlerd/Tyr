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

#ifndef TYR_GROUNDER_CONSISTENCY_GRAPH_HPP_
#define TYR_GROUNDER_CONSISTENCY_GRAPH_HPP_

#include "tyr/analysis/domains.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/grounder/assignment_sets.hpp"
#include "tyr/grounder/declarations.hpp"

#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <optional>
#include <ranges>
#include <sstream>
#include <vector>

namespace tyr::grounder
{
namespace details
{
/**
 * Vertex
 */

/// @brief A vertex [parameter_index/object_index] in the consistency graph.
template<formalism::Context C>
class Vertex
{
private:
    uint_t m_index;
    formalism::ParameterIndex m_parameter_index;
    Index<formalism::Object> m_object_index;

public:
    Vertex(uint_t index, formalism::ParameterIndex parameter_index, Index<formalism::Object> object_index) noexcept;

    template<formalism::FactKind T>
    bool consistent_literals(View<IndexList<formalism::Literal<T>>, C> literals, const PredicateAssignmentSets<T, C>& predicate_assignment_sets) const noexcept;

    bool consistent_numeric_constraints(View<DataList<formalism::BooleanOperator<Data<formalism::FunctionExpression>>>, C> numeric_constraints,
                                        const AssignmentSets<C>& assignment_sets) const noexcept;

    Index<formalism::Object> get_object_if_overlap(View<Data<formalism::Term>, C> term) const noexcept;

    uint_t get_index() const noexcept;
    formalism::ParameterIndex get_parameter_index() const noexcept;
    Index<formalism::Object> get_object_index() const noexcept;
};

/**
 * Edge
 */

/// @brief An undirected edge {src,dst} in the consistency graph.
template<formalism::Context C>
class Edge
{
private:
    Vertex<C> m_src;
    Vertex<C> m_dst;

public:
    Edge(Vertex<C> src, Vertex<C> dst) noexcept;

    template<formalism::FactKind T>
    bool consistent_literals(View<IndexList<formalism::Literal<T>>, C> literals, const PredicateAssignmentSets<T, C>& predicate_assignment_sets) const noexcept;

    bool consistent_numeric_constraints(View<DataList<formalism::BooleanOperator<Data<formalism::FunctionExpression>>>, C> numeric_constraints,
                                        const AssignmentSets<C>& assignment_sets) const noexcept;

    Index<formalism::Object> get_object_if_overlap(View<Data<formalism::Term>, C> term) const noexcept;

    const Vertex<C>& get_src() const noexcept;
    const Vertex<C>& get_dst() const noexcept;
};

template<formalism::Context C>
using Vertices = std::vector<Vertex<C>>;
}

template<formalism::Context C, class ConditionTag>
    requires ConjunctiveConditionConcept<ConditionTag, C>
class StaticConsistencyGraph
{
private:
    /// @brief Helper to initialize vertices.
    std::pair<details::Vertices<C>, std::vector<std::vector<uint_t>>>
    compute_vertices(ConditionView<ConditionTag, C> condition,
                     const analysis::DomainListList& parameter_domains,
                     uint_t begin_parameter_index,
                     uint_t end_parameter_index,
                     const TaggedAssignmentSets<formalism::StaticTag, C>& static_assignment_sets);

    /// @brief Helper to initialize edges.
    std::tuple<std::vector<uint_t>, std::vector<uint_t>, std::vector<uint_t>>
    compute_edges(ConditionView<ConditionTag, C> condition,
                  const TaggedAssignmentSets<formalism::StaticTag, C>& static_assignment_sets,
                  const details::Vertices<C>& vertices);

public:
    StaticConsistencyGraph(ConditionView<ConditionTag, C> condition,
                           ConditionView<ConditionTag, C> arity_geq_1_overapproximation_condition,
                           ConditionView<ConditionTag, C> arity_geq_2_overapproximation_condition,
                           const analysis::DomainListList& parameter_domains,
                           uint_t begin_parameter_index,
                           uint_t end_parameter_index,
                           const TaggedAssignmentSets<formalism::StaticTag, C>& static_assignment_sets);

    class EdgeIterator
    {
    private:
        const StaticConsistencyGraph* m_graph;
        size_t m_sources_pos;
        size_t m_targets_pos;

        const StaticConsistencyGraph& get_graph() const noexcept;

        void advance() noexcept;

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = details::Edge<C>;
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

    auto consistent_vertices(const AssignmentSets<C>& assignment_sets) const
    {
        return get_vertices()
               | std::views::filter(
                   [this, &assignment_sets](auto&& vertex)
                   {
                       return vertex.consistent_literals(m_arity_geq_1_overapproximation_condition.template get_literals<formalism::FluentTag>(),
                                                         assignment_sets.fluent_sets.predicate)
                              && vertex.consistent_numeric_constraints(m_arity_geq_1_overapproximation_condition.get_numeric_constraints(), assignment_sets);
                   });
    }

    auto consistent_edges(const AssignmentSets<C>& assignment_sets, const boost::dynamic_bitset<>& consistent_vertices) const
    {
        return get_edges()
               | std::views::filter(
                   [this, &consistent_vertices, &assignment_sets](auto&& edge)
                   {
                       return consistent_vertices.test(edge.get_src().get_index()) && consistent_vertices.test(edge.get_dst().get_index())
                              && edge.consistent_literals(m_arity_geq_2_overapproximation_condition.template get_literals<formalism::FluentTag>(),
                                                          assignment_sets.fluent_sets.predicate)
                              && edge.consistent_numeric_constraints(m_arity_geq_2_overapproximation_condition.get_numeric_constraints(), assignment_sets);
                   });
    }

    const details::Vertex<C>& get_vertex(uint_t index) const noexcept;

    size_t get_num_vertices() const noexcept;
    size_t get_num_edges() const noexcept;

    ConditionView<ConditionTag, C> get_condition() const noexcept;
    const std::vector<std::vector<uint_t>>& get_partitions() const noexcept;

private:
    ConditionView<ConditionTag, C> m_condition;
    ConditionView<ConditionTag, C> m_arity_geq_1_overapproximation_condition;
    ConditionView<ConditionTag, C> m_arity_geq_2_overapproximation_condition;

    /* The data member of the consistency graph. */
    details::Vertices<C> m_vertices;

    // Adjacency list of edges.
    std::vector<uint_t> m_sources;  ///< sources with non-zero out-degree
    std::vector<uint_t> m_target_offsets;
    std::vector<uint_t> m_targets;

    std::vector<std::vector<uint_t>> m_partitions;
};

extern std::pair<Index<formalism::GroundConjunctiveCondition>, bool>
create_ground_nullary_condition(View<Index<formalism::ConjunctiveCondition>, formalism::Repository> condition,
                                formalism::Builder& builder,
                                formalism::Repository& context);

extern std::pair<Index<formalism::ConjunctiveCondition>, bool>
create_arity_geq_k_overapproximation_conjunctive_condition(size_t k,
                                                           View<Index<formalism::ConjunctiveCondition>, formalism::Repository> condition,
                                                           formalism::Builder& builder,
                                                           formalism::Repository& context);

extern std::pair<Index<formalism::ConjunctiveCondition>, bool>
create_overapproximation_conflicting_conjunctive_condition(View<Index<formalism::ConjunctiveCondition>, formalism::Repository> condition,
                                                           formalism::Builder& builder,
                                                           formalism::Repository& context);
}

#endif
