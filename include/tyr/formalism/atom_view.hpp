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

#ifndef TYR_FORMALISM_ATOM_VIEW_HPP_
#define TYR_FORMALISM_ATOM_VIEW_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/atom_index.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/predicate_view.hpp"
#include "tyr/formalism/term_data.hpp"

namespace tyr
{
template<formalism::FactKind T, formalism::Context C>
class View<Index<formalism::Atom<T>>, C>
{
private:
    const C* m_context;
    Index<formalism::Atom<T>> m_handle;

public:
    using Tag = formalism::Atom<T>;

    View(Index<formalism::Atom<T>> handle, const C& context) : m_context(&context), m_handle(handle) {}

    const auto& get_data() const { return get_repository(*m_context)[m_handle]; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

    auto get_index() const { return m_handle; }
    auto get_predicate() const { return View<Index<formalism::Predicate<T>>, C>(get_data().predicate, *m_context); }
    auto get_terms() const { return View<DataList<formalism::Term>, C>(get_data().terms, *m_context); }

    auto identifying_members() const noexcept { return std::tie(m_context, m_handle); }
};
}

#endif
