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

#ifndef TYR_FORMALISM_GROUND_ATOM_VIEW_HPP_
#define TYR_FORMALISM_GROUND_ATOM_VIEW_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_atom_index.hpp"
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/predicate_view.hpp"
#include "tyr/formalism/repository.hpp"

namespace tyr
{
template<formalism::IsFactTag T, formalism::IsContext C>
class View<Index<formalism::GroundAtom<T>>, C>
{
private:
    const C* m_context;
    Index<formalism::GroundAtom<T>> m_handle;

public:
    using Tag = formalism::GroundAtom<T>;

    View(Index<formalism::GroundAtom<T>> handle, const C& context) : m_context(&context), m_handle(handle) {}

    const auto& get_data() const { return get_repository(*m_context)[m_handle]; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

    auto get_index() const noexcept { return m_handle; }
    auto get_predicate() const { return View<Index<formalism::Predicate<T>>, C>(get_data().predicate, *m_context); }
    auto get_objects() const { return View<IndexList<formalism::Object>, C>(get_data().objects, *m_context); }

    auto identifying_members() const noexcept { return std::tie(m_context, m_handle); }
};
}

#endif
