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

#include "tyr/planning/packed_state.hpp"

namespace tyr::planning
{

PackedState::PackedState(StateIndex index, valla::Slot<uint_t> fluent_atoms, valla::Slot<uint_t> derived_atoms, valla::Slot<uint_t> numeric_variables) noexcept
    :
    m_index(index),
    m_fluent_atoms(fluent_atoms),
    m_derived_atoms(derived_atoms),
    m_numeric_variables(numeric_variables)
{
}

StateIndex PackedState::get_index() const noexcept { return m_index; }

template<formalism::IsFactTag T>
valla::Slot<uint_t> PackedState::get_atoms() const noexcept
{
    if constexpr (std::same_as<T, formalism::FluentTag>)
        return m_fluent_atoms;
    else if constexpr (std::same_as<T, formalism::DerivedTag>)
        return m_derived_atoms;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template valla::Slot<uint_t> PackedState::get_atoms<formalism::FluentTag>() const noexcept;
template valla::Slot<uint_t> PackedState::get_atoms<formalism::DerivedTag>() const noexcept;

valla::Slot<uint_t> PackedState::get_numeric_variables() const noexcept { return m_numeric_variables; }

}
