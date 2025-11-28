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

#ifndef TYR_FORMALISM_GROUND_LITERAL_VIEW_HPP_
#define TYR_FORMALISM_GROUND_LITERAL_VIEW_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_atom_view.hpp"
#include "tyr/formalism/ground_literal_index.hpp"
#include "tyr/formalism/predicate_view.hpp"
#include "tyr/formalism/repository.hpp"

namespace tyr
{
template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
class View<Index<formalism::GroundLiteral<T>>, C>
{
private:
    const C* m_context;
    Index<formalism::GroundLiteral<T>> m_data;

public:
    using Tag = formalism::GroundLiteral<T>;

    View(Index<formalism::GroundLiteral<T>> data, const C& context) : m_context(&context), m_data(data) {}

    const auto& get() const { return get_repository(*m_context)[m_data]; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_data() const noexcept { return m_data; }

    auto get_index() const { return m_data; }
    auto get_predicate() const { return View<Index<formalism::Predicate<T>>, C>(m_data.predicate_index, *m_context); }
    auto get_atom() const { return View<Index<formalism::GroundAtom<T>>, C>(get().atom, *m_context); }
    auto get_polarity() const { return get().polarity; }

    auto identifying_members() const noexcept { return std::tie(m_context, m_data); }
};
}

#endif
