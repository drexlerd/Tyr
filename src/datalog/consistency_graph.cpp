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
#include "tyr/formalism/datalog/merge.hpp"
#include "tyr/formalism/datalog/repository.hpp"
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

template<f::FactKind T>
inline bool consistent_literals(const Vertex& vertex,
                                const TaggedRuleToLiteralInfos<T>& indexed_literals,
                                const PredicateAssignmentSets<T>& predicate_assignment_sets) noexcept
{
    const auto object_index = vertex.get_object_index();
    const auto parameter_index = vertex.get_parameter_index();

    // std::cout << "Vertex: " << *this << std::endl;

    for (const auto lit_id : indexed_literals.info_mappings.parameter_to_infos[uint_t(parameter_index)])
    {
        const auto& info = indexed_literals.infos[lit_id];
        const auto predicate = info.predicate;
        const auto polarity = info.polarity;

        assert(polarity || info.kpkc_arity == 1);  ///< Can only handly unary negated literals due to overapproximation

        const auto& pred_set = predicate_assignment_sets.get_set(predicate);

        for (const auto position : info.position_mappings.parameter_to_positions[uint_t(parameter_index)])
        {
            {
                auto assignment = VertexAssignment(f::ParameterIndex(position), object_index);
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
                    auto first_obj = object_index;
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

        /// constants c,c' with position pos_c < pos_c'
        if (info.num_constants >= 1)
        {
            for (const auto& [pos_c, obj_c] : info.position_mappings.constant_positions)
            {
                auto assignment = VertexAssignment(f::ParameterIndex(pos_c), obj_c);
                assert(assignment.is_valid());

                // std::cout << assignment << std::endl;

                if (polarity != pred_set.at(assignment))
                    return false;
            }
        }
    }

    return true;
}

template<f::FactKind T>
ClosedInterval<float_t>
consistent_interval(const RuleToFunctionTermInfo<T>& info, const Vertex& vertex, const FunctionAssignmentSets<T>& function_assignment_sets) noexcept;

template<f::FactKind T>
ClosedInterval<float_t>
consistent_interval(const RuleToFunctionTermInfo<T>& info, const Edge& edge, const FunctionAssignmentSets<T>& function_assignment_sets) noexcept;

template<f::ArithmeticOpKind O, typename GraphStructure>
ClosedInterval<float_t> consistent_interval(fd::LiftedUnaryOperatorView<O> element,
                                            const GraphStructure& structure,
                                            const RuleToConstraintInfo& constraint_info,
                                            const AssignmentSets& assignment_sets) noexcept;

template<f::ArithmeticOpKind O, typename GraphStructure>
ClosedInterval<float_t> consistent_interval(fd::LiftedBinaryOperatorView<O> element,
                                            const GraphStructure& structure,
                                            const RuleToConstraintInfo& constraint_info,
                                            const AssignmentSets& assignment_sets) noexcept;

template<f::ArithmeticOpKind O, typename GraphStructure>
ClosedInterval<float_t> consistent_interval(fd::LiftedMultiOperatorView<O> element,
                                            const GraphStructure& structure,
                                            const RuleToConstraintInfo& constraint_info,
                                            const AssignmentSets& assignment_sets) noexcept;

template<typename GraphStructure>
ClosedInterval<float_t> consistent_interval(fd::FunctionExpressionView element,
                                            const GraphStructure& structure,
                                            const RuleToConstraintInfo& constraint_info,
                                            const AssignmentSets& assignment_sets) noexcept;

template<typename GraphStructure>
ClosedInterval<float_t> consistent_interval(fd::LiftedArithmeticOperatorView element,
                                            const GraphStructure& structure,
                                            const RuleToConstraintInfo& constraint_info,
                                            const AssignmentSets& assignment_sets) noexcept;

template<typename GraphStructure>
bool consistent_numeric_constraint(fd::LiftedBooleanOperatorView element,
                                   const GraphStructure& structure,
                                   const RuleToConstraintInfo& constraint_info,
                                   const AssignmentSets& assignment_sets) noexcept;

template<f::FactKind T>
inline ClosedInterval<float_t>
consistent_interval(const RuleToFunctionTermInfo<T>& info, const Vertex& vertex, const FunctionAssignmentSets<T>& function_assignment_sets) noexcept
{
    const auto object_index = vertex.get_object_index();
    const auto parameter_index = vertex.get_parameter_index();

    const auto function = info.function;
    const auto& func_set = function_assignment_sets.get_set(function);

    auto bounds = func_set.at(EmptyAssignment());
    if (empty(bounds))
        return bounds;  // early exit

    if (info.num_parameters >= 1)
    {
        for (const auto position : info.position_mappings.parameter_to_positions[uint_t(parameter_index)])
        {
            auto assignment = VertexAssignment(f::ParameterIndex(position), object_index);
            assert(assignment.is_valid());

            // std::cout << assignment << std::endl;

            bounds = intersect(bounds, func_set.at(assignment));
            if (empty(bounds))
                return bounds;  // early exit

            if (info.num_constants >= 1)
            {
                for (const auto& [pos_c, obj_c] : info.position_mappings.constant_positions)
                {
                    assert(position != pos_c);

                    auto first_pos = position;
                    auto second_pos = pos_c;
                    auto first_obj = object_index;
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
    }

    if (info.num_constants >= 1)
    {
        for (const auto& [pos_c, obj_c] : info.position_mappings.constant_positions)
        {
            auto assignment = VertexAssignment(f::ParameterIndex(pos_c), obj_c);
            assert(assignment.is_valid());

            // std::cout << assignment << std::endl;

            bounds = intersect(bounds, func_set.at(assignment));
            if (empty(bounds))
                return bounds;  // early exit
        }
    }

    return bounds;
}

template<f::FactKind T>
inline ClosedInterval<float_t>
consistent_interval(const RuleToFunctionTermInfo<T>& info, const Edge& edge, const FunctionAssignmentSets<T>& function_assignment_sets) noexcept
{
    auto p = uint_t(edge.vi().get_parameter_index());
    auto q = uint_t(edge.vj().get_parameter_index());
    auto obj_p = edge.vi().get_object_index();
    auto obj_q = edge.vj().get_object_index();

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

    bounds = intersect(bounds, consistent_interval(info, edge.vi(), function_assignment_sets));
    if (empty(bounds))
        return bounds;  // early exit

    bounds = intersect(bounds, consistent_interval(info, edge.vj(), function_assignment_sets));
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
            const auto& [first_pos_c, first_obj_c] = info.position_mappings.constant_positions[i];

            for (uint_t j = i + 1; j < info.position_mappings.constant_positions.size(); ++j)
            {
                const auto& [second_pos_c, second_obj_c] = info.position_mappings.constant_positions[j];
                assert(first_pos_c < second_pos_c);

                auto assignment = EdgeAssignment(f::ParameterIndex(first_pos_c), first_obj_c, f::ParameterIndex(second_pos_c), second_obj_c);
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
inline ClosedInterval<float_t> consistent_interval(fd::LiftedUnaryOperatorView<O> element,
                                                   const GraphStructure& structure,
                                                   const RuleToConstraintInfo& constraint_info,
                                                   const AssignmentSets& assignment_sets) noexcept
{
    return apply(O {}, consistent_interval(element.get_arg(), structure, constraint_info, assignment_sets));
}

template<f::ArithmeticOpKind O, typename GraphStructure>
inline ClosedInterval<float_t> consistent_interval(fd::LiftedBinaryOperatorView<O> element,
                                                   const GraphStructure& structure,
                                                   const RuleToConstraintInfo& constraint_info,
                                                   const AssignmentSets& assignment_sets) noexcept
{
    return apply(O {},
                 consistent_interval(element.get_lhs(), structure, constraint_info, assignment_sets),
                 consistent_interval(element.get_rhs(), structure, constraint_info, assignment_sets));
}

template<f::ArithmeticOpKind O, typename GraphStructure>
inline ClosedInterval<float_t> consistent_interval(fd::LiftedMultiOperatorView<O> element,
                                                   const GraphStructure& structure,
                                                   const RuleToConstraintInfo& constraint_info,
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
inline ClosedInterval<float_t> consistent_interval(fd::FunctionExpressionView element,
                                                   const GraphStructure& structure,
                                                   const RuleToConstraintInfo& constraint_info,
                                                   const AssignmentSets& assignment_sets) noexcept
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return ClosedInterval<float_t>(arg, arg);
            else if constexpr (std::is_same_v<Alternative, fd::LiftedArithmeticOperatorView>)
                return consistent_interval(arg, structure, constraint_info, assignment_sets);
            else if constexpr (std::is_same_v<Alternative, fd::FunctionTermView<f::StaticTag>>)
                return consistent_interval(constraint_info.static_infos.infos.at(arg.get_index()), structure, assignment_sets.static_sets.function);
            else if constexpr (std::is_same_v<Alternative, fd::FunctionTermView<f::FluentTag>>)
                return consistent_interval(constraint_info.fluent_infos.infos.at(arg.get_index()), structure, assignment_sets.fluent_sets.function);
            else
                static_assert(dependent_false<Alternative>::value, "Missing case");
        },
        element.get_variant());
}

template<typename GraphStructure>
inline ClosedInterval<float_t> consistent_interval(fd::LiftedArithmeticOperatorView element,
                                                   const GraphStructure& structure,
                                                   const RuleToConstraintInfo& constraint_info,
                                                   const AssignmentSets& assignment_sets) noexcept
{
    return visit([&](auto&& arg) { return consistent_interval(arg, structure, constraint_info, assignment_sets); }, element.get_variant());
}

template<typename GraphStructure>
inline bool consistent_numeric_constraint(fd::LiftedBooleanOperatorView element,
                                          const GraphStructure& structure,
                                          const RuleToConstraintInfo& constraint_info,
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

inline bool consistent_numeric_constraints(const Vertex& vertex,
                                           fd::LiftedBooleanOperatorListView numeric_constraints,
                                           const RuleToRuleToConstraintInfos& indexed_constraints,
                                           const AssignmentSets& assignment_sets) noexcept
{
    assert(numeric_constraints.size() == indexed_constraints.infos.size());

    for (uint_t i = 0; i < numeric_constraints.size(); ++i)
    {
        const auto numeric_constraint = numeric_constraints[i];
        const auto& info = indexed_constraints.infos[i];

        assert(kpkc_arity(numeric_constraint) > 0);  ///< We test nullary constraints separately.

        if (!consistent_numeric_constraint(numeric_constraint, vertex, info, assignment_sets))
            return false;
    }

    return true;
}

/**
 * Edge
 */

template<f::FactKind T>
inline bool
consistent_literals(const Edge& edge, const TaggedRuleToLiteralInfos<T>& indexed_literals, const PredicateAssignmentSets<T>& predicate_assignment_sets) noexcept
{
    auto p = uint_t(edge.vi().get_parameter_index());
    auto q = uint_t(edge.vj().get_parameter_index());
    auto obj_p = edge.vi().get_object_index();
    auto obj_q = edge.vj().get_object_index();

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

inline bool consistent_numeric_constraints(const Edge& edge,
                                           fd::LiftedBooleanOperatorListView numeric_constraints,
                                           const RuleToRuleToConstraintInfos& indexed_constraints,
                                           const AssignmentSets& assignment_sets) noexcept
{
    assert(numeric_constraints.size() == indexed_constraints.infos.size());

    for (uint_t i = 0; i < numeric_constraints.size(); ++i)
    {
        const auto numeric_constraint = numeric_constraints[i];
        const auto& info = indexed_constraints.infos[i];

        assert(kpkc_arity(numeric_constraint) > 1);  ///< We test nullary constraints separately.

        if (!consistent_numeric_constraint(numeric_constraint, edge, info, assignment_sets))
            return false;
    }

    return true;
}

}

/**
 * StaticConsistencyGraph
 */

std::tuple<details::Vertices, std::vector<std::vector<uint_t>>, std::vector<std::vector<uint_t>>>
StaticConsistencyGraph::compute_vertices(const details::TaggedRuleToLiteralInfos<f::StaticTag>& indexed_literals,
                                         const analysis::DomainListList& parameter_domains,
                                         size_t num_objects,
                                         uint_t begin_parameter_index,
                                         uint_t end_parameter_index,
                                         const TaggedAssignmentSets<f::StaticTag>& static_assignment_sets)
{
    auto vertices = details::Vertices {};

    auto vertex_partitions = std::vector<std::vector<uint_t>> {};
    auto object_to_vertex_per_partition = std::vector<std::vector<uint_t>> {};

    for (uint_t parameter_index = begin_parameter_index; parameter_index < end_parameter_index; ++parameter_index)
    {
        auto& parameter_domain = parameter_domains[parameter_index];

        auto vertex_partition = std::vector<uint_t> {};
        auto object_to_vertex_partition = std::vector<uint_t>(num_objects, std::numeric_limits<uint_t>::max());

        for (const auto object_index : parameter_domain)
        {
            const auto vertex_index = static_cast<uint_t>(vertices.size());

            auto vertex = details::Vertex(f::ParameterIndex(parameter_index), Index<f::Object>(object_index));

            if (consistent_literals(vertex, indexed_literals, static_assignment_sets.predicate))
            {
                vertices.push_back(std::move(vertex));
                vertex_partition.push_back(vertex_index);
                object_to_vertex_partition[uint_t(object_index)] = vertex_index;
            }
        }

        vertex_partitions.push_back(std::move(vertex_partition));
        object_to_vertex_per_partition.push_back(std::move(object_to_vertex_partition));
    }

    return { std::move(vertices), std::move(vertex_partitions), std::move(object_to_vertex_per_partition) };
}

kpkc::DeduplicatedAdjacencyMatrix StaticConsistencyGraph::compute_edges(const details::TaggedRuleToLiteralInfos<f::StaticTag>& indexed_literals,
                                                                        const TaggedAssignmentSets<f::StaticTag>& static_assignment_sets,
                                                                        const details::Vertices& vertices,
                                                                        const std::vector<std::vector<uint_t>>& vertex_partitions)
{
    const auto k = vertex_partitions.size();

    auto matrix = kpkc::AdjacencyMatrix(m_layout);

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

                    const auto edge = details::Edge(vertex_i, vertex_j);

                    if (consistent_literals(edge, indexed_literals, static_assignment_sets.predicate))
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

    return kpkc::DeduplicatedAdjacencyMatrix(matrix);
}

template<f::FactKind T>
static auto compute_tagged_indexed_literals(fd::LiteralListView<T> literals, size_t arity)
{
    auto result = details::TaggedRuleToLiteralInfos<T> {};

    result.info_mappings.parameter_to_infos = std::vector<std::vector<uint_t>>(arity);
    result.info_mappings.parameter_pairs_to_infos = std::vector<std::vector<std::vector<uint_t>>>(arity, std::vector<std::vector<uint_t>>(arity));
    result.info_mappings.parameter_to_infos_with_constants = std::vector<std::vector<uint_t>>(arity);
    result.info_mappings.infos_with_constants = std::vector<uint_t> {};
    result.info_mappings.infos_with_constant_pairs = std::vector<uint_t> {};

    for (const auto literal : literals)
    {
        auto info = details::RuleToLiteralInfo<T> {};
        info.predicate = literal.get_atom().get_predicate().get_index();
        info.polarity = literal.get_polarity();
        info.kpkc_arity = kpkc_arity(literal);
        info.num_parameters = uint_t(0);
        info.num_constants = uint_t(0);
        info.position_mappings.constant_positions = std::vector<std::pair<uint_t, Index<f::Object>>> {};
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
                    else if constexpr (std::is_same_v<Alternative, fd::ObjectView>)
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
static auto compute_tagged_indexed_fterms(const std::vector<fd::FunctionTermView<T>>& fterms, size_t arity)
{
    auto result = details::TaggedRuleToFunctionTermInfos<T> {};

    result.info_mappings.parameter_to_infos = std::vector<std::vector<uint_t>>(arity);
    result.info_mappings.parameter_pairs_to_infos = std::vector<std::vector<std::vector<uint_t>>>(arity, std::vector<std::vector<uint_t>>(arity));
    result.info_mappings.parameter_to_infos_with_constants = std::vector<std::vector<uint_t>>(arity);
    result.info_mappings.infos_with_constants = std::vector<uint_t> {};
    result.info_mappings.infos_with_constant_pairs = std::vector<uint_t> {};

    for (const auto fterm : fterms)
    {
        auto info = details::RuleToFunctionTermInfo<T> {};
        info.function = fterm.get_function().get_index();
        info.kpkc_arity = kpkc_arity(fterm);
        info.num_parameters = uint_t(0);
        info.num_constants = uint_t(0);
        info.position_mappings.constant_positions = std::vector<std::pair<uint_t, Index<f::Object>>> {};
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
                    else if constexpr (std::is_same_v<Alternative, fd::ObjectView>)
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

static auto compute_constraint_info(fd::LiftedBooleanOperatorView element, size_t arity)
{
    auto result = details::RuleToConstraintInfo {};

    auto static_fterms = fd::collect_fterms<f::StaticTag>(element);
    auto fluent_fterms = fd::collect_fterms<f::FluentTag>(element);

    result.static_infos = compute_tagged_indexed_fterms(static_fterms, arity);
    result.fluent_infos = compute_tagged_indexed_fterms(fluent_fterms, arity);

    result.kpkc_arity = kpkc_arity(element);

    return result;
}

static auto compute_indexed_constraints(fd::ConjunctiveConditionView element)
{
    auto result = details::RuleToRuleToConstraintInfos {};
    result.infos = std::vector<details::RuleToConstraintInfo> {};
    for (const auto constraint : element.get_numeric_constraints())
        result.infos.push_back(compute_constraint_info(constraint, element.get_arity()));
    return result;
}

auto compute_indexed_literals(fd::ConjunctiveConditionView element)
{
    return details::RuleToLiteralInfos { compute_tagged_indexed_literals(element.get_literals<f::StaticTag>(), element.get_arity()),
                                         compute_tagged_indexed_literals(element.get_literals<f::FluentTag>(), element.get_arity()) };
}

static auto compute_indexed_anchors(fd::ConjunctiveConditionView element, size_t num_fluent_predicates)
{
    auto result = details::LiteralToRuleInfos {};
    result.groups = std::vector<details::LiteralToRuleInfos::GroupInfo> {};
    result.infos = std::vector<details::LiteralToRuleInfo> {};
    result.bound_parameters = boost::dynamic_bitset<>(element.get_arity(), false);
    result.negated_bound_parameters = boost::dynamic_bitset<>(element.get_arity(), false);

    auto bound_parameters = UnorderedSet<uint_t> {};

    auto map = UnorderedMap<Index<f::Predicate<f::FluentTag>>, std::vector<details::LiteralToRuleInfo>> {};

    for (const auto literal : element.get_literals<f::FluentTag>())
    {
        auto info = details::LiteralToRuleInfo {};
        info.parameter_mappings.position_to_parameter =
            std::vector<uint_t>(literal.get_atom().get_predicate().get_arity(), details::LiteralToRuleParameterMapping::NoParam);
        info.parameter_mappings.position_parameter_pairs = std::vector<std::pair<uint_t, uint_t>> {};

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
                        info.parameter_mappings.position_parameter_pairs.emplace_back(position, uint_t(arg));
                        result.bound_parameters.set(uint_t(arg));
                        if (!literal.get_polarity())
                            result.negated_bound_parameters.set(uint_t(arg));
                    }
                    else if constexpr (std::is_same_v<Alternative, fd::ObjectView>) {}
                    else
                        static_assert(dependent_false<Alternative>::value, "Missing case");
                },
                term.get_variant());
        }

        map[literal.get_atom().get_predicate().get_index()].push_back(std::move(info));
    }

    for (auto& [predicate, infos] : map)
    {
        result.groups.emplace_back(predicate, result.infos.size(), result.infos.size() + infos.size());
        result.infos.insert(result.infos.end(), infos.begin(), infos.end());
    }

    return result;
}

StaticConsistencyGraph::StaticConsistencyGraph(fd::RuleView rule,
                                               fd::ConjunctiveConditionView condition,
                                               fd::ConjunctiveConditionView unary_overapproximation_condition,
                                               fd::ConjunctiveConditionView binary_overapproximation_condition,
                                               fd::ConjunctiveConditionView static_binary_overapproximation_condition,
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
    m_binary_overapproximation_vdg(binary_overapproximation_condition),
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
    auto [vertices_, vertex_partitions_, object_to_vertex_per_partition_] = compute_vertices(m_unary_overapproximation_indexed_literals.static_indexed,
                                                                                             parameter_domains,
                                                                                             num_objects,
                                                                                             begin_parameter_index,
                                                                                             end_parameter_index,
                                                                                             static_assignment_sets);
    m_vertices = std::move(vertices_);
    m_vertex_partitions = std::move(vertex_partitions_);
    m_object_to_vertex_per_partition = std::move(object_to_vertex_per_partition_);

    m_layout = kpkc::GraphLayout(m_vertices.size(), m_vertex_partitions);

    m_matrix = compute_edges(m_binary_overapproximation_indexed_literals.static_indexed, static_assignment_sets, m_vertices, m_vertex_partitions);

    // std::ofstream file("graph_" + std::to_string(uint_t(m_rule.get_index())) + ".dot");
    // file << fd::VariableDependencyGraph(m_condition) << std::endl;

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
                                                                   const TaggedFactSets<f::FluentTag>& delta_fact_sets,
                                                                   const kpkc::GraphLayout& layout,
                                                                   kpkc::Graph& delta_graph,
                                                                   kpkc::Graph& full_graph,
                                                                   std::vector<kpkc::Edge>& delta_edges,
                                                                   kpkc::VertexPartitions& fact_induced_candidates) const
{
    /// 1. Copy old full into delta, then add new vertices and edges into delta, before finally subtracting full from delta.

    fact_induced_candidates.reset();
    delta_graph.reset();

    /**
     * Compute delta fact induced vertices.
     */

    const auto& predicate_sets = delta_fact_sets.predicate.get_sets();

    const auto& predicate_to_anchors = m_unary_overapproximation_predicate_to_anchors;

    for (const auto& group : predicate_to_anchors.groups)
    {
        const auto predicate_index = group.predicate;

        for (const auto binding : predicate_sets[uint_t(predicate_index)].get_bindings())  ///< Outter loop because |facts| > |infos|
        {
            const auto objects = binding.get_objects();

            for (const auto& info : predicate_to_anchors[group])
            {
                const auto& pairs = info.parameter_mappings.position_parameter_pairs;
                const auto pairs_size = pairs.size();

                for (uint_t i = 0; i < pairs_size; ++i)
                {
                    const auto& [pos_i, pi] = pairs[i];

                    const auto vi = m_object_to_vertex_per_partition[pi][uint_t(objects[pos_i].get_index())];

                    if (vi == std::numeric_limits<uint_t>::max())
                        continue;

                    fact_induced_candidates.get_bitset(pi).set(layout.vertex_to_bit[vi]);
                }
            }
        }
    }

    // Overapproximate negated part or those where we dont have anchors
    for (uint_t p = 0; p < layout.k; ++p)
        if (!m_unary_overapproximation_predicate_to_anchors.bound_parameters.test(p)
            || m_unary_overapproximation_predicate_to_anchors.negated_bound_parameters.test(p))
            fact_induced_candidates.get_bitset(p).set();

    /// 2. Monotonically update full consistent vertices partition

    {
        const auto unary_overapproximation_constraints = m_unary_overapproximation_condition.get_numeric_constraints();

        auto vertex_index_offset = uint_t(0);

        for (uint_t p = 0; p < layout.k; ++p)
        {
            const auto& info = layout.info.infos[p];
            auto induced_partition = fact_induced_candidates.get_bitset(p);
            auto full_affected_partition = full_graph.affected_partitions.get_bitset(info);
            auto delta_affected_partition = delta_graph.affected_partitions.get_bitset(info);
            auto full_delta_partition = full_graph.delta_partitions.get_bitset(info);
            auto delta_delta_partition = delta_graph.delta_partitions.get_bitset(info);

            for_each_bit(
                [&](auto&& bit)
                {
                    const auto v = vertex_index_offset + bit;
                    const auto& vertex = get_vertex(v);

                    if (consistent_literals(vertex, m_unary_overapproximation_indexed_literals.fluent_indexed, assignment_sets.fluent_sets.predicate)
                        && consistent_numeric_constraints(vertex,
                                                          unary_overapproximation_constraints,
                                                          m_unary_overapproximation_indexed_constraints,
                                                          assignment_sets))
                    {
                        /// Process delta consistent vertex.
                        full_affected_partition.set(bit);
                        delta_affected_partition.set(bit);

                        full_delta_partition.set(bit);
                        delta_delta_partition.set(bit);
                    }
                },
                [](auto&& a, auto&& b) noexcept { return a & ~b; },
                induced_partition,
                full_affected_partition);

            vertex_index_offset += info.num_bits;
        }
    }

    /// 3. Monotonically update full explicitly consistent edges

    {
        delta_edges.clear();

        const auto binary_overapproximation_constraints = m_binary_overapproximation_condition.get_numeric_constraints();

        auto offset_i = 0;

        for (uint_t pi = 0; pi < layout.k; ++pi)
        {
            const auto& info_i = layout.info.infos[pi];

            const auto full_affected_partition_i = full_graph.affected_partitions.get_bitset(info_i);
            auto delta_affected_partition_i = delta_graph.affected_partitions.get_bitset(info_i);

            for (auto bi = full_affected_partition_i.find_first(); bi != BitsetSpan<const uint64_t>::npos; bi = full_affected_partition_i.find_next(bi))
            {
                const auto vi = offset_i + bi;  ///< vi is consistent + delta
                const auto& vertex_i = get_vertex(vi);

                auto offset_j = offset_i + info_i.num_bits;

                for (uint_t pj = pi + 1; pj < layout.k; ++pj)
                {
                    const auto& info_j = layout.info.infos[pj];

                    if (!m_binary_overapproximation_vdg.has_dependency(pi, pj))
                    {
                        offset_j += info_j.num_bits;
                        continue;  // Already checked via vertex consistency
                    }

                    const auto full_affected_partition_j = full_graph.affected_partitions.get_bitset(info_j);
                    auto delta_affected_partition_j = delta_graph.affected_partitions.get_bitset(info_j);

                    const auto static_edges = m_matrix.get_bitset(vi, pj);
                    auto full_edges_i = full_graph.matrix.get_bitset(vi, pj);
                    auto delta_edges_i = delta_graph.matrix.get_bitset(vi, pj);
                    auto delta_touched_i = delta_graph.matrix.touched_partitions(vi, pj);
                    auto full_touched_i = full_graph.matrix.touched_partitions(vi, pj);

                    for_each_bit(
                        [&](auto&& bj)
                        {
                            const auto vj = offset_j + bj;

                            const auto& vertex_j = get_vertex(vj);

                            const auto edge = details::Edge(vertex_i, vertex_j);

                            if (consistent_literals(edge, m_binary_overapproximation_indexed_literals.fluent_indexed, assignment_sets.fluent_sets.predicate)
                                && consistent_numeric_constraints(edge,
                                                                  binary_overapproximation_constraints,
                                                                  m_binary_overapproximation_indexed_constraints,
                                                                  assignment_sets))
                            {
                                /// Process delta consistent edge.

                                // Set edges
                                full_edges_i.set(bj);
                                full_graph.matrix.get_bitset(vj, pi).set(bi);

                                delta_edges_i.set(bj);
                                delta_graph.matrix.get_bitset(vj, pi).set(bi);

                                delta_edges.emplace_back(kpkc::Vertex(vi), kpkc::Vertex(vj));

                                // Set/test affected partitions
                                assert(full_affected_partition_i.test(bi));
                                assert(full_affected_partition_j.test(bj));
                                delta_affected_partition_i.set(bi);
                                delta_affected_partition_j.set(bj);

                                // Set/test delta partitions
                                assert(full_graph.delta_partitions.get_bitset(info_i).test(bi));
                                assert(full_graph.delta_partitions.get_bitset(info_j).test(bj));

                                // Set touched partitions
                                delta_touched_i = true;
                                full_touched_i = true;
                                delta_graph.matrix.touched_partitions(vj, pi) = true;
                                full_graph.matrix.touched_partitions(vj, pi) = true;
                            }
                        },
                        [](auto&& a, auto&& b, auto&& c) noexcept { return a & b & ~c; },
                        static_edges,
                        full_affected_partition_j,
                        full_edges_i);

                    offset_j += info_j.num_bits;
                }
            }

            offset_i += info_i.num_bits;
        }
    }

    /// If vi is new, then mark all implicit vj's in pj as affected, and vice versa.
    for (uint_t pi = 0; pi < layout.k; ++pi)
    {
        const auto& info_i = layout.info.infos[pi];
        auto delta_affected_partition_i = delta_graph.affected_partitions.get_bitset(info_i);
        const auto delta_delta_partition_i = delta_graph.delta_partitions.get_bitset(info_i);

        for (uint_t pj = pi + 1; pj < layout.k; ++pj)
        {
            if (!m_binary_overapproximation_vdg.has_dependency(pi, pj))
            {
                const auto& info_j = layout.info.infos[pj];
                auto delta_affected_partition_j = delta_graph.affected_partitions.get_bitset(info_j);
                const auto delta_delta_partition_j = delta_graph.delta_partitions.get_bitset(info_j);

                if (delta_delta_partition_i.any())
                    delta_affected_partition_j |= full_graph.affected_partitions.get_bitset(info_j);

                if (delta_delta_partition_j.any())
                    delta_affected_partition_i |= full_graph.affected_partitions.get_bitset(info_i);
            }
        }
    }
}

const details::Vertex& StaticConsistencyGraph::get_vertex(uint_t index) const { return m_vertices[index]; }

size_t StaticConsistencyGraph::get_num_vertices() const noexcept { return m_vertices.size(); }

fd::RuleView StaticConsistencyGraph::get_rule() const noexcept { return m_rule; }

fd::ConjunctiveConditionView StaticConsistencyGraph::get_condition() const noexcept { return m_condition; }

const fd::VariableDependencyGraph& StaticConsistencyGraph::get_variable_dependeny_graph() const noexcept { return m_binary_overapproximation_vdg; }

const std::vector<std::vector<uint_t>>& StaticConsistencyGraph::get_vertex_partitions() const noexcept { return m_vertex_partitions; }

const std::vector<std::vector<uint_t>>& StaticConsistencyGraph::get_object_to_vertex_per_partition() const noexcept { return m_object_to_vertex_per_partition; }

const details::LiteralToRuleInfos& StaticConsistencyGraph::get_predicate_to_anchors() const noexcept { return m_predicate_to_anchors; }

const kpkc::DeduplicatedAdjacencyMatrix& StaticConsistencyGraph::get_adjacency_matrix() const noexcept { return m_matrix; }

namespace
{
std::pair<fd::ConjunctiveConditionView, bool>
create_overapproximation_conjunctive_condition(size_t k, fd::ConjunctiveConditionView condition, fd::Repository& context)
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
    return context.get_or_create(conj_cond);
}

std::pair<fd::ConjunctiveConditionView, bool>
create_static_overapproximation_conjunctive_condition(size_t k, fd::ConjunctiveConditionView condition, fd::Repository& context)
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
    return context.get_or_create(conj_cond);
}

std::pair<fd::ConjunctiveConditionView, bool>
create_overapproximation_conflicting_conjunctive_condition(size_t k, fd::ConjunctiveConditionView condition, fd::Repository& context)
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
    return context.get_or_create(conj_cond);
}
}

std::pair<fd::GroundConjunctiveConditionView, bool> create_ground_nullary_conjunctive_condition(fd::ConjunctiveConditionView condition, fd::Repository& context)
{
    auto builder = fd::Builder {};
    auto conj_cond_ptr = builder.get_builder<fd::GroundConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    auto binding_empty = IndexList<f::Object> {};
    auto grounder_context = fd::GrounderContext { builder, context, binding_empty };

    for (const auto literal : condition.get_literals<f::StaticTag>())
        if (parameter_arity(literal) == 0)
            conj_cond.static_literals.push_back(ground(literal, grounder_context).first.get_index());

    for (const auto literal : condition.get_literals<f::FluentTag>())
        if (parameter_arity(literal) == 0)
            conj_cond.fluent_literals.push_back(ground(literal, grounder_context).first.get_index());

    for (const auto numeric_constraint : condition.get_numeric_constraints())
        if (parameter_arity(numeric_constraint) == 0)
            conj_cond.numeric_constraints.push_back(ground(numeric_constraint, grounder_context));

    canonicalize(conj_cond);
    return context.get_or_create(conj_cond);
}

std::pair<fd::RuleView, bool> create_overapproximation_rule(size_t k, fd::RuleView element, fd::Repository& context)
{
    auto builder = fd::Builder {};
    auto merge_context = fd::MergeContext { builder, context };
    auto rule_ptr = builder.get_builder<fd::Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    rule.variables = element.get_variables().get_data();
    rule.body = create_overapproximation_conjunctive_condition(k, element.get_body(), context).first.get_index();
    rule.head = merge_d2d(element.get_head(), merge_context).first.get_index();

    canonicalize(rule);
    return context.get_or_create(rule);
}

std::pair<fd::RuleView, bool> create_static_overapproximation_rule(size_t k, fd::RuleView element, fd::Repository& context)
{
    auto builder = fd::Builder {};
    auto merge_context = fd::MergeContext { builder, context };
    auto rule_ptr = builder.get_builder<fd::Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    rule.variables = element.get_variables().get_data();
    rule.body = create_static_overapproximation_conjunctive_condition(k, element.get_body(), context).first.get_index();
    rule.head = merge_d2d(element.get_head(), merge_context).first.get_index();

    canonicalize(rule);
    return context.get_or_create(rule);
}

std::pair<fd::RuleView, bool> create_overapproximation_conflicting_rule(size_t k, fd::RuleView element, fd::Repository& context)
{
    auto builder = fd::Builder {};
    auto merge_context = fd::MergeContext { builder, context };
    auto rule_ptr = builder.get_builder<fd::Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    rule.variables = element.get_variables().get_data();
    rule.body = create_overapproximation_conflicting_conjunctive_condition(k, element.get_body(), context).first.get_index();
    rule.head = merge_d2d(element.get_head(), merge_context).first.get_index();

    canonicalize(rule);
    return context.get_or_create(rule);
}

}
