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

#ifndef TYR_FORMALISM_GROUND_ATOM_HPP_
#define TYR_FORMALISM_GROUND_ATOM_HPP_

#include "tyr/formalism/constant.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/relation.hpp"

namespace tyr::formalism
{
template<IsStaticOrFluentTag T>
struct GroundAtomIndex
{
    uint_t value {};

    GroundAtomIndex() = default;
    explicit GroundAtomIndex(uint_t value) : value(value) {}

    uint_t get() const noexcept { return value; }

    auto cista_members() const noexcept { return std::tie(value); }
};

template<IsStaticOrFluentTag T>
class GroundAtom
{
private:
    GroundAtomIndex<T> m_index;
    RelationIndex<T> m_relation_index;
    ConstantList m_constants;

public:
    using IndexType = GroundAtomIndex<T>;

    GroundAtom();
    GroundAtom(GroundAtomIndex<T> index, RelationIndex<T> relation_index, ConstantList constants);

    GroundAtomIndex<T> get_index() const noexcept;
    RelationIndex<T> get_relation_index() const noexcept;
    const ConstantList& get_constants() const noexcept;

    auto cista_members() const noexcept { return std::tie(m_index, m_relation_index, m_constants); }
};

template<IsStaticOrFluentTag T>
using GroundAtomIndexList = cista::offset::vector<GroundAtomIndex<T>>;
}

#endif
