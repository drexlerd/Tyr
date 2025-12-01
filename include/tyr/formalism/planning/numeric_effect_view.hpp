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

#ifndef TYR_FORMALISM_PLANNING_NUMERIC_EFFECT_VIEW_HPP_
#define TYR_FORMALISM_PLANNING_NUMERIC_EFFECT_VIEW_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/variant.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function_expression_view.hpp"
#include "tyr/formalism/function_term_view.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/planning/numeric_effect_index.hpp"
#include "tyr/formalism/repository.hpp"

namespace tyr
{

template<formalism::NumericEffectOpKind Op, formalism::FactKind T, formalism::Context C>
class View<Index<formalism::NumericEffect<Op, T>>, C>
{
    static_assert(std::same_as<T, formalism::FluentTag> || (std::same_as<T, formalism::AuxiliaryTag> && std::same_as<Op, formalism::OpIncrease>),
                  "Unsupported NumericEffect<Op, T> combination.");

private:
    const C* m_context;
    Index<formalism::NumericEffect<Op, T>> m_handle;

public:
    using Tag = formalism::NumericEffect<Op, T>;

    View(Index<formalism::NumericEffect<Op, T>> handle, const C& context) : m_context(&context), m_handle(handle) {}

    const auto& get_data() const { return get_repository(*m_context)[m_handle]; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

    auto get_index() const noexcept { return m_handle; }
    auto get_fterm() const noexcept { return View<Index<formalism::FunctionTerm<T>>, C>(get_data().fterm, *m_context); }
    auto get_fexpr() const noexcept { return View<Data<formalism::FunctionExpression>, C>(get_data().fexpr, *m_context); }

    auto identifying_members() const noexcept { return std::tie(m_context, m_handle); }
};

}

#endif