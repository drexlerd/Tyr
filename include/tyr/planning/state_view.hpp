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

#ifndef TYR_PLANNING_STATE_VIEW_HPP_
#define TYR_PLANNING_STATE_VIEW_HPP_

#include "tyr/common/shared_object_pool.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/state_index.hpp"
#include "tyr/planning/task.hpp"
#include "tyr/planning/unpacked_state.hpp"

#include <concepts>
#include <ranges>

namespace tyr
{

template<planning::TaskKind Kind, typename C>
class View<Index<planning::State<Kind>>, C>
{
    static_assert(dependent_false<Kind>::value, "State is not defined for type Kind.");
};

namespace planning
{
template<TaskKind Kind>
using StateView = View<Index<State<Kind>>, std::shared_ptr<StateRepository<Kind>>>;

/**
 * IterableStateConcept
 */

template<class R, class Tag>
concept AtomRangeConcept =
    std::ranges::input_range<R> && std::same_as<std::remove_cvref_t<std::ranges::range_value_t<R>>, Index<formalism::planning::GroundAtom<Tag>>>;

template<class R, class Tag>
concept FactRangeConcept =
    std::ranges::input_range<R> && std::same_as<std::remove_cvref_t<std::ranges::range_value_t<R>>, Data<formalism::planning::FDRFact<Tag>>>;

template<class R, class Tag>
concept FunctionTermValueRangeConcept =
    std::ranges::input_range<R>
    && std::same_as<std::remove_cvref_t<std::ranges::range_value_t<R>>, std::pair<Index<formalism::planning::GroundFunctionTerm<Tag>>, float_t>>;

template<typename T>
concept IterableStateConcept = requires(const T& cs) {
    requires AtomRangeConcept<decltype(cs.get_static_atoms()), formalism::StaticTag>;
    requires FactRangeConcept<decltype(cs.get_fluent_facts()), formalism::FluentTag>;
    requires AtomRangeConcept<decltype(cs.get_derived_atoms()), formalism::DerivedTag>;
    requires FunctionTermValueRangeConcept<decltype(cs.get_static_fterm_values()), formalism::StaticTag>;
    requires FunctionTermValueRangeConcept<decltype(cs.get_fluent_fterm_values()), formalism::FluentTag>;
};

/**
 * IterableViewStateConcept
 */

template<class R, class Tag>
concept AtomViewRangeConcept =
    std::ranges::input_range<R> && std::same_as<std::remove_cvref_t<std::ranges::range_value_t<R>>, formalism::planning::GroundAtomView<Tag>>;

template<class R, class Tag>
concept FactViewRangeConcept =
    std::ranges::input_range<R> && std::same_as<std::remove_cvref_t<std::ranges::range_value_t<R>>, formalism::planning::FDRFactView<Tag>>;

template<class R, class Tag>
concept FunctionTermViewValueRangeConcept =
    std::ranges::input_range<R> && std::same_as<std::remove_cvref_t<std::ranges::range_value_t<R>>, formalism::planning::GroundFunctionTermViewValuePair<Tag>>;

template<typename T>
concept IterableViewStateConcept = requires(const T& cs) {
    requires AtomViewRangeConcept<decltype(cs.get_static_atoms_view()), formalism::StaticTag>;
    requires FactViewRangeConcept<decltype(cs.get_fluent_facts_view()), formalism::FluentTag>;
    requires AtomViewRangeConcept<decltype(cs.get_derived_atoms_view()), formalism::DerivedTag>;
    requires FunctionTermViewValueRangeConcept<decltype(cs.get_static_fterm_values_view()), formalism::StaticTag>;
    requires FunctionTermViewValueRangeConcept<decltype(cs.get_fluent_fterm_values_view()), formalism::FluentTag>;
};

/**
 * IndexableStateConcept
 */

template<typename T, typename Kind>
concept IndexableStateConcept = requires(const T& cs,
                                         Index<formalism::planning::FDRVariable<formalism::FluentTag>> variable,
                                         Index<formalism::planning::GroundFunctionTerm<formalism::StaticTag>> static_fterm,
                                         Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>> fluent_fterm,
                                         Index<formalism::planning::GroundAtom<formalism::StaticTag>> static_atom,
                                         Index<formalism::planning::GroundAtom<formalism::DerivedTag>> derived_atom) {
    requires TaskKind<Kind>;
    requires std::same_as<typename T::TaskType, Task<Kind>>;
    { cs.get_index() } -> std::same_as<Index<State<Kind>>>;
    { cs.get(variable) } -> std::same_as<formalism::planning::FDRValue>;
    { cs.get(static_fterm) } -> std::same_as<float_t>;
    { cs.get(fluent_fterm) } -> std::same_as<float_t>;
    { cs.test(static_atom) } -> std::same_as<bool>;
    { cs.test(derived_atom) } -> std::same_as<bool>;
    { cs.get_state_repository() } -> std::same_as<const std::shared_ptr<StateRepository<Kind>>&>;
};

/**
 * IndexableStateConcept
 */

template<typename T, typename Kind>
concept IndexableViewStateConcept = requires(const T& cs,
                                             formalism::planning::FDRVariableView<formalism::FluentTag> variable,
                                             formalism::planning::GroundFunctionTermView<formalism::StaticTag> static_fterm,
                                             formalism::planning::GroundFunctionTermView<formalism::FluentTag> fluent_fterm,
                                             formalism::planning::GroundAtomView<formalism::StaticTag> static_atom,
                                             formalism::planning::GroundAtomView<formalism::DerivedTag> derived_atom) {
    requires TaskKind<Kind>;
    requires std::same_as<typename T::TaskType, Task<Kind>>;
    { cs.get_index() } -> std::same_as<Index<State<Kind>>>;
    { cs.get(variable) } -> std::same_as<formalism::planning::FDRValue>;
    { cs.get(static_fterm) } -> std::same_as<float_t>;
    { cs.get(fluent_fterm) } -> std::same_as<float_t>;
    { cs.test(static_atom) } -> std::same_as<bool>;
    { cs.test(derived_atom) } -> std::same_as<bool>;
    { cs.get_state_repository() } -> std::same_as<const std::shared_ptr<StateRepository<Kind>>&>;
};

}

}

#endif
