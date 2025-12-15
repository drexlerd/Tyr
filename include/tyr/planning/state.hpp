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

template<typename Derived, typename Task>
class StateMixin
{
private:
    /// @brief Helper to cast to Derived.
    constexpr const auto& self() const { return static_cast<const Derived&>(*this); }
    constexpr auto& self() { return static_cast<Derived&>(*this); }

public:
    StateIndex get_index() const { return self().get_index_impl(); }

    /**
     * Static part
     */

    // Static atoms
    bool test(Index<formalism::GroundAtom<formalism::StaticTag>> index) const { return self().test_impl(index); }

    // Static numeric variables
    float_t get(Index<formalism::GroundFunctionTerm<formalism::StaticTag>> index) const { return self().get_impl(index); }

    /**
     * Fluent part
     */

    // Fluent facts
    formalism::FDRValue get(Index<formalism::FDRVariable<formalism::FluentTag>> index) const { return self().get_impl(index); }

    // Fluent numeric variables
    float_t get(Index<formalism::GroundFunctionTerm<formalism::FluentTag>> index) const { return self().get_impl(index); }

    /**
     * Derived part
     */

    // Derived atoms
    bool test(Index<formalism::GroundAtom<formalism::DerivedTag>> index) const { return self().test_impl(index); }

    /**
     * Unpacked State
     */

    const UnpackedState<Task>& get_unpacked_state() const { return self().get_unpacked_state_impl(); }
    Task& get_task() noexcept { return self().get_task_impl(); }
    const Task& get_task() const noexcept { return self().get_task_impl(); }
};

template<typename Task>
class State
{
    static_assert(dependent_false<Task>::value, "State is not defined for type T.");
};
}

#endif
