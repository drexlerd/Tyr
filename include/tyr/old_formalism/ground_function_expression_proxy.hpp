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

#ifndef TYR_FORMALISM_GROUND_FUNCTION_EXPRESSION_PROXY_HPP_
#define TYR_FORMALISM_GROUND_FUNCTION_EXPRESSION_PROXY_HPP_

#include "tyr/common/variant.hpp"
#include "tyr/formalism/ground_function_expression.hpp"
#include "tyr/formalism/repository.hpp"

namespace tyr::formalism
{
template<IsContext C>
class GroundFunctionExpressionProxy : public VariantProxy<GroundFunctionExpression::Variant, C>
{
private:
    using Base = VariantProxy<GroundFunctionExpression::Variant, C>;

public:
    // Have to pass by const ref because VariantProxy holds a pointer
    GroundFunctionExpressionProxy(const GroundFunctionExpression& fexpr, const C& context) : Base(fexpr.value, context) {}
};
}

#endif
