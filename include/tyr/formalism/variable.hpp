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

#ifndef TYR_FORMALISM_VARIABLE_HPP_
#define TYR_FORMALISM_VARIABLE_HPP_

#include "tyr/formalism/declarations.hpp"

namespace tyr::formalism
{
enum class Variable : int_t
{
};

using VariableList = cista::offset::vector<Variable>;

constexpr int_t to_int(Variable v) noexcept { return static_cast<int_t>(v); }

constexpr bool is_constant(Variable variable) noexcept { return to_int(variable) >= 0; }

constexpr bool is_variable(Variable variable) noexcept { return to_int(variable) < 0; }

constexpr uint_t get_constant_index(Variable variable) noexcept
{
    assert(is_constant(variable));
    return to_int(variable);
}

constexpr uint_t get_parameter_index(Variable variable) noexcept
{
    assert(is_variable(variable));
    return -(to_int(variable) + 1);
}
}

#endif
