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
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/expression_arity.hpp"
#include "tyr/formalism/datalog/expression_properties.hpp"
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/datalog/grounder.hpp"
#include "tyr/formalism/datalog/views.hpp"

#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <optional>
#include <ranges>
#include <sstream>
#include <vector>

namespace f = tyr::formalism;
namespace fd = tyr::formalism::datalog;

namespace tyr::datalog
{
namespace details
{

/**
 * Forward declarations
 */

template<std::ranges::forward_range Range, f::FactKind T>
ClosedInterval<float_t>
compute_tightest_closed_interval_helper(ClosedInterval<float_t> bounds, const FunctionAssignmentSet<T>& sets, const Range& range) noexcept;

template<f::FactKind T>
ClosedInterval<float_t> compute_tightest_closed_interval(View<Index<fd::FunctionTerm<T>>, fd::Repository> function_term,
                                                         const Vertex& element,
                                                         const FunctionAssignmentSets<T>& function_assignment_sets) noexcept;

template<f::FactKind T>
ClosedInterval<float_t> compute_tightest_closed_interval(View<Index<fd::FunctionTerm<T>>, fd::Repository> function_term,
                                                         const Edge& element,
                                                         const FunctionAssignmentSets<T>& function_skeleton_assignment_sets) noexcept;

template<typename StructureType>
auto evaluate_partially(float_t element, const StructureType&, const AssignmentSets&);

template<typename StructureType, f::ArithmeticOpKind O>
auto evaluate_partially(View<Index<fd::UnaryOperator<O, Data<fd::FunctionExpression>>>, fd::Repository> element,
                        const StructureType& graph_entity,
                        const AssignmentSets& assignment_sets);

template<typename StructureType, f::OpKind O>
auto evaluate_partially(View<Index<fd::BinaryOperator<O, Data<fd::FunctionExpression>>>, fd::Repository> element,
                        const StructureType& graph_entity,
                        const AssignmentSets& assignment_sets);

template<typename StructureType, f::ArithmeticOpKind O>
auto evaluate_partially(View<Index<fd::MultiOperator<O, Data<fd::FunctionExpression>>>, fd::Repository> element,
                        const StructureType& graph_entity,
                        const AssignmentSets& assignment_sets);

template<typename StructureType, f::FactKind T>
auto evaluate_partially(View<Index<fd::FunctionTerm<T>>, fd::Repository> element, const StructureType& graph_entity, const AssignmentSets& assignment_sets);

template<typename StructureType>
auto evaluate_partially(View<Data<fd::FunctionExpression>, fd::Repository> element, const StructureType& graph_entity, const AssignmentSets& assignment_sets);

template<typename StructureType>
auto evaluate_partially(View<Data<fd::ArithmeticOperator<Data<fd::FunctionExpression>>>, fd::Repository> element,
                        const StructureType& graph_entity,
                        const AssignmentSets& assignment_sets);

template<typename StructureType>
auto evaluate_partially(View<Data<fd::BooleanOperator<Data<fd::FunctionExpression>>>, fd::Repository> element,
                        const StructureType& graph_entity,
                        const AssignmentSets& assignment_sets);

template<typename StructureType>
bool is_satisfiable(View<Data<fd::BooleanOperator<Data<fd::FunctionExpression>>>, fd::Repository> element,
                    const StructureType& graph_entity,
                    const AssignmentSets& assignment_sets) noexcept;

/**
 * VertexIndexIterator
 */

class VertexAssignmentIterator
{
private:
    const View<DataList<f::Term>, fd::Repository>* m_terms;
    const Vertex* m_vertex;
    uint_t m_pos;

    VertexAssignment m_assignment;

    const View<DataList<f::Term>, fd::Repository>& get_terms() const noexcept { return *m_terms; }
    const Vertex& get_vertex() const noexcept { return *m_vertex; }

    void advance() noexcept
    {
        ++m_pos;

        /* Try to advance index. */
        for (auto index = m_assignment.index + 1; index < f::ParameterIndex(get_terms().size()); ++index)
        {
            auto object = get_vertex().get_object_if_overlap(get_terms()[uint_t(index)]);

            if (object != Index<f::Object>::max())
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
    VertexAssignmentIterator(const View<DataList<f::Term>, fd::Repository>& terms, const Vertex& vertex, bool begin) noexcept :
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
    View<DataList<f::Term>, fd::Repository> m_terms;
    const Vertex& m_vertex;

public:
    VertexAssignmentRange(View<DataList<f::Term>, fd::Repository> terms, const Vertex& vertex) noexcept : m_terms(terms), m_vertex(vertex) {}

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
    const View<DataList<f::Term>, fd::Repository>* m_terms;
    const Edge* m_edge;
    uint_t m_pos;

    EdgeAssignment m_assignment;

    const View<DataList<f::Term>, fd::Repository>& get_terms() const noexcept { return *m_terms; }
    const Edge& get_edge() const noexcept { return *m_edge; }

    void advance() noexcept
    {
        ++m_pos;

        if (m_assignment.second_index == f::ParameterIndex::max())
        {
            /* Try to advance first_index. */

            // Reduced branching by setting iterator index and unsetting first index.
            // Note: unsetting first object is unnecessary because it will either be set or the iterator reaches its end.
            auto first_index = m_assignment.first_index + 1;
            m_assignment.first_index = f::ParameterIndex::max();

            for (; first_index < f::ParameterIndex(get_terms().size()); ++first_index)
            {
                auto first_object = get_edge().get_object_if_overlap(get_terms()[uint_t(first_index)]);

                if (first_object != Index<f::Object>::max())
                {
                    m_assignment.first_index = first_index;
                    m_assignment.first_object = first_object;
                    m_assignment.second_index = first_index;
                    break;  ///< successfully generated left vertex
                }
            }
        }

        if (m_assignment.first_index != f::ParameterIndex::max())
        {
            /* Try to advance second_index. */

            // Reduced branching by setting iterator index and unsetting second index and object
            auto second_index = m_assignment.second_index + 1;
            m_assignment.second_index = f::ParameterIndex::max();
            m_assignment.second_object = Index<f::Object>::max();

            for (; second_index < f::ParameterIndex(get_terms().size()); ++second_index)
            {
                auto second_object = get_edge().get_object_if_overlap(get_terms()[uint_t(second_index)]);

                if (second_object != Index<f::Object>::max())
                {
                    m_assignment.second_index = second_index;
                    m_assignment.second_object = second_object;
                    return;  ///< successfully generated right vertex => successfully generated edge
                }
            }
        }

        if (m_assignment.second_object == Index<f::Object>::max())
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
    EdgeAssignmentIterator(const View<DataList<f::Term>, fd::Repository>& terms, const Edge& edge, bool begin) noexcept :
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
    View<DataList<f::Term>, fd::Repository> m_terms;
    const Edge& m_edge;

public:
    EdgeAssignmentRange(View<DataList<f::Term>, fd::Repository> terms, const Edge& edge) noexcept : m_terms(terms), m_edge(edge) {}

    auto begin() const noexcept { return EdgeAssignmentIterator(m_terms, m_edge, true); }

    auto end() const noexcept { return EdgeAssignmentIterator(m_terms, m_edge, false); }
};

/**
 * Vertex
 */

Vertex::Vertex(uint_t index, f::ParameterIndex parameter_index, Index<f::Object> object_index) noexcept :
    m_index(index),
    m_parameter_index(parameter_index),
    m_object_index(object_index)
{
}

template<f::FactKind T>
bool Vertex::consistent_literals(View<IndexList<fd::Literal<T>>, fd::Repository> literals,
                                 const PredicateAssignmentSets<T>& predicate_assignment_sets) const noexcept
{
    for (const auto& literal : literals)
    {
        const auto atom = literal.get_atom();
        const auto predicate = atom.get_predicate();

        assert(kpkc_arity(literal) > 0);  ///< We test nullary literals separately

        const auto negated = !literal.get_polarity();

        assert(!negated || kpkc_arity(literal) == 1);  ///< Can only handly unary negated literals due to overapproximation

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

template bool Vertex::consistent_literals(View<IndexList<fd::Literal<f::StaticTag>>, fd::Repository> literals,
                                          const PredicateAssignmentSets<f::StaticTag>& predicate_assignment_sets) const noexcept;
template bool Vertex::consistent_literals(View<IndexList<fd::Literal<f::FluentTag>>, fd::Repository> literals,
                                          const PredicateAssignmentSets<f::FluentTag>& predicate_assignment_sets) const noexcept;

template<formalism::FactKind T>
bool Vertex::consistent_literals(const TaggedIndexedLiterals<T>& indexed_literals, const PredicateAssignmentSets<T>& predicate_assignment_sets) const noexcept
{
    // std::cout << "Vertex: " << *this << std::endl;

    for (const auto lit_id : indexed_literals.mappings.parameter_to_infos[uint_t(m_parameter_index)])
    {
        const auto& info = indexed_literals.infos[lit_id];
        const auto predicate = info.predicate;
        const auto polarity = info.polarity;

        assert(polarity || info.kpkc_arity == 1);  ///< Can only handly unary negated literals due to overapproximation

        const auto& pred_set = predicate_assignment_sets.get_set(predicate);

        for (const auto position : info.mappings.parameter_to_positions[uint_t(m_parameter_index)])
        {
            auto assignment = VertexAssignment(f::ParameterIndex(position), m_object_index);
            assert(assignment.is_valid());

            // std::cout << assignment << std::endl;

            const auto true_assignment = pred_set.at(assignment);

            if (polarity != true_assignment)
                return false;
        }
    }

    return true;
}

template bool Vertex::consistent_literals(const TaggedIndexedLiterals<f::StaticTag>& indexed_literals,
                                          const PredicateAssignmentSets<f::StaticTag>& predicate_assignment_sets) const noexcept;
template bool Vertex::consistent_literals(const TaggedIndexedLiterals<f::FluentTag>& indexed_literals,
                                          const PredicateAssignmentSets<f::FluentTag>& predicate_assignment_sets) const noexcept;

bool Vertex::consistent_numeric_constraints(View<DataList<fd::BooleanOperator<Data<fd::FunctionExpression>>>, fd::Repository> numeric_constraints,
                                            const AssignmentSets& assignment_sets) const noexcept
{
    for (const auto numeric_constraint : numeric_constraints)
    {
        assert(kpkc_arity(numeric_constraint) > 0);  ///< We test nullary constraints separately.

        if (!is_satisfiable(numeric_constraint, *this, assignment_sets))
            return false;
    }

    return true;
}

Index<f::Object> Vertex::get_object_if_overlap(View<Data<f::Term>, fd::Repository> term) const noexcept
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, f::ParameterIndex>)
            {
                if (m_parameter_index == arg)
                    return m_object_index;
                else
                    return Index<f::Object>::max();
            }
            else if constexpr (std::is_same_v<Alternative, View<Index<f::Object>, fd::Repository>>)
                return arg.get_index();
            else
                static_assert(dependent_false<Alternative>::value, "Missing case");
        },
        term.get_variant());
}

uint_t Vertex::get_index() const noexcept { return m_index; }

f::ParameterIndex Vertex::get_parameter_index() const noexcept { return m_parameter_index; }

Index<f::Object> Vertex::get_object_index() const noexcept { return m_object_index; }

/**
 * Edge
 */

Edge::Edge(uint_t index, Vertex src, Vertex dst) noexcept : m_index(index), m_src(std::move(src)), m_dst(std::move(dst)) {}

template<f::FactKind T>
bool Edge::consistent_literals(View<IndexList<fd::Literal<T>>, fd::Repository> literals,
                               const PredicateAssignmentSets<T>& predicate_assignment_sets) const noexcept
{
    for (const auto& literal : literals)
    {
        const auto atom = literal.get_atom();
        const auto predicate = atom.get_predicate();

        assert(kpkc_arity(literal) > 1);  ///< We test nullary and unary literals separately.

        const auto negated = !literal.get_polarity();

        assert(!negated || kpkc_arity(literal) == 2);  ///< Can only handly binary negated literals due to overapproximation

        const auto& pred_set = predicate_assignment_sets.get_set(predicate.get_index());
        const auto terms = atom.get_terms();

        /* Iterate edges. */

        for (const auto& assignment : EdgeAssignmentRange(terms, *this))
        {
            assert(assignment.is_valid());

            const auto true_assignment = pred_set.at(assignment);

            if (negated == true_assignment)
                return false;
        }
    }

    return true;
}

template bool Edge::consistent_literals(View<IndexList<fd::Literal<f::StaticTag>>, fd::Repository> literals,
                                        const PredicateAssignmentSets<f::StaticTag>& predicate_assignment_sets) const noexcept;
template bool Edge::consistent_literals(View<IndexList<fd::Literal<f::FluentTag>>, fd::Repository> literals,
                                        const PredicateAssignmentSets<f::FluentTag>& predicate_assignment_sets) const noexcept;

template<formalism::FactKind T>
bool Edge::consistent_literals(const TaggedIndexedLiterals<T>& indexed_literals, const PredicateAssignmentSets<T>& predicate_assignment_sets) const noexcept
{
    auto p = uint_t(m_src.get_parameter_index());
    auto q = uint_t(m_dst.get_parameter_index());
    auto obj_p = m_src.get_object_index();
    auto obj_q = m_dst.get_object_index();

    if (p > q)
    {
        std::swap(p, q);
        std::swap(obj_p, obj_q);
    }

    // std::cout << "Edge: " << p << " " << q << std::endl;

    /// positions where p/q occur in that literal
    for (const auto lit_id : indexed_literals.mappings.parameter_pairs_to_infos[p][q])
    {
        const auto& info = indexed_literals.infos[lit_id];
        const auto& pred_set = predicate_assignment_sets.get_set(info.predicate);
        const auto polarity = info.polarity;

        assert(polarity || info.kpkc_arity == 2);  ///< Can only handly binary negated literals due to overapproximation

        for (auto pos_p : info.mappings.parameter_to_positions[p])
        {
            for (auto pos_q : info.mappings.parameter_to_positions[q])
            {
                assert(pos_p != pos_q);

                auto first_pos = pos_p;
                auto second_pos = pos_q;
                auto first_obj = obj_p;
                auto second_obj = obj_q;

                if (first_pos > second_pos)
                {
                    std::swap(first_pos, second_pos);
                    std::swap(first_obj, second_obj);
                }

                auto assignment = EdgeAssignment(f::ParameterIndex(first_pos), first_obj, f::ParameterIndex(second_pos), second_obj);
                assert(assignment.is_valid());

                // std::cout << assignment << std::endl;

                const auto true_assignment = pred_set.at(assignment);
                if (polarity != true_assignment)
                    return false;
            }
        }
    }

    /// constant c with position pos_c < pos_p or pos_c > pos_p
    for (const auto lit_id : indexed_literals.mappings.parameter_to_infos_with_constants[p])
    {
        const auto& info = indexed_literals.infos[lit_id];
        const auto& pred_set = predicate_assignment_sets.get_set(info.predicate);
        const auto polarity = info.polarity;

        assert(polarity || info.kpkc_arity == 2);  ///< Can only handly binary negated literals due to overapproximation

        for (auto pos_p : info.mappings.parameter_to_positions[p])
        {
            for (const auto& [pos_c, obj_c] : info.mappings.constant_positions)
            {
                assert(pos_p != pos_c);

                auto first_pos = pos_p;
                auto second_pos = pos_c;
                auto first_obj = obj_p;
                auto second_obj = obj_c;

                if (first_pos > second_pos)
                {
                    std::swap(first_pos, second_pos);
                    std::swap(first_obj, second_obj);
                }

                auto assignment = EdgeAssignment(f::ParameterIndex(first_pos), first_obj, f::ParameterIndex(second_pos), second_obj);
                assert(assignment.is_valid());

                // std::cout << assignment << std::endl;

                if (polarity != pred_set.at(assignment))
                    return false;
            }
        }
    }

    /// constant c with position pos_c < pos_q or pos_c > pos_q
    for (const auto lit_id : indexed_literals.mappings.parameter_to_infos_with_constants[q])
    {
        const auto& info = indexed_literals.infos[lit_id];
        const auto& pred_set = predicate_assignment_sets.get_set(info.predicate);
        const auto polarity = info.polarity;

        assert(polarity || info.kpkc_arity == 2);  ///< Can only handly binary negated literals due to overapproximation

        for (auto pos_q : info.mappings.parameter_to_positions[q])
        {
            for (const auto& [pos_c, obj_c] : info.mappings.constant_positions)
            {
                assert(pos_q != pos_c);

                auto first_pos = pos_q;
                auto second_pos = pos_c;
                auto first_obj = obj_q;
                auto second_obj = obj_c;

                if (first_pos > second_pos)
                {
                    std::swap(first_pos, second_pos);
                    std::swap(first_obj, second_obj);
                }

                auto assignment = EdgeAssignment(f::ParameterIndex(first_pos), first_obj, f::ParameterIndex(second_pos), second_obj);
                assert(assignment.is_valid());

                // std::cout << assignment << std::endl;

                if (polarity != pred_set.at(assignment))
                    return false;
            }
        }
    }

    return true;
}

template bool Edge::consistent_literals(const TaggedIndexedLiterals<f::StaticTag>& indexed_literals,
                                        const PredicateAssignmentSets<f::StaticTag>& predicate_assignment_sets) const noexcept;
template bool Edge::consistent_literals(const TaggedIndexedLiterals<f::FluentTag>& indexed_literals,
                                        const PredicateAssignmentSets<f::FluentTag>& predicate_assignment_sets) const noexcept;

bool Edge::consistent_numeric_constraints(View<DataList<fd::BooleanOperator<Data<fd::FunctionExpression>>>, fd::Repository> numeric_constraints,
                                          const AssignmentSets& assignment_sets) const noexcept
{
    for (const auto numeric_constraint : numeric_constraints)
    {
        assert(kpkc_arity(numeric_constraint) > 1);  ///< We test nullary and unary constraints separately.

        if (!is_satisfiable(numeric_constraint, *this, assignment_sets))
            return false;
    }

    return true;
}

Index<f::Object> Edge::get_object_if_overlap(View<Data<f::Term>, fd::Repository> term) const noexcept
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, f::ParameterIndex>)
            {
                if (m_src.get_parameter_index() == arg)
                    return m_src.get_object_index();
                else if (m_dst.get_parameter_index() == arg)
                    return m_dst.get_object_index();
                else
                    return Index<f::Object>::max();
            }
            else if constexpr (std::is_same_v<Alternative, View<Index<f::Object>, fd::Repository>>)
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

uint_t Edge::get_index() const noexcept { return m_index; }

const Vertex& Edge::get_src() const noexcept { return m_src; }

const Vertex& Edge::get_dst() const noexcept { return m_dst; }
}

/**
 * StaticConsistencyGraph
 */

std::pair<details::Vertices, std::vector<std::vector<uint_t>>>
StaticConsistencyGraph::compute_vertices(const details::TaggedIndexedLiterals<f::StaticTag>& indexed_literals,
                                         const analysis::DomainListList& parameter_domains,
                                         uint_t begin_parameter_index,
                                         uint_t end_parameter_index,
                                         const TaggedAssignmentSets<f::StaticTag>& static_assignment_sets)
{
    auto vertices = details::Vertices {};

    auto partitions = std::vector<std::vector<uint_t>> {};

    if (constant_consistent_literals(indexed_literals, static_assignment_sets.predicate))
    {
        for (uint_t parameter_index = begin_parameter_index; parameter_index < end_parameter_index; ++parameter_index)
        {
            auto& parameter_domain = parameter_domains[parameter_index];

            auto partition = std::vector<uint_t> {};

            for (const auto object_index : parameter_domain)
            {
                const auto vertex_index = static_cast<uint_t>(vertices.size());

                auto vertex = details::Vertex(vertex_index, f::ParameterIndex(parameter_index), Index<f::Object>(object_index));

                assert(vertex.get_index() == vertex_index);

                if (vertex.consistent_literals(indexed_literals, static_assignment_sets.predicate))
                {
                    vertices.push_back(std::move(vertex));
                    partition.push_back(vertex.get_index());
                }
            }

            partitions.push_back(partition);
        }
    }
    else
    {
        // We need the partitions for the remaining code to work. Ideally we prune such actions
        partitions.resize(end_parameter_index - begin_parameter_index);
    }

    return { std::move(vertices), std::move(partitions) };
}

std::tuple<std::vector<uint_t>, std::vector<uint_t>, std::vector<uint_t>>
StaticConsistencyGraph::compute_edges(const details::TaggedIndexedLiterals<f::StaticTag>& indexed_literals,
                                      const TaggedAssignmentSets<f::StaticTag>& static_assignment_sets,
                                      const details::Vertices& vertices)
{
    auto sources = std::vector<uint_t> {};

    auto target_offsets = std::vector<uint_t> {};
    target_offsets.reserve(vertices.size() + 1);
    target_offsets.push_back(0);

    auto targets = std::vector<uint_t> {};

    if (constant_pair_consistent_literals(indexed_literals, static_assignment_sets.predicate))
    {
        for (uint_t first_vertex_index = 0; first_vertex_index < vertices.size(); ++first_vertex_index)
        {
            const auto targets_before = targets.size();
            for (uint_t second_vertex_index = (first_vertex_index + 1); second_vertex_index < vertices.size(); ++second_vertex_index)
            {
                const auto& first_vertex = vertices.at(first_vertex_index);
                const auto& second_vertex = vertices.at(second_vertex_index);

                assert(first_vertex.get_index() == first_vertex_index);
                assert(second_vertex.get_index() == second_vertex_index);

                auto edge = details::Edge(std::numeric_limits<uint_t>::max(), first_vertex, second_vertex);

                // Part 1 of definition of substitution consistency graph (Stahlberg-ecai2023): exclude I^\neq
                if (first_vertex.get_parameter_index() != second_vertex.get_parameter_index()
                    && edge.consistent_literals(indexed_literals, static_assignment_sets.predicate))
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
    }

    return { std::move(sources), std::move(target_offsets), std::move(targets) };
}

template<formalism::FactKind T>
bool StaticConsistencyGraph::constant_consistent_literals(const details::TaggedIndexedLiterals<T>& indexed_literals,
                                                          const PredicateAssignmentSets<T>& predicate_assignment_sets) const noexcept
{
    for (const auto& lit_id : indexed_literals.mappings.infos_with_constants)
    {
        const auto& info = indexed_literals.infos[lit_id];
        const auto predicate = info.predicate;
        const auto& pred_set = predicate_assignment_sets.get_set(predicate);
        const auto polarity = info.polarity;

        assert(polarity || info.kpkc_arity == 1);  ///< Can only handly unary negated literals due to overapproximation

        for (const auto& [position, object] : info.mappings.constant_positions)
        {
            auto assignment = VertexAssignment(f::ParameterIndex(position), object);
            assert(assignment.is_valid());

            // std::cout << assignment << std::endl;

            const auto true_assignment = pred_set.at(assignment);

            if (polarity != true_assignment)
                return false;
        }
    }

    return true;
}

template bool StaticConsistencyGraph::constant_consistent_literals(const details::TaggedIndexedLiterals<f::StaticTag>& indexed_literals,
                                                                   const PredicateAssignmentSets<f::StaticTag>& predicate_assignment_sets) const noexcept;
template bool StaticConsistencyGraph::constant_consistent_literals(const details::TaggedIndexedLiterals<f::FluentTag>& indexed_literals,
                                                                   const PredicateAssignmentSets<f::FluentTag>& predicate_assignment_sets) const noexcept;

template<formalism::FactKind T>
bool StaticConsistencyGraph::constant_pair_consistent_literals(const details::TaggedIndexedLiterals<T>& indexed_literals,
                                                               const PredicateAssignmentSets<T>& predicate_assignment_sets) const noexcept
{
    /// constants c,c' with positions pos_c < pos_c'
    for (const auto& lit_id : indexed_literals.mappings.infos_with_constant_pairs)
    {
        const auto& info = indexed_literals.infos[lit_id];
        const auto predicate = info.predicate;
        const auto& pred_set = predicate_assignment_sets.get_set(predicate);
        const auto polarity = info.polarity;

        assert(polarity || info.kpkc_arity == 2);  ///< Can only handly binary negated literals due to overapproximation

        for (uint_t i = 0; i < info.mappings.constant_positions.size(); ++i)
        {
            const auto& [first_pos, first_obj] = info.mappings.constant_positions[i];

            for (uint_t j = i + 1; j < info.mappings.constant_positions.size(); ++j)
            {
                const auto& [second_pos, second_obj] = info.mappings.constant_positions[j];
                assert(first_pos < second_pos);

                auto assignment = EdgeAssignment(f::ParameterIndex(first_pos), first_obj, f::ParameterIndex(second_pos), second_obj);
                assert(assignment.is_valid());

                // std::cout << assignment << std::endl;

                const auto true_assignment = pred_set.at(assignment);
                if (polarity != true_assignment)
                    return false;
            }
        }
    }

    return true;
}

template bool StaticConsistencyGraph::constant_pair_consistent_literals(const details::TaggedIndexedLiterals<f::StaticTag>& indexed_literals,
                                                                        const PredicateAssignmentSets<f::StaticTag>& predicate_assignment_sets) const noexcept;
template bool StaticConsistencyGraph::constant_pair_consistent_literals(const details::TaggedIndexedLiterals<f::FluentTag>& indexed_literals,
                                                                        const PredicateAssignmentSets<f::FluentTag>& predicate_assignment_sets) const noexcept;

template<f::FactKind T>
auto compute_tagged_indexed_literals(View<IndexList<fd::Literal<T>>, fd::Repository> literals, size_t arity)
{
    auto result = details::TaggedIndexedLiterals<T> {};

    result.mappings.parameter_to_infos = std::vector<std::vector<uint_t>>(arity);
    result.mappings.parameter_pairs_to_infos = std::vector<std::vector<std::vector<uint_t>>>(arity, std::vector<std::vector<uint_t>>(arity));
    result.mappings.parameter_to_infos_with_constants = std::vector<std::vector<uint_t>>(arity);
    result.mappings.infos_with_constants = std::vector<uint_t> {};
    result.mappings.infos_with_constant_pairs = std::vector<uint_t> {};

    for (const auto literal : literals)
    {
        auto info = details::LiteralInfo<T> {};
        info.predicate = literal.get_atom().get_predicate().get_index();
        info.polarity = literal.get_polarity();
        info.kpkc_arity = kpkc_arity(literal);
        info.mappings.constant_positions = std::vector<std::pair<uint_t, Index<formalism::Object>>> {};
        info.mappings.parameter_to_positions = std::vector<std::vector<uint_t>>(arity);

        const auto terms = literal.get_atom().get_terms();

        auto num_parameters = uint_t(0);
        auto num_constants = uint_t(0);

        for (uint_t position = 0; position < terms.size(); ++position)
        {
            const auto term = terms[position];

            visit(
                [&](auto&& arg)
                {
                    using Alternative = std::decay_t<decltype(arg)>;

                    if constexpr (std::is_same_v<Alternative, f::ParameterIndex>)
                    {
                        info.mappings.parameter_to_positions[uint_t(arg)].push_back(position);
                        ++num_parameters;
                    }
                    else if constexpr (std::is_same_v<Alternative, View<Index<f::Object>, fd::Repository>>)
                    {
                        info.mappings.constant_positions.emplace_back(position, arg.get_index());
                        ++num_constants;
                    }
                    else
                        static_assert(dependent_false<Alternative>::value, "Missing case");
                },
                term.get_variant());
        }

        auto parameters = fd::collect_parameters(literal);
        const auto index = result.infos.size();

        for (const auto param1 : parameters)
        {
            result.mappings.parameter_to_infos[uint_t(param1)].push_back(index);

            for (const auto param2 : parameters)
            {
                if (param1 >= param2)
                    continue;

                result.mappings.parameter_pairs_to_infos[uint_t(param1)][uint_t(param2)].push_back(index);
            }
        }

        if (num_constants > 0)
        {
            result.mappings.infos_with_constants.push_back(index);
            if (num_constants > 1)
                result.mappings.infos_with_constant_pairs.push_back(index);

            if (num_parameters > 0)
            {
                for (uint_t param = 0; param < arity; ++param)
                {
                    if (!info.mappings.parameter_to_positions[param].empty())
                        result.mappings.parameter_to_infos_with_constants[param].push_back(index);
                }
            }
        }

        result.infos.push_back(std::move(info));
    }

    return result;
}

template<f::FactKind T>
auto compute_tagged_indexed_fterms(View<IndexList<fd::FunctionTerm<T>>, fd::Repository> fterms, size_t arity)
{
    auto result = details::TaggedIndexedFunctionTerms<T> {};

    result.mappings.parameter_to_infos = std::vector<std::vector<uint_t>>(arity);
    result.mappings.parameter_pairs_to_infos = std::vector<std::vector<std::vector<uint_t>>>(arity, std::vector<std::vector<uint_t>>(arity));
    result.mappings.parameter_to_infos_with_constants = std::vector<std::vector<uint_t>>(arity);
    result.mappings.infos_with_constants = std::vector<uint_t> {};
    result.mappings.infos_with_constant_pairs = std::vector<uint_t> {};

    for (const auto fterm : fterms)
    {
        auto info = details::FunctionTermInfo<T> {};
        info.function = fterm.get_function().get_index();
        info.kpkc_arity = kpkc_arity(fterm);
        info.mappings.constant_positions = std::vector<std::pair<uint_t, Index<formalism::Object>>> {};
        info.mappings.parameter_to_positions = std::vector<std::vector<uint_t>>(arity);

        const auto terms = fterm.get_terms();

        auto num_parameters = uint_t(0);
        auto num_constants = uint_t(0);

        for (uint_t position = 0; position < terms.size(); ++position)
        {
            const auto term = terms[position];

            visit(
                [&](auto&& arg)
                {
                    using Alternative = std::decay_t<decltype(arg)>;

                    if constexpr (std::is_same_v<Alternative, f::ParameterIndex>)
                    {
                        info.mappings.parameter_to_positions[uint_t(arg)].push_back(position);
                        ++num_parameters;
                    }
                    else if constexpr (std::is_same_v<Alternative, View<Index<f::Object>, fd::Repository>>)
                    {
                        info.mappings.constant_positions.emplace_back(position, arg.get_index());
                        ++num_constants;
                    }
                    else
                        static_assert(dependent_false<Alternative>::value, "Missing case");
                },
                term.get_variant());
        }

        auto parameters = fd::collect_parameters(fterm);
        const auto index = result.infos.size();

        for (const auto param1 : parameters)
        {
            result.mappings.parameter_to_infos[uint_t(param1)].push_back(index);

            for (const auto param2 : parameters)
            {
                if (param1 >= param2)
                    continue;

                result.mappings.parameter_pairs_to_infos[uint_t(param1)][uint_t(param2)].push_back(index);
            }
        }

        if (num_constants > 0)
        {
            result.mappings.infos_with_constants.push_back(index);
            if (num_constants > 1)
                result.mappings.infos_with_constant_pairs.push_back(index);

            if (num_parameters > 0)
            {
                for (uint_t param = 0; param < arity; ++param)
                {
                    if (!info.mappings.parameter_to_positions[param].empty())
                        result.mappings.parameter_to_infos_with_constants[param].push_back(index);
                }
            }
        }

        result.infos.emplace(fterm.get_index(), std::move(info));
    }

    return result;
}

auto compute_constraint_info(View<Data<fd::BooleanOperator<Data<fd::FunctionExpression>>>, fd::Repository> element, size_t arity)
{
    auto result = details::ConstraintInfo {};

    auto static_fterms = fd::collect_fterms<f::StaticTag>(element);
    auto fluent_fterms = fd::collect_fterms<f::FluentTag>(element);

    result.static_infos = compute_tagged_indexed_fterms(make_view(static_fterms, element.get_context()), arity);
    result.fluent_infos = compute_tagged_indexed_fterms(make_view(fluent_fterms, element.get_context()), arity);

    return result;
}

auto compute_indexed_constraints(View<Index<fd::ConjunctiveCondition>, fd::Repository> element)
{
    auto result = details::IndexedConstraints {};
    result.infos = std::vector<details::ConstraintInfo> {};
    for (const auto constraint : element.get_numeric_constraints())
        result.infos.push_back(compute_constraint_info(constraint, element.get_arity()));
    return result;
}

auto compute_indexed_literals(View<Index<fd::ConjunctiveCondition>, fd::Repository> element)
{
    return details::IndexedLiterals { compute_tagged_indexed_literals(element.get_literals<f::StaticTag>(), element.get_arity()),
                                      compute_tagged_indexed_literals(element.get_literals<f::FluentTag>(), element.get_arity()) };
}

StaticConsistencyGraph::StaticConsistencyGraph(View<Index<formalism::datalog::Rule>, formalism::datalog::Repository> rule,
                                               View<Index<fd::ConjunctiveCondition>, fd::Repository> condition,
                                               View<Index<fd::ConjunctiveCondition>, fd::Repository> unary_overapproximation_condition,
                                               View<Index<fd::ConjunctiveCondition>, fd::Repository> binary_overapproximation_condition,
                                               const analysis::DomainListList& parameter_domains,
                                               uint_t begin_parameter_index,
                                               uint_t end_parameter_index,
                                               const TaggedAssignmentSets<f::StaticTag>& static_assignment_sets) :
    m_rule(rule),
    m_condition(condition),
    m_unary_overapproximation_condition(unary_overapproximation_condition),
    m_binary_overapproximation_condition(binary_overapproximation_condition),
    m_unary_overapproximation_indexed_literals(compute_indexed_literals(m_unary_overapproximation_condition)),
    m_binary_overapproximation_indexed_literals(compute_indexed_literals(m_binary_overapproximation_condition)),
    m_unary_overapproximation_indexed_constraints(compute_indexed_constraints(m_unary_overapproximation_condition)),
    m_binary_overapproximation_indexed_constraints(compute_indexed_constraints(m_binary_overapproximation_condition))
{
    auto [vertices_, partitions_] = compute_vertices(m_unary_overapproximation_indexed_literals.static_indexed,
                                                     parameter_domains,
                                                     begin_parameter_index,
                                                     end_parameter_index,
                                                     static_assignment_sets);
    m_vertices = std::move(vertices_);
    m_partitions = std::move(partitions_);

    auto [sources_, target_offsets_, targets_] = compute_edges(m_binary_overapproximation_indexed_literals.static_indexed, static_assignment_sets, m_vertices);

    m_sources = std::move(sources_);
    m_target_offsets = std::move(target_offsets_);
    m_targets = std::move(targets_);

    // std::cout << "Num vertices: " << m_vertices.size() << " num edges: " << m_targets.size() << std::endl;

    // std::cout << std::endl;
    // std::cout << "Unary overapproximation condition" << std::endl;
    // std::cout << m_unary_overapproximation_condition << std::endl;
    // std::cout << "Unary overapproximation indexed literals" << std::endl;
    // std::cout << m_unary_overapproximation_indexed_literals << std::endl;
    // std::cout << std::endl;
    // std::cout << "Binary overapproximation condition" << std::endl;
    // std::cout << m_binary_overapproximation_condition << std::endl;
    // std::cout << "Binary overapproximation indexed literals" << std::endl;
    // std::cout << m_binary_overapproximation_indexed_literals << std::endl;
}

const details::Vertex& StaticConsistencyGraph::get_vertex(uint_t index) const noexcept { return m_vertices[index]; }

size_t StaticConsistencyGraph::get_num_vertices() const noexcept { return m_vertices.size(); }

size_t StaticConsistencyGraph::get_num_edges() const noexcept { return m_targets.size(); }

View<Index<fd::Rule>, fd::Repository> StaticConsistencyGraph::get_rule() const noexcept { return m_rule; }

View<Index<fd::ConjunctiveCondition>, fd::Repository> StaticConsistencyGraph::get_condition() const noexcept { return m_condition; }

const std::vector<std::vector<uint_t>>& StaticConsistencyGraph::get_partitions() const noexcept { return m_partitions; }

/**
 * EdgeIterator
 */

const StaticConsistencyGraph& StaticConsistencyGraph::EdgeIterator::get_graph() const noexcept
{
    assert(m_graph);
    return *m_graph;
}

void StaticConsistencyGraph::EdgeIterator::advance() noexcept
{
    ++m_index;

    if (++m_targets_pos >= get_graph().m_target_offsets[m_sources_pos + 1])
        ++m_sources_pos;
}

StaticConsistencyGraph::EdgeIterator::EdgeIterator() noexcept : m_graph(nullptr), m_index(0), m_sources_pos(0), m_targets_pos(0) {}

StaticConsistencyGraph::EdgeIterator::EdgeIterator(const StaticConsistencyGraph& graph, bool begin) noexcept :
    m_graph(&graph),
    m_index(begin ? 0 : graph.get_num_edges()),
    m_sources_pos(begin ? 0 : graph.m_sources.size()),
    m_targets_pos(begin ? 0 : graph.m_targets.size())
{
}

StaticConsistencyGraph::EdgeIterator::value_type StaticConsistencyGraph::EdgeIterator::operator*() const noexcept
{
    assert(m_index < get_graph().get_num_edges());
    assert(m_sources_pos < get_graph().m_sources.size());
    assert(m_targets_pos < get_graph().m_targets.size());
    return details::Edge(m_index, get_graph().m_vertices[get_graph().m_sources[m_sources_pos]], get_graph().m_vertices[get_graph().m_targets[m_targets_pos]]);
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
    return m_index == other.m_index && m_targets_pos == other.m_targets_pos && m_sources_pos == other.m_sources_pos;
}

bool StaticConsistencyGraph::EdgeIterator::operator!=(const StaticConsistencyGraph::EdgeIterator& other) const noexcept { return !(*this == other); }

namespace details
{

template<std::ranges::forward_range Range, f::FactKind T>
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

template<f::FactKind T>
ClosedInterval<float_t> compute_tightest_closed_interval(View<Index<fd::FunctionTerm<T>>, fd::Repository> function_term,
                                                         const Vertex& element,
                                                         const FunctionAssignmentSets<T>& function_assignment_sets) noexcept
{
    const auto& function_assignment_set = function_assignment_sets.get_set(function_term.get_function().get_index());

    const auto terms = function_term.get_terms();

    auto bounds = function_assignment_set[EmptyAssignment()];

    return compute_tightest_closed_interval_helper(bounds, function_assignment_set, VertexAssignmentRange(terms, element));
}

template<f::FactKind T>
ClosedInterval<float_t> compute_tightest_closed_interval(View<Index<fd::FunctionTerm<T>>, fd::Repository> function_term,
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

template<typename StructureType, f::ArithmeticOpKind O>
auto evaluate_partially(View<Index<fd::UnaryOperator<O, Data<fd::FunctionExpression>>>, fd::Repository> element,
                        const StructureType& graph_entity,
                        const AssignmentSets& assignment_sets)
{
    return apply(O {}, evaluate_partially(element.get_arg(), graph_entity, assignment_sets));
}

template<typename StructureType, f::OpKind O>
auto evaluate_partially(View<Index<fd::BinaryOperator<O, Data<fd::FunctionExpression>>>, fd::Repository> element,
                        const StructureType& graph_entity,
                        const AssignmentSets& assignment_sets)
{
    return apply(O {},
                 evaluate_partially(element.get_lhs(), graph_entity, assignment_sets),
                 evaluate_partially(element.get_rhs(), graph_entity, assignment_sets));
}

template<typename StructureType, f::ArithmeticOpKind O>
auto evaluate_partially(View<Index<fd::MultiOperator<O, Data<fd::FunctionExpression>>>, fd::Repository> element,
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

template<typename StructureType, f::FactKind T>
auto evaluate_partially(View<Index<fd::FunctionTerm<T>>, fd::Repository> element, const StructureType& graph_entity, const AssignmentSets& assignment_sets)
{
    return compute_tightest_closed_interval(element, graph_entity, assignment_sets.template get<T>().function);
}

template<typename StructureType>
auto evaluate_partially(View<Data<fd::FunctionExpression>, fd::Repository> element, const StructureType& graph_entity, const AssignmentSets& assignment_sets)
{
    return visit([&](auto&& arg) { return evaluate_partially(arg, graph_entity, assignment_sets); }, element.get_variant());
}

template<typename StructureType>
auto evaluate_partially(View<Data<fd::ArithmeticOperator<Data<fd::FunctionExpression>>>, fd::Repository> element,
                        const StructureType& graph_entity,
                        const AssignmentSets& assignment_sets)
{
    return visit([&](auto&& arg) { return evaluate_partially(arg, graph_entity, assignment_sets); }, element.get_variant());
}

template<typename StructureType>
auto evaluate_partially(View<Data<fd::BooleanOperator<Data<fd::FunctionExpression>>>, fd::Repository> element,
                        const StructureType& graph_entity,
                        const AssignmentSets& assignment_sets)
{
    return visit([&](auto&& arg) { return evaluate_partially(arg, graph_entity, assignment_sets); }, element.get_variant());
}

template<typename StructureType>
bool is_satisfiable(View<Data<fd::BooleanOperator<Data<fd::FunctionExpression>>>, fd::Repository> element,
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

std::pair<Index<fd::GroundConjunctiveCondition>, bool> create_ground_nullary_condition(View<Index<fd::ConjunctiveCondition>, fd::Repository> condition,
                                                                                       fd::Repository& context)
{
    auto builder = fd::Builder {};
    auto conj_cond_ptr = builder.get_builder<fd::GroundConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    auto binding_empty = IndexList<f::Object> {};
    auto grounder_context = fd::GrounderContext { builder, context, binding_empty };

    for (const auto literal : condition.get_literals<f::StaticTag>())
        if (kpkc_arity(literal) == 0)
            conj_cond.static_literals.push_back(ground(literal, grounder_context).first);

    for (const auto literal : condition.get_literals<f::FluentTag>())
        if (kpkc_arity(literal) == 0)
            conj_cond.fluent_literals.push_back(ground(literal, grounder_context).first);

    for (const auto numeric_constraint : condition.get_numeric_constraints())
        if (kpkc_arity(numeric_constraint) == 0)
            conj_cond.numeric_constraints.push_back(ground(numeric_constraint, grounder_context));

    canonicalize(conj_cond);
    return context.get_or_create(conj_cond, builder.get_buffer());
}

std::pair<Index<fd::ConjunctiveCondition>, bool>
create_overapproximation_conjunctive_condition(size_t k, View<Index<fd::ConjunctiveCondition>, fd::Repository> condition, fd::Repository& context)
{
    auto builder = fd::Builder {};
    auto conj_cond_ptr = builder.get_builder<fd::ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto variable : condition.get_variables())
        conj_cond.variables.push_back(variable.get_index());

    for (const auto literal : condition.get_literals<f::StaticTag>())
        if ((!literal.get_polarity() && kpkc_arity(literal) == k) || (literal.get_polarity() && kpkc_arity(literal) >= k))
            conj_cond.static_literals.push_back(literal.get_index());

    for (const auto literal : condition.get_literals<f::FluentTag>())
        if ((!literal.get_polarity() && kpkc_arity(literal) == k) || (literal.get_polarity() && kpkc_arity(literal) >= k))
            conj_cond.fluent_literals.push_back(literal.get_index());

    for (const auto numeric_constraint : condition.get_numeric_constraints())
        if (kpkc_arity(numeric_constraint) >= k)
            conj_cond.numeric_constraints.push_back(numeric_constraint.get_data());

    canonicalize(conj_cond);
    return context.get_or_create(conj_cond, builder.get_buffer());
}

std::pair<Index<fd::ConjunctiveCondition>, bool>
create_overapproximation_conflicting_conjunctive_condition(size_t k, View<Index<fd::ConjunctiveCondition>, fd::Repository> condition, fd::Repository& context)
{
    auto builder = fd::Builder {};
    auto conj_cond_ptr = builder.get_builder<fd::ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto variable : condition.get_variables())
        conj_cond.variables.push_back(variable.get_index());

    for (const auto literal : condition.get_literals<f::StaticTag>())
        if (kpkc_arity(literal) > k)
            conj_cond.static_literals.push_back(literal.get_index());

    for (const auto literal : condition.get_literals<f::FluentTag>())
        if (kpkc_arity(literal) > k)
            conj_cond.fluent_literals.push_back(literal.get_index());

    for (const auto numeric_constraint : condition.get_numeric_constraints())
        if (kpkc_arity(numeric_constraint) > k)
            conj_cond.numeric_constraints.push_back(numeric_constraint.get_data());

    canonicalize(conj_cond);
    return context.get_or_create(conj_cond, builder.get_buffer());
}
}
