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

template<TaskKind Kind>
class UnpackedState
{
    static_assert(dependent_false<Kind>::value, "UnpackedState is not defined for type T.");
};

template<typename T, typename Kind>
concept UnpackedStateConcept = requires(T& s,
                                        const T& cs,
                                        Index<State<Kind>> index,
                                        Index<formalism::planning::FDRVariable<formalism::FluentTag>> variable,
                                        Data<formalism::planning::FDRFact<formalism::FluentTag>> fact,
                                        Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>> fterm,
                                        float_t value,
                                        Index<formalism::planning::GroundAtom<formalism::DerivedTag>> atom) {
    requires TaskKind<Kind>;
    typename T::TaskType;
    { s.clear() };
    { s.clear_unextended_part() };
    { s.clear_extended_part() };
    { s.assign_unextended_part(cs) };
    { cs.get_index() } -> std::same_as<Index<State<Kind>>>;
    { s.set(index) };
    { cs.get(variable) } -> std::same_as<formalism::planning::FDRValue>;
    { s.set(fact) };
    { cs.get(fterm) } -> std::same_as<float_t>;
    { s.set(fterm, value) };
    { cs.test(atom) } -> std::same_as<bool>;
    { s.set(atom) };
};

}

#endif
