/*
 * Copyright (C) 2023 Dominik Drexler and Simon Stahlberg
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
#include "tyr/common/closed_interval.hpp"
#include "tyr/formalism/formalism.hpp"
#include "tyr/grounder/assignment_set.hpp"

#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <optional>
#include <ranges>
#include <sstream>
#include <vector>

namespace tyr::grounder
{

template<formalism::IsContext C>
class StaticConsistencyGraph
{
public:
    class Vertex;
    class Edge;

    /**
     * VertexIndexIterator
     */

    class VertexAssignmentIterator
    {
    private:
        SpanProxy<formalism::Term, C> m_terms;
        const Vertex* m_vertex;
        uint_t m_pos;

        VertexAssignment m_assignment;

        const SpanProxy<formalism::Term, C>& get_terms() const noexcept { return m_terms; }
        const Vertex& get_vertex() const noexcept { return *m_vertex; }

        void advance() noexcept
        {
            /* Try to advance index. */
            for (uint_t index = m_assignment.index + 1; index < get_terms().size(); ++index)
            {
                auto object = get_vertex().get_object_if_overlap(get_terms()[index]);

                if (object != std::numeric_limits<uint_t>::max())
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

        VertexAssignmentIterator() noexcept : m_terms(nullptr), m_vertex(nullptr), m_pos(std::numeric_limits<uint_t>::max()) {}
        VertexAssignmentIterator(SpanProxy<formalism::Term, C> terms, const Vertex& vertex, bool begin) noexcept :
            m_terms(terms),
            m_vertex(&vertex),
            m_pos(begin ? 0 : std::numeric_limits<uint_t>::max())
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
        SpanProxy<formalism::Term, C> m_terms;
        const Vertex& m_vertex;

    public:
        VertexAssignmentRange(SpanProxy<formalism::Term, C> terms, const Vertex& vertex) noexcept : m_terms(terms), m_vertex(vertex) {}

        VertexAssignmentIterator begin() const noexcept { return VertexAssignmentIterator(m_terms, m_vertex, true); }

        VertexAssignmentIterator end() const noexcept { return VertexAssignmentIterator(m_terms, m_vertex, false); }
    };

    /// @brief `EdgeAssignmentIterator` is used to generate vertices and edges in the consistency graph.
    /// It is used in literals
    ///
    /// It simultaneously iterates over vertices [x/o] and edges [x/o],[y/o'] with o < o'
    /// to avoid having iterating over literals or numeric constraints twice.
    class EdgeAssignmentIterator
    {
    private:
        SpanProxy<formalism::Term, C> m_terms;
        const Edge* m_edge;
        uint_t m_pos;

        EdgeAssignment m_assignment;

        const SpanProxy<formalism::Term, C>& get_terms() const noexcept { return m_terms; }
        const Edge& get_edge() const noexcept { return *m_edge; }

        void advance() noexcept
        {
            if (m_assignment.second_index == std::numeric_limits<uint_t>::max())
            {
                /* Try to advance first_index. */

                // Reduced branching by setting iterator index and unsetting first index.
                // Note: unsetting first object is unnecessary because it will either be set or the iterator reaches its end.
                uint_t first_index = m_assignment.first_index + 1;
                m_assignment.first_index = std::numeric_limits<uint_t>::max();

                for (; first_index < get_terms().size(); ++first_index)
                {
                    auto first_object = get_edge().get_object_if_overlap(get_terms()[first_index]);

                    if (first_object != std::numeric_limits<uint_t>::max())
                    {
                        m_assignment.first_index = first_index;
                        m_assignment.first_object = first_object;
                        m_assignment.second_index = first_index;
                        break;  ///< successfully generated left vertex
                    }
                }
            }

            if (m_assignment.first_index != std::numeric_limits<uint_t>::max())
            {
                /* Try to advance second_index. */

                // Reduced branching by setting iterator index and unsetting second index and object
                uint_t second_index = m_assignment.second_index + 1;
                m_assignment.second_index = std::numeric_limits<uint_t>::max();
                m_assignment.second_object = std::numeric_limits<uint_t>::max();

                for (; second_index < get_terms().size(); ++second_index)
                {
                    auto second_object = get_edge().get_object_if_overlap(get_terms()[second_index]);

                    if (second_object != std::numeric_limits<uint_t>::max())
                    {
                        m_assignment.second_index = second_index;
                        m_assignment.second_object = second_object;
                        return;  ///< successfully generated right vertex => successfully generated edge
                    }
                }
            }

            if (m_assignment.second_object == std::numeric_limits<uint_t>::max())
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

        EdgeAssignmentIterator() noexcept : m_terms(nullptr), m_edge(nullptr), m_pos(std::numeric_limits<uint_t>::max()), m_assignment() {}
        EdgeAssignmentIterator(SpanProxy<formalism::Term, C> terms, const Edge& edge, bool begin) noexcept :
            m_terms(terms),
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
        SpanProxy<formalism::Term, C> m_terms;
        const Edge& m_edge;

    public:
        EdgeAssignmentRange(SpanProxy<formalism::Term, C> terms, const Edge& edge) noexcept : m_terms(terms), m_edge(edge) {}

        EdgeAssignmentIterator begin() const noexcept { return EdgeAssignmentIterator(m_terms, m_edge, true); }

        EdgeAssignmentIterator end() const noexcept { return EdgeAssignmentIterator(m_terms, m_edge, false); }
    };

    /**
     * Vertex
     */

    /// @brief A vertex [parameter_index/object_index] in the consistency graph.
    class Vertex
    {
    private:
        uint_t m_index;
        uint_t m_parameter_index;
        uint_t m_object_index;

    public:
        Vertex(uint_t index, uint_t parameter_index, uint_t object_index) noexcept :
            m_index(index),
            m_parameter_index(parameter_index),
            m_object_index(object_index)
        {
        }

        template<formalism::IsStaticOrFluentTag T>
        bool consistent_literals(SpanProxy<formalism::Literal<T>, C> literals, const PredicateAssignmentSets<T>& predicate_assignment_sets) const noexcept
        {
            for (const auto& literal : literals)
            {
                const auto atom = literal.get_atom();
                const auto predicate = atom.get_predicate();
                const auto arity = predicate.get_arity();

                if (arity < 1)
                {
                    continue;  ///< We test nullary literals separately
                }

                const auto negated = !literal.get_polarity();

                if (negated && arity != 1)
                {
                    continue;  ///< Can only handly unary negated literals due to overapproximation
                }

                const auto& predicate_assignment_set = predicate_assignment_sets.get_set(predicate.get_index());
                const auto terms = atom.get_terms();

                for (const auto& assignment : VertexAssignmentRange(terms, *this))
                {
                    assert(assignment.is_valid());

                    const auto true_assignment = predicate_assignment_set[assignment];

                    if (!negated && !true_assignment)
                    {
                        return false;
                    }

                    if (negated && true_assignment && (1 == arity))  ///< Due to overapproximation can only test valid assigned unary literals.
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        template<typename T>
        bool consistent_literals(SpanProxy<formalism::BooleanOperator<T>, C> numeric_constraints,
                                 const FunctionAssignmentSet<formalism::StaticTag>& static_function_assignment_sets,
                                 const FunctionAssignmentSet<formalism::FluentTag>& fluent_function_assignment_sets) const noexcept;

        uint_t get_object_if_overlap(Proxy<formalism::Term, C> term) const noexcept
        {
            return std::visit(
                [&](auto&& arg)
                {
                    using Type = std::decay_t<decltype(arg)>;

                    if constexpr (std::is_same_v<Type, formalism::ParameterIndex>)
                    {
                        if (m_parameter_index == to_uint_t(arg))
                            return m_object_index;
                        else
                            return std::numeric_limits<uint_t>::max();
                    }
                    else if constexpr (std::is_same_v<Type, Index<formalism::Object>>)
                    {
                        return arg.get_value();
                    }
                    else
                    {
                        static_assert(dependent_false<Type>::value, "Missing case");
                    }
                },
                term.index_variant());
        }

        uint_t get_index() const noexcept { return m_index; }
        uint_t get_parameter_index() const noexcept { return m_parameter_index; }
        uint_t get_object_index() const noexcept { return m_object_index; }
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

        template<formalism::IsStaticOrFluentTag T>
        bool consistent_literals(SpanProxy<formalism::Literal<T>, C> literals, const PredicateAssignmentSets<T>& predicate_assignment_sets) const noexcept
        {
            for (const auto& literal : literals)
            {
                const auto atom = literal.get_atom();
                const auto predicate = atom.get_predicate();
                const auto arity = predicate.get_arity();

                if (arity < 2)
                {
                    continue;  ///< We test nullary and unary literals separately.
                }

                const auto negated = !literal.get_polarity();

                if (negated && arity != 2)
                {
                    continue;  ///< Can only handly binary negated literals due to overapproximation
                }

                const auto& predicate_assignment_set = predicate_assignment_sets.get_set(predicate.get_index());
                const auto terms = atom.get_terms();

                /* Iterate edges. */

                for (const auto& assignment : EdgeAssignmentRange(terms, *this))
                {
                    assert(assignment.is_valid());

                    const auto true_assignment = predicate_assignment_set[assignment];

                    if (!negated && !true_assignment)
                    {
                        return false;
                    }

                    if (negated && true_assignment && (2 == arity))  ///< Due to overapproximation can only test valid assigned binary literals.
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        template<typename T>
        bool consistent_literals(SpanProxy<formalism::BooleanOperator<T>, C> numeric_constraints,
                                 const FunctionAssignmentSet<formalism::StaticTag>& static_function_assignment_sets,
                                 const FunctionAssignmentSet<formalism::FluentTag>& fluent_function_assignment_sets) const noexcept;

        uint_t get_object_if_overlap(Proxy<formalism::Term, C> term) const noexcept
        {
            return std::visit(
                [&](auto&& arg)
                {
                    using Type = std::decay_t<decltype(arg)>;

                    if constexpr (std::is_same_v<Type, formalism::ParameterIndex>)
                    {
                        if (m_src.get_parameter_index() == to_uint_t(arg))
                            return m_src.get_object_index();
                        else if (m_dst.get_parameter_index() == to_uint_t(arg))
                            return m_dst.get_object_index();
                        else
                            return std::numeric_limits<uint_t>::max();
                    }
                    else if constexpr (std::is_same_v<Type, Index<formalism::Object>>)
                    {
                        return arg.get_value();
                    }
                    else
                    {
                        static_assert(dependent_false<Type>::value, "Missing case");
                    }
                },
                term.index_variant());
        }

        const Vertex& get_src() const noexcept { return m_src; }
        const Vertex& get_dst() const noexcept { return m_dst; }
    };

    using Vertices = std::vector<Vertex>;

    auto get_vertices() const noexcept { return std::ranges::subrange(m_vertices.cbegin(), m_vertices.cend()); }

    auto get_edges() const noexcept { return std::ranges::subrange(EdgeIterator(*this, true), EdgeIterator(*this, false)); }

    /// @brief Helper to initialize vertices.
    Vertices compute_vertices(Proxy<formalism::ConjunctiveCondition, C> condition,
                              const analysis::DomainListList& parameter_domains,
                              const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets)
    {
        auto vertices = Vertices {};

        for (uint_t parameter_index = 0; parameter_index < condition.get_arity(); ++parameter_index)
        {
            auto& parameter_domain = parameter_domains[parameter_index];

            for (const auto object_index : parameter_domain)
            {
                const auto vertex_index = static_cast<uint_t>(vertices.size());

                auto vertex = Vertex(vertex_index, parameter_index, object_index.get_value());

                assert(vertex.get_index() == vertex_index);

                if (vertex.consistent_literals(condition.template get_literals<formalism::StaticTag>(), static_assignment_sets.predicate))
                    vertices.push_back(std::move(vertex));
            }
        }

        return vertices;
    }

    /// @brief Helper to initialize edges.
    std::tuple<std::vector<uint_t>, std::vector<uint_t>, std::vector<uint_t>>
    compute_edges(Proxy<formalism::ConjunctiveCondition, C> condition,
                  const analysis::DomainListList& parameter_domains,
                  const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets,
                  const Vertices& vertices)
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

                auto edge = Edge(first_vertex, second_vertex);

                // Part 1 of definition of substitution consistency graph (Stahlberg-ecai2023): exclude I^\neq
                if (first_vertex.get_parameter_index() != second_vertex.get_parameter_index()
                    && edge.consistent_literals(condition.template get_literals<formalism::StaticTag>(), static_assignment_sets.predicate))
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

public:
    StaticConsistencyGraph(Proxy<formalism::ConjunctiveCondition, C> condition,
                           const analysis::DomainListList& parameter_domains,
                           const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets) :
        m_condition(condition)
    {
        m_vertices = compute_vertices(condition, parameter_domains, static_assignment_sets);

        auto [sources_, target_offsets_, targets_] = compute_edges(condition, parameter_domains, static_assignment_sets, m_vertices);

        m_sources = std::move(sources_);
        m_target_offsets = std::move(target_offsets_);
        m_targets = std::move(targets_);

        m_vertex_mask.resize(m_vertices.size());
    }

    auto consistent_vertices(const AssignmentSets& assignment_sets) const
    {
        return get_vertices()
               | std::views::filter(
                   [this, &assignment_sets](auto&& vertex)
                   {
                       return vertex.consistent_literals(m_condition.template get_literals<formalism::FluentTag>(), assignment_sets.fluent_sets.predicate)
                              && vertex.consistent_literals(m_condition.get_numeric_constraints(),
                                                            assignment_sets.static_sets.function,
                                                            assignment_sets.fluent_sets.function);
                   });
    }

    auto consistent_edges(const AssignmentSets& assignment_sets) const
    {
        m_vertex_mask.reset();
        for (const auto& v : consistent_vertices(assignment_sets))
            m_vertex_mask.set(v.get_index());

        return get_edges()
               | std::views::filter(
                   [this, &assignment_sets](auto&& edge)
                   {
                       return m_vertex_mask.test(edge.get_src().get_index()) && m_vertex_mask.test(edge.get_dst().get_index())
                              && edge.consistent_literals(m_condition.template get_literals<formalism::FluentTag>(), assignment_sets.fluent_sets.predicate)
                              && edge.consistent_literals(m_condition.get_numeric_constraints(),
                                                          assignment_sets.static_sets.function,
                                                          assignment_sets.fluent_sets.function);
                   });
    }

    size_t get_num_vertices() const noexcept { return m_vertices.size(); }
    size_t get_num_edges() const noexcept { return m_targets.size(); }

private:
    class EdgeIterator
    {
    private:
        const StaticConsistencyGraph* m_graph;
        size_t m_sources_pos;
        size_t m_targets_pos;

        const StaticConsistencyGraph& get_graph() const noexcept
        {
            assert(m_graph);
            return *m_graph;
        }

        void advance() noexcept
        {
            if (++m_targets_pos >= get_graph().m_target_offsets[m_sources_pos])
                ++m_sources_pos;
        }

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Edge;
        using pointer = value_type*;
        using reference = const value_type&;
        using iterator_category = std::forward_iterator_tag;

        EdgeIterator() noexcept : m_graph(nullptr), m_sources_pos(0), m_targets_pos(0) {}
        EdgeIterator(const StaticConsistencyGraph& graph, bool begin) noexcept :
            m_graph(&graph),
            m_sources_pos(begin ? 0 : graph.m_sources.size()),
            m_targets_pos(begin ? 0 : graph.m_targets.size())
        {
        }
        value_type operator*() const noexcept
        {
            assert(m_sources_pos < get_graph().m_sources.size());
            assert(m_targets_pos < get_graph().m_targets.size());
            return Edge(get_graph().m_vertices[get_graph().m_sources[m_sources_pos]], get_graph().m_vertices[get_graph().m_targets[m_targets_pos]]);
        }
        EdgeIterator& operator++() noexcept
        {
            advance();
            return *this;
        }
        EdgeIterator operator++(int) noexcept
        {
            EdgeIterator tmp = *this;
            ++(*this);
            return tmp;
        }
        bool operator==(const EdgeIterator& other) const noexcept { return m_targets_pos == other.m_targets_pos && m_sources_pos == other.m_sources_pos; }
        bool operator!=(const EdgeIterator& other) const noexcept { return !(*this == other); }
    };

    friend class EdgeIterator;

private:
    Proxy<formalism::ConjunctiveCondition, C> m_condition;

    /* The data member of the consistency graph. */
    Vertices m_vertices;

    // Adjacency list of edges.
    std::vector<uint_t> m_sources;  ///< sources with non-zero out-degree
    std::vector<uint_t> m_target_offsets;
    std::vector<uint_t> m_targets;

    // To speedup consistent_edges
    mutable boost::dynamic_bitset<> m_vertex_mask;
};
}

#endif
