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

#ifndef TYR_FORMALISM_ARITHMETIC_OPERATOR_PROXY_HPP_
#define TYR_FORMALISM_ARITHMETIC_OPERATOR_PROXY_HPP_

#include "tyr/common/variant.hpp"
#include "tyr/formalism/arithmetic_operator.hpp"
#include "tyr/formalism/repository.hpp"

namespace tyr::formalism
{
template<typename T, IsContext C>
class ArithmeticOperatorProxy : public VariantProxy<typename ArithmeticOperator<T>::Variant, C>
{
private:
    using Base = VariantProxy<typename ArithmeticOperator<T>::Variant, C>;

public:
    // Have to pass by const ref because VariantProxy holds a pointer
    ArithmeticOperatorProxy(const ArithmeticOperator<T>& op, const C& context) : Base(op.value, context) {}
};
}

#endif
