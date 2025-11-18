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

#ifndef TYR_FORMALISM_TERM_HPP_
#define TYR_FORMALISM_TERM_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/double.hpp"
#include "tyr/formalism/signed.hpp"
#include "tyr/formalism/symbol.hpp"
#include "tyr/formalism/unsigned.hpp"
#include "tyr/formalism/variable.hpp"

namespace tyr::formalism
{
struct Term
{
    using Variant = ::cista::offset::variant<SymbolIndex, VariableIndex, Unsigned, Signed, Double>;

    Variant value;

    Term() = default;
    Term(Variant value) : value(value) {}

    auto cista_members() const noexcept { return std::tie(value); }
    auto identifying_members() const noexcept { return std::tie(value); }
};

using TermList = ::cista::offset::vector<Term>;

}

#endif
