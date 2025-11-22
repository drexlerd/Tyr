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

#ifndef TYR_FORMALISM_GROUND_RULE_DATA_HPP_
#define TYR_FORMALISM_GROUND_RULE_DATA_HPP_

#include "tyr/common/span.hpp"
#include "tyr/formalism/boolean_operator_data.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_atom_index.hpp"
#include "tyr/formalism/ground_literal_index.hpp"
#include "tyr/formalism/ground_rule_index.hpp"

namespace tyr
{
template<>
struct Data<formalism::GroundRule>
{
    using Tag = formalism::GroundRule;

    Index<formalism::GroundRule> index;
    IndexList<formalism::Object> binding;
    IndexList<formalism::GroundLiteral<formalism::StaticTag>> static_body;
    IndexList<formalism::GroundLiteral<formalism::FluentTag>> fluent_body;
    DataList<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>> numeric_body;
    Index<formalism::GroundAtom<formalism::FluentTag>> head;

    Data() = default;
    Data(Index<formalism::GroundRule> index,
         IndexList<formalism::Object> binding,
         IndexList<formalism::GroundLiteral<formalism::StaticTag>> static_body,
         IndexList<formalism::GroundLiteral<formalism::FluentTag>> fluent_body,
         DataList<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>> numeric_body,
         Index<formalism::GroundAtom<formalism::FluentTag>> head) :
        index(index),
        binding(std::move(binding)),
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

    auto cista_members() const noexcept { return std::tie(index, binding, static_body, fluent_body, numeric_body, head); }
    auto identifying_members() const noexcept { return std::tie(index.group, binding, static_body, fluent_body, numeric_body, head); }
};

}

#endif
