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

#ifndef TYR_FORMALISM2_RULE_DATA_HPP_
#define TYR_FORMALISM2_RULE_DATA_HPP_

#include "tyr/formalism2/atom_index.hpp"
#include "tyr/formalism2/boolean_operator_data.hpp"
#include "tyr/formalism2/declarations.hpp"
#include "tyr/formalism2/literal_index.hpp"
#include "tyr/formalism2/rule_index.hpp"
#include "tyr/formalism2/variable_index.hpp"

namespace tyr
{
template<>
struct Data<formalism::Rule>
{
    using Tag = formalism::Rule;

    Index<formalism::Rule> index;
    IndexList<formalism::Variable> variables;
    IndexList<formalism::Literal<formalism::StaticTag>> static_body;
    IndexList<formalism::Literal<formalism::FluentTag>> fluent_body;
    DataList<formalism::BooleanOperator<formalism::FunctionExpression>> numeric_body;
    Index<formalism::Atom<formalism::FluentTag>> head;

    Data() = default;
    Data(Index<formalism::Rule> index,
         IndexList<formalism::Variable> variables,
         IndexList<formalism::Literal<formalism::StaticTag>> static_body,
         IndexList<formalism::Literal<formalism::FluentTag>> fluent_body,
         DataList<formalism::BooleanOperator<formalism::FunctionExpression>> numeric_body,
         Index<formalism::Atom<formalism::FluentTag>> head) :
        index(index),
        variables(std::move(variables)),
        static_body(std::move(static_body)),
        fluent_body(std::move(fluent_body)),
        numeric_body(std::move(numeric_body)),
        head(head)
    {
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    auto cista_members() const noexcept { return std::tie(index, variables, static_body, fluent_body, numeric_body, head); }
    auto identifying_members() const noexcept { return std::tie(variables, static_body, fluent_body, numeric_body, head); }
};
}

#endif
