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

#ifndef TYR_FORMALISM_BINDING_DATA_HPP_
#define TYR_FORMALISM_BINDING_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/binding_index.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/object_index.hpp"

namespace tyr
{
template<typename T>
struct Data<formalism::RelationBinding<T>>
{
    Index<T> relation;
    IndexList<formalism::Object> objects;

    Data() = default;
    Data(Index<T> relation_, size_t arity_, IndexList<formalism::Object> objects_) : relation(relation_), objects(std::move(objects_)) {}
    // Python constructor
    template<typename C>
    Data(View<Index<T>, C> relation_, const std::vector<View<Index<formalism::Object>, C>>& objects_) : relation(), objects()
    {
        set(relation_, relation);
        set(objects_, objects);
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    void clear() noexcept
    {
        tyr::clear(relation);
        tyr::clear(objects);
    }

    auto cista_members() const noexcept { return std::tie(relation, objects); }
    auto identifying_members() const noexcept { return std::tie(relation, objects); }
};

}

#endif
