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

#ifndef TYR_FORMALISM_GROUND_LITERAL_HPP_
#define TYR_FORMALISM_GROUND_LITERAL_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_atom.hpp"

namespace tyr::formalism
{
template<IsStaticOrFluentTag T>
struct GroundLiteralIndex
{
    RelationIndex<T> relation_index {};
    uint_t value {};

    GroundLiteralIndex() = default;
    explicit GroundLiteralIndex(RelationIndex<T> relation_index, uint_t value) : relation_index(relation_index), value(value) {}

    friend bool operator==(const GroundLiteralIndex& lhs, const GroundLiteralIndex& rhs)
    {
        return EqualTo<RelationIndex<T>> {}(lhs.relation_index, rhs.relation_index) && EqualTo<uint_t> {}(lhs.value, rhs.value);
    }

    uint_t get() const noexcept { return value; }

    auto cista_members() const noexcept { return std::tie(relation_index, value); }
    auto identifying_members() const noexcept { return std::tie(relation_index, value); }
};

template<IsStaticOrFluentTag T>
using GroundLiteralIndexList = ::cista::offset::vector<GroundLiteralIndex<T>>;

template<IsStaticOrFluentTag T>
struct GroundLiteralImpl
{
    GroundLiteralIndex<T> index;
    GroundAtomIndex<T> atom_index;
    bool polarity;

    using IndexType = GroundLiteralIndex<T>;

    GroundLiteralImpl() = default;
    GroundLiteralImpl(GroundLiteralIndex<T> index, GroundAtomIndex<T> atom_index, bool polarity) : index(index), atom_index(atom_index), polarity(polarity) {}

    auto cista_members() const noexcept { return std::tie(index, atom_index, polarity); }
    auto identifying_members() const noexcept { return std::tie(index.relation_index, atom_index, polarity); }
};
}

#endif
