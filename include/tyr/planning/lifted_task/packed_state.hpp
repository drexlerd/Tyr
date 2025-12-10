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

#ifndef TYR_PLANNING_LIFTED_TASK_PACKED_STATE_HPP_
#define TYR_PLANNING_LIFTED_TASK_PACKED_STATE_HPP_

#include "tyr/common/config.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/packed_state.hpp"
#include "tyr/planning/state_index.hpp"

#include <valla/valla.hpp>

namespace tyr::planning
{
template<>
class PackedState<LiftedTask>
{
public:
    using TaskType = LiftedTask;

    PackedState(StateIndex index, valla::Slot<uint_t> fluent_atoms, valla::Slot<uint_t> derived_atoms, valla::Slot<uint_t> numeric_variables) noexcept :
        m_index(index),
        m_fluent_atoms(fluent_atoms),
        m_derived_atoms(derived_atoms),
        m_numeric_variables(numeric_variables)
    {
    }

    StateIndex get_index() const noexcept { return m_index; }

    template<formalism::FactKind T>
    valla::Slot<uint_t> get_atoms() const noexcept
    {
        if constexpr (std::same_as<T, formalism::FluentTag>)
            return m_fluent_atoms;
        else if constexpr (std::same_as<T, formalism::DerivedTag>)
            return m_derived_atoms;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }

    valla::Slot<uint_t> get_numeric_variables() const noexcept { return m_numeric_variables; }

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

static_assert(PackedStateConcept<PackedState<LiftedTask>>);
}

#endif
