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

#ifndef TYR_FORMALISM2_GROUND_LITERAL_DATA_HPP_
#define TYR_FORMALISM2_GROUND_LITERAL_DATA_HPP_

#include "tyr/formalism2/declarations.hpp"
#include "tyr/formalism2/ground_atom_index.hpp"
#include "tyr/formalism2/ground_literal_index.hpp"

namespace tyr
{

template<formalism::IsStaticOrFluentTag T>
struct Data<formalism::GroundLiteral<T>>
{
    using Tag = formalism::GroundLiteral<T>;

    Index<formalism::GroundLiteral<T>> index;
    Index<formalism::GroundAtom<T>> atom_index;
    bool polarity;

    Data() = default;
    Data(Index<formalism::GroundLiteral<T>> index, Index<formalism::GroundAtom<T>> atom_index, bool polarity) :
        index(index),
        atom_index(atom_index),
        polarity(polarity)
    {
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    auto cista_members() const noexcept { return std::tie(index, atom_index, polarity); }
    auto identifying_members() const noexcept { return std::tie(index.group, atom_index, polarity); }
};
}

#endif
