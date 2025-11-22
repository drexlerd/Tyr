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

#ifndef TYR_FORMALISM_GROUND_FUNCTION_TERM_VALUE_PROXY_HPP_
#define TYR_FORMALISM_GROUND_FUNCTION_TERM_VALUE_PROXY_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_function_term_proxy.hpp"
#include "tyr/formalism/ground_function_term_value_index.hpp"
#include "tyr/formalism/repository.hpp"

namespace tyr
{
template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
class Proxy<formalism::GroundFunctionTermValue<T>, C>
{
private:
    const C* context;
    Index<formalism::GroundFunctionTermValue<T>> index;

public:
    using Tag = formalism::GroundFunctionTermValue<T>;

    Proxy(Index<formalism::GroundFunctionTermValue<T>> index, const C& context) : context(&context), index(index) {}

    const auto& get() const { return get_repository(*context)[index]; }

    auto get_index() const { return index; }
    auto get_term() const { return Proxy<formalism::GroundFunctionTerm<T>, C>(get().term, *context); }
    auto get_value() const { return get().value; }
};
}

#endif
