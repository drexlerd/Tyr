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

#ifndef TYR_FORMALISM_ATOM_DATA_HPP_
#define TYR_FORMALISM_ATOM_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/formalism/atom_index.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/predicate_index.hpp"
#include "tyr/formalism/term_data.hpp"

namespace tyr
{
template<formalism::IsStaticOrFluentTag T>
struct Data<formalism::Atom<T>>
{
    using Tag = formalism::Atom<T>;

    Index<formalism::Atom<T>> index;
    Index<formalism::Predicate<T>> predicate;
    DataList<formalism::Term> terms;

    Data() = default;
    Data(Index<formalism::Predicate<T>> predicate, Index<formalism::Atom<T>> index, DataList<formalism::Term> terms) :
        index(index),
        predicate(predicate),
        terms(std::move(terms))
    {
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    auto cista_members() const noexcept { return std::tie(index, predicate, terms); }
    auto identifying_members() const noexcept { return std::tie(predicate, terms); }
};
}

#endif
