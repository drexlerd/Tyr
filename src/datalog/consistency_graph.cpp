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

#include "tyr/datalog/consistency_graph.hpp"

#include "tyr/analysis/domains.hpp"
#include "tyr/common/closed_interval.hpp"
#include "tyr/datalog/assignment_sets.hpp"
#include "tyr/datalog/declarations.hpp"
#include "tyr/datalog/formatter.hpp"
#include "tyr/formalism/arithmetic_operator_utils.hpp"
#include "tyr/formalism/boolean_operator_utils.hpp"
#include "tyr/formalism/datalog/arity.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/datalog/grounder.hpp"
#include "tyr/formalism/datalog/views.hpp"

#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <optional>
#include <ranges>
#include <sstream>
#include <vector>

using namespace tyr::formalism;
using namespace tyr::formalism::datalog;

namespace tyr::datalog
{
namespace details
{

/**
 * Forward declarations
 */

template<std::ranges::forward_range Range, FactKind T>
ClosedInterval<float_t>
compute_tightest_closed_interval_helper(ClosedInterval<float_t> bounds, const FunctionAssignmentSet<T>& sets, const Range& range) noexcept;

template<FactKind T>
ClosedInterval<float_t> compute_tightest_closed_interval(View<Index<FunctionTerm<T>>, Repository> function_term,
                                                         const Vertex& element,
                                                         const FunctionAssignmentSets<T>& function_assignment_sets) noexcept;

template<FactKind T>
ClosedInterval<float_t> compute_tightest_closed_interval(View<Index<FunctionTerm<T>>, Repository> function_term,
                                                         const Edge& element,
                                                         const FunctionAssignmentSets<T>& function_skeleton_assignment_sets) noexcept;

template<typename StructureType>
auto evaluate_partially(float_t element, const StructureType&, const AssignmentSets&);

template<typename StructureType, ArithmeticOpKind O>
auto evaluate_partially(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, Repository> element,
                        const StructureType& graph_entity,
                        const AssignmentSets& assignment_sets);

template<typename StructureType, OpKind O>
auto evaluate_partially(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, Repository> element,
                        const StructureType& graph_entity,
                        const AssignmentSets& assignment_sets);

template<typename StructureType, ArithmeticOpKind O>
auto evaluate_partially(View<Index<MultiOperator<O, Data<FunctionExpression>>>, Repository> element,
                        const StructureType& graph_entity,
                        const AssignmentSets& assignment_sets);

template<typename StructureType, FactKind T>
auto evaluate_partially(View<Index<FunctionTerm<T>>, Repository> element, const StructureType& graph_entity, const AssignmentSets& assignment_sets);

template<typename StructureType>
auto evaluate_partially(View<Data<FunctionExpression>, Repository> element, const StructureType& graph_entity, const AssignmentSets& assignment_sets);

template<typename StructureType>
auto evaluate_partially(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, Repository> element,
                        const StructureType& graph_entity,
                        const AssignmentSets& assignment_sets);

template<typename StructureType>
auto evaluate_partially(View<Data<BooleanOperator<Data<FunctionExpression>>>, Repository> element,
                        const StructureType& graph_entity,
                        const AssignmentSets& assignment_sets);

template<typename StructureType>
bool is_satisfiable(View<Data<BooleanOperator<Data<FunctionExpression>>>, Repository> element,
                    const StructureType& graph_entity,
                    const AssignmentSets& assignment_sets) noexcept;

/**
 * VertexIndexIterator
 */

class VertexAssignmentIterator
{
private:
    const View<DataList<Term>, Repository>* m_terms;
    const Vertex* m_vertex;
    uint_t m_pos;

    VertexAssignment m_assignment;

    const View<DataList<Term>, Repository>& get_terms() const noexcept { return *m_terms; }
    const Vertex& get_vertex() const noexcept { return *m_vertex; }

    void advance() noexcept
    {
        ++m_pos;

        /* Try to advance index. */
        for (auto index = m_assignment.index + 1; index < ParameterIndex(get_terms().size()); ++index)
        {
            auto object = get_vertex().get_object_if_overlap(get_terms()[uint_t(index)]);

            if (object != Index<Object>::max())
            {
                m_assignment.index = index;
                m_assignment.object = object;
                return;  ///< successfully generated vertex
            }
        }

        /* Failed to generate valid vertex assignment. */
        m_pos = std::numeric_limits<uint_t>::max();  ///< mark end of iteration
    }

public:
    using difference_type = std::ptrdiff_t;
    using value_type = VertexAssignment;
    using pointer = value_type*;
    using reference = const value_type&;
    using iterator_category = std::forward_iterator_tag;

    VertexAssignmentIterator() : m_terms(nullptr), m_vertex(nullptr), m_pos(0), m_assignment() {}
    VertexAssignmentIterator(const View<DataList<Term>, Repository>& terms, const Vertex& vertex, bool begin) noexcept :
        m_terms(&terms),
        m_vertex(&vertex),
        m_pos(begin ? 0 : std::numeric_limits<uint_t>::max()),
        m_assignment()
    {
        if (begin)
        {
            advance();  // first advance might result in end immediately, e.g., if terms is empty.
        }
    }
    reference operator*() const noexcept { return m_assignment; }
    VertexAssignmentIterator& operator++() noexcept
    {
        advance();
        return *this;
    }
    VertexAssignmentIterator operator++(int) noexcept
    {
        VertexAssignmentIterator tmp = *this;
        ++(*this);
        return tmp;
    }
    bool operator==(const VertexAssignmentIterator& other) const noexcept { return m_pos == other.m_pos; }
    bool operator!=(const VertexAssignmentIterator& other) const noexcept { return !(*this == other); }
};

class VertexAssignmentRange
{
private:
    View<DataList<Term>, Repository> m_terms;
    const Vertex& m_vertex;

public:
    VertexAssignmentRange(View<DataList<Term>, Repository> terms, const Vertex& vertex) noexcept : m_terms(terms), m_vertex(vertex) {}

    auto begin() const noexcept { return VertexAssignmentIterator(m_terms, m_vertex, true); }

    auto end() const noexcept { return VertexAssignmentIterator(m_terms, m_vertex, false); }
};

/// @brief `EdgeAssignmentIterator` is used to generate vertices and edges in the consistency graph.
/// It is used in literals
///
/// It simultaneously iterates over vertices [x/o] and edges [x/o],[y/o'] with o < o'
/// to avoid having iterating over literals or numeric constraints twice.
class EdgeAssignmentIterator
{
private:
    const View<DataList<Term>, Repository>* m_terms;
    const Edge* m_edge;
    uint_t m_pos;

    EdgeAssignment m_assignment;

    const View<DataList<Term>, Repository>& get_terms() const noexcept { return *m_terms; }
    const Edge& get_edge() const noexcept { return *m_edge; }

    void advance() noexcept
    {
        ++m_pos;

        if (m_assignment.second_index == ParameterIndex::max())
        {
            /* Try to advance first_index. */

            // Reduced branching by setting iterator index and unsetting first index.
            // Note: unsetting first object is unnecessary because it will either be set or the iterator reaches its end.
            auto first_index = m_assignment.first_index + 1;
            m_assignment.first_index = ParameterIndex::max();

            for (; first_index < ParameterIndex(get_terms().size()); ++first_index)
            {
                auto first_object = get_edge().get_object_if_overlap(get_terms()[uint_t(first_index)]);

                if (first_object != Index<Object>::max())
                {
                    m_assignment.first_index = first_index;
                    m_assignment.first_object = first_object;
                    m_assignment.second_index = first_index;
                    break;  ///< successfully generated left vertex
                }
            }
        }

        if (m_assignment.first_index != ParameterIndex::max())
        {
            /* Try to advance second_index. */

            // Reduced branching by setting iterator index and unsetting second index and object
            auto second_index = m_assignment.second_index + 1;
            m_assignment.second_index = ParameterIndex::max();
            m_assignment.second_object = Index<Object>::max();

            for (; second_index < ParameterIndex(get_terms().size()); ++second_index)
            {
                auto second_object = get_edge().get_object_if_overlap(get_terms()[uint_t(second_index)]);

                if (second_object != Index<Object>::max())
                {
                    m_assignment.second_index = second_index;
                    m_assignment.second_object = second_object;
                    return;  ///< successfully generated right vertex => successfully generated edge
                }
            }
        }

        if (m_assignment.second_object == Index<Object>::max())
        {
            /* Failed to generate valid edge assignment */

            m_pos = std::numeric_limits<uint_t>::max();  ///< mark end of iteration
        }
    }

public:
    using difference_type = std::ptrdiff_t;
    using value_type = EdgeAssignment;
    using pointer = value_type*;
    using reference = const value_type&;
    using iterator_category = std::forward_iterator_tag;

    EdgeAssignmentIterator() : m_terms(nullptr), m_edge(nullptr), m_pos(0), m_assignment() {}
    EdgeAssignmentIterator(const View<DataList<Term>, Repository>& terms, const Edge& edge, bool begin) noexcept :
        m_terms(&terms),
        m_edge(&edge),
        m_pos(begin ? 0 : std::numeric_limits<uint_t>::max()),
        m_assignment()
    {
        if (begin)
        {
            advance();  // first advance might result in end immediately, e.g., if terms is empty.
        }
    }
    reference operator*() const noexcept { return m_assignment; }
    EdgeAssignmentIterator& operator++() noexcept
    {
        advance();
        return *this;
    }
    EdgeAssignmentIterator operator++(int) noexcept
    {
        EdgeAssignmentIterator tmp = *this;
        ++(*this);
        return tmp;
    }
    bool operator==(const EdgeAssignmentIterator& other) const noexcept { return m_pos == other.m_pos; }
    bool operator!=(const EdgeAssignmentIterator& other) const noexcept { return !(*this == other); }
};

class EdgeAssignmentRange
{
private:
    View<DataList<Term>, Repository> m_terms;
    const Edge& m_edge;

public:
    EdgeAssignmentRange(View<DataList<Term>, Repository> terms, const Edge& edge) noexcept : m_terms(terms), m_edge(edge) {}

    auto begin() const noexcept { return EdgeAssignmentIterator(m_terms, m_edge, true); }

    auto end() const noexcept { return EdgeAssignmentIterator(m_terms, m_edge, false); }
};

/**
 * Vertex
 */

Vertex::Vertex(uint_t index, ParameterIndex parameter_index, Index<Object> object_index) noexcept :
    m_index(index),
    m_parameter_index(parameter_index),
    m_object_index(object_index)
{
}

template<FactKind T>
bool Vertex::consistent_literals(View<IndexList<Literal<T>>, Repository> literals, const PredicateAssignmentSets<T>& predicate_assignment_sets) const noexcept
{
    for (const auto& literal : literals)
    {
        const auto atom = literal.get_atom();
        const auto predicate = atom.get_predicate();

        assert(effective_arity(literal) >= 1);  ///< We test nullary literals separately

        const auto negated = !literal.get_polarity();

        assert(!negated || effective_arity(literal) == 1);  ///< Can only handly unary negated literals due to overapproximation

        const auto& predicate_assignment_set = predicate_assignment_sets.get_set(predicate.get_index());
        const auto terms = atom.get_terms();

        for (const auto& assignment : VertexAssignmentRange(terms, *this))
        {
            assert(assignment.is_valid());

            const auto true_assignment = predicate_assignment_set.at(assignment);

            if (negated == true_assignment)
                return false;
        }
    }

    return true;
}

template bool Vertex::consistent_literals(View<IndexList<Literal<StaticTag>>, Repository> literals,
                                          const PredicateAssignmentSets<StaticTag>& predicate_assignment_sets) const noexcept;
template bool Vertex::consistent_literals(View<IndexList<Literal<FluentTag>>, Repository> literals,
                                          const PredicateAssignmentSets<FluentTag>& predicate_assignment_sets) const noexcept;

bool Vertex::consistent_numeric_constraints(View<DataList<BooleanOperator<Data<FunctionExpression>>>, Repository> numeric_constraints,
                                            const AssignmentSets& assignment_sets) const noexcept
{
    for (const auto numeric_constraint : numeric_constraints)
    {
        assert(effective_arity(numeric_constraint) >= 1);  ///< We test nullary constraints separately.

        if (!is_satisfiable(numeric_constraint, *this, assignment_sets))
            return false;
    }

    return true;
}

Index<Object> Vertex::get_object_if_overlap(View<Data<Term>, Repository> term) const noexcept
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, ParameterIndex>)
            {
                if (m_parameter_index == arg)
                    return m_object_index;
                else
                    return Index<Object>::max();
            }
            else if constexpr (std::is_same_v<Alternative, View<Index<Object>, Repository>>)
                return arg.get_index();
            else
                static_assert(dependent_false<Alternative>::value, "Missing case");
        },
        term.get_variant());
}

uint_t Vertex::get_index() const noexcept { return m_index; }

ParameterIndex Vertex::get_parameter_index() const noexcept { return m_parameter_index; }

Index<Object> Vertex::get_object_index() const noexcept { return m_object_index; }

/**
 * Edge
 */

Edge::Edge(Vertex src, Vertex dst) noexcept : m_src(std::move(src)), m_dst(std::move(dst)) {}

template<FactKind T>
bool Edge::consistent_literals(View<IndexList<Literal<T>>, Repository> literals, const PredicateAssignmentSets<T>& predicate_assignment_sets) const noexcept
{
    for (const auto& literal : literals)
    {
        const auto atom = literal.get_atom();
        const auto predicate = atom.get_predicate();

        assert(effective_arity(literal) >= 2);  ///< We test nullary and unary literals separately.

        const auto negated = !literal.get_polarity();

        assert(!negated || effective_arity(literal) == 2);  ///< Can only handly binary negated literals due to overapproximation

        const auto& predicate_assignment_set = predicate_assignment_sets.get_set(predicate.get_index());
        const auto terms = atom.get_terms();

        /* Iterate edges. */

        for (const auto& assignment : EdgeAssignmentRange(terms, *this))
        {
            assert(assignment.is_valid());

            const auto true_assignment = predicate_assignment_set.at(assignment);

            if (negated == true_assignment)
                return false;
        }
    }

    return true;
}

template bool Edge::consistent_literals(View<IndexList<Literal<StaticTag>>, Repository> literals,
                                        const PredicateAssignmentSets<StaticTag>& predicate_assignment_sets) const noexcept;
template bool Edge::consistent_literals(View<IndexList<Literal<FluentTag>>, Repository> literals,
                                        const PredicateAssignmentSets<FluentTag>& predicate_assignment_sets) const noexcept;

bool Edge::consistent_numeric_constraints(View<DataList<BooleanOperator<Data<FunctionExpression>>>, Repository> numeric_constraints,
                                          const AssignmentSets& assignment_sets) const noexcept
{
    for (const auto numeric_constraint : numeric_constraints)
    {
        assert(effective_arity(numeric_constraint) >= 2);  ///< We test nullary and unary constraints separately.

        if (!is_satisfiable(numeric_constraint, *this, assignment_sets))
            return false;
    }

    return true;
}

Index<Object> Edge::get_object_if_overlap(View<Data<Term>, Repository> term) const noexcept
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, ParameterIndex>)
            {
                if (m_src.get_parameter_index() == arg)
                    return m_src.get_object_index();
                else if (m_dst.get_parameter_index() == arg)
                    return m_dst.get_object_index();
                else
                    return Index<Object>::max();
            }
            else if constexpr (std::is_same_v<Alternative, View<Index<Object>, Repository>>)
            {
                return arg.get_index();
            }
            else
            {
                static_assert(dependent_false<Alternative>::value, "Missing case");
            }
        },
        term.get_variant());
}

const Vertex& Edge::get_src() const noexcept { return m_src; }

const Vertex& Edge::get_dst() const noexcept { return m_dst; }

}

/**
 * StaticConsistencyGraph
 */

std::pair<details::Vertices, std::vector<std::vector<uint_t>>>
StaticConsistencyGraph::compute_vertices(View<Index<ConjunctiveCondition>, Repository> condition,
                                         const analysis::DomainListList& parameter_domains,
                                         uint_t begin_parameter_index,
                                         uint_t end_parameter_index,
                                         const TaggedAssignmentSets<StaticTag>& static_assignment_sets)
{
    auto vertices = details::Vertices {};

    auto partitions = std::vector<std::vector<uint_t>> {};

    for (uint_t parameter_index = begin_parameter_index; parameter_index < end_parameter_index; ++parameter_index)
    {
        auto& parameter_domain = parameter_domains[parameter_index];

        auto partition = std::vector<uint_t> {};

        for (const auto object_index : parameter_domain)
        {
            const auto vertex_index = static_cast<uint_t>(vertices.size());

            auto vertex = details::Vertex(vertex_index, ParameterIndex(parameter_index), Index<Object>(object_index));

            assert(vertex.get_index() == vertex_index);

            if (vertex.consistent_literals(condition.get_literals<StaticTag>(), static_assignment_sets.predicate))
            {
                vertices.push_back(std::move(vertex));
                partition.push_back(vertex.get_index());
            }
        }

        partitions.push_back(partition);
    }

    return { std::move(vertices), std::move(partitions) };
}

std::tuple<std::vector<uint_t>, std::vector<uint_t>, std::vector<uint_t>>
StaticConsistencyGraph::compute_edges(View<Index<ConjunctiveCondition>, Repository> condition,
                                      const TaggedAssignmentSets<StaticTag>& static_assignment_sets,
                                      const details::Vertices& vertices)
{
    auto sources = std::vector<uint_t> {};

    auto target_offsets = std::vector<uint_t> {};
    target_offsets.reserve(vertices.size());

    auto targets = std::vector<uint_t> {};

    for (uint_t first_vertex_index = 0; first_vertex_index < vertices.size(); ++first_vertex_index)
    {
        const auto targets_before = targets.size();
        for (uint_t second_vertex_index = (first_vertex_index + 1); second_vertex_index < vertices.size(); ++second_vertex_index)
        {
            const auto& first_vertex = vertices.at(first_vertex_index);
            const auto& second_vertex = vertices.at(second_vertex_index);

            assert(first_vertex.get_index() == first_vertex_index);
            assert(second_vertex.get_index() == second_vertex_index);

            auto edge = details::Edge(first_vertex, second_vertex);

            // Part 1 of definition of substitution consistency graph (Stahlberg-ecai2023): exclude I^\neq
            if (first_vertex.get_parameter_index() != second_vertex.get_parameter_index()
                && edge.consistent_literals(condition.get_literals<StaticTag>(), static_assignment_sets.predicate))
            {
                targets.push_back(second_vertex_index);
            }
        }

        if (targets_before < targets.size())
        {
            sources.push_back(first_vertex_index);
            target_offsets.push_back(targets.size());
        }
    }

    return { std::move(sources), std::move(target_offsets), std::move(targets) };
}

StaticConsistencyGraph::StaticConsistencyGraph(View<Index<ConjunctiveCondition>, Repository> condition,
                                               View<Index<ConjunctiveCondition>, Repository> unary_overapproximation_condition,
                                               View<Index<ConjunctiveCondition>, Repository> binary_overapproximation_condition,
                                               const analysis::DomainListList& parameter_domains,
                                               uint_t begin_parameter_index,
                                               uint_t end_parameter_index,
                                               const TaggedAssignmentSets<StaticTag>& static_assignment_sets) :
    m_condition(condition),
    m_unary_overapproximation_condition(unary_overapproximation_condition),
    m_binary_overapproximation_condition(binary_overapproximation_condition)
{
    auto [vertices_, partitions_] =
        compute_vertices(unary_overapproximation_condition, parameter_domains, begin_parameter_index, end_parameter_index, static_assignment_sets);
    m_vertices = std::move(vertices_);
    m_partitions = std::move(partitions_);

    auto [sources_, target_offsets_, targets_] = compute_edges(binary_overapproximation_condition, static_assignment_sets, m_vertices);

    m_sources = std::move(sources_);
    m_target_offsets = std::move(target_offsets_);
    m_targets = std::move(targets_);
}

const details::Vertex& StaticConsistencyGraph::get_vertex(uint_t index) const noexcept { return m_vertices[index]; }

size_t StaticConsistencyGraph::get_num_vertices() const noexcept { return m_vertices.size(); }

size_t StaticConsistencyGraph::get_num_edges() const noexcept { return m_targets.size(); }

View<Index<ConjunctiveCondition>, Repository> StaticConsistencyGraph::get_condition() const noexcept { return m_condition; }

const std::vector<std::vector<uint_t>>& StaticConsistencyGraph::get_partitions() const noexcept { return m_partitions; }

const StaticConsistencyGraph& StaticConsistencyGraph::EdgeIterator::get_graph() const noexcept
{
    assert(m_graph);
    return *m_graph;
}

void StaticConsistencyGraph::EdgeIterator::advance() noexcept
{
    if (++m_targets_pos >= get_graph().m_target_offsets[m_sources_pos])
        ++m_sources_pos;
}

StaticConsistencyGraph::EdgeIterator::EdgeIterator() noexcept : m_graph(nullptr), m_sources_pos(0), m_targets_pos(0) {}

StaticConsistencyGraph::EdgeIterator::EdgeIterator(const StaticConsistencyGraph& graph, bool begin) noexcept :
    m_graph(&graph),
    m_sources_pos(begin ? 0 : graph.m_sources.size()),
    m_targets_pos(begin ? 0 : graph.m_targets.size())
{
}

StaticConsistencyGraph::EdgeIterator::value_type StaticConsistencyGraph::EdgeIterator::operator*() const noexcept
{
    assert(m_sources_pos < get_graph().m_sources.size());
    assert(m_targets_pos < get_graph().m_targets.size());
    return details::Edge(get_graph().m_vertices[get_graph().m_sources[m_sources_pos]], get_graph().m_vertices[get_graph().m_targets[m_targets_pos]]);
}

StaticConsistencyGraph::EdgeIterator& StaticConsistencyGraph::EdgeIterator::operator++() noexcept
{
    advance();
    return *this;
}

StaticConsistencyGraph::EdgeIterator StaticConsistencyGraph::EdgeIterator::operator++(int) noexcept
{
    EdgeIterator tmp = *this;
    ++(*this);
    return tmp;
}

bool StaticConsistencyGraph::EdgeIterator::operator==(const StaticConsistencyGraph::EdgeIterator& other) const noexcept
{
    return m_targets_pos == other.m_targets_pos && m_sources_pos == other.m_sources_pos;
}

bool StaticConsistencyGraph::EdgeIterator::operator!=(const StaticConsistencyGraph::EdgeIterator& other) const noexcept { return !(*this == other); }

namespace details
{

template<std::ranges::forward_range Range, FactKind T>
ClosedInterval<float_t>
compute_tightest_closed_interval_helper(ClosedInterval<float_t> bounds, const FunctionAssignmentSet<T>& sets, const Range& range) noexcept
{
    if (empty(bounds))
        return bounds;

    for (const auto& assignment : range)
    {
        assert(assignment.is_valid());

        bounds = intersect(bounds, sets.at(assignment));
        if (empty(bounds))
            break;  // early exit
    }
    return bounds;
}

template<FactKind T>
ClosedInterval<float_t> compute_tightest_closed_interval(View<Index<FunctionTerm<T>>, Repository> function_term,
                                                         const Vertex& element,
                                                         const FunctionAssignmentSets<T>& function_assignment_sets) noexcept
{
    const auto& function_assignment_set = function_assignment_sets.get_set(function_term.get_function().get_index());

    const auto terms = function_term.get_terms();

    auto bounds = function_assignment_set[EmptyAssignment()];

    return compute_tightest_closed_interval_helper(bounds, function_assignment_set, VertexAssignmentRange(terms, element));
}

template<FactKind T>
ClosedInterval<float_t> compute_tightest_closed_interval(View<Index<FunctionTerm<T>>, Repository> function_term,
                                                         const Edge& element,
                                                         const FunctionAssignmentSets<T>& function_skeleton_assignment_sets) noexcept
{
    const auto& function_skeleton_assignment_set = function_skeleton_assignment_sets.get_set(function_term.get_function().get_index());

    const auto terms = function_term.get_terms();

    auto bounds = function_skeleton_assignment_set[EmptyAssignment()];

    bounds = compute_tightest_closed_interval_helper(bounds, function_skeleton_assignment_set, VertexAssignmentRange(terms, element.get_src()));
    bounds = compute_tightest_closed_interval_helper(bounds, function_skeleton_assignment_set, VertexAssignmentRange(terms, element.get_dst()));
    bounds = compute_tightest_closed_interval_helper(bounds, function_skeleton_assignment_set, EdgeAssignmentRange(terms, element));

    return bounds;
}

template<typename StructureType>
auto evaluate_partially(float_t element, const StructureType&, const AssignmentSets&)
{
    return ClosedInterval<float_t>(element, element);
}

template<typename StructureType, ArithmeticOpKind O>
auto evaluate_partially(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, Repository> element,
                        const StructureType& graph_entity,
                        const AssignmentSets& assignment_sets)
{
    return apply(O {}, evaluate_partially(element.get_arg(), graph_entity, assignment_sets));
}

template<typename StructureType, OpKind O>
auto evaluate_partially(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, Repository> element,
                        const StructureType& graph_entity,
                        const AssignmentSets& assignment_sets)
{
    return apply(O {},
                 evaluate_partially(element.get_lhs(), graph_entity, assignment_sets),
                 evaluate_partially(element.get_rhs(), graph_entity, assignment_sets));
}

template<typename StructureType, ArithmeticOpKind O>
auto evaluate_partially(View<Index<MultiOperator<O, Data<FunctionExpression>>>, Repository> element,
                        const StructureType& graph_entity,
                        const AssignmentSets& assignment_sets)
{
    const auto child_fexprs = element.get_args();

    return std::accumulate(std::next(child_fexprs.begin()),  // Start from the second expression
                           child_fexprs.end(),
                           evaluate_partially(child_fexprs.front(), graph_entity, assignment_sets),
                           [&](const auto& value, const auto& child_expr)
                           { return apply(O {}, value, evaluate_partially(child_expr, graph_entity, assignment_sets)); });
}

template<typename StructureType, FactKind T>
auto evaluate_partially(View<Index<FunctionTerm<T>>, Repository> element, const StructureType& graph_entity, const AssignmentSets& assignment_sets)
{
    return compute_tightest_closed_interval(element, graph_entity, assignment_sets.template get<T>().function);
}

template<typename StructureType>
auto evaluate_partially(View<Data<FunctionExpression>, Repository> element, const StructureType& graph_entity, const AssignmentSets& assignment_sets)
{
    return visit([&](auto&& arg) { return evaluate_partially(arg, graph_entity, assignment_sets); }, element.get_variant());
}

template<typename StructureType>
auto evaluate_partially(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, Repository> element,
                        const StructureType& graph_entity,
                        const AssignmentSets& assignment_sets)
{
    return visit([&](auto&& arg) { return evaluate_partially(arg, graph_entity, assignment_sets); }, element.get_variant());
}

template<typename StructureType>
auto evaluate_partially(View<Data<BooleanOperator<Data<FunctionExpression>>>, Repository> element,
                        const StructureType& graph_entity,
                        const AssignmentSets& assignment_sets)
{
    return visit([&](auto&& arg) { return evaluate_partially(arg, graph_entity, assignment_sets); }, element.get_variant());
}

template<typename StructureType>
bool is_satisfiable(View<Data<BooleanOperator<Data<FunctionExpression>>>, Repository> element,
                    const StructureType& graph_entity,
                    const AssignmentSets& assignment_sets) noexcept
{
    return visit(
        [&](auto&& arg) -> bool
        {
            using Alternative = std::decay_t<decltype(arg)>;

            return apply_existential(typename Alternative::OpType {},
                                     evaluate_partially(arg.get_lhs(), graph_entity, assignment_sets),
                                     evaluate_partially(arg.get_rhs(), graph_entity, assignment_sets));
        },
        element.get_variant());
}

}

std::pair<Index<GroundConjunctiveCondition>, bool>
create_ground_nullary_condition(View<Index<ConjunctiveCondition>, Repository> condition, Builder& builder, Repository& context)
{
    auto conj_cond_ptr = builder.get_builder<GroundConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    auto binding_empty = IndexList<Object> {};
    auto grounder_context = GrounderContext { builder, context, binding_empty };

    for (const auto literal : condition.get_literals<StaticTag>())
        if (effective_arity(literal) == 0)
            conj_cond.static_literals.push_back(ground(literal, grounder_context).first);

    for (const auto literal : condition.get_literals<FluentTag>())
        if (effective_arity(literal) == 0)
            conj_cond.fluent_literals.push_back(ground(literal, grounder_context).first);

    for (const auto numeric_constraint : condition.get_numeric_constraints())
        if (effective_arity(numeric_constraint) == 0)
            conj_cond.numeric_constraints.push_back(ground(numeric_constraint, grounder_context));

    canonicalize(conj_cond);
    return context.get_or_create(conj_cond, builder.get_buffer());
}

std::pair<Index<ConjunctiveCondition>, bool>
create_overapproximation_conjunctive_condition(size_t k, View<Index<ConjunctiveCondition>, Repository> condition, Builder& builder, Repository& context)
{
    auto conj_cond_ptr = builder.get_builder<ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto variable : condition.get_variables())
        conj_cond.variables.push_back(variable.get_index());

    for (const auto literal : condition.get_literals<StaticTag>())
        if ((!literal.get_polarity() && effective_arity(literal) == k) || (literal.get_polarity() && effective_arity(literal) >= k))
            conj_cond.static_literals.push_back(literal.get_index());

    for (const auto literal : condition.get_literals<FluentTag>())
        if ((!literal.get_polarity() && effective_arity(literal) == k) || (literal.get_polarity() && effective_arity(literal) >= k))
            conj_cond.fluent_literals.push_back(literal.get_index());

    for (const auto numeric_constraint : condition.get_numeric_constraints())
        if (effective_arity(numeric_constraint) >= k)
            conj_cond.numeric_constraints.push_back(numeric_constraint.get_data());

    canonicalize(conj_cond);
    return context.get_or_create(conj_cond, builder.get_buffer());
}

std::pair<Index<ConjunctiveCondition>, bool> create_overapproximation_conflicting_conjunctive_condition(size_t k,
                                                                                                        View<Index<ConjunctiveCondition>, Repository> condition,
                                                                                                        Builder& builder,
                                                                                                        Repository& context)
{
    auto conj_cond_ptr = builder.get_builder<ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto variable : condition.get_variables())
        conj_cond.variables.push_back(variable.get_index());

    for (const auto literal : condition.get_literals<StaticTag>())
        if (effective_arity(literal) > k)
            conj_cond.static_literals.push_back(literal.get_index());

    for (const auto literal : condition.get_literals<FluentTag>())
        if (effective_arity(literal) > k)
            conj_cond.fluent_literals.push_back(literal.get_index());

    for (const auto numeric_constraint : condition.get_numeric_constraints())
        if (effective_arity(numeric_constraint) > k)
            conj_cond.numeric_constraints.push_back(numeric_constraint.get_data());

    canonicalize(conj_cond);
    return context.get_or_create(conj_cond, builder.get_buffer());
}

}
