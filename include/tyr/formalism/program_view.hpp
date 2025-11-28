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

#ifndef TYR_FORMALISM_PROGRAM_VIEW_HPP_
#define TYR_FORMALISM_PROGRAM_VIEW_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function_index.hpp"
#include "tyr/formalism/ground_atom_index.hpp"
#include "tyr/formalism/ground_function_term_value_index.hpp"
#include "tyr/formalism/predicate_index.hpp"
#include "tyr/formalism/program_index.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/rule_view.hpp"

namespace tyr
{
template<formalism::IsContext C>
class View<Index<formalism::Program>, C>
{
private:
    const C* m_context;
    Index<formalism::Program> m_handle;

public:
    using Tag = formalism::Program;

    View(Index<formalism::Program> data, const C& context) : m_context(&context), m_handle(data) {}

    const auto& get_data() const { return get_repository(*m_context)[m_handle]; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

    auto get_index() const noexcept { return m_handle; }
    template<formalism::IsFactTag T>
    auto get_predicates() const
    {
        return View<IndexList<formalism::Predicate<T>>, C>(get_data().template get_predicates<T>(), *m_context);
    }
    template<formalism::IsFactTag T>
    auto get_functions() const
    {
        return View<IndexList<formalism::Function<T>>, C>(get_data().template get_functions<T>(), *m_context);
    }
    auto get_objects() const { return View<IndexList<formalism::Object>, C>(get_data().objects, *m_context); }
    template<formalism::IsFactTag T>
    auto get_atoms() const
    {
        return View<IndexList<formalism::GroundAtom<T>>, C>(get_data().template get_atoms<T>(), *m_context);
    }
    template<formalism::IsFactTag T>
    auto get_fterm_values() const
    {
        return View<IndexList<formalism::GroundFunctionTermValue<T>>, C>(get_data().template get_fterm_values<T>(), *m_context);
    }
    auto get_rules() const { return View<IndexList<formalism::Rule>, C>(get_data().rules, *m_context); }

    auto identifying_members() const noexcept { return std::tie(m_context, m_handle); }
};
}

#endif
