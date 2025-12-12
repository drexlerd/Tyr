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

#ifndef TYR_PLANNING_GROUND_TASK_UNPACKED_STATE_HPP_
#define TYR_PLANNING_GROUND_TASK_UNPACKED_STATE_HPP_

#include "tyr/common/config.hpp"
#include "tyr/formalism/planning/fdr_fact_data.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/unpacked_state.hpp"

#include <boost/dynamic_bitset.hpp>
#include <vector>

namespace tyr::planning
{
template<>
class UnpackedState<GroundTask>
{
public:
    using TaskType = GroundTask;

    UnpackedState() = default;

    StateIndex& get_index() noexcept { return m_index; }

    StateIndex get_index() const noexcept { return m_index; }

    template<formalism::FactKind T>
    DataList<formalism::FDRFact<T>>& get_facts() noexcept
    {
        if constexpr (std::same_as<T, formalism::FluentTag>)
            return m_fluent_facts;
        else if constexpr (std::same_as<T, formalism::DerivedTag>)
            return m_derived_facts;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }

    template<formalism::FactKind T>
    const DataList<formalism::FDRFact<T>>& get_facts() const noexcept
    {
        if constexpr (std::same_as<T, formalism::FluentTag>)
            return m_fluent_facts;
        else if constexpr (std::same_as<T, formalism::DerivedTag>)
            return m_derived_facts;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }

    std::vector<float_t>& get_numeric_variables() noexcept { return m_numeric_variables; }

    const std::vector<float_t>& get_numeric_variables() const noexcept { return m_numeric_variables; }

    void clear()
    {
        m_fluent_facts.clear();
        m_derived_facts.clear();
        m_numeric_variables.clear();
    }

private:
    StateIndex m_index;
    DataList<formalism::FDRFact<formalism::FluentTag>> m_fluent_facts;
    DataList<formalism::FDRFact<formalism::DerivedTag>> m_derived_facts;
    std::vector<float_t> m_numeric_variables;
};
}

#endif
