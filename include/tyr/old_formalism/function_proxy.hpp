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

#ifndef TYR_FORMALISM_FUNCTION_PROXY_HPP_
#define TYR_FORMALISM_FUNCTION_PROXY_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function_index.hpp"
#include "tyr/formalism/repository.hpp"

namespace tyr::formalism
{
template<IsStaticOrFluentTag T, IsContext C>
class FunctionProxy
{
private:
    const C* context;
    FunctionIndex<T> index;

public:
    FunctionProxy(FunctionIndex<T> index, const C& context) : context(&context), index(index) {}

    const auto& get() const { return get_repository(*context)[index]; }

    auto get_index() const { return index; }
    const auto& get_name() const { return get().name; }
    auto get_arity() const { return get().arity; }
};
}

#endif
