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

#ifndef TYR_FORMALISM_BINARY_OPERATOR_VIEW_HPP_
#define TYR_FORMALISM_BINARY_OPERATOR_VIEW_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/variant.hpp"
#include "tyr/formalism/binary_operator_index.hpp"
#include "tyr/formalism/declarations.hpp"

namespace tyr
{
template<formalism::IsOp Op, typename T, formalism::IsContext C>
class View<Index<formalism::BinaryOperator<Op, T>>, C>
{
private:
    const C* m_context;
    Index<formalism::BinaryOperator<Op, T>> m_data;

public:
    using Tag = formalism::BinaryOperator<Op, T>;
    using OpType = Op;

    View(Index<formalism::BinaryOperator<Op, T>> data, const C& context) : m_context(&context), m_data(data) {}

    const auto& get() const { return get_repository(*m_context)[m_data]; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_data() const noexcept { return m_data; }

    auto get_index() const { return m_data; }
    auto get_lhs() const
    {
        if constexpr (IsViewable<T, C>)
        {
            return View<T, C>(get().lhs, *m_context);
        }
        else
        {
            return get().lhs;
        }
    }
    auto get_rhs() const
    {
        if constexpr (IsViewable<T, C>)
        {
            return View<T, C>(get().rhs, *m_context);
        }
        else
        {
            return get().rhs;
        }
    }

    auto identifying_members() const noexcept { return std::tie(m_context, m_data); }
};

}

#endif
