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
#include "tyr/common/chrono.hpp"
#include "tyr/common/closed_interval.hpp"
#include "tyr/datalog/assignment_sets.hpp"
#include "tyr/datalog/declarations.hpp"
#include "tyr/datalog/formatter.hpp"
#include "tyr/formalism/arithmetic_operator_utils.hpp"
#include "tyr/formalism/boolean_operator_utils.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/canonicalization.hpp"
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
            {
                auto assignment = VertexAssignment(f::ParameterIndex(position), m_object_index);
                assert(assignment.is_valid());

                // std::cout << assignment << std::endl;

                const auto true_assignment = pred_set.at(assignment);

                if (polarity != true_assignment)
                    return false;
            }

            {
                /// constant c with position pos_c < pos_p or pos_c > pos_p
                /// E.g. (category_round V0 tharvest)
                for (const auto& [pos_c, obj_c] : info.position_mappings.constant_positions)
                {
                    assert(position != pos_c);

                    auto first_pos = position;
                    auto second_pos = pos_c;
                    auto first_obj = m_object_index;
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

/**
 *
 ConjunctiveCondition(
    variables = [?r_0, ?l_0, ?p_0, ?x_0, ?y_0]
    static literals = [(object V0), (rover V0), (object V1), (object V2), (waypoint V2), (object V3), (waypoint V3), (object V4), (waypoint V4), (lander V1),
(at_lander V1 V4), (visible V3 V4)] fluent literals = [(available V0), (at V0 V3), (have_soil_analysis V0 V2), (channel_free V1)] numeric constraints = []
)
ConjunctiveCondition(
    variables = [?r_0, ?l_0, ?p_0, ?x_0, ?y_0]
    static literals = [(at_lander V1 V4), (visible V3 V4)]
    fluent literals = [(at V0 V3), (have_soil_analysis V0 V2)]
    numeric constraints = []
)

In the example above, there exist many pairs of variables are unrestricted by static literals, resulting in dense reguiosn

 */
kpkc::DeduplicatedAdjacencyMatrix StaticConsistencyGraph::compute_edges(const details::TaggedIndexedLiterals<f::StaticTag>& indexed_literals,
                                                                        const TaggedAssignmentSets<f::StaticTag>& static_assignment_sets,
                                                                        const details::Vertices& vertices,
                                                                        const std::vector<std::vector<uint_t>>& vertex_partitions)
{
    const auto k = vertex_partitions.size();

    auto matrix = kpkc::AdjacencyMatrix(m_layout);

    // std::cout << m_condition << std::endl;
    // std::cout << m_unary_overapproximation_condition << std::endl;
    // std::cout << m_binary_overapproximation_condition << std::endl;
    // std::cout << m_binary_overapproximation_vdg << std::endl;

    if (constant_pair_consistent_literals(indexed_literals, static_assignment_sets.predicate))
    {
        auto offset_i = 0;

        for (uint_t pi = 0; pi < k; ++pi)
        {
            const auto pi_size = vertex_partitions[pi].size();

            for (uint_t bi = 0; bi < pi_size; ++bi)
            {
                const auto vi = offset_i + bi;
                const auto& vertex_i = get_vertex(vi);
                auto offset_j = offset_i + pi_size;

                for (uint_t pj = pi + 1; pj < k; ++pj)
                {
                    const auto pj_size = vertex_partitions[pj].size();

                    for (uint_t bj = 0; bj < pj_size; ++bj)
                    {
                        const auto vj = offset_j + bj;
                        const auto& vertex_j = get_vertex(vj);

                        const auto edge = details::Edge(std::numeric_limits<uint_t>::max(), vertex_i, vertex_j);

                        if (edge.consistent_literals(indexed_literals, static_assignment_sets.predicate))
                        {
                            matrix.get_bitset(vi, pj).set(bj);
                            matrix.get_bitset(vj, pi).set(bi);
                        }
                    }

                    offset_j += pj_size;
                }
            }

            offset_i += pi_size;
        }
    }

    return kpkc::DeduplicatedAdjacencyMatrix(matrix);
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
    result.unbound_parameters = std::vector<uint_t> {};

    auto bound_parameters = UnorderedSet<uint_t> {};

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
                    {
                        info.parameter_mappings.position_to_parameter[position] = uint_t(arg);
                        bound_parameters.insert(uint_t(arg));
                    }
                    else if constexpr (std::is_same_v<Alternative, View<Index<f::Object>, fd::Repository>>) {}
                    else
                        static_assert(dependent_false<Alternative>::value, "Missing case");
                },
                term.get_variant());
        }

        result.predicate_to_infos[uint_t(literal.get_atom().get_predicate().get_index())].push_back(std::move(info));
    }

    for (uint_t p = 0; p < element.get_arity(); ++p)
    {
        if (!bound_parameters.contains(p))
            result.unbound_parameters.push_back(p);
    }

    return result;
}

StaticConsistencyGraph::StaticConsistencyGraph(
    View<Index<formalism::datalog::Rule>, formalism::datalog::Repository> rule,
    View<Index<fd::ConjunctiveCondition>, fd::Repository> condition,
    View<Index<fd::ConjunctiveCondition>, fd::Repository> unary_overapproximation_condition,
    View<Index<fd::ConjunctiveCondition>, fd::Repository> binary_overapproximation_condition,
    View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> static_binary_overapproximation_condition,
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
    m_static_binary_overapproximation_condition(static_binary_overapproximation_condition),
    m_binary_overapproximation_vdg(binary_overapproximation_condition),
    m_static_binary_overapproximation_vdg(static_binary_overapproximation_condition),
    m_layout(),
    m_matrix(m_layout),
    m_unary_overapproximation_indexed_literals(compute_indexed_literals(m_unary_overapproximation_condition)),
    m_binary_overapproximation_indexed_literals(compute_indexed_literals(m_binary_overapproximation_condition)),
    m_unary_overapproximation_indexed_constraints(compute_indexed_constraints(m_unary_overapproximation_condition)),
    m_binary_overapproximation_indexed_constraints(compute_indexed_constraints(m_binary_overapproximation_condition)),
    m_predicate_to_anchors(compute_indexed_anchors(condition, num_fluent_predicates)),
    m_unary_overapproximation_predicate_to_anchors(compute_indexed_anchors(m_unary_overapproximation_condition, num_fluent_predicates)),
    m_binary_overapproximation_predicate_to_anchors(compute_indexed_anchors(m_binary_overapproximation_condition, num_fluent_predicates))
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

    m_layout = kpkc::GraphLayout(m_vertices.size(), m_vertex_partitions);

    m_matrix = compute_edges(m_binary_overapproximation_indexed_literals.static_indexed, static_assignment_sets, m_vertices, m_vertex_partitions);

    // std::cout << "adj matrix bitset bytes: " << m_matrix.bitset_data().size() * sizeof(uint64_t) << "\n";
    // std::cout << "adj matrix row_offset bytes: " << m_matrix.row_offset().size() * sizeof(uint_t) << "\n";
    // std::cout << "adj matrix row_data bytes: " << m_matrix.row_data().size() * sizeof(uint_t) << "\n";
    // std::cout << "adj matrix total bytes: "
    //           << m_matrix.bitset_data().size() * sizeof(uint64_t) + m_matrix.row_offset().size() * sizeof(uint_t) + m_matrix.row_data().size() *
    //           sizeof(uint_t)
    //           << "\n";
    // std::cout << std::endl;

    // std::cout << "Num vertices: " << m_vertices.size() << " num edges: " << m_targets.size() << std::endl;

    // std::cout << m_binary_overapproximation_vdg << std::endl;

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

void StaticConsistencyGraph::initialize_dynamic_consistency_graphs(const AssignmentSets& assignment_sets,
                                                                   const TaggedFactSets<formalism::FluentTag>& delta_fact_sets,
                                                                   const kpkc::GraphLayout& layout,
                                                                   kpkc::Graph& delta_graph,
                                                                   kpkc::Graph& full_graph,
                                                                   kpkc::VertexPartitions& fact_induced_candidates) const
{
    struct PhaseTimes
    {
        uint64_t calls = 0;
        uint64_t reset_ns = 0;
        uint64_t vertex_ns = 0;
        uint64_t edge_ns = 0;
        uint64_t implicit_ns = 0;
        uint64_t delta_touched_partitions = 0;
        uint64_t full_touched_partitions = 0;
        uint64_t delta_consistent_vertices = 0;
        uint64_t delta_consistent_edges = 0;
        uint64_t matrix_bitset_data_bytes = 0;
        uint64_t matrix_cell_data_bytes = 0;
    };

    static PhaseTimes T;

    /// 1. Copy old full into delta, then add new vertices and edges into delta, before finally subtracting full from delta.

    // std::cout << std::endl;
    // std::cout << m_unary_overapproximation_condition << std::endl;
    // std::cout << m_binary_overapproximation_condition << std::endl;

    std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();

    fact_induced_candidates.reset();
    delta_graph.reset();

    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

    // std::cout << m_unary_overapproximation_condition << std::endl;

    const auto& predicate_sets = delta_fact_sets.predicate.get_sets();

    /*
    for (uint_t i = 0; i < delta_fact_sets.predicate.get_sets().size(); ++i)
    {
        const auto& predicate_set = predicate_sets[i];
        const auto predicate = predicate_set.get_predicate();
        const auto arity = predicate.get_arity();
        const auto& predicate_to_anchors = m_unary_overapproximation_predicate_to_anchors.predicate_to_infos[i];

        for (const auto fact : predicate_set.get_facts())
        {
            const auto objects = fact.get_objects().get_data();

            std::cout << fact << " " << predicate_to_anchors.size() << std::endl;
            for (const auto& info : predicate_to_anchors)
            {
                const auto& pos2param = info.parameter_mappings.position_to_parameter;

                for (uint_t pos = 0; pos < arity; ++pos)
                {
                    const auto param = pos2param[pos];

                    std::cout << pos << " " << param << std::endl;

                    if (param == details::ParameterMappings::NoParam)
                        continue;

                    const auto object = objects[pos];
                    // need index of param/object
                    const auto& vertex = get_vertex(f::ParameterIndex(param), object);

                    fact_induced_candidates.get_bitset(param).set(layout.vertex_to_bit[vertex.get_index()]);
                }
            }
        }
    }

    std::cout << fact_induced_candidates << std::endl;
    */

    // std::cout << "Delta graph:" << std::endl;
    // std::cout << delta_graph.matrix << std::endl;
    // std::cout << "Full graph:" << std::endl;
    // std::cout << full_graph.matrix << std::endl;

    /// 2. Monotonically update full consistent vertices partition

    auto vertex_index_offset = uint_t(0);

    if (constant_consistent_literals(m_unary_overapproximation_indexed_literals.fluent_indexed, assignment_sets.fluent_sets.predicate))
    {
        for (uint_t p = 0; p < layout.k; ++p)
        {
            const auto& info = layout.info.infos[p];
            // auto induced_partition = fact_induced_candidates.get_bitset(p);
            auto full_affected_partition = full_graph.affected_partitions.get_bitset(info);
            auto delta_affected_partition = delta_graph.affected_partitions.get_bitset(info);
            auto full_delta_partition = full_graph.delta_partitions.get_bitset(info);
            auto delta_delta_partition = delta_graph.delta_partitions.get_bitset(info);

            /// TODO: Iterate over deltas only
            // for (auto bit = induced_partition.find_first(); bit != BitsetSpan<uint64_t>::npos; bit = induced_partition.find_next(bit))
            for (auto bit = full_affected_partition.find_first_zero(); bit != BitsetSpan<uint64_t>::npos; bit = full_affected_partition.find_next_zero(bit))
            {
                const auto v = vertex_index_offset + bit;
                const auto& vertex = get_vertex(v);

                if (vertex.consistent_literals(m_unary_overapproximation_indexed_literals.fluent_indexed, assignment_sets.fluent_sets.predicate)
                    && vertex.consistent_numeric_constraints(m_unary_overapproximation_condition.get_numeric_constraints(),
                                                             m_unary_overapproximation_indexed_constraints,
                                                             assignment_sets))
                {
                    /// Process delta consistent vertex.
                    full_affected_partition.set(bit);
                    delta_affected_partition.set(bit);

                    full_delta_partition.set(bit);
                    delta_delta_partition.set(bit);

                    delta_graph.matrix.touched_partitions(v, layout.vertex_to_partition[v]) = true;
                    full_graph.matrix.touched_partitions(v, layout.vertex_to_partition[v]) = true;

                    ++T.delta_consistent_vertices;
                }
                else
                {
                    // induced_partition.reset(bit);
                }
            }

            vertex_index_offset += info.num_bits;
        }
    }

    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();

    /// 3. Monotonically update full explicitly consistent edges

    if (constant_pair_consistent_literals(m_binary_overapproximation_indexed_literals.fluent_indexed, assignment_sets.fluent_sets.predicate))
    {
        auto offset_i = 0;

        for (uint_t pi = 0; pi < layout.k; ++pi)
        {
            IndentScope scope1(std::cout);

            const auto& info_i = layout.info.infos[pi];

            const auto full_affected_partition_i = full_graph.affected_partitions.get_bitset(info_i);
            auto delta_affected_partition_i = delta_graph.affected_partitions.get_bitset(info_i);

            // std::cout << print_indent << "pi: " << pi << std::endl;

            for (auto bi = full_affected_partition_i.find_first(); bi != BitsetSpan<const uint64_t>::npos; bi = full_affected_partition_i.find_next(bi))
            {
                IndentScope scope2(std::cout);

                const auto vi = offset_i + bi;  ///< vi is consistent + delta
                const auto& vertex_i = get_vertex(vi);

                // std::cout << print_indent << "vi: " << vi << std::endl;

                auto offset_j = offset_i + info_i.num_bits;

                for (uint_t pj = pi + 1; pj < layout.k; ++pj)
                {
                    IndentScope scope3(std::cout);

                    // std::cout << print_indent << "pj: " << pj << std::endl;

                    const auto info_j = layout.info.infos[pj];

                    if (pi == pj)
                    {
                        offset_j += info_j.num_bits;
                        continue;  // No edges between vertices in the same partition
                    }

                    if (full_graph.matrix.get_cell(vi, pj).is_implicit())
                    {
                        offset_j += info_j.num_bits;
                        continue;  // Already checked via vertex consistency
                    }

                    const auto full_affected_partition_j = full_graph.affected_partitions.get_bitset(info_j);
                    auto delta_affected_partition_j = delta_graph.affected_partitions.get_bitset(info_j);

                    const auto static_edges = m_matrix.get_bitset(vi, pj);

                    // TODO: intersect with full_affected_partition_j and ~full_graph.matrix.get_bitset(vi, pj)
                    for (auto bj = static_edges.find_first(); bj != BitsetSpan<const uint64_t>::npos; bj = static_edges.find_next(bj))
                    {
                        IndentScope scope4(std::cout);

                        if (!full_affected_partition_j.test(bj))
                            continue;  // vj is inconsistent

                        if (full_graph.matrix.get_bitset(vi, pj).test(bj))
                            continue;  // Already edge consistent

                        const auto vj = offset_j + bj;

                        // std::cout << print_indent << "vj: " << vj << std::endl;

                        const auto& vertex_j = get_vertex(vj);
                        const auto edge = details::Edge(uint_t(0), vertex_i, vertex_j);

                        if (edge.consistent_literals(m_binary_overapproximation_indexed_literals.fluent_indexed, assignment_sets.fluent_sets.predicate)
                            && edge.consistent_numeric_constraints(m_binary_overapproximation_condition.get_numeric_constraints(),
                                                                   m_binary_overapproximation_indexed_constraints,
                                                                   assignment_sets))
                        {
                            /// Process delta consistent edge.

                            // Set edges
                            full_graph.matrix.get_bitset(vi, pj).set(bj);
                            full_graph.matrix.get_bitset(vj, pi).set(bi);

                            delta_graph.matrix.get_bitset(vi, pj).set(bj);
                            delta_graph.matrix.get_bitset(vj, pi).set(bi);

                            // Set/test affected partitions
                            assert(full_affected_partition_i.test(bi));
                            assert(full_affected_partition_j.test(bj));
                            delta_affected_partition_i.set(bi);
                            delta_affected_partition_j.set(bj);

                            // Set/test delta partitions
                            assert(full_graph.delta_partitions.get_bitset(info_i).test(bi));
                            assert(full_graph.delta_partitions.get_bitset(info_j).test(bj));

                            // Set touched partitions
                            delta_graph.matrix.touched_partitions(vi, pj) = true;
                            full_graph.matrix.touched_partitions(vi, pj) = true;
                            delta_graph.matrix.touched_partitions(vj, pi) = true;
                            full_graph.matrix.touched_partitions(vj, pi) = true;

                            ++T.delta_consistent_edges;
                        }
                    }

                    offset_j += info_j.num_bits;
                }
            }

            offset_i += info_i.num_bits;
        }
    }

    std::chrono::steady_clock::time_point t3 = std::chrono::steady_clock::now();

    /// If vi is new, then mark all implicit vj's in pj as affected, and vice versa.
    for (uint_t pi = 0; pi < layout.k; ++pi)
    {
        const auto& info_i = layout.info.infos[pi];
        auto delta_affected_partition_i = delta_graph.affected_partitions.get_bitset(info_i);
        const auto delta_delta_partition_i = delta_graph.delta_partitions.get_bitset(info_i);

        for (uint_t pj = pi + 1; pj < layout.k; ++pj)
        {
            const auto& info_j = layout.info.infos[pj];
            auto delta_affected_partition_j = delta_graph.affected_partitions.get_bitset(info_j);
            const auto delta_delta_partition_j = delta_graph.delta_partitions.get_bitset(info_j);

            if (m_binary_overapproximation_vdg.get_adj_matrix().get_cell(f::ParameterIndex(pi), f::ParameterIndex(pj)).empty())
            {
                if (delta_delta_partition_i.any())
                    delta_affected_partition_j |= full_graph.affected_partitions.get_bitset(info_j);

                if (delta_delta_partition_j.any())
                    delta_affected_partition_i |= full_graph.affected_partitions.get_bitset(info_i);
            }
        }
    }

    std::chrono::steady_clock::time_point t4 = std::chrono::steady_clock::now();

    // std::cout << "Delta graph:" << std::endl;
    // std::cout << delta_graph.matrix << std::endl;
    // std::cout << "Full graph:" << std::endl;
    // std::cout << full_graph.matrix << std::endl;

    // T.calls++;
    // T.reset_ns += to_ns(t1 - t0);
    // T.vertex_ns += to_ns(t2 - t1);
    // T.edge_ns += to_ns(t3 - t2);
    // T.implicit_ns += to_ns(t4 - t3);
    // T.delta_touched_partitions += delta_graph.matrix.touched_partitions().count();
    // T.full_touched_partitions += full_graph.matrix.touched_partitions().count();
    // T.matrix_bitset_data_bytes += delta_graph.matrix.bitset_data().size() * sizeof(uint64_t);
    // T.matrix_cell_data_bytes += delta_graph.matrix.adj_data().size() * sizeof(kpkc::PartitionedAdjacencyMatrix::Cell);

    // if ((T.calls % 1000) == 0)
    //{
    //     std::cout << "avg reset " << (T.reset_ns / T.calls) << " ns\n"
    //               << "avg vertex " << (T.vertex_ns / T.calls) << " ns\n"
    //               << "avg edge " << (T.edge_ns / T.calls) << " ns\n"
    //               << "avg implicit " << (T.implicit_ns / T.calls) << " ns\n"
    //               << "delta touched partitions " << static_cast<double>(T.delta_touched_partitions / T.calls) / (layout.nv * layout.k) << "\n"
    //               << "full touched partitions " << static_cast<double>(T.full_touched_partitions / T.calls) / (layout.nv * layout.k) << "\n"
    //               << "delta consistent vertices " << T.delta_consistent_vertices / T.calls << "\n"
    //               << "delta consistent edges " << T.delta_consistent_edges / T.calls << "\n"
    //               << "matrix bitset data bytes " << T.matrix_bitset_data_bytes / T.calls << "\n"
    //               << "matrix cell data bytes " << T.matrix_cell_data_bytes / T.calls << "\n";
    // }
}

const details::Vertex& StaticConsistencyGraph::get_vertex(uint_t index) const { return m_vertices.at(index); }

const details::Vertex& StaticConsistencyGraph::get_vertex(formalism::ParameterIndex parameter, Index<formalism::Object> object) const
{
    return get_vertex(m_object_to_vertex_partitions[uint_t(parameter)][uint_t(object)]);
}

size_t StaticConsistencyGraph::get_num_vertices() const noexcept { return m_vertices.size(); }

View<Index<fd::Rule>, fd::Repository> StaticConsistencyGraph::get_rule() const noexcept { return m_rule; }

View<Index<fd::ConjunctiveCondition>, fd::Repository> StaticConsistencyGraph::get_condition() const noexcept { return m_condition; }

const formalism::datalog::VariableDependencyGraph& StaticConsistencyGraph::get_variable_dependeny_graph() const noexcept
{
    return m_binary_overapproximation_vdg;
}

const std::vector<std::vector<uint_t>>& StaticConsistencyGraph::get_vertex_partitions() const noexcept { return m_vertex_partitions; }

const std::vector<std::vector<uint_t>>& StaticConsistencyGraph::get_object_to_vertex_partitions() const noexcept { return m_object_to_vertex_partitions; }

const details::IndexedAnchors& StaticConsistencyGraph::get_predicate_to_anchors() const noexcept { return m_predicate_to_anchors; }

const kpkc::DeduplicatedAdjacencyMatrix& StaticConsistencyGraph::get_adjacency_matrix() const noexcept { return m_matrix; }

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
create_static_overapproximation_conjunctive_condition(size_t k, View<Index<fd::ConjunctiveCondition>, fd::Repository> condition, fd::Repository& context)
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
