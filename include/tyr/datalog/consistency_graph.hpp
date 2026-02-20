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
#include "tyr/formalism/datalog/variable_dependency_graph.hpp"
#include "tyr/formalism/datalog/views.hpp"

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
    std::vector<std::pair<uint_t, uint_t>> position_parameter_pairs;
};

struct LiteralAnchorInfo
{
    ParameterMappings parameter_mappings;
};

struct IndexedAnchors
{
    std::vector<std::vector<LiteralAnchorInfo>> predicate_to_infos;

    boost::dynamic_bitset<> bound_parameters;
    boost::dynamic_bitset<> negated_bound_parameters;
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
    Vertex m_src;
    Vertex m_dst;

public:
    Edge(Vertex src, Vertex dst) noexcept : m_src(std::move(src)), m_dst(std::move(dst)) {}

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

    const Vertex& get_src() const noexcept { return m_src; }
    const Vertex& get_dst() const noexcept { return m_dst; }
};

using Vertices = std::vector<Vertex>;

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
    kpkc::DeduplicatedAdjacencyMatrix compute_edges(const details::TaggedIndexedLiterals<formalism::StaticTag>& indexed_literals,
                                                    const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets,
                                                    const details::Vertices& vertices,
                                                    const std::vector<std::vector<uint_t>>& vertex_partitions);

public:
    StaticConsistencyGraph(View<Index<formalism::datalog::Rule>, formalism::datalog::Repository> rule,
                           View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> condition,
                           View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> unary_overapproximation_condition,
                           View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> binary_overapproximation_condition,
                           View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> static_binary_overapproximation_condition,
                           const analysis::DomainListList& parameter_domains,
                           size_t num_objects,
                           size_t num_fluent_predicates,
                           uint_t begin_parameter_index,
                           uint_t end_parameter_index,
                           const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets);

    void initialize_dynamic_consistency_graphs(const AssignmentSets& assignment_sets,
                                               const TaggedFactSets<formalism::FluentTag>& delta_fact_sets,
                                               const kpkc::GraphLayout& layout,
                                               kpkc::Graph& delta_graph,
                                               kpkc::Graph& full_graph,
                                               kpkc::VertexPartitions& fact_induced_candidates) const;

    auto get_vertices() const noexcept { return std::ranges::subrange(m_vertices.cbegin(), m_vertices.cend()); }

    const details::Vertex& get_vertex(uint_t index) const;
    const details::Vertex& get_vertex(formalism::ParameterIndex parameter, Index<formalism::Object> object) const;

    size_t get_num_vertices() const noexcept;

    View<Index<formalism::datalog::Rule>, formalism::datalog::Repository> get_rule() const noexcept;
    View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> get_condition() const noexcept;
    const formalism::datalog::VariableDependencyGraph& get_variable_dependeny_graph() const noexcept;
    const std::vector<std::vector<uint_t>>& get_vertex_partitions() const noexcept;
    const std::vector<std::vector<uint_t>>& get_object_to_vertex_partitions() const noexcept;
    const details::IndexedAnchors& get_predicate_to_anchors() const noexcept;
    const kpkc::DeduplicatedAdjacencyMatrix& get_adjacency_matrix() const noexcept;

private:
    View<Index<formalism::datalog::Rule>, formalism::datalog::Repository> m_rule;
    View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> m_condition;
    View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> m_unary_overapproximation_condition;
    View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> m_binary_overapproximation_condition;
    View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> m_static_binary_overapproximation_condition;

    formalism::datalog::VariableDependencyGraph m_binary_overapproximation_vdg;
    formalism::datalog::VariableDependencyGraph m_static_binary_overapproximation_vdg;

    /* The data member of the consistency graph. */
    details::Vertices m_vertices;

    // Adjacency list of edges.
    std::vector<std::vector<uint_t>> m_vertex_partitions;
    std::vector<std::vector<uint_t>> m_object_to_vertex_partitions;

    kpkc::GraphLayout m_layout;
    kpkc::DeduplicatedAdjacencyMatrix m_matrix;

    details::IndexedLiterals m_unary_overapproximation_indexed_literals;
    details::IndexedLiterals m_binary_overapproximation_indexed_literals;

    details::IndexedConstraints m_unary_overapproximation_indexed_constraints;
    details::IndexedConstraints m_binary_overapproximation_indexed_constraints;

    details::IndexedAnchors m_predicate_to_anchors;
    details::IndexedAnchors m_unary_overapproximation_predicate_to_anchors;
    details::IndexedAnchors m_binary_overapproximation_predicate_to_anchors;
};

extern std::pair<Index<formalism::datalog::GroundConjunctiveCondition>, bool>
create_ground_nullary_condition(View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> condition,
                                formalism::datalog::Repository& context);

extern std::pair<Index<formalism::datalog::ConjunctiveCondition>, bool>
create_overapproximation_conjunctive_condition(size_t k,
                                               View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> condition,
                                               formalism::datalog::Repository& context);

extern std::pair<Index<formalism::datalog::ConjunctiveCondition>, bool>
create_static_overapproximation_conjunctive_condition(size_t k,
                                                      View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> condition,
                                                      formalism::datalog::Repository& context);

extern std::pair<Index<formalism::datalog::ConjunctiveCondition>, bool>
create_overapproximation_conflicting_conjunctive_condition(size_t k,
                                                           View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> condition,
                                                           formalism::datalog::Repository& context);

}

#endif
