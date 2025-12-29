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

#ifndef TYR_FORMALISM_PLANNING_DOMAIN_VIEW_HPP_
#define TYR_FORMALISM_PLANNING_DOMAIN_VIEW_HPP_

#include "tyr/common/optional.hpp"
#include "tyr/common/types.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/function_view.hpp"
#include "tyr/formalism/object_view.hpp"
#include "tyr/formalism/planning/action_view.hpp"
#include "tyr/formalism/planning/axiom_view.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/domain_index.hpp"
#include "tyr/formalism/predicate_view.hpp"

namespace tyr
{

template<formalism::planning::Context C>
class View<Index<formalism::planning::Domain>, C>
{
private:
    const C* m_context;
    Index<formalism::planning::Domain> m_handle;

public:
    View(Index<formalism::planning::Domain> handle, const C& context) noexcept : m_context(&context), m_handle(handle) {}

    const auto& get_data() const noexcept { return get_repository(*m_context)[m_handle]; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

    auto get_index() const noexcept { return m_handle; }
    const auto& get_name() const noexcept { return get_data().name; }
    template<formalism::FactKind T>
    auto get_predicates() const noexcept
    {
        return make_view(get_data().template get_predicates<T>(), *m_context);
    }
    template<formalism::FactKind T>
    auto get_functions() const noexcept
    {
        return make_view(get_data().template get_functions<T>(), *m_context);
    }
    auto get_auxiliary_function() const noexcept { return make_view(get_data().auxiliary_function, *m_context); }
    auto get_constants() const noexcept { return make_view(get_data().constants, *m_context); }
    auto get_actions() const noexcept { return make_view(get_data().actions, *m_context); }
    auto get_axioms() const noexcept { return make_view(get_data().axioms, *m_context); }

    auto identifying_members() const noexcept { return std::tie(m_context, m_handle); }
};
}

#endif
