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

#ifndef TYR_FORMALISM_RULE_PROXY_HPP_
#define TYR_FORMALISM_RULE_PROXY_HPP_

#include "tyr/common/span.hpp"
#include "tyr/formalism/conjunctive_condition_proxy.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/rule_index.hpp"

namespace tyr
{
template<formalism::IsContext C>
class Proxy<formalism::Rule, C>
{
private:
    const C* context;
    Index<formalism::Rule> index;

public:
    using Tag = formalism::Rule;

    Proxy(Index<formalism::Rule> index, const C& context) : context(&context), index(index) {}

    const auto& get() const { return get_repository(*context)[index]; }

    auto get_index() const { return index; }
    auto get_body() const { return Proxy<formalism::ConjunctiveCondition, C>(get().body, *context); }
    auto get_head() const { return Proxy<formalism::Atom<formalism::FluentTag>, C>(get().head, *context); }
};
}

#endif
