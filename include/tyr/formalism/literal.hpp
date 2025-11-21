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

#ifndef TYR_FORMALISM_LITERAL_HPP_
#define TYR_FORMALISM_LITERAL_HPP_

#include "tyr/formalism/atom.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/literal_index.hpp"

namespace tyr::formalism
{
template<IsStaticOrFluentTag T>
struct Literal
{
    LiteralIndex<T> index;
    AtomIndex<T> atom_index;
    bool polarity;

    Literal() = default;
    Literal(LiteralIndex<T> index, AtomIndex<T> atom_index, bool polarity) : index(index), atom_index(atom_index), polarity(polarity) {}

    auto cista_members() const noexcept { return std::tie(index, atom_index, polarity); }
    auto identifying_members() const noexcept { return std::tie(index.predicate_index, atom_index, polarity); }
};

}

#endif
