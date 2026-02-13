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

#include "tyr/formalism/datalog/variable_dependency_graph.hpp"

#include "tyr/common/comparators.hpp"
#include "tyr/formalism/datalog/expression_arity.hpp"
#include "tyr/formalism/views.hpp"

namespace tyr::formalism::datalog
{

template<FactKind T>
static void insert_literal(View<Index<Literal<T>>, Repository> literal, VariableDependencyGraph::AdjacencyMatrix& adj_matrix)
{
    const auto predicate = literal.get_atom().get_predicate().get_index();
    const auto parameters_set = collect_parameters(literal);
    auto parameters = std::vector<ParameterIndex>(parameters_set.begin(), parameters_set.end());
    std::sort(parameters.begin(), parameters.end());

    for (uint_t i = 0; i < parameters.size(); ++i)
    {
        const auto first_param = parameters[i];

        for (uint_t j = i + 1; j < parameters.size(); ++j)
        {
            const auto second_param = parameters[j];

            auto& cell = adj_matrix.get_cell(first_param, second_param);

            if (literal.get_polarity())
                cell.template get_predicate_labels<T, PositiveTag>().push_back(predicate);
            else
                cell.template get_predicate_labels<T, NegativeTag>().push_back(predicate);
        }
    }
}

VariableDependencyGraph::VariableDependencyGraph(View<Index<ConjunctiveCondition>, Repository> condition) : adj_matrix(condition.get_variables().size())
{
    for (const auto literal : condition.get_literals<StaticTag>())
        insert_literal(literal, adj_matrix);

    for (const auto literal : condition.get_literals<FluentTag>())
        insert_literal(literal, adj_matrix);

    for (const auto numeric_constraint : condition.get_numeric_constraints()) {}

    auto dedup = [](auto& v)
    {
        std::sort(v.begin(), v.end());
        v.erase(std::unique(v.begin(), v.end()), v.end());
        v.shrink_to_fit();
    };

    for (uint_t i = 0; i < condition.get_variables().size(); ++i)
    {
        for (uint_t j = i + 1; j < condition.get_variables().size(); ++j)
        {
            auto& cell = adj_matrix.get_cell(ParameterIndex(i), ParameterIndex(j));
            dedup(cell.positive_static_predicate_labels);
            dedup(cell.negative_static_predicate_labels);
            dedup(cell.positive_fluent_predicate_labels);
            dedup(cell.negative_fluent_predicate_labels);
            dedup(cell.static_function_labels);
            dedup(cell.fluent_function_labels);
            dedup(cell.numeric_constraint_labels);
        }
    }
}

}
