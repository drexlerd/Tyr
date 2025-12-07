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

#ifndef TYR_FORMALISM_MULTI_OPERATOR_VIEW_HPP_
#define TYR_FORMALISM_MULTI_OPERATOR_VIEW_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function_expression_view.hpp"
#include "tyr/formalism/multi_operator_index.hpp"

namespace tyr
{
template<formalism::OpKind Op, typename T, formalism::Context C>
class View<Index<formalism::MultiOperator<Op, T>>, C>
{
private:
    const C* m_context;
    Index<formalism::MultiOperator<Op, T>> m_handle;

public:
    using Tag = formalism::MultiOperator<Op, T>;
    using OpType = Op;

    View(Index<formalism::MultiOperator<Op, T>> handle, const C& context) : m_context(&context), m_handle(handle) {}

    const auto& get_data() const { return get_repository(*m_context)[m_handle]; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

    auto get_index() const noexcept { return m_handle; }
    auto get_args() const { return View<::cista::offset::vector<T>, C>(get_data().args, *m_context); }

    auto identifying_members() const noexcept { return std::tie(m_context, m_handle); }
};

}

#endif
