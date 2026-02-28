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
/**
 * For mapping rule bindings to literal bindings
 */

struct RuleToLiteralInfoMappings
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

struct RuleToLiteralPositionMappings
{
    std::vector<std::pair<uint_t, Index<formalism::Object>>> constant_positions;
    std::vector<std::vector<uint_t>> parameter_to_positions;
};

template<formalism::FactKind T>
struct RuleToLiteralInfo
{
    Index<formalism::Predicate<T>> predicate;
    bool polarity;
    size_t kpkc_arity;
    size_t num_parameters;
    size_t num_constants;

    RuleToLiteralPositionMappings position_mappings;
};

template<formalism::FactKind T>
struct TaggedRuleToLiteralInfos
{
    std::vector<RuleToLiteralInfo<T>> infos;

    RuleToLiteralInfoMappings info_mappings;
};

struct RuleToLiteralInfos
{
    details::TaggedRuleToLiteralInfos<formalism::StaticTag> static_indexed;
    details::TaggedRuleToLiteralInfos<formalism::FluentTag> fluent_indexed;

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
struct RuleToFunctionTermInfo
{
    Index<formalism::Function<T>> function;
    size_t kpkc_arity;
    size_t num_parameters;
    size_t num_constants;

    RuleToLiteralPositionMappings position_mappings;
};

template<formalism::FactKind T>
struct TaggedRuleToFunctionTermInfos
{
    UnorderedMap<Index<formalism::datalog::FunctionTerm<T>>, RuleToFunctionTermInfo<T>> infos;

    RuleToLiteralInfoMappings info_mappings;
};

struct RuleToConstraintInfo
{
    TaggedRuleToFunctionTermInfos<formalism::StaticTag> static_infos;
    TaggedRuleToFunctionTermInfos<formalism::FluentTag> fluent_infos;

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

struct RuleToRuleToConstraintInfos
{
    std::vector<RuleToConstraintInfo> infos;
};

/**
 * For mapping literal bindings to rule bindings
 */

struct LiteralToRuleParameterMapping
{
    static constexpr uint_t NoParam = std::numeric_limits<uint_t>::max();

    std::vector<uint_t> position_to_parameter;
    std::vector<std::pair<uint_t, uint_t>> position_parameter_pairs;
};

struct LiteralToRuleInfo
{
    LiteralToRuleParameterMapping parameter_mappings;
};

struct LiteralToRuleInfos
{
    struct GroupInfo
    {
        Index<formalism::Predicate<formalism::FluentTag>> predicate;
        uint_t start;
        uint_t end;
    };

    std::vector<GroupInfo> groups;

    std::vector<LiteralToRuleInfo> infos;

    std::span<const LiteralToRuleInfo> operator[](const GroupInfo& pos) const noexcept
    {
        assert(pos.end > pos.start && pos.end <= infos.size());
        return std::span<const LiteralToRuleInfo>(infos.data() + pos.start, pos.end - pos.start);
    }

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
    formalism::ParameterIndex m_parameter_index;
    Index<formalism::Object> m_object_index;

public:
    Vertex(formalism::ParameterIndex parameter_index, Index<formalism::Object> object_index) noexcept :
        m_parameter_index(parameter_index),
        m_object_index(object_index)
    {
    }

    /**
     * Classical
     */

    template<formalism::FactKind T>
    bool consistent_literals(const TaggedRuleToLiteralInfos<T>& indexed_literals, const PredicateAssignmentSets<T>& predicate_assignment_sets) const noexcept;

    /**
     * Numeric
     */

    bool consistent_numeric_constraints(
        View<DataList<formalism::datalog::BooleanOperator<Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> numeric_constraints,
        const RuleToRuleToConstraintInfos& indexed_constraints,
        const AssignmentSets& assignment_sets) const noexcept;

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
    bool consistent_literals(const TaggedRuleToLiteralInfos<T>& indexed_literals, const PredicateAssignmentSets<T>& predicate_assignment_sets) const noexcept;

    /**
     * Numeric
     */

    bool consistent_numeric_constraints(
        View<DataList<formalism::datalog::BooleanOperator<Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> numeric_constraints,
        const RuleToRuleToConstraintInfos& indexed_constraints,
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
    compute_vertices(const details::TaggedRuleToLiteralInfos<formalism::StaticTag>& indexed_literals,
                     const analysis::DomainListList& parameter_domains,
                     size_t num_objects,
                     uint_t begin_parameter_index,
                     uint_t end_parameter_index,
                     const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets);

    /// @brief Helper to initialize edges.
    kpkc::DeduplicatedAdjacencyMatrix compute_edges(const details::TaggedRuleToLiteralInfos<formalism::StaticTag>& indexed_literals,
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
                                               std::vector<kpkc::Edge>& delta_edges,
                                               kpkc::VertexPartitions& fact_induced_candidates) const;

    auto get_vertices() const noexcept { return std::ranges::subrange(m_vertices.cbegin(), m_vertices.cend()); }

    const details::Vertex& get_vertex(uint_t index) const;

    size_t get_num_vertices() const noexcept;

    View<Index<formalism::datalog::Rule>, formalism::datalog::Repository> get_rule() const noexcept;
    View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> get_condition() const noexcept;
    const formalism::datalog::VariableDependencyGraph& get_variable_dependeny_graph() const noexcept;
    const std::vector<std::vector<uint_t>>& get_vertex_partitions() const noexcept;
    const std::vector<std::vector<uint_t>>& get_object_to_vertex_per_partition() const noexcept;
    const details::LiteralToRuleInfos& get_predicate_to_anchors() const noexcept;
    const kpkc::DeduplicatedAdjacencyMatrix& get_adjacency_matrix() const noexcept;

private:
    View<Index<formalism::datalog::Rule>, formalism::datalog::Repository> m_rule;
    View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> m_condition;
    View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> m_unary_overapproximation_condition;
    View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> m_binary_overapproximation_condition;

    formalism::datalog::VariableDependencyGraph m_binary_overapproximation_vdg;

    /* The data member of the consistency graph. */
    details::Vertices m_vertices;

    // Adjacency list of edges.
    std::vector<std::vector<uint_t>> m_vertex_partitions;
    std::vector<std::vector<uint_t>> m_object_to_vertex_per_partition;

    kpkc::GraphLayout m_layout;
    kpkc::DeduplicatedAdjacencyMatrix m_matrix;

    details::RuleToLiteralInfos m_unary_overapproximation_indexed_literals;
    details::RuleToLiteralInfos m_binary_overapproximation_indexed_literals;

    details::RuleToRuleToConstraintInfos m_unary_overapproximation_indexed_constraints;
    details::RuleToRuleToConstraintInfos m_binary_overapproximation_indexed_constraints;

    details::LiteralToRuleInfos m_predicate_to_anchors;
    details::LiteralToRuleInfos m_unary_overapproximation_predicate_to_anchors;
    details::LiteralToRuleInfos m_binary_overapproximation_predicate_to_anchors;
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
