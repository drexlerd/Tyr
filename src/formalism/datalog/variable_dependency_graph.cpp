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
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/views.hpp"

namespace tyr::formalism::datalog
{
template<FactKind T>
static void insert_literal(LiteralView<T> literal, uint_t k, boost::dynamic_bitset<>& positive_dependencies, boost::dynamic_bitset<>& negative_dependencies)
{
    const auto parameters_set = collect_parameters(literal);
    auto parameters = std::vector<ParameterIndex>(parameters_set.begin(), parameters_set.end());
    std::sort(parameters.begin(), parameters.end());

    for (uint_t i = 0; i < parameters.size(); ++i)
    {
        const auto pi = uint_t(parameters[i]);

        for (uint_t j = i + 1; j < parameters.size(); ++j)
        {
            const auto pj = uint_t(parameters[j]);

            const auto i1 = VariableDependencyGraph::get_index(pi, pj, k);
            const auto i2 = VariableDependencyGraph::get_index(pj, pi, k);

            if (literal.get_polarity())
            {
                positive_dependencies.set(i1);
                positive_dependencies.set(i2);
            }
            else
            {
                negative_dependencies.set(i1);
                negative_dependencies.set(i2);
            }
        }
    }
}

static void insert_numeric_constraint(BooleanOperatorView numeric_constraint, uint_t k, boost::dynamic_bitset<>& positive_dependencies)
{
    const auto parameters_set = collect_parameters(numeric_constraint);
    auto parameters = std::vector<ParameterIndex>(parameters_set.begin(), parameters_set.end());
    std::sort(parameters.begin(), parameters.end());

    for (uint_t i = 0; i < parameters.size(); ++i)
    {
        const auto pi = uint_t(parameters[i]);

        for (uint_t j = i + 1; j < parameters.size(); ++j)
        {
            const auto pj = uint_t(parameters[j]);

            const auto i1 = VariableDependencyGraph::get_index(pi, pj, k);
            const auto i2 = VariableDependencyGraph::get_index(pj, pi, k);

            positive_dependencies.set(i1);
            positive_dependencies.set(i2);
        }
    }
}

VariableDependencyGraph::VariableDependencyGraph(ConjunctiveConditionView condition) :
    m_k(condition.get_arity()),
    m_static_positive_dependencies(m_k * m_k),
    m_static_negative_dependencies(m_k * m_k),
    m_fluent_positive_dependencies(m_k * m_k),
    m_fluent_negative_dependencies(m_k * m_k)
{
    for (const auto literal : condition.get_literals<StaticTag>())
        insert_literal(literal, m_k, m_static_positive_dependencies, m_static_negative_dependencies);

    for (const auto literal : condition.get_literals<FluentTag>())
        insert_literal(literal, m_k, m_fluent_positive_dependencies, m_fluent_negative_dependencies);

    for (const auto numeric_constraint : condition.get_numeric_constraints())
        insert_numeric_constraint(numeric_constraint, m_k, m_fluent_positive_dependencies);
}

}
