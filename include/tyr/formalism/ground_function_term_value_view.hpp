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

#ifndef TYR_FORMALISM_GROUND_FUNCTION_TERM_VALUE_VIEW_HPP_
#define TYR_FORMALISM_GROUND_FUNCTION_TERM_VALUE_VIEW_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_function_term_value_index.hpp"
#include "tyr/formalism/ground_function_term_view.hpp"
#include "tyr/formalism/repository.hpp"

namespace tyr
{
template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
class View<Index<formalism::GroundFunctionTermValue<T>>, C>
{
private:
    const C* m_context;
    Index<formalism::GroundFunctionTermValue<T>> m_data;

public:
    using Tag = formalism::GroundFunctionTermValue<T>;

    View(Index<formalism::GroundFunctionTermValue<T>> data, const C& context) : m_context(&context), m_data(data) {}

    const auto& get() const { return get_repository(*m_context)[m_data]; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_data() const noexcept { return m_data; }

    auto get_index() const { return m_data; }
    auto get_fterm() const { return View<Index<formalism::GroundFunctionTerm<T>>, C>(get().fterm, *m_context); }
    auto get_value() const { return get().value; }
};
}

#endif
