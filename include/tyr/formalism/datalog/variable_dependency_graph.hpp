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
            template<FactKind T>
            auto& get_literal_labels() noexcept
            {
                if constexpr (std::is_same_v<T, StaticTag>)
                    return static_literal_labels;
                else if constexpr (std::is_same_v<T, FluentTag>)
                    return fluent_literal_labels;
                else
                    static_assert(dependent_false<T>::value, "Missing case");
            }

            template<FactKind T>
            const auto& get_literal_labels() const noexcept
            {
                if constexpr (std::is_same_v<T, StaticTag>)
                    return static_literal_labels;
                else if constexpr (std::is_same_v<T, FluentTag>)
                    return fluent_literal_labels;
                else
                    static_assert(dependent_false<T>::value, "Missing case");
            }

            auto& get_numeric_constraint_labels() noexcept { return numeric_constraint_labels; }
            const auto& get_numeric_constraint_labels() const noexcept { return numeric_constraint_labels; }

            bool statically_empty() const noexcept { return static_literal_labels.empty(); }
            bool dynamically_empty() const noexcept { return fluent_literal_labels.empty() && numeric_constraint_labels.empty(); }
            bool empty() const noexcept { return statically_empty() && dynamically_empty(); }

            std::vector<Index<Literal<StaticTag>>> static_literal_labels;
            std::vector<Index<Literal<FluentTag>>> fluent_literal_labels;
            std::vector<Data<BooleanOperator<Data<FunctionExpression>>>> numeric_constraint_labels;
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

        auto k() const noexcept { return m_k; }

    private:
        uint_t m_k;
        std::vector<Cell> m_upper_adj_lists;
    };

    explicit VariableDependencyGraph(View<Index<ConjunctiveCondition>, Repository> condition);

    const AdjacencyMatrix& get_adj_matrix() const noexcept { return adj_matrix; }

private:
    AdjacencyMatrix adj_matrix;
};
}

#endif
