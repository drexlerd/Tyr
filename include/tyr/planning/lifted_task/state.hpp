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

#ifndef TYR_PLANNING_LIFTED_TASK_STATE_HPP_
#define TYR_PLANNING_LIFTED_TASK_STATE_HPP_

#include "tyr/common/shared_object_pool.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"
#include "tyr/planning/state.hpp"

namespace tyr::planning
{
template<>
class State<LiftedTask>
{
public:
    using TaskType = LiftedTask;

    State(LiftedTask& task, SharedObjectPoolPtr<UnpackedState<LiftedTask>> unpacked) noexcept : m_unpacked(std::move(unpacked)), m_task(&task) {}

    StateIndex get_index() const noexcept { return m_unpacked->get_index(); }

    template<formalism::FactKind T>
    const boost::dynamic_bitset<>& get_atoms() const noexcept;

    template<formalism::FactKind T>
    const std::vector<float_t>& get_numeric_variables() const noexcept;

    const UnpackedState<LiftedTask>& get_unpacked_state() const noexcept { return *m_unpacked; }

    LiftedTask& get_task() noexcept { return *m_task; }
    const LiftedTask& get_task() const noexcept { return *m_task; }

private:
    SharedObjectPoolPtr<UnpackedState<LiftedTask>> m_unpacked;
    LiftedTask* m_task;
};
}

#endif
