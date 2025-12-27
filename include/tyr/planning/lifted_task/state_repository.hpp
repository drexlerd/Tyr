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

#ifndef TYR_PLANNING_LIFTED_TASK_STATE_REPOSITORY_HPP_
#define TYR_PLANNING_LIFTED_TASK_STATE_REPOSITORY_HPP_

#include "tyr/planning/lifted_task/packed_state.hpp"
#include "tyr/planning/lifted_task/state.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"
//
#include "tyr/common/config.hpp"              // for uint_t, float_t
#include "tyr/common/indexed_hash_set.hpp"    // for IndexedHashSet
#include "tyr/common/shared_object_pool.hpp"  // for SharedObjectPool
#include "tyr/planning/state_index.hpp"       // for StateIndex

#include <memory>  // for shared_ptr
#include <valla/valla.hpp>
#include <vector>
#include <vector>  // for vector

namespace tyr::planning
{

template<>
class StateRepository<LiftedTask>
{
public:
    explicit StateRepository(std::shared_ptr<LiftedTask> task);

    State<LiftedTask> get_initial_state();

    State<LiftedTask> get_registered_state(StateIndex state_index);

    SharedObjectPoolPtr<UnpackedState<LiftedTask>> get_unregistered_state();

    State<LiftedTask> register_state(SharedObjectPoolPtr<UnpackedState<LiftedTask>> state);

private:
    std::shared_ptr<LiftedTask> m_task;

    valla::IndexedHashSet<valla::Slot<uint_t>, uint_t> m_uint_nodes;
    valla::IndexedHashSet<float_t, uint_t> m_float_nodes;
    std::vector<uint_t> m_nodes_buffer;
    IndexedHashSet<PackedState<LiftedTask>, StateIndex> m_packed_states;
    SharedObjectPool<UnpackedState<LiftedTask>> m_unpacked_state_pool;

    std::shared_ptr<AxiomEvaluator<LiftedTask>> m_axiom_evaluator;
};

}

#endif