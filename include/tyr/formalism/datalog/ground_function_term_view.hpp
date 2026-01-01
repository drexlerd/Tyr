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

#ifndef TYR_FORMALISM_DATALOG_GROUND_FUNCTION_TERM_VIEW_HPP_
#define TYR_FORMALISM_DATALOG_GROUND_FUNCTION_TERM_VIEW_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/binding_view.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/ground_function_term_index.hpp"
#include "tyr/formalism/function_view.hpp"

namespace tyr
{
template<formalism::FactKind T, formalism::datalog::Context C>
class View<Index<formalism::datalog::GroundFunctionTerm<T>>, C>
{
private:
    const C* m_context;
    Index<formalism::datalog::GroundFunctionTerm<T>> m_handle;

public:
    View(Index<formalism::datalog::GroundFunctionTerm<T>> handle, const C& context) noexcept : m_context(&context), m_handle(handle) {}

    const auto& get_data() const noexcept { return get_repository(*m_context)[m_handle]; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

    auto get_index() const noexcept { return m_handle; }
    auto get_function() const noexcept { return make_view(m_handle.group, *m_context); }
    auto get_binding() const noexcept { return make_view(get_data().binding, *m_context); }

    auto identifying_members() const noexcept { return std::tie(m_context, m_handle); }
};
}

#endif
