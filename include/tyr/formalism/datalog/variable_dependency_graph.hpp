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

#ifndef TYR_FORMALISM_DATALOG_VARIABLE_DEPENDENCY_GRAPH_HPP_
#define TYR_FORMALISM_DATALOG_VARIABLE_DEPENDENCY_GRAPH_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/variant.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/datalog/conjunctive_condition_view.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/repository.hpp"

namespace tyr::formalism::datalog
{
class VariableDependencyGraph
{
public:
    struct AdjacencyMatrix
    {
    private:
        static constexpr uint_t upper_index(uint_t i, uint_t j, uint_t k) noexcept
        {
            // number of entries in rows 0..i-1:
            // (k-1) + (k-2) + ... + (k-i) = i*k - i*(i+1)/2
            // then offset within row i: (j - i - 1)
            return i * k - (i * (i + 1)) / 2 + (j - i - 1);
        }

    public:
        AdjacencyMatrix() = default;

        explicit AdjacencyMatrix(uint_t k) : m_k(k), m_upper_adj_lists(k * (k - 1) / 2) {}

        struct Cell
        {
            template<FactKind T, PolarityKind P>
            auto& get_predicate_labels() noexcept
            {
                if constexpr (std::is_same_v<T, StaticTag>)
                {
                    if constexpr (std::is_same_v<P, PositiveTag>)
                        return positive_static_predicate_labels;
                    else if constexpr (std::is_same_v<P, NegativeTag>)
                        return negative_static_predicate_labels;
                    else
                        static_assert(dependent_false<P>::value, "Missing case");
                }
                else if constexpr (std::is_same_v<T, FluentTag>)
                {
                    if constexpr (std::is_same_v<P, PositiveTag>)
                        return positive_fluent_predicate_labels;
                    else if constexpr (std::is_same_v<P, NegativeTag>)
                        return negative_fluent_predicate_labels;
                    else
                        static_assert(dependent_false<P>::value, "Missing case");
                }
                else
                    static_assert(dependent_false<T>::value, "Missing case");
            }

            template<FactKind T, PolarityKind P>
            const auto& get_predicate_labels() const noexcept
            {
                if constexpr (std::is_same_v<T, StaticTag>)
                {
                    if constexpr (std::is_same_v<P, PositiveTag>)
                        return positive_static_predicate_labels;
                    else if constexpr (std::is_same_v<P, NegativeTag>)
                        return negative_static_predicate_labels;
                    else
                        static_assert(dependent_false<P>::value, "Missing case");
                }
                else if constexpr (std::is_same_v<T, FluentTag>)
                {
                    if constexpr (std::is_same_v<P, PositiveTag>)
                        return positive_fluent_predicate_labels;
                    else if constexpr (std::is_same_v<P, NegativeTag>)
                        return negative_fluent_predicate_labels;
                    else
                        static_assert(dependent_false<P>::value, "Missing case");
                }
                else
                    static_assert(dependent_false<T>::value, "Missing case");
            }

            template<FactKind T>
            auto& get_function_labels() noexcept
            {
                if constexpr (std::is_same_v<T, StaticTag>)
                    return static_function_labels;
                else if constexpr (std::is_same_v<T, FluentTag>)
                    return fluent_function_labels;
                else
                    static_assert(dependent_false<T>::value, "Missing case");
            }

            template<FactKind T>
            const auto& get_function_labels() const noexcept
            {
                if constexpr (std::is_same_v<T, StaticTag>)
                    return static_function_labels;
                else if constexpr (std::is_same_v<T, FluentTag>)
                    return fluent_function_labels;
                else
                    static_assert(dependent_false<T>::value, "Missing case");
            }

            std::vector<Index<Predicate<StaticTag>>> positive_static_predicate_labels;
            std::vector<Index<Predicate<FluentTag>>> positive_fluent_predicate_labels;
            std::vector<Index<Predicate<StaticTag>>> negative_static_predicate_labels;
            std::vector<Index<Predicate<FluentTag>>> negative_fluent_predicate_labels;
            std::vector<Index<Function<StaticTag>>> static_function_labels;
            std::vector<Index<Function<FluentTag>>> fluent_function_labels;
            std::vector<Data<BooleanOperator<FunctionExpression>>> numeric_constraint_labels;
        };

        auto& get_cell(ParameterIndex lhs, ParameterIndex rhs) noexcept
        {
            assert(lhs < rhs);
            assert(uint_t(rhs) < m_k);
            return m_upper_adj_lists[upper_index(uint_t(lhs), uint_t(rhs), m_k)];
        }
        const auto& get_cell(ParameterIndex lhs, ParameterIndex rhs) const noexcept
        {
            assert(lhs < rhs);
            assert(uint_t(rhs) < m_k);
            return m_upper_adj_lists[upper_index(uint_t(lhs), uint_t(rhs), m_k)];
        }

    private:
        uint_t m_k;
        std::vector<Cell> m_upper_adj_lists;
    };

    explicit VariableDependencyGraph(View<Index<ConjunctiveCondition>, Repository> condition);

private:
    AdjacencyMatrix adj_matrix;
};
}

#endif
