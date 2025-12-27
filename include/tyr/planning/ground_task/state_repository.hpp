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

#ifndef TYR_PLANNING_GROUND_TASK_STATE_REPOSITORY_HPP_
#define TYR_PLANNING_GROUND_TASK_STATE_REPOSITORY_HPP_

#include "tyr/planning/ground_task/packed_state.hpp"
#include "tyr/planning/ground_task/state.hpp"
#include "tyr/planning/ground_task/unpacked_state.hpp"
//

#include "tyr/common/bit_packed_layout.hpp"           // for BitPackedArra...
#include "tyr/common/config.hpp"                      // for uint_t, float_t
#include "tyr/common/indexed_hash_set.hpp"            // for IndexedHashSet
#include "tyr/common/segmented_array_repository.hpp"  // for SegmentedArra...
#include "tyr/common/shared_object_pool.hpp"          // for SharedObjectPool
#include "tyr/planning/declarations.hpp"              // for GroundTask
#include "tyr/planning/state_index.hpp"

#include <memory>  // for shared_ptr
#include <valla/valla.hpp>
#include <vector>
#include <vector>  // for vector

namespace tyr::planning
{

template<>
class StateRepository<GroundTask>
{
public:
    explicit StateRepository(std::shared_ptr<GroundTask> task);

    State<GroundTask> get_initial_state();

    State<GroundTask> get_registered_state(StateIndex state_index);

    SharedObjectPoolPtr<UnpackedState<GroundTask>> get_unregistered_state();

    State<GroundTask> register_state(SharedObjectPoolPtr<UnpackedState<GroundTask>> state);

private:
    std::shared_ptr<GroundTask> m_task;
    BitPackedArrayLayout<uint_t> m_fluent_layout;
    BitsetLayout<uint_t> m_derived_layout;

    valla::IndexedHashSet<valla::Slot<uint_t>, uint_t> m_uint_nodes;
    valla::IndexedHashSet<float_t, uint_t> m_float_nodes;
    std::vector<uint_t> m_nodes_buffer;
    IndexedHashSet<PackedState<GroundTask>, StateIndex> m_packed_states;
    SegmentedArrayRepository<uint_t> m_fluent_repository;
    SegmentedArrayRepository<uint_t> m_derived_repository;
    std::vector<uint_t> m_fluent_buffer;
    std::vector<uint_t> m_derived_buffer;
    SharedObjectPool<UnpackedState<GroundTask>> m_unpacked_state_pool;

    std::shared_ptr<AxiomEvaluator<GroundTask>> m_axiom_evaluator;
};

}

#endif