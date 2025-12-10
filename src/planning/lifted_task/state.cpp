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

#include "tyr/planning/lifted_task/state.hpp"

#include "tyr/planning/lifted_task.hpp"

namespace tyr::planning
{

template<formalism::FactKind T>
const boost::dynamic_bitset<>& State<LiftedTask>::get_atoms() const noexcept
{
    if constexpr (std::is_same_v<T, formalism::StaticTag>)
        return m_task->get_static_atoms_bitset();
    else if constexpr (std::is_same_v<T, formalism::FluentTag> || std::is_same_v<T, formalism::DerivedTag>)
        return m_unpacked->template get_atoms<T>();
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template const boost::dynamic_bitset<>& State<LiftedTask>::get_atoms<formalism::StaticTag>() const noexcept;
template const boost::dynamic_bitset<>& State<LiftedTask>::get_atoms<formalism::FluentTag>() const noexcept;
template const boost::dynamic_bitset<>& State<LiftedTask>::get_atoms<formalism::DerivedTag>() const noexcept;

template<formalism::FactKind T>
const std::vector<float_t>& State<LiftedTask>::get_numeric_variables() const noexcept
{
    if constexpr (std::is_same_v<T, formalism::StaticTag>)
        return m_task->get_static_numeric_variables();
    else if constexpr (std::is_same_v<T, formalism::FluentTag>)
        return m_unpacked->get_numeric_variables();
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template const std::vector<float_t>& State<LiftedTask>::get_numeric_variables<formalism::StaticTag>() const noexcept;
template const std::vector<float_t>& State<LiftedTask>::get_numeric_variables<formalism::FluentTag>() const noexcept;

static_assert(StateConcept<State<LiftedTask>>);

}
