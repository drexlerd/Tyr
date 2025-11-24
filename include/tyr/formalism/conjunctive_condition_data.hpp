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

#ifndef TYR_FORMALISM_CONJUNCTIVE_CONDITION_DATA_HPP_
#define TYR_FORMALISM_CONJUNCTIVE_CONDITION_DATA_HPP_

#include "tyr/formalism/boolean_operator_data.hpp"
#include "tyr/formalism/conjunctive_condition_index.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/literal_index.hpp"
#include "tyr/formalism/variable_index.hpp"

namespace tyr
{
template<>
struct Data<formalism::ConjunctiveCondition>
{
    using Tag = formalism::ConjunctiveCondition;

    Index<formalism::ConjunctiveCondition> index;
    IndexList<formalism::Variable> variables;
    IndexList<formalism::Literal<formalism::StaticTag>> static_literals;
    IndexList<formalism::Literal<formalism::FluentTag>> fluent_literals;
    DataList<formalism::BooleanOperator<formalism::FunctionExpression>> numeric_constraints;

    Data() = default;
    Data(Index<formalism::ConjunctiveCondition> index,
         IndexList<formalism::Variable> variables,
         IndexList<formalism::Literal<formalism::StaticTag>> static_literals,
         IndexList<formalism::Literal<formalism::FluentTag>> fluent_literals,
         DataList<formalism::BooleanOperator<formalism::FunctionExpression>> numeric_constraints) :
        index(index),
        variables(std::move(variables)),
        static_literals(std::move(static_literals)),
        fluent_literals(std::move(fluent_literals)),
        numeric_constraints(std::move(numeric_constraints))
    {
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    auto cista_members() const noexcept { return std::tie(index, variables, static_literals, fluent_literals, numeric_constraints); }
    auto identifying_members() const noexcept { return std::tie(variables, static_literals, fluent_literals, numeric_constraints); }
};
}

#endif
