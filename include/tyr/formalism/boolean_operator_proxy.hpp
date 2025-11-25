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

#ifndef TYR_FORMALISM_BOOLEAN_OPERATOR_PROXY_HPP_
#define TYR_FORMALISM_BOOLEAN_OPERATOR_PROXY_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/variant.hpp"
#include "tyr/formalism/boolean_operator_data.hpp"
#include "tyr/formalism/repository.hpp"

namespace tyr
{
template<typename T, formalism::IsContext C>
class Proxy<Data<formalism::BooleanOperator<T>>, C>
{
private:
    const C* context;
    Data<formalism::BooleanOperator<T>> op;

public:
    using Tag = formalism::BooleanOperator<T>;

    auto get() const { return Proxy<typename Data<formalism::BooleanOperator<T>>::Variant, C>(op.value, *context); }

    auto get_arity() const { return op.arity; }

    Proxy(Data<formalism::BooleanOperator<T>> op, const C& context) : context(&context), op(op) {}
};
}

#endif
