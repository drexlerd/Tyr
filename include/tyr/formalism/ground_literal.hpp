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
    uint_t value {};

    GroundLiteralIndex() = default;
    explicit GroundLiteralIndex(uint_t value) : value(value) {}

    uint_t get() const noexcept { return value; }

    auto cista_members() const noexcept { return std::tie(value); }
};

template<IsStaticOrFluentTag T>
class GroundLiteral
{
private:
    GroundLiteralIndex<T> m_index;
    GroundAtomIndex<T> m_atom_index;
    bool m_polarity;

public:
    GroundLiteral();
    GroundLiteral(GroundLiteralIndex<T> index, GroundAtomIndex<T> atom_index, bool polarity);

    GroundLiteralIndex<T> get_index() const noexcept;
    GroundAtomIndex<T> get_atom_index() const noexcept;
    bool get_polarity() const noexcept;

    auto cista_members() const noexcept { return std::tie(m_index, m_atom_index, m_polarity); }
};

template<IsStaticOrFluentTag T>
using GroundLiteralIndexList = cista::offset::vector<GroundLiteralIndex<T>>;
}

#endif
