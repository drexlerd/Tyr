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

#ifndef TYR_FORMALISM_GROUND_RULE_PROXY_HPP_
#define TYR_FORMALISM_GROUND_RULE_PROXY_HPP_

#include "tyr/common/span.hpp"
#include "tyr/formalism/boolean_operator.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_atom_proxy.hpp"
#include "tyr/formalism/ground_literal_proxy.hpp"
#include "tyr/formalism/ground_rule_index.hpp"
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/repository.hpp"

namespace tyr::formalism
{
template<IsContext C>
class GroundRuleProxy
{
private:
    const C* context;
    GroundRuleIndex index;

public:
    GroundRuleProxy(GroundRuleIndex index, const C& context) : context(&context), index(index) {}

    const auto& get() const { return get_repository(*context)[index]; }

    auto get_index() const { return index; }
    auto get_binding() const { return SpanProxy<ObjectIndex, C>(get().objects, *context); }
    auto get_static_body() const { return SpanProxy<GroundLiteralIndex<StaticTag>, C>(get().static_body, *context); }
    auto get_fluent_body() const { return SpanProxy<GroundLiteralIndex<FluentTag>, C>(get().fluent_body, *context); }
    auto get_numeric_body() const { return SpanProxy<BooleanOperator<GroundFunctionExpression>, C>(get().numeric_body, *context); }
    auto get_head() const { return GroundAtomProxy(get().head, *context); }
};
}

#endif
