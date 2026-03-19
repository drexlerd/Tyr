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

#ifndef TYR_PLANNING_GROUND_TASK_STATE_DATA_HPP_
#define TYR_PLANNING_GROUND_TASK_STATE_DATA_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/state_data.hpp"

#include <valla/valla.hpp>

/**
 * Definitions
 */

namespace tyr
{

template<>
struct Data<planning::State<planning::GroundTask>>
{
public:
    using TaskType = planning::GroundTask;

    Data() noexcept = default;
    Data(Index<planning::State<planning::GroundTask>> index, uint_t fluent_facts, uint_t derived_facts, valla::Slot<uint_t> numeric_variables) noexcept :
        m_index(index),
        m_fluent_facts(fluent_facts),
        m_derived_facts(derived_facts),
        m_numeric_variables(numeric_variables)
    {
    }

    Index<planning::State<planning::GroundTask>> get_index() const noexcept { return m_index; }

    template<formalism::FactKind T>
    const uint_t get_facts() const noexcept
    {
        if constexpr (std::same_as<T, formalism::FluentTag>)
            return m_fluent_facts;
        else if constexpr (std::same_as<T, formalism::DerivedTag>)
            return m_derived_facts;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }

    valla::Slot<uint_t> get_numeric_variables() const noexcept { return m_numeric_variables; }

    auto identifying_members() const noexcept { return std::tie(m_fluent_facts, m_derived_facts, m_numeric_variables.i1, m_numeric_variables.i2); }

private:
    Index<planning::State<planning::GroundTask>> m_index;
    uint_t m_fluent_facts;
    uint_t m_derived_facts;
    valla::Slot<uint_t> m_numeric_variables;
};
}

#endif
