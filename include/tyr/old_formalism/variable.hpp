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
#include "tyr/formalism/variable_index.hpp"

namespace tyr::formalism
{
struct Variable
{
    VariableIndex index;
    ::cista::offset::string name;

    Variable() = default;
    Variable(VariableIndex index, ::cista::offset::string name) : index(index), name(std::move(name)) {}
    Variable(const Variable& other) = delete;
    Variable& operator=(const Variable& other) = delete;
    Variable(Variable&& other) = default;
    Variable& operator=(Variable&& other) = default;

    auto cista_members() const noexcept { return std::tie(index, name); }
    auto identifying_members() const noexcept { return std::tie(name); }
};
}

#endif
