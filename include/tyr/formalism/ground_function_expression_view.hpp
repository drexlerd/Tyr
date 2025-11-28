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

#ifndef TYR_FORMALISM_GROUND_FUNCTION_EXPRESSION_VIEW_HPP_
#define TYR_FORMALISM_GROUND_FUNCTION_EXPRESSION_VIEW_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/variant.hpp"
#include "tyr/formalism/ground_function_expression_data.hpp"
#include "tyr/formalism/repository.hpp"

namespace tyr
{

template<formalism::IsContext C>
class View<Data<formalism::GroundFunctionExpression>, C>
{
private:
    const C* m_context;
    Data<formalism::GroundFunctionExpression> m_handle;

public:
    using Tag = formalism::GroundFunctionExpression;

    View(Data<formalism::GroundFunctionExpression> handle, const C& context) : m_context(&context), m_handle(handle) {}

    const auto& get_data() const { return m_handle; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

    auto get_variant() const { return View<typename Data<formalism::GroundFunctionExpression>::Variant, C>(m_handle.value, *m_context); }

    auto identifying_members() const noexcept { return std::tie(m_context, m_handle); }
};
}

#endif
