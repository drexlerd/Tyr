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

#ifndef TYR_FORMALISM_RULE_VIEW_HPP_
#define TYR_FORMALISM_RULE_VIEW_HPP_

#include "tyr/common/vector.hpp"
#include "tyr/formalism/conjunctive_condition_view.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/rule_index.hpp"

namespace tyr
{
template<formalism::IsContext C>
class View<Index<formalism::Rule>, C>
{
private:
    const C* m_context;
    Index<formalism::Rule> m_data;

public:
    using Tag = formalism::Rule;

    View(Index<formalism::Rule> data, const C& context) : m_context(&context), m_data(data) {}

    const auto& get() const { return get_repository(*m_context)[m_data]; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_data() const noexcept { return m_data; }

    auto get_index() const { return m_data; }
    auto get_body() const { return View<Index<formalism::ConjunctiveCondition>, C>(get().body, *m_context); }
    auto get_head() const { return View<Index<formalism::Atom<formalism::FluentTag>>, C>(get().head, *m_context); }

    auto identifying_members() const noexcept { return std::tie(m_context, m_data); }
};
}

#endif
