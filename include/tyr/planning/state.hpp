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

#ifndef TYR_PLANNING_STATE_HPP_
#define TYR_PLANNING_STATE_HPP_

#include "tyr/common/shared_object_pool.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/state_index.hpp"
#include "tyr/planning/unpacked_state.hpp"

#include <concepts>

namespace tyr::planning
{
template<typename T>
concept StateConcept = requires(T state) {
    typename T::TaskType;
    { state.get_index() } -> std::same_as<StateIndex>;
    { state.get_task() } -> std::same_as<typename T::TaskType&>;
    { state.get_unpacked_state() } -> std::same_as<const UnpackedState<typename T::TaskType>&>;
};

template<typename Task>
class State
{
    static_assert(dependent_false<Task>::value, "State is not defined for type T.");
};
}

#endif
