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

#ifndef TYR_FORMALISM_DOUBLE_HPP_
#define TYR_FORMALISM_DOUBLE_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/formalism/declarations.hpp"

namespace tyr::formalism
{
struct Double
{
    double value {};

    Double() = default;
    explicit Double(double v) : value(v) {}

    operator double() const noexcept { return value; }

    auto cista_members() const noexcept { return std::tie(value); }
    auto identifying_members() const noexcept { return std::tie(value); }
};

}

#endif
