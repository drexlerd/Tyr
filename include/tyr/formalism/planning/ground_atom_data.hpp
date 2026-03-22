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

#ifndef TYR_FORMALISM_PLANNING_GROUND_ATOM_DATA_HPP_
#define TYR_FORMALISM_PLANNING_GROUND_ATOM_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/binding_index.hpp"
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/ground_atom_index.hpp"
#include "tyr/formalism/predicate_index.hpp"

namespace tyr
{
template<formalism::FactKind T>
struct Data<formalism::planning::GroundAtom<T>>
{
    Index<formalism::planning::GroundAtom<T>> index;
    Index<formalism::RelationBinding<formalism::Predicate<T>>> binding;

    Data() = default;
    Data(Index<formalism::RelationBinding<formalism::Predicate<T>>> binding_) : index(), binding(binding_) {}
    // Python constructor
    template<typename C>
    Data(View<Index<formalism::RelationBinding<formalism::Predicate<T>>>, C> binding_) : index(), binding()
    {
        set(binding_, binding);
    }
    Data(const Data& other) = default;
    Data& operator=(const Data& other) = default;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    void clear() noexcept
    {
        tyr::clear(index);
        tyr::clear(binding);
    }

    auto cista_members() const noexcept { return std::tie(index, binding); }
    auto identifying_members() const noexcept { return std::tie(binding); }
};

static_assert(uses_trivial_storage_v<formalism::planning::GroundAtom<formalism::StaticTag>>);

}

#endif
