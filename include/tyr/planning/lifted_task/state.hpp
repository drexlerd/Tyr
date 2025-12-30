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

#include "tyr/planning/lifted_task/unpacked_state.hpp"
//
#include "tyr/common/shared_object_pool.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/state.hpp"

#include <boost/dynamic_bitset.hpp>

namespace tyr::planning
{
template<>
class State<LiftedTask>
{
public:
    using TaskType = LiftedTask;

    State(const LiftedTask& task, SharedObjectPoolPtr<UnpackedState<LiftedTask>> unpacked) noexcept;

    /**
     * StateConcept
     */

    StateIndex get_index() const;

    bool test(Index<formalism::planning::GroundAtom<formalism::StaticTag>> index) const;
    float_t get(Index<formalism::planning::GroundFunctionTerm<formalism::StaticTag>> index) const;

    formalism::planning::FDRValue get(Index<formalism::planning::FDRVariable<formalism::FluentTag>> index) const;
    float_t get(Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>> index) const;

    bool test(Index<formalism::planning::GroundAtom<formalism::DerivedTag>> index) const;

    const LiftedTask& get_task() const noexcept;

    const UnpackedState<LiftedTask>& get_unpacked_state() const noexcept;

    /**
     * For LiftedTask
     */

    template<formalism::FactKind T>
    const boost::dynamic_bitset<>& get_atoms() const noexcept;

    template<formalism::FactKind T>
    const std::vector<float_t>& get_numeric_variables() const noexcept;

private:
    SharedObjectPoolPtr<UnpackedState<LiftedTask>> m_unpacked;
    const LiftedTask* m_task;
};

/**
 * Implementations
 */

inline State<LiftedTask>::State(const LiftedTask& task, SharedObjectPoolPtr<UnpackedState<LiftedTask>> unpacked) noexcept :
    m_unpacked(std::move(unpacked)),
    m_task(&task)
{
}

inline StateIndex State<LiftedTask>::get_index() const { return m_unpacked->get_index(); }

inline formalism::planning::FDRValue State<LiftedTask>::get(Index<formalism::planning::FDRVariable<formalism::FluentTag>> index) const
{
    return m_unpacked->get(index);
}

inline float_t State<LiftedTask>::get(Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>> index) const { return m_unpacked->get(index); }

inline bool State<LiftedTask>::test(Index<formalism::planning::GroundAtom<formalism::DerivedTag>> index) const { return m_unpacked->test(index); }

inline const LiftedTask& State<LiftedTask>::get_task() const noexcept { return *m_task; }

inline const UnpackedState<LiftedTask>& State<LiftedTask>::get_unpacked_state() const noexcept { return *m_unpacked; }
}

#endif
