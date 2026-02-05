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
 * Vertex
 */

template<formalism::FactKind T>
bool Vertex::consistent_literals(const TaggedIndexedLiterals<T>& indexed_literals, const PredicateAssignmentSets<T>& predicate_assignment_sets) const noexcept
{
    // std::cout << "Vertex: " << *this << std::endl;

    for (const auto lit_id : indexed_literals.info_mappings.parameter_to_infos[uint_t(m_parameter_index)])
    {
        const auto& info = indexed_literals.infos[lit_id];
        const auto predicate = info.predicate;
        const auto polarity = info.polarity;

        assert(polarity || info.kpkc_arity == 1);  ///< Can only handly unary negated literals due to overapproximation

        const auto& pred_set = predicate_assignment_sets.get_set(predicate);

        for (const auto position : info.position_mappings.parameter_to_positions[uint_t(m_parameter_index)])
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

template<formalism::FactKind T>
ClosedInterval<float_t>
consistent_interval(const FunctionTermInfo<T>& info, const Vertex& vertex, const FunctionAssignmentSets<T>& function_assignment_sets) noexcept;

template<formalism::FactKind T>
ClosedInterval<float_t>
consistent_interval(const FunctionTermInfo<T>& info, const Edge& vertex, const FunctionAssignmentSets<T>& function_assignment_sets) noexcept;

template<f::ArithmeticOpKind O, typename GraphStructure>
ClosedInterval<float_t>
consistent_interval(View<Index<formalism::datalog::UnaryOperator<O, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> element,
                    const GraphStructure& structure,
                    const ConstraintInfo& constraint_info,
                    const AssignmentSets& assignment_sets) noexcept;

template<f::ArithmeticOpKind O, typename GraphStructure>
ClosedInterval<float_t>
consistent_interval(View<Index<formalism::datalog::BinaryOperator<O, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> element,
                    const GraphStructure& structure,
                    const ConstraintInfo& constraint_info,
                    const AssignmentSets& assignment_sets) noexcept;

template<f::ArithmeticOpKind O, typename GraphStructure>
ClosedInterval<float_t>
consistent_interval(View<Index<formalism::datalog::MultiOperator<O, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> element,
                    const GraphStructure& structure,
                    const ConstraintInfo& constraint_info,
                    const AssignmentSets& assignment_sets) noexcept;

template<typename GraphStructure>
ClosedInterval<float_t> consistent_interval(View<Data<formalism::datalog::FunctionExpression>, formalism::datalog::Repository> element,
                                            const GraphStructure& structure,
                                            const ConstraintInfo& constraint_info,
                                            const AssignmentSets& assignment_sets) noexcept;

template<typename GraphStructure>
ClosedInterval<float_t>
consistent_interval(View<Data<formalism::datalog::ArithmeticOperator<Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> element,
                    const GraphStructure& structure,
                    const ConstraintInfo& constraint_info,
                    const AssignmentSets& assignment_sets) noexcept;

template<typename GraphStructure>
bool consistent_numeric_constraint(
    View<Data<formalism::datalog::BooleanOperator<Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> element,
    const GraphStructure& structure,
    const ConstraintInfo& constraint_info,
    const AssignmentSets& assignment_sets) noexcept;

template<formalism::FactKind T>
ClosedInterval<float_t>
consistent_interval(const FunctionTermInfo<T>& info, const Vertex& vertex, const FunctionAssignmentSets<T>& function_assignment_sets) noexcept
{
    const auto function = info.function;
    const auto& func_set = function_assignment_sets.get_set(function);

    auto bounds = func_set.at(EmptyAssignment());
    if (empty(bounds))
        return bounds;  // early exit

    if (info.num_parameters >= 1)
    {
        for (const auto position : info.position_mappings.parameter_to_positions[uint_t(vertex.get_parameter_index())])
        {
            auto assignment = VertexAssignment(f::ParameterIndex(position), vertex.get_object_index());
            assert(assignment.is_valid());

            // std::cout << assignment << std::endl;

            bounds = intersect(bounds, func_set.at(assignment));
            if (empty(bounds))
                return bounds;  // early exit
        }
    }

    if (info.num_constants >= 1)
    {
        for (const auto& [position, object] : info.position_mappings.constant_positions)
        {
            auto assignment = VertexAssignment(f::ParameterIndex(position), object);
            assert(assignment.is_valid());

            // std::cout << assignment << std::endl;

            bounds = intersect(bounds, func_set.at(assignment));
            if (empty(bounds))
                return bounds;  // early exit
        }
    }

    return bounds;
}

template<formalism::FactKind T>
ClosedInterval<float_t>
consistent_interval(const FunctionTermInfo<T>& info, const Edge& edge, const FunctionAssignmentSets<T>& function_assignment_sets) noexcept
{
    auto p = uint_t(edge.get_src().get_parameter_index());
    auto q = uint_t(edge.get_dst().get_parameter_index());
    auto obj_p = edge.get_src().get_object_index();
    auto obj_q = edge.get_dst().get_object_index();

    if (p > q)
    {
        std::swap(p, q);
        std::swap(obj_p, obj_q);
    }

    // std::cout << "Edge: " << p << " " << q << std::endl;

    const auto& func_set = function_assignment_sets.get_set(info.function);

    auto bounds = func_set.at(EmptyAssignment());
    if (empty(bounds))
        return bounds;  // early exit

    bounds = intersect(bounds, consistent_interval(info, edge.get_src(), function_assignment_sets));
    if (empty(bounds))
        return bounds;  // early exit

    bounds = intersect(bounds, consistent_interval(info, edge.get_dst(), function_assignment_sets));
    if (empty(bounds))
        return bounds;  // early exit

    /// positions where p/q occur in that literal
    if (info.num_parameters >= 2)
    {
        for (auto pos_p : info.position_mappings.parameter_to_positions[p])
        {
            for (auto pos_q : info.position_mappings.parameter_to_positions[q])
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

                bounds = intersect(bounds, func_set.at(assignment));
                if (empty(bounds))
                    return bounds;  // early exit
            }
        }
    }

    /// constant c with position pos_c < pos_p or pos_c > pos_p
    if (info.num_parameters >= 1 && info.num_constants >= 1)
    {
        for (auto pos_p : info.position_mappings.parameter_to_positions[p])
        {
            for (const auto& [pos_c, obj_c] : info.position_mappings.constant_positions)
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

                bounds = intersect(bounds, func_set.at(assignment));
                if (empty(bounds))
                    return bounds;  // early exit
            }
        }
    }

    /// constant c with position pos_c < pos_q or pos_c > pos_q
    if (info.num_parameters >= 1 && info.num_constants >= 1)
    {
        for (auto pos_q : info.position_mappings.parameter_to_positions[q])
        {
            for (const auto& [pos_c, obj_c] : info.position_mappings.constant_positions)
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

                bounds = intersect(bounds, func_set.at(assignment));
                if (empty(bounds))
                    return bounds;  // early exit
            }
        }
    }

    /// constants c,c' with position pos_c < pos_c'
    if (info.num_constants >= 2)
    {
        for (uint_t i = 0; i < info.position_mappings.constant_positions.size(); ++i)
        {
            const auto& [first_pos, first_obj] = info.position_mappings.constant_positions[i];

            for (uint_t j = i + 1; j < info.position_mappings.constant_positions.size(); ++j)
            {
                const auto& [second_pos, second_obj] = info.position_mappings.constant_positions[j];
                assert(first_pos < second_pos);

                auto assignment = EdgeAssignment(f::ParameterIndex(first_pos), first_obj, f::ParameterIndex(second_pos), second_obj);
                assert(assignment.is_valid());

                // std::cout << assignment << std::endl;

                bounds = intersect(bounds, func_set.at(assignment));
                if (empty(bounds))
                    return bounds;  // early exit
            }
        }
    }

    return bounds;
}

template<f::ArithmeticOpKind O, typename GraphStructure>
ClosedInterval<float_t>
consistent_interval(View<Index<formalism::datalog::UnaryOperator<O, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> element,
                    const GraphStructure& structure,
                    const ConstraintInfo& constraint_info,
                    const AssignmentSets& assignment_sets) noexcept
{
    return apply(O {}, consistent_interval(element.get_arg(), structure, constraint_info, assignment_sets));
}

template<f::ArithmeticOpKind O, typename GraphStructure>
ClosedInterval<float_t>
consistent_interval(View<Index<formalism::datalog::BinaryOperator<O, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> element,
                    const GraphStructure& structure,
                    const ConstraintInfo& constraint_info,
                    const AssignmentSets& assignment_sets) noexcept
{
    return apply(O {},
                 consistent_interval(element.get_lhs(), structure, constraint_info, assignment_sets),
                 consistent_interval(element.get_rhs(), structure, constraint_info, assignment_sets));
}

template<f::ArithmeticOpKind O, typename GraphStructure>
ClosedInterval<float_t>
consistent_interval(View<Index<formalism::datalog::MultiOperator<O, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> element,
                    const GraphStructure& structure,
                    const ConstraintInfo& constraint_info,
                    const AssignmentSets& assignment_sets) noexcept
{
    const auto child_fexprs = element.get_args();

    return std::accumulate(std::next(child_fexprs.begin()),  // Start from the second expression
                           child_fexprs.end(),
                           consistent_interval(child_fexprs.front(), structure, constraint_info, assignment_sets),
                           [&](const auto& value, const auto& child_expr)
                           { return apply(O {}, value, consistent_interval(child_expr, structure, constraint_info, assignment_sets)); });
}

template<typename GraphStructure>
ClosedInterval<float_t> consistent_interval(View<Data<formalism::datalog::FunctionExpression>, formalism::datalog::Repository> element,
                                            const GraphStructure& structure,
                                            const ConstraintInfo& constraint_info,
                                            const AssignmentSets& assignment_sets) noexcept
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return ClosedInterval<float_t>(arg, arg);
            else if constexpr (std::is_same_v<Alternative, View<Data<fd::ArithmeticOperator<Data<fd::FunctionExpression>>>, fd::Repository>>)
                return consistent_interval(arg, structure, constraint_info, assignment_sets);
            else if constexpr (std::is_same_v<Alternative, View<Index<fd::FunctionTerm<f::StaticTag>>, fd::Repository>>)
                return consistent_interval(constraint_info.static_infos.infos.at(arg.get_index()), structure, assignment_sets.static_sets.function);
            else if constexpr (std::is_same_v<Alternative, View<Index<fd::FunctionTerm<f::FluentTag>>, fd::Repository>>)
                return consistent_interval(constraint_info.fluent_infos.infos.at(arg.get_index()), structure, assignment_sets.fluent_sets.function);
            else
                static_assert(dependent_false<Alternative>::value, "Missing case");
        },
        element.get_variant());
}

template<typename GraphStructure>
ClosedInterval<float_t>
consistent_interval(View<Data<formalism::datalog::ArithmeticOperator<Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> element,
                    const GraphStructure& structure,
                    const ConstraintInfo& constraint_info,
                    const AssignmentSets& assignment_sets) noexcept
{
    return visit([&](auto&& arg) { return consistent_interval(arg, structure, constraint_info, assignment_sets); }, element.get_variant());
}

template<typename GraphStructure>
bool consistent_numeric_constraint(
    View<Data<formalism::datalog::BooleanOperator<Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> element,
    const GraphStructure& structure,
    const ConstraintInfo& constraint_info,
    const AssignmentSets& assignment_sets) noexcept
{
    return visit(
        [&](auto&& arg) -> bool
        {
            using Alternative = std::decay_t<decltype(arg)>;

            return apply_existential(typename Alternative::OpType {},
                                     consistent_interval(arg.get_lhs(), structure, constraint_info, assignment_sets),
                                     consistent_interval(arg.get_rhs(), structure, constraint_info, assignment_sets));
        },
        element.get_variant());
}

bool Vertex::consistent_numeric_constraints(
    View<DataList<formalism::datalog::BooleanOperator<Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository> numeric_constraints,
    const IndexedConstraints& indexed_constraints,
    const AssignmentSets& assignment_sets) const noexcept
{
    assert(numeric_constraints.size() == indexed_constraints.infos.size());

    for (uint_t i = 0; i < numeric_constraints.size(); ++i)
    {
        const auto numeric_constraint = numeric_constraints[i];
        const auto& info = indexed_constraints.infos[i];

        assert(kpkc_arity(numeric_constraint) > 0);  ///< We test nullary constraints separately.

        if (!consistent_numeric_constraint(numeric_constraint, *this, info, assignment_sets))
            return false;
    }

    return true;
}

/**
 * Edge
 */

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
    for (const auto lit_id : indexed_literals.info_mappings.parameter_pairs_to_infos[p][q])
    {
        const auto& info = indexed_literals.infos[lit_id];
        const auto& pred_set = predicate_assignment_sets.get_set(info.predicate);
        const auto polarity = info.polarity;

        assert(polarity || info.kpkc_arity == 2);  ///< Can only handly binary negated literals due to overapproximation

        for (auto pos_p : info.position_mappings.parameter_to_positions[p])
        {
            for (auto pos_q : info.position_mappings.parameter_to_positions[q])
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
    for (const auto lit_id : indexed_literals.info_mappings.parameter_to_infos_with_constants[p])
    {
        const auto& info = indexed_literals.infos[lit_id];
        const auto& pred_set = predicate_assignment_sets.get_set(info.predicate);
        const auto polarity = info.polarity;

        assert(polarity || info.kpkc_arity == 2);  ///< Can only handly binary negated literals due to overapproximation

        for (auto pos_p : info.position_mappings.parameter_to_positions[p])
        {
            for (const auto& [pos_c, obj_c] : info.position_mappings.constant_positions)
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
    for (const auto lit_id : indexed_literals.info_mappings.parameter_to_infos_with_constants[q])
    {
        const auto& info = indexed_literals.infos[lit_id];
        const auto& pred_set = predicate_assignment_sets.get_set(info.predicate);
        const auto polarity = info.polarity;

        assert(polarity || info.kpkc_arity == 2);  ///< Can only handly binary negated literals due to overapproximation

        for (auto pos_q : info.position_mappings.parameter_to_positions[q])
        {
            for (const auto& [pos_c, obj_c] : info.position_mappings.constant_positions)
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
                                          const IndexedConstraints& indexed_constraints,
                                          const AssignmentSets& assignment_sets) const noexcept
{
    assert(numeric_constraints.size() == indexed_constraints.infos.size());

    for (uint_t i = 0; i < numeric_constraints.size(); ++i)
    {
        const auto numeric_constraint = numeric_constraints[i];
        const auto& info = indexed_constraints.infos[i];

        assert(kpkc_arity(numeric_constraint) > 1);  ///< We test nullary constraints separately.

        if (!consistent_numeric_constraint(numeric_constraint, *this, info, assignment_sets))
            return false;
    }

    return true;
}
}

/**
 * StaticConsistencyGraph
 */

std::tuple<details::Vertices, std::vector<std::vector<uint_t>>, std::vector<std::vector<uint_t>>>
StaticConsistencyGraph::compute_vertices(const details::TaggedIndexedLiterals<f::StaticTag>& indexed_literals,
                                         const analysis::DomainListList& parameter_domains,
                                         size_t num_objects,
                                         uint_t begin_parameter_index,
                                         uint_t end_parameter_index,
                                         const TaggedAssignmentSets<f::StaticTag>& static_assignment_sets)
{
    auto vertices = details::Vertices {};

    auto vertex_partitions = std::vector<std::vector<uint_t>> {};
    auto object_to_vertex_partitions = std::vector<std::vector<uint_t>> {};

    if (constant_consistent_literals(indexed_literals, static_assignment_sets.predicate))
    {
        for (uint_t parameter_index = begin_parameter_index; parameter_index < end_parameter_index; ++parameter_index)
        {
            auto& parameter_domain = parameter_domains[parameter_index];

            auto vertex_partition = std::vector<uint_t> {};
            auto object_to_vertex_partition = std::vector<uint_t>(num_objects, std::numeric_limits<uint_t>::max());

            for (const auto object_index : parameter_domain)
            {
                const auto vertex_index = static_cast<uint_t>(vertices.size());

                auto vertex = details::Vertex(vertex_index, f::ParameterIndex(parameter_index), Index<f::Object>(object_index));

                assert(vertex.get_index() == vertex_index);

                if (vertex.consistent_literals(indexed_literals, static_assignment_sets.predicate))
                {
                    vertices.push_back(std::move(vertex));
                    vertex_partition.push_back(vertex.get_index());
                    object_to_vertex_partition[uint_t(object_index)] = vertex.get_index();
                }
            }

            vertex_partitions.push_back(std::move(vertex_partition));
            object_to_vertex_partitions.push_back(std::move(object_to_vertex_partition));
        }
    }
    else
    {
        // We need the partitions for the remaining code to work. Ideally we prune such actions
        vertex_partitions.resize(end_parameter_index - begin_parameter_index);
    }

    return { std::move(vertices), std::move(vertex_partitions), std::move(object_to_vertex_partitions) };
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
    for (const auto& lit_id : indexed_literals.info_mappings.infos_with_constants)
    {
        const auto& info = indexed_literals.infos[lit_id];
        const auto predicate = info.predicate;
        const auto& pred_set = predicate_assignment_sets.get_set(predicate);
        const auto polarity = info.polarity;

        assert(polarity || info.kpkc_arity == 1);  ///< Can only handly unary negated literals due to overapproximation

        for (const auto& [position, object] : info.position_mappings.constant_positions)
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
    for (const auto& lit_id : indexed_literals.info_mappings.infos_with_constant_pairs)
    {
        const auto& info = indexed_literals.infos[lit_id];
        const auto predicate = info.predicate;
        const auto& pred_set = predicate_assignment_sets.get_set(predicate);
        const auto polarity = info.polarity;

        assert(polarity || info.kpkc_arity == 2);  ///< Can only handly binary negated literals due to overapproximation

        for (uint_t i = 0; i < info.position_mappings.constant_positions.size(); ++i)
        {
            const auto& [first_pos, first_obj] = info.position_mappings.constant_positions[i];

            for (uint_t j = i + 1; j < info.position_mappings.constant_positions.size(); ++j)
            {
                const auto& [second_pos, second_obj] = info.position_mappings.constant_positions[j];
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
static auto compute_tagged_indexed_literals(View<IndexList<fd::Literal<T>>, fd::Repository> literals, size_t arity)
{
    auto result = details::TaggedIndexedLiterals<T> {};

    result.info_mappings.parameter_to_infos = std::vector<std::vector<uint_t>>(arity);
    result.info_mappings.parameter_pairs_to_infos = std::vector<std::vector<std::vector<uint_t>>>(arity, std::vector<std::vector<uint_t>>(arity));
    result.info_mappings.parameter_to_infos_with_constants = std::vector<std::vector<uint_t>>(arity);
    result.info_mappings.infos_with_constants = std::vector<uint_t> {};
    result.info_mappings.infos_with_constant_pairs = std::vector<uint_t> {};

    for (const auto literal : literals)
    {
        auto info = details::LiteralInfo<T> {};
        info.predicate = literal.get_atom().get_predicate().get_index();
        info.polarity = literal.get_polarity();
        info.kpkc_arity = kpkc_arity(literal);
        info.num_parameters = uint_t(0);
        info.num_constants = uint_t(0);
        info.position_mappings.constant_positions = std::vector<std::pair<uint_t, Index<formalism::Object>>> {};
        info.position_mappings.parameter_to_positions = std::vector<std::vector<uint_t>>(arity);

        const auto terms = literal.get_atom().get_terms();

        for (uint_t position = 0; position < terms.size(); ++position)
        {
            const auto term = terms[position];

            visit(
                [&](auto&& arg)
                {
                    using Alternative = std::decay_t<decltype(arg)>;

                    if constexpr (std::is_same_v<Alternative, f::ParameterIndex>)
                    {
                        info.position_mappings.parameter_to_positions[uint_t(arg)].push_back(position);
                        ++info.num_parameters;
                    }
                    else if constexpr (std::is_same_v<Alternative, View<Index<f::Object>, fd::Repository>>)
                    {
                        info.position_mappings.constant_positions.emplace_back(position, arg.get_index());
                        ++info.num_constants;
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
            result.info_mappings.parameter_to_infos[uint_t(param1)].push_back(index);

            for (const auto param2 : parameters)
            {
                if (param1 >= param2)
                    continue;

                result.info_mappings.parameter_pairs_to_infos[uint_t(param1)][uint_t(param2)].push_back(index);
            }
        }

        if (info.num_constants > 0)
        {
            result.info_mappings.infos_with_constants.push_back(index);
            if (info.num_constants > 1)
                result.info_mappings.infos_with_constant_pairs.push_back(index);

            if (info.num_parameters > 0)
            {
                for (uint_t param = 0; param < arity; ++param)
                {
                    if (!info.position_mappings.parameter_to_positions[param].empty())
                        result.info_mappings.parameter_to_infos_with_constants[param].push_back(index);
                }
            }
        }

        result.infos.push_back(std::move(info));
    }

    return result;
}

template<f::FactKind T>
static auto compute_tagged_indexed_fterms(View<IndexList<fd::FunctionTerm<T>>, fd::Repository> fterms, size_t arity)
{
    auto result = details::TaggedIndexedFunctionTerms<T> {};

    result.info_mappings.parameter_to_infos = std::vector<std::vector<uint_t>>(arity);
    result.info_mappings.parameter_pairs_to_infos = std::vector<std::vector<std::vector<uint_t>>>(arity, std::vector<std::vector<uint_t>>(arity));
    result.info_mappings.parameter_to_infos_with_constants = std::vector<std::vector<uint_t>>(arity);
    result.info_mappings.infos_with_constants = std::vector<uint_t> {};
    result.info_mappings.infos_with_constant_pairs = std::vector<uint_t> {};

    for (const auto fterm : fterms)
    {
        auto info = details::FunctionTermInfo<T> {};
        info.function = fterm.get_function().get_index();
        info.kpkc_arity = kpkc_arity(fterm);
        info.num_parameters = uint_t(0);
        info.num_constants = uint_t(0);
        info.position_mappings.constant_positions = std::vector<std::pair<uint_t, Index<formalism::Object>>> {};
        info.position_mappings.parameter_to_positions = std::vector<std::vector<uint_t>>(arity);

        const auto terms = fterm.get_terms();

        for (uint_t position = 0; position < terms.size(); ++position)
        {
            const auto term = terms[position];

            visit(
                [&](auto&& arg)
                {
                    using Alternative = std::decay_t<decltype(arg)>;

                    if constexpr (std::is_same_v<Alternative, f::ParameterIndex>)
                    {
                        info.position_mappings.parameter_to_positions[uint_t(arg)].push_back(position);
                        ++info.num_parameters;
                    }
                    else if constexpr (std::is_same_v<Alternative, View<Index<f::Object>, fd::Repository>>)
                    {
                        info.position_mappings.constant_positions.emplace_back(position, arg.get_index());
                        ++info.num_constants;
                    }
                    else
                        static_assert(dependent_false<Alternative>::value, "Missing case");
                },
                term.get_variant());
        }

        auto parameters = fd::collect_parameters(fterm);
        const auto index = uint_t(fterm.get_index());

        for (const auto param1 : parameters)
        {
            result.info_mappings.parameter_to_infos[uint_t(param1)].push_back(index);

            for (const auto param2 : parameters)
            {
                if (param1 >= param2)
                    continue;

                result.info_mappings.parameter_pairs_to_infos[uint_t(param1)][uint_t(param2)].push_back(index);
            }
        }

        if (info.num_constants > 0)
        {
            result.info_mappings.infos_with_constants.push_back(index);
            if (info.num_constants > 1)
                result.info_mappings.infos_with_constant_pairs.push_back(index);

            if (info.num_parameters > 0)
            {
                for (uint_t param = 0; param < arity; ++param)
                {
                    if (!info.position_mappings.parameter_to_positions[param].empty())
                        result.info_mappings.parameter_to_infos_with_constants[param].push_back(index);
                }
            }
        }

        result.infos.emplace(fterm.get_index(), std::move(info));
    }

    return result;
}

static auto compute_constraint_info(View<Data<fd::BooleanOperator<Data<fd::FunctionExpression>>>, fd::Repository> element, size_t arity)
{
    auto result = details::ConstraintInfo {};

    auto static_fterms = fd::collect_fterms<f::StaticTag>(element);
    auto fluent_fterms = fd::collect_fterms<f::FluentTag>(element);

    result.static_infos = compute_tagged_indexed_fterms(make_view(static_fterms, element.get_context()), arity);
    result.fluent_infos = compute_tagged_indexed_fterms(make_view(fluent_fterms, element.get_context()), arity);

    result.kpkc_arity = kpkc_arity(element);

    return result;
}

static auto compute_indexed_constraints(View<Index<fd::ConjunctiveCondition>, fd::Repository> element)
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

static auto compute_indexed_anchors(View<Index<fd::ConjunctiveCondition>, fd::Repository> element, size_t num_fluent_predicates)
{
    auto result = details::IndexedAnchors {};
    result.predicate_to_infos = std::vector<std::vector<details::LiteralAnchorInfo>>(num_fluent_predicates);

    for (const auto literal : element.get_literals<f::FluentTag>())
    {
        auto info = details::LiteralAnchorInfo {};
        info.parameter_mappings.position_to_parameter =
            std::vector<uint_t>(literal.get_atom().get_predicate().get_arity(), details::ParameterMappings::NoParam);

        const auto terms = literal.get_atom().get_terms();

        for (uint_t position = 0; position < terms.size(); ++position)
        {
            const auto term = terms[position];

            visit(
                [&](auto&& arg)
                {
                    using Alternative = std::decay_t<decltype(arg)>;

                    if constexpr (std::is_same_v<Alternative, f::ParameterIndex>)
                        info.parameter_mappings.position_to_parameter[position] = uint_t(arg);
                    else if constexpr (std::is_same_v<Alternative, View<Index<f::Object>, fd::Repository>>) {}
                    else
                        static_assert(dependent_false<Alternative>::value, "Missing case");
                },
                term.get_variant());
        }

        result.predicate_to_infos[uint_t(literal.get_atom().get_predicate().get_index())].push_back(std::move(info));
    }

    return result;
}

StaticConsistencyGraph::StaticConsistencyGraph(View<Index<formalism::datalog::Rule>, formalism::datalog::Repository> rule,
                                               View<Index<fd::ConjunctiveCondition>, fd::Repository> condition,
                                               View<Index<fd::ConjunctiveCondition>, fd::Repository> unary_overapproximation_condition,
                                               View<Index<fd::ConjunctiveCondition>, fd::Repository> binary_overapproximation_condition,
                                               const analysis::DomainListList& parameter_domains,
                                               size_t num_objects,
                                               size_t num_fluent_predicates,
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
    m_binary_overapproximation_indexed_constraints(compute_indexed_constraints(m_binary_overapproximation_condition)),
    m_predicate_to_anchors(compute_indexed_anchors(condition, num_fluent_predicates))
{
    auto [vertices_, vertex_partitions_, object_to_vertex_partitions_] = compute_vertices(m_unary_overapproximation_indexed_literals.static_indexed,
                                                                                          parameter_domains,
                                                                                          num_objects,
                                                                                          begin_parameter_index,
                                                                                          end_parameter_index,
                                                                                          static_assignment_sets);
    m_vertices = std::move(vertices_);
    m_vertex_partitions = std::move(vertex_partitions_);
    m_object_to_vertex_partitions = std::move(object_to_vertex_partitions_);

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

const details::Vertex& StaticConsistencyGraph::get_vertex(uint_t index) const { return m_vertices.at(index); }

const details::Vertex& StaticConsistencyGraph::get_vertex(formalism::ParameterIndex parameter, Index<formalism::Object> object) const
{
    return get_vertex(m_object_to_vertex_partitions[uint_t(parameter)][uint_t(object)]);
}

size_t StaticConsistencyGraph::get_num_vertices() const noexcept { return m_vertices.size(); }

size_t StaticConsistencyGraph::get_num_edges() const noexcept { return m_targets.size(); }

View<Index<fd::Rule>, fd::Repository> StaticConsistencyGraph::get_rule() const noexcept { return m_rule; }

View<Index<fd::ConjunctiveCondition>, fd::Repository> StaticConsistencyGraph::get_condition() const noexcept { return m_condition; }

const std::vector<std::vector<uint_t>>& StaticConsistencyGraph::get_vertex_partitions() const noexcept { return m_vertex_partitions; }

const std::vector<std::vector<uint_t>>& StaticConsistencyGraph::get_object_to_vertex_partitions() const noexcept { return m_object_to_vertex_partitions; }

const details::IndexedAnchors& StaticConsistencyGraph::get_predicate_to_anchors() const noexcept { return m_predicate_to_anchors; }

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
