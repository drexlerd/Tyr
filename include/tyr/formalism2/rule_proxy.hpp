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

#ifndef TYR_FORMALISM2_RULE_PROXY_HPP_
#define TYR_FORMALISM2_RULE_PROXY_HPP_

#include "tyr/common/span.hpp"
#include "tyr/formalism2/atom_proxy.hpp"
#include "tyr/formalism2/boolean_operator_proxy.hpp"
#include "tyr/formalism2/declarations.hpp"
#include "tyr/formalism2/literal_proxy.hpp"
#include "tyr/formalism2/repository.hpp"
#include "tyr/formalism2/rule_index.hpp"
#include "tyr/formalism2/variable_proxy.hpp"

namespace tyr
{
template<formalism::IsContext C>
class Proxy<formalism::Rule, C>
{
private:
    const C* context;
    Index<formalism::Rule> index;

public:
    using Tag = formalism::Rule;

    Proxy(Index<formalism::Rule> index, const C& context) : context(&context), index(index) {}

    const auto& get() const { return get_repository(*context)[index]; }

    auto get_index() const { return index; }
    auto get_variables() const { return SpanProxy<formalism::Variable, C>(get().variables, *context); }
    auto get_static_body() const { return SpanProxy<formalism::Literal<formalism::StaticTag>, C>(get().static_body, *context); }
    auto get_fluent_body() const { return SpanProxy<formalism::Literal<formalism::FluentTag>, C>(get().fluent_body, *context); }
    auto get_numeric_body() const { return SpanProxy<formalism::BooleanOperator<formalism::FunctionExpression>, C>(get().numeric_body, *context); }
    auto get_head() const { return Proxy<formalism::Atom<formalism::FluentTag>, C>(get().head, *context); }
};
}

#endif
