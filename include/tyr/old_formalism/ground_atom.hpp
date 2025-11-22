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

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_atom_index.hpp"
#include "tyr/formalism/object_index.hpp"

namespace tyr::formalism
{
template<IsStaticOrFluentTag T>
struct GroundAtom
{
    GroundAtomIndex<T> index;
    ObjectIndexList terms;

    GroundAtom() = default;
    GroundAtom(GroundAtomIndex<T> index, ObjectIndexList terms) : index(index), terms(std::move(terms)) {}
    GroundAtom(const GroundAtom& other) = delete;
    GroundAtom& operator=(const GroundAtom& other) = delete;
    GroundAtom(GroundAtom&& other) = default;
    GroundAtom& operator=(GroundAtom&& other) = default;

    auto cista_members() const noexcept { return std::tie(index, terms); }
    auto identifying_members() const noexcept { return std::tie(index.group, terms); }
};

}

#endif
