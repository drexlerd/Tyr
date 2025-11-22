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

#include "tyr/common/variant.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/parameter_index.hpp"

namespace tyr::formalism
{
struct Term
{
    using Variant = ::cista::offset::variant<ObjectIndex, ParameterIndex>;

    Variant value;

    Term() = default;
    Term(Variant value) : value(value) {}

    friend bool operator==(const Term& lhs, const Term& rhs) { return EqualTo<Variant> {}(lhs.value, rhs.value); }

    auto cista_members() const noexcept { return std::tie(value); }
    auto identifying_members() const noexcept { return std::tie(value); }
};

using TermList = ::cista::offset::vector<Term>;

}

#endif
