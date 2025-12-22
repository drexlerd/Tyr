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

template<typename Task>
class UnpackedState
{
    static_assert(dependent_false<Task>::value, "UnpackedState is not defined for type T.");
};

template<typename T>
concept UnpackedStateConcept = requires(T& a,
                                        const T& b,
                                        StateIndex index,
                                        Index<formalism::FDRVariable<formalism::FluentTag>> variable,
                                        Data<formalism::FDRFact<formalism::FluentTag>> fact,
                                        Index<formalism::GroundFunctionTerm<formalism::FluentTag>> fterm,
                                        float_t value,
                                        Index<formalism::GroundAtom<formalism::DerivedTag>> atom) {
    typename T::TaskType;
    { a.clear() };
    { b.get_index() } -> std::same_as<StateIndex>;
    { a.set(index) };
    { b.get(variable) } -> std::same_as<formalism::FDRValue>;
    { a.set(fact) };
    { b.get(fterm) } -> std::same_as<float_t>;
    { a.set(fterm, value) };
    { b.test(atom) } -> std::same_as<bool>;
    { a.set(atom) };
};

}

#endif
