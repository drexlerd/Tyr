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

#ifndef TYR_PLANNING_LIFTED_TASK_UNPACKED_STATE_HPP_
#define TYR_PLANNING_LIFTED_TASK_UNPACKED_STATE_HPP_

#include "tyr/common/config.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/state_index.hpp"
#include "tyr/planning/unpacked_state.hpp"

#include <boost/dynamic_bitset.hpp>
#include <vector>

namespace tyr::planning
{
template<>
class UnpackedState<LiftedTask>
{
public:
    using TaskType = LiftedTask;

    UnpackedState() = default;

    StateIndex& get_index() noexcept { return m_index; }

    StateIndex get_index() const noexcept { return m_index; }

    template<formalism::FactKind T>
    boost::dynamic_bitset<>& get_atoms() noexcept
    {
        if constexpr (std::same_as<T, formalism::FluentTag>)
            return m_fluent_atoms;
        else if constexpr (std::same_as<T, formalism::DerivedTag>)
            return m_derived_atoms;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }

    template<formalism::FactKind T>
    const boost::dynamic_bitset<>& get_atoms() const noexcept
    {
        if constexpr (std::same_as<T, formalism::FluentTag>)
            return m_fluent_atoms;
        else if constexpr (std::same_as<T, formalism::DerivedTag>)
            return m_derived_atoms;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }

    std::vector<float_t>& get_numeric_variables() noexcept { return m_numeric_variables; }

    const std::vector<float_t>& get_numeric_variables() const noexcept { return m_numeric_variables; }

    void clear()
    {
        m_fluent_atoms.clear();
        m_derived_atoms.clear();
        m_numeric_variables.clear();
    }

private:
    StateIndex m_index;
    boost::dynamic_bitset<> m_fluent_atoms;
    boost::dynamic_bitset<> m_derived_atoms;
    std::vector<float_t> m_numeric_variables;
};

static_assert(UnpackedStateConcept<UnpackedState<LiftedTask>>);
}

#endif
