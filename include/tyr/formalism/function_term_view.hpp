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

#ifndef TYR_FORMALISM_FUNCTION_TERM_VIEW_HPP_
#define TYR_FORMALISM_FUNCTION_TERM_VIEW_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function_term_index.hpp"
#include "tyr/formalism/function_view.hpp"
#include "tyr/formalism/repository.hpp"

namespace tyr
{
template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
class View<Index<formalism::FunctionTerm<T>>, C>
{
private:
    const C* m_context;
    Index<formalism::FunctionTerm<T>> m_data;

public:
    using Tag = formalism::FunctionTerm<T>;

    View(Index<formalism::FunctionTerm<T>> data, const C& context) : m_context(&context), m_data(data) {}

    const auto& get() const { return get_repository(*m_context)[m_data]; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_data() const noexcept { return m_data; }

    auto get_index() const { return m_data; }
    auto get_function() const { return View<Index<formalism::Function<T>>, C>(m_data.group, *m_context); }
    auto get_terms() const { return View<DataList<formalism::Term>, C>(get().terms, *m_context); }

    auto identifying_members() const noexcept { return std::tie(m_context, m_data); }
};
}

#endif
