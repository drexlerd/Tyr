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
//
#include "tyr/planning/ground_task/state_storage/tree_compression/atom.hpp"
#include "tyr/planning/ground_task/state_storage/tree_compression/fact.hpp"
#include "tyr/planning/state_storage/config.hpp"
#include "tyr/planning/state_storage/tree_compression/numeric.hpp"

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
    Data(Index<planning::State<TaskType>> index,
         planning::FactPackedStorage<TaskType, planning::StateStoragePolicyTag> fact_storage,
         planning::AtomPackedStorage<TaskType, planning::StateStoragePolicyTag> atom_storage,
         planning::NumericPackedStorage<TaskType, planning::StateStoragePolicyTag> numeric_storage) noexcept :
        m_index(index),
        m_fact_storage(fact_storage),
        m_atom_storage(atom_storage),
        m_numeric_storage(numeric_storage)
    {
    }

    Index<planning::State<TaskType>> get_index() const noexcept { return m_index; }

    template<formalism::FactKind T>
    const auto get_atoms() const noexcept
    {
        if constexpr (std::same_as<T, formalism::FluentTag>)
            return m_fact_storage;
        else if constexpr (std::same_as<T, formalism::DerivedTag>)
            return m_atom_storage;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }

    auto get_numeric_variables() const noexcept { return m_numeric_storage; }

    auto identifying_members() const noexcept { return std::tie(m_fact_storage, m_atom_storage, m_numeric_storage); }

private:
    Index<planning::State<TaskType>> m_index;

    planning::FactPackedStorage<TaskType, planning::StateStoragePolicyTag> m_fact_storage;
    planning::AtomPackedStorage<TaskType, planning::StateStoragePolicyTag> m_atom_storage;
    planning::NumericPackedStorage<TaskType, planning::StateStoragePolicyTag> m_numeric_storage;
};
}

#endif
