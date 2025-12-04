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

#ifndef TYR_FORMALISM_PLANNING_AXIOM_VIEW_HPP_
#define TYR_FORMALISM_PLANNING_AXIOM_VIEW_HPP_

#include "tyr/common/types.hpp"
#include "tyr/formalism/atom_view.hpp"
#include "tyr/formalism/conjunctive_condition_view.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/planning/axiom_index.hpp"
#include "tyr/formalism/repository.hpp"

namespace tyr
{

template<formalism::Context C>
class View<Index<formalism::Axiom>, C>
{
private:
    const C* m_context;
    Index<formalism::Axiom> m_handle;

public:
    using Tag = formalism::Axiom;

    View(Index<formalism::Axiom> handle, const C& context) : m_context(&context), m_handle(handle) {}

    const auto& get_data() const { return get_repository(*m_context)[m_handle]; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

    auto get_index() const noexcept { return m_handle; }
    auto get_arity() const noexcept { return get_body().get_arity(); }
    auto get_variables() const noexcept { return get_body().get_variables(); }
    auto get_body() const noexcept { return View<Index<formalism::ConjunctiveCondition>, C>(get_data().body, *m_context); }
    auto get_head() const noexcept { return View<Index<formalism::Atom<formalism::DerivedTag>>, C>(get_data().head, *m_context); }

    auto identifying_members() const noexcept { return std::tie(m_context, m_handle); }
};
}

#endif
