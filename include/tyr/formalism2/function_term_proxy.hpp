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

#ifndef TYR_FORMALISM2_FUNCTION_TERM_PROXY_HPP_
#define TYR_FORMALISM2_FUNCTION_TERM_PROXY_HPP_

#include "tyr/formalism2/declarations.hpp"
#include "tyr/formalism2/function_proxy.hpp"
#include "tyr/formalism2/function_term_index.hpp"
#include "tyr/formalism2/repository.hpp"

namespace tyr
{
template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
class Proxy<formalism::FunctionTerm<T>, C>
{
private:
    const C* context;
    Index<formalism::FunctionTerm<T>> index;

public:
    using Tag = formalism::FunctionTerm<T>;

    Proxy(Index<formalism::FunctionTerm<T>> index, const C& context) : context(&context), index(index) {}

    const auto& get() const { return get_repository(*context)[index]; }

    auto get_index() const { return index; }
    auto get_function() const { return Proxy<formalism::Function<T>, C>(index.group, *context); }
    auto get_terms() const { return SpanProxy<formalism::Term, C>(get().terms, *context); }
};
}

#endif
