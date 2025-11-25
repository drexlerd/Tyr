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

#ifndef TYR_FORMALISM_CONJUNCTIVE_CONDITION_PROXY_HPP_
#define TYR_FORMALISM_CONJUNCTIVE_CONDITION_PROXY_HPP_

#include "tyr/common/vector.hpp"
#include "tyr/formalism/boolean_operator_proxy.hpp"
#include "tyr/formalism/conjunctive_condition_index.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/literal_proxy.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/variable_proxy.hpp"

namespace tyr
{
template<formalism::IsContext C>
class Proxy<Index<formalism::ConjunctiveCondition>, C>
{
private:
    const C* context;
    Index<formalism::ConjunctiveCondition> index;

public:
    using Tag = formalism::ConjunctiveCondition;

    Proxy(Index<formalism::ConjunctiveCondition> index, const C& context) : context(&context), index(index) {}

    const auto& get() const { return get_repository(*context)[index]; }

    auto get_index() const { return index; }
    auto get_variables() const { return Proxy<IndexList<formalism::Variable>, C>(get().variables, *context); }
    template<formalism::IsStaticOrFluentTag T>
    auto get_literals() const
    {
        return Proxy<IndexList<formalism::Literal<T>>, C>(get().template get_literals<T>(), *context);
    }
    auto get_numeric_constraints() const
    {
        return Proxy<DataList<formalism::BooleanOperator<Data<formalism::FunctionExpression>>>, C>(get().numeric_constraints, *context);
    }
    template<formalism::IsStaticOrFluentTag T>
    auto get_nullary_literals() const
    {
        return Proxy<IndexList<formalism::GroundLiteral<T>>, C>(get().template get_literals<T>(), *context);
    }
    auto get_nullary_numeric_constraints() const
    {
        return Proxy<DataList<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>>, C>(get().numeric_constraints, *context);
    }
    auto get_arity() const { return get().variables.size(); }
};
}

#endif
