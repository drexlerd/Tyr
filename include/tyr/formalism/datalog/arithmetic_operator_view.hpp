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

#ifndef TYR_FORMALISM_DATALOG_ARITHMETIC_OPERATOR_VIEW_HPP_
#define TYR_FORMALISM_DATALOG_ARITHMETIC_OPERATOR_VIEW_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/variant.hpp"
#include "tyr/formalism/datalog/arithmetic_operator_data.hpp"
#include "tyr/formalism/datalog/declarations.hpp"

namespace tyr
{
template<typename T, formalism::datalog::Context C>
class View<Data<formalism::datalog::ArithmeticOperator<T>>, C>
{
private:
    const C* m_context;
    Data<formalism::datalog::ArithmeticOperator<T>> m_handle;

public:
    View(Data<formalism::datalog::ArithmeticOperator<T>> handle, const C& context) noexcept : m_context(&context), m_handle(handle) {}

    const auto& get_data() const noexcept { return m_handle; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

    auto get_variant() const noexcept { return make_view(m_handle.value, *m_context); }

    auto identifying_members() const noexcept { return std::tie(m_context, m_handle); }
};
}

#endif
