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

#ifndef TYR_FORMALISM2_MULTI_OPERATOR_PROXY_HPP_
#define TYR_FORMALISM2_MULTI_OPERATOR_PROXY_HPP_

#include "tyr/common/span.hpp"
#include "tyr/common/types.hpp"
#include "tyr/formalism2/declarations.hpp"
#include "tyr/formalism2/multi_operator_index.hpp"

namespace tyr
{
template<formalism::IsOp Op, typename T, formalism::IsContext C>
class Proxy<formalism::MultiOperator<Op, T>, C>
{
private:
    const C* context;
    Index<formalism::MultiOperator<Op, T>> index;

public:
    using Tag = formalism::MultiOperator<Op, T>;

    Proxy(Index<formalism::MultiOperator<Op, T>> index, const C& context) : context(&context), index(index) {}

    const auto& get() const { return get_repository(*context)[index]; }

    auto get_index() const { return index; }
    auto get_args() const
    {
        if constexpr (!HasTag<T>)
        {
            SpanProxy<T, C>(get().args, *context);
        }
        else
        {
            return SpanProxy<typename T::Tag, C>(get().args, *context);
        }
    }
};

}

#endif
