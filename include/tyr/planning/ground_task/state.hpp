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

#ifndef TYR_PLANNING_GROUND_TASK_STATE_HPP_
#define TYR_PLANNING_GROUND_TASK_STATE_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task/unpacked_state.hpp"
#include "tyr/planning/state.hpp"

#include <boost/dynamic_bitset.hpp>

namespace tyr::planning
{
template<>
class State<GroundTask>
{
public:
    using TaskType = GroundTask;

    State(const GroundTask& task, SharedObjectPoolPtr<UnpackedState<GroundTask>> unpacked) noexcept;

    /**
     * StateConcept
     */

    StateIndex get_index() const;

    bool test(Index<formalism::GroundAtom<formalism::StaticTag>> index) const;
    float_t get(Index<formalism::GroundFunctionTerm<formalism::StaticTag>> index) const;

    formalism::FDRValue get(Index<formalism::FDRVariable<formalism::FluentTag>> index) const;
    float_t get(Index<formalism::GroundFunctionTerm<formalism::FluentTag>> index) const;

    bool test(Index<formalism::GroundAtom<formalism::DerivedTag>> index) const;

    const GroundTask& get_task() const noexcept;

    const UnpackedState<GroundTask>& get_unpacked_state() const noexcept;

    /**
     * For GroundTask
     */

    template<formalism::FactKind T>
    const boost::dynamic_bitset<>& get_atoms() const noexcept;

    const std::vector<formalism::FDRValue>& get_fluent_values() const noexcept;

    template<formalism::FactKind T>
    const std::vector<float_t>& get_numeric_variables() const noexcept;

private:
    SharedObjectPoolPtr<UnpackedState<GroundTask>> m_unpacked;
    const GroundTask* m_task;
};

/**
 * Implemntations
 */

inline State<GroundTask>::State(const GroundTask& task, SharedObjectPoolPtr<UnpackedState<GroundTask>> unpacked) noexcept :
    m_unpacked(std::move(unpacked)),
    m_task(&task)
{
}

inline StateIndex State<GroundTask>::get_index() const { return m_unpacked->get_index(); }

inline formalism::FDRValue State<GroundTask>::get(Index<formalism::FDRVariable<formalism::FluentTag>> index) const { return m_unpacked->get(index); }

inline float_t State<GroundTask>::get(Index<formalism::GroundFunctionTerm<formalism::FluentTag>> index) const { return m_unpacked->get(index); }

inline bool State<GroundTask>::test(Index<formalism::GroundAtom<formalism::DerivedTag>> index) const { return m_unpacked->test(index); }

inline const GroundTask& State<GroundTask>::get_task() const noexcept { return *m_task; }

inline const UnpackedState<GroundTask>& State<GroundTask>::get_unpacked_state() const noexcept { return *m_unpacked; }

inline const std::vector<formalism::FDRValue>& State<GroundTask>::get_fluent_values() const noexcept { return m_unpacked->get_fluent_values(); }

}

#endif
