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

#ifndef TYR_FORMALISM_ATOM_INDEX_HPP_
#define TYR_FORMALISM_ATOM_INDEX_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/predicate_index.hpp"

namespace tyr::formalism
{
template<IsStaticOrFluentTag T>
struct AtomIndex
{
    PredicateIndex<T> predicate_index {};
    uint_t value {};

    AtomIndex() = default;
    explicit AtomIndex(PredicateIndex<T> predicate_index, uint_t value) : predicate_index(predicate_index), value(value) {}

    friend bool operator==(const AtomIndex& lhs, const AtomIndex& rhs)
    {
        return EqualTo<PredicateIndex<T>> {}(lhs.predicate_index, rhs.predicate_index) && EqualTo<uint_t> {}(lhs.value, rhs.value);
    }

    uint_t get() const noexcept { return value; }

    auto cista_members() const noexcept { return std::tie(predicate_index, value); }
    auto identifying_members() const noexcept { return std::tie(predicate_index, value); }
};

template<IsStaticOrFluentTag T>
using AtomIndexList = ::cista::offset::vector<AtomIndex<T>>;

}

#endif
