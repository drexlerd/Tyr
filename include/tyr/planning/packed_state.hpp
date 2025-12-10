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

#ifndef TYR_PLANNING_PACKED_STATE_HPP_
#define TYR_PLANNING_PACKED_STATE_HPP_

#include "tyr/common/config.hpp"
#include "tyr/common/declarations.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/planning/state_index.hpp"

#include <concepts>
#include <valla/valla.hpp>

namespace tyr::planning
{
template<typename T>
concept PackedStateConcept = requires(T state) {
    typename T::TaskType;
    { state.get_index() } -> std::same_as<StateIndex>;
};

template<typename Task>
class PackedState
{
    static_assert(dependent_false<Task>::value, "PackedState is not defined for type T.");
};
}

#endif
