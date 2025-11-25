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

#ifndef TYR_FORMALISM_MULTI_OPERATOR_PROXY_HPP_
#define TYR_FORMALISM_MULTI_OPERATOR_PROXY_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function_expression_proxy.hpp"
#include "tyr/formalism/multi_operator_index.hpp"

namespace tyr
{
template<formalism::IsOp Op, typename T, formalism::IsContext C>
class Proxy<Index<formalism::MultiOperator<Op, T>>, C>
{
private:
    const C* context;
    Index<formalism::MultiOperator<Op, T>> index;

public:
    using Tag = formalism::MultiOperator<Op, T>;
    using OpType = Op;

    Proxy(Index<formalism::MultiOperator<Op, T>> index, const C& context) : context(&context), index(index) {}

    const auto& get() const { return get_repository(*context)[index]; }

    auto get_index() const { return index; }
    auto get_args() const { return Proxy<::cista::offset::vector<T>, C>(get().args, *context); }
};

}

#endif
