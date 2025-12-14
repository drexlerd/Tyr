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

#ifndef TYR_PLANNING_UNPACKED_STATE_HPP_
#define TYR_PLANNING_UNPACKED_STATE_HPP_

#include "tyr/common/config.hpp"
#include "tyr/common/declarations.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/planning/fdr_value.hpp"
#include "tyr/planning/state_index.hpp"

#include <boost/dynamic_bitset.hpp>
#include <concepts>

namespace tyr::planning
{

template<typename Derived>
class UnpackedStateMixin
{
private:
    /// @brief Helper to cast to Derived.
    constexpr const auto& self() const { return static_cast<const Derived&>(*this); }
    constexpr auto& self() { return static_cast<Derived&>(*this); }

public:
    void clear() { self().clear_impl(); }

    StateIndex get_index() const { return self().get_index_impl(); }
    void set(StateIndex index) { self().set_impl(index); }

    /**
     * Fluent part
     */

    // Fluent facts
    formalism::FDRValue get(Index<formalism::FDRVariable<formalism::FluentTag>> index) const { return self().get_impl(index); }
    void set(Data<formalism::FDRFact<formalism::FluentTag>> fact) { self().set_impl(fact); }

    // Fluent numeric variables
    float_t get(Index<formalism::GroundFunctionTerm<formalism::FluentTag>> index) const { return self().get_impl(index); }
    void set(Index<formalism::GroundFunctionTerm<formalism::FluentTag>> index, float_t value) { self().set_impl(index, value); }

    /**
     * Derived part
     */

    // Derived atoms
    bool test(Index<formalism::GroundAtom<formalism::DerivedTag>> index) const { return self().test_impl(index); }
    void set(Index<formalism::GroundAtom<formalism::DerivedTag>> index) { self().set_impl(index); }
};

template<typename Task>
class UnpackedState
{
    static_assert(dependent_false<Task>::value, "UnpackedState is not defined for type T.");
};
}

#endif
