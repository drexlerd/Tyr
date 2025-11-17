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

namespace tyr::formalism
{
template<IsStaticOrFluentTag T>
struct LiteralIndex
{
    uint_t value {};

    LiteralIndex() = default;
    explicit LiteralIndex(uint_t value) : value(value) {}

    uint_t get() const noexcept { return value; }

    auto cista_members() const noexcept { return std::tie(value); }
};

template<IsStaticOrFluentTag T>
class Literal
{
private:
    LiteralIndex<T> m_index;
    AtomIndex<T> m_atom_index;
    bool m_polarity;

public:
    using IndexType = LiteralIndex<T>;

    Literal();
    Literal(LiteralIndex<T> index, AtomIndex<T> atom_index, bool polarity);

    LiteralIndex<T> get_index() const noexcept;
    AtomIndex<T> get_atom_index() const noexcept;
    bool get_polarity() const noexcept;

    auto cista_members() const noexcept { return std::tie(m_index, m_atom_index, m_polarity); }
};

template<IsStaticOrFluentTag T>
using LiteralIndexList = cista::offset::vector<LiteralIndex<T>>;
}

#endif
