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

#ifndef TYR_FORMALISM_GROUND_CONJUNCTIVE_CONDITION_PROXY_HPP_
#define TYR_FORMALISM_GROUND_CONJUNCTIVE_CONDITION_PROXY_HPP_

#include "tyr/common/span.hpp"
#include "tyr/formalism/boolean_operator_proxy.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_conjunctive_condition_index.hpp"
#include "tyr/formalism/ground_literal_proxy.hpp"
#include "tyr/formalism/object_proxy.hpp"
#include "tyr/formalism/repository.hpp"

namespace tyr
{
template<formalism::IsContext C>
class Proxy<formalism::GroundConjunctiveCondition, C>
{
private:
    const C* context;
    Index<formalism::GroundConjunctiveCondition> index;

public:
    using Tag = formalism::GroundConjunctiveCondition;

    Proxy(Index<formalism::GroundConjunctiveCondition> index, const C& context) : context(&context), index(index) {}

    const auto& get() const { return get_repository(*context)[index]; }

    auto get_index() const { return index; }
    auto get_objects() const { return SpanProxy<formalism::Object, C>(get().objects, *context); }
    template<formalism::IsStaticOrFluentTag T>
    auto get_literals() const
    {
        return SpanProxy<formalism::GroundLiteral<T>, C>(get().template get_literals<T>(), *context);
    }
    auto get_numeric_constraints() const
    {
        return SpanProxy<formalism::BooleanOperator<formalism::GroundFunctionExpression>, C>(get().numeric_constraints, *context);
    }
    auto get_arity() const { return get().objects.size(); }
};
}

#endif
