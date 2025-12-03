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

#ifndef TYR_FORMALISM_CONJUNCTIVE_CONDITION_VIEW_HPP_
#define TYR_FORMALISM_CONJUNCTIVE_CONDITION_VIEW_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/boolean_operator_view.hpp"
#include "tyr/formalism/conjunctive_condition_index.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/literal_view.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/variable_view.hpp"

namespace tyr
{
template<formalism::Context C>
class View<Index<formalism::ConjunctiveCondition>, C>
{
private:
    const C* m_context;
    Index<formalism::ConjunctiveCondition> m_handle;

public:
    using Tag = formalism::ConjunctiveCondition;

    View(Index<formalism::ConjunctiveCondition> handle, const C& context) : m_context(&context), m_handle(handle) {}

    const auto& get_data() const { return get_repository(*m_context)[m_handle]; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

    auto get_index() const noexcept { return m_handle; }
    auto get_variables() const noexcept { return View<IndexList<formalism::Variable>, C>(get_data().variables, *m_context); }
    template<formalism::FactKind T>
    auto get_literals() const
    {
        return View<IndexList<formalism::Literal<T>>, C>(get_data().template get_literals<T>(), *m_context);
    }
    auto get_numeric_constraints() const
    {
        return View<DataList<formalism::BooleanOperator<Data<formalism::FunctionExpression>>>, C>(get_data().numeric_constraints, *m_context);
    }
    template<formalism::FactKind T>
    auto get_nullary_literals() const
    {
        return View<IndexList<formalism::GroundLiteral<T>>, C>(get_data().template get_nullary_literals<T>(), *m_context);
    }
    auto get_nullary_numeric_constraints() const
    {
        return View<DataList<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>>, C>(get_data().nullary_numeric_constraints, *m_context);
    }
    auto get_arity() const { return get_data().variables.size(); }

    auto identifying_members() const noexcept { return std::tie(m_context, m_handle); }
};
}

#endif
