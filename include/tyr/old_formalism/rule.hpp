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

#ifndef TYR_FORMALISM_RULE_HPP_
#define TYR_FORMALISM_RULE_HPP_

#include "tyr/formalism/atom_index.hpp"
#include "tyr/formalism/boolean_operator.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/literal_index.hpp"
#include "tyr/formalism/rule_index.hpp"
#include "tyr/formalism/variable_index.hpp"

namespace tyr::formalism
{
struct Rule
{
    RuleIndex index;
    VariableIndexList variables;
    LiteralIndexList<StaticTag> static_body;
    LiteralIndexList<FluentTag> fluent_body;
    BooleanOperatorList<FunctionExpression> numeric_body;
    AtomIndex<FluentTag> head;

    Rule() = default;
    Rule(RuleIndex index,
         VariableIndexList variables,
         LiteralIndexList<StaticTag> static_body,
         LiteralIndexList<FluentTag> fluent_body,
         BooleanOperatorList<FunctionExpression> numeric_body,
         AtomIndex<FluentTag> head) :
        index(index),
        variables(std::move(variables)),
        static_body(std::move(static_body)),
        fluent_body(std::move(fluent_body)),
        numeric_body(std::move(numeric_body)),
        head(head)
    {
    }
    Rule(const Rule& other) = delete;
    Rule& operator=(const Rule& other) = delete;
    Rule(Rule&& other) = default;
    Rule& operator=(Rule&& other) = default;

    auto cista_members() const noexcept { return std::tie(index, variables, static_body, fluent_body, numeric_body, head); }
    auto identifying_members() const noexcept { return std::tie(variables, static_body, fluent_body, numeric_body, head); }
};
}

#endif
