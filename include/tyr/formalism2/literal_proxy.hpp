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

#ifndef TYR_FORMALISM2_LITERAL_PROXY_HPP_
#define TYR_FORMALISM2_LITERAL_PROXY_HPP_

#include "tyr/formalism2/atom_proxy.hpp"
#include "tyr/formalism2/declarations.hpp"
#include "tyr/formalism2/literal_index.hpp"
#include "tyr/formalism2/predicate_proxy.hpp"

namespace tyr
{
template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
class Proxy<formalism::Literal<T>, C>
{
private:
    const C* context;
    Index<formalism::Literal<T>> index;

public:
    using Tag = formalism::Literal<T>;

    Proxy(Index<formalism::Literal<T>> index, const C& context) : context(&context), index(index) {}

    const auto& get() const { return get_repository(*context)[index]; }

    auto get_index() const { return index; }
    auto get_predicate() const { return Proxy<formalism::Predicate<T>, C>(index.predicate_index, *context); }
    auto get_atom() const { return Proxy<formalism::Atom<T>, C>(get().atom_index, *context); }
    auto get_polarity() const { return get().polarity; }
};
}

#endif
