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

#ifndef TYR_FORMALISM_VARIABLE_PROXY_HPP_
#define TYR_FORMALISM_VARIABLE_PROXY_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/variable_index.hpp"

namespace tyr::formalism
{
template<IsContext C>
class VariableProxy
{
private:
    const C* context;
    VariableIndex index;

public:
    VariableProxy(VariableIndex index, const C& context) : context(&context), index(index) {}

    const auto& get() const { return get_repository(*context)[index]; }

    auto get_index() const { return index; }
    const auto& get_name() const { return get().name; }
};
}

#endif
