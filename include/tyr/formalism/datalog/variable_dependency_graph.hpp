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

#include <boost/dynamic_bitset.hpp>

namespace tyr::formalism::datalog
{
class VariableDependencyGraph
{
private:
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

        explicit AdjacencyMatrix(uint_t k) :
            m_k(k),
            m_upper_adj_lists(k * (k - 1) / 2),
            m_positive_static_predicate_labels(),
            m_positive_fluent_predicate_labels(),
            m_negative_static_predicate_labels(),
            m_negative_fluent_predicate_labels(),
            m_static_function_labels(),
            m_fluent_function_labels(),
            m_numeric_constraint_labels()
        {
        }

        struct Cell
        {
            std::vector<Index<formalism::Predicate<formalism::StaticTag>>> positive_static_predicate_labels;
            std::vector<Index<formalism::Predicate<formalism::FluentTag>>> positive_fluent_predicate_labels;
            std::vector<Index<formalism::Predicate<formalism::StaticTag>>> negative_static_predicate_labels;
            std::vector<Index<formalism::Predicate<formalism::FluentTag>>> negative_fluent_predicate_labels;
            std::vector<Index<formalism::Function<formalism::StaticTag>>> static_function_labels;
            std::vector<Index<formalism::Function<formalism::FluentTag>>> fluent_function_labels;
            std::vector<Data<formalism::datalog::BooleanOperator<formalism::datalog::FunctionExpression>>> numeric_constraint_labels;
        };

        auto& get_cell(formalism::ParameterIndex lhs, formalism::ParameterIndex rhs) noexcept
        {
            assert(lhs < rhs);
            assert(rhs.value < m_k);
            return m_upper_adj_lists[upper_index(lhs.value, rhs.value, m_k)];
        }
        const auto& get_cell(formalism::ParameterIndex lhs, formalism::ParameterIndex rhs) const noexcept
        {
            assert(lhs < rhs);
            assert(rhs.value < m_k);
            return m_upper_adj_lists[upper_index(lhs.value, rhs.value, m_k)];
        }

    private:
        uint_t m_k;
        std::vector<Cell> m_upper_adj_lists;
    };

public:
    explicit VariableDependencyGraph(View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> condition);

private:
    AdjacencyMatrix adj_matrix;
};
}

#endif
