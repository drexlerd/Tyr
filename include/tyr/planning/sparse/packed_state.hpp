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

#ifndef TYR_PLANNING_SPARSE_PACKED_STATE_HPP_
#define TYR_PLANNING_SPARSE_PACKED_STATE_HPP_

#include "tyr/common/config.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/planning/state_index.hpp"

#include <valla/valla.hpp>

namespace tyr::planning
{
class SparsePackedState
{
public:
    SparsePackedState(StateIndex index, valla::Slot<uint_t> fluent_atoms, valla::Slot<uint_t> derived_atoms, valla::Slot<uint_t> numeric_variables) noexcept;

    StateIndex get_index() const noexcept;

    template<formalism::IsFactTag T>
    valla::Slot<uint_t> get_atoms() const noexcept;

    valla::Slot<uint_t> get_numeric_variables() const noexcept;

    auto identifying_members() const noexcept
    {
        return std::tie(m_fluent_atoms.i1, m_fluent_atoms.i2, m_derived_atoms.i1, m_derived_atoms.i2, m_numeric_variables.i1, m_numeric_variables.i2);
    }

private:
    StateIndex m_index;
    valla::Slot<uint_t> m_fluent_atoms;
    valla::Slot<uint_t> m_derived_atoms;
    valla::Slot<uint_t> m_numeric_variables;
};
}

#endif
