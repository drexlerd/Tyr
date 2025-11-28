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

#ifndef TYR_FORMALISM_GROUND_ATOM_DATA_HPP_
#define TYR_FORMALISM_GROUND_ATOM_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_atom_index.hpp"
#include "tyr/formalism/object_index.hpp"

namespace tyr
{
template<formalism::IsFactTag T>
struct Data<formalism::GroundAtom<T>>
{
    using Tag = formalism::GroundAtom<T>;

    Index<formalism::GroundAtom<T>> index;
    Index<formalism::Predicate<T>> predicate;
    IndexList<formalism::Object> objects;

    Data() = default;
    Data(Index<formalism::GroundAtom<T>> index, Index<formalism::Predicate<T>> predicate, IndexList<formalism::Object> objects) :
        index(index),
        predicate(predicate),
        objects(std::move(objects))
    {
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    auto cista_members() const noexcept { return std::tie(index, predicate, objects); }
    auto identifying_members() const noexcept { return std::tie(predicate, objects); }
};

}

#endif
