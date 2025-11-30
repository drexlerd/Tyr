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

#include "tyr/planning/sparse/unpacked_state.hpp"

namespace tyr::planning
{

StateIndex& SparseUnpackedState::get_index() noexcept { return m_index; }

StateIndex SparseUnpackedState::get_index() const noexcept { return m_index; }

template<formalism::IsFactTag T>
boost::dynamic_bitset<>& SparseUnpackedState::get_atoms() noexcept
{
    if constexpr (std::same_as<T, formalism::FluentTag>)
        return m_fluent_atoms;
    else if constexpr (std::same_as<T, formalism::DerivedTag>)
        return m_derived_atoms;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template boost::dynamic_bitset<>& SparseUnpackedState::get_atoms<formalism::FluentTag>() noexcept;
template boost::dynamic_bitset<>& SparseUnpackedState::get_atoms<formalism::DerivedTag>() noexcept;

template<formalism::IsFactTag T>
const boost::dynamic_bitset<>& SparseUnpackedState::get_atoms() const noexcept
{
    if constexpr (std::same_as<T, formalism::FluentTag>)
        return m_fluent_atoms;
    else if constexpr (std::same_as<T, formalism::DerivedTag>)
        return m_derived_atoms;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template const boost::dynamic_bitset<>& SparseUnpackedState::get_atoms<formalism::FluentTag>() const noexcept;
template const boost::dynamic_bitset<>& SparseUnpackedState::get_atoms<formalism::DerivedTag>() const noexcept;

std::vector<float_t>& SparseUnpackedState::get_numeric_variables() noexcept { return m_numeric_variables; }

const std::vector<float_t>& SparseUnpackedState::get_numeric_variables() const noexcept { return m_numeric_variables; }

void SparseUnpackedState::clear()
{
    m_fluent_atoms.clear();
    m_derived_atoms.clear();
    m_numeric_variables.clear();
}

}
