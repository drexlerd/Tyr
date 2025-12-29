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

#ifndef TYR_FORMALISM_DATALOG_ATOM_DATA_HPP_
#define TYR_FORMALISM_DATALOG_ATOM_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/datalog/atom_index.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/predicate_index.hpp"
#include "tyr/formalism/term_data.hpp"

namespace tyr
{
template<formalism::FactKind T>
struct Data<formalism::datalog::Atom<T>>
{
    Index<formalism::datalog::Atom<T>> index;
    Index<formalism::Predicate<T>> predicate;
    DataList<formalism::Term> terms;

    Data() = default;
    Data(Index<formalism::Predicate<T>> predicate, Index<formalism::datalog::Atom<T>> index, DataList<formalism::Term> terms) :
        index(index),
        predicate(predicate),
        terms(std::move(terms))
    {
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    void clear() noexcept
    {
        tyr::clear(index);
        tyr::clear(predicate);
        tyr::clear(terms);
    }

    auto cista_members() const noexcept { return std::tie(index, predicate, terms); }
    auto identifying_members() const noexcept { return std::tie(predicate, terms); }
};
}

#endif
