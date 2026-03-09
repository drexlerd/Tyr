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

#ifndef TYR_PLANNING_STATE_REPOSITORY_HPP_
#define TYR_PLANNING_STATE_REPOSITORY_HPP_

#include "tyr/common/shared_object_pool.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/state.hpp"
#include "tyr/planning/state_index.hpp"
#include "tyr/planning/state_repository.hpp"
#include "tyr/planning/unpacked_state.hpp"

#include <concepts>

namespace tyr::planning
{

template<typename Task>
class StateRepository
{
    static_assert(dependent_false<Task>::value, "State is not defined for type T.");
};

template<typename T, typename Task>
concept StateRepositoryConcept =
    requires(T& r,
             std::shared_ptr<Task> task,
             StateIndex index,
             SharedObjectPoolPtr<UnpackedState<Task>> unregistered_state,
             const std::vector<Data<formalism::planning::FDRFact<formalism::FluentTag>>>& fluent_facts,
             const std::vector<std::pair<Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>>, float_t>>& fterm_values,
             const std::vector<formalism::planning::FDRFactView<formalism::FluentTag>>& fluent_fact_views,
             const std::vector<formalism::planning::GroundFunctionTermViewValuePair<formalism::FluentTag>>& fterm_value_views) {
        { T(task) };
        { r.get_initial_state() } -> std::same_as<State<Task>>;
        { r.get_registered_state(index) } -> std::same_as<State<Task>>;
        { r.create_state(fluent_facts, fterm_values) } -> std::same_as<State<Task>>;
        { r.create_state(fluent_fact_views, fterm_value_views) } -> std::same_as<State<Task>>;
        { r.get_unregistered_state() } -> std::same_as<SharedObjectPoolPtr<UnpackedState<Task>>>;
        { r.register_state(unregistered_state) } -> std::same_as<State<Task>>;
        { r.get_task() } -> std::same_as<const std::shared_ptr<Task>&>;
    };
}

#endif
