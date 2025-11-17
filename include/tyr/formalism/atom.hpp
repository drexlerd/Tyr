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

#ifndef TYR_FORMALISM_ATOM_HPP_
#define TYR_FORMALISM_ATOM_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/relation.hpp"
#include "tyr/formalism/variable.hpp"

namespace tyr::formalism
{
template<IsStaticOrFluentTag T>
struct AtomIndex
{
    uint_t value {};

    AtomIndex() = default;
    explicit AtomIndex(uint_t value) : value(value) {}

    uint_t get() const noexcept { return value; }

    auto cista_members() const noexcept { return std::tie(value); }
};

template<IsStaticOrFluentTag T>
class Atom
{
private:
    AtomIndex<T> m_index;
    RelationIndex<T> m_relation_index;
    VariableList m_terms;

public:
    Atom();
    Atom(AtomIndex<T> index, RelationIndex<T> relation_index, VariableList terms);

    AtomIndex<T> get_index() const noexcept;
    RelationIndex<T> get_relation_index() const noexcept;
    const VariableList& get_terms() const noexcept;

    auto cista_members() const noexcept { return std::tie(m_index, m_relation_index, m_terms); }
};

template<IsStaticOrFluentTag T>
using AtomIndexList = cista::offset::vector<AtomIndex<T>>;
}

#endif
