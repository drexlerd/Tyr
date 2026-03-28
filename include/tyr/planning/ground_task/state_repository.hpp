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

#include "tyr/common/config.hpp"
#include "tyr/common/indexed_hash_set.hpp"
#include "tyr/common/raw_array_set.hpp"
#include "tyr/common/shared_object_pool.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task/state_data.hpp"
#include "tyr/planning/ground_task/state_view.hpp"
#include "tyr/planning/ground_task/unpacked_state.hpp"
#include "tyr/planning/state_index.hpp"
#include "tyr/planning/state_repository.hpp"
//
#include "tyr/planning/ground_task/state_storage/tree_compression/atom.hpp"
#include "tyr/planning/ground_task/state_storage/tree_compression/context.hpp"
#include "tyr/planning/ground_task/state_storage/tree_compression/fact.hpp"
#include "tyr/planning/state_storage/config.hpp"
#include "tyr/planning/state_storage/tree_compression/numeric.hpp"

#include <memory>
#include <valla/valla.hpp>
#include <vector>

namespace tyr::planning
{

template<>
class StateRepository<GroundTask> : public std::enable_shared_from_this<StateRepository<GroundTask>>
{
public:
    explicit StateRepository(std::shared_ptr<GroundTask> task, ExecutionContextPtr execution_context);

    static std::shared_ptr<StateRepository<GroundTask>> create(std::shared_ptr<GroundTask> task, ExecutionContextPtr execution_context);

    StateView<GroundTask> get_initial_state();

    StateView<GroundTask> get_registered_state(Index<State<GroundTask>> state_index);

    StateView<GroundTask>
    create_state(const std::vector<Data<formalism::planning::FDRFact<formalism::FluentTag>>>& fluent_facts,
                 const std::vector<std::pair<Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>>, float_t>>& fterm_values);

    StateView<GroundTask> create_state(const std::vector<formalism::planning::FDRFactView<formalism::FluentTag>>& fluent_facts,
                                       const std::vector<formalism::planning::GroundFunctionTermViewValuePair<formalism::FluentTag>>& fterm_values);

    SharedObjectPoolPtr<UnpackedState<GroundTask>> get_unregistered_state();

    StateView<GroundTask> register_state(SharedObjectPoolPtr<UnpackedState<GroundTask>> state);

    const auto& get_task() const noexcept { return m_task; }
    const auto& get_axiom_evaluator() const noexcept { return m_axiom_evaluator; }

private:
    std::shared_ptr<GroundTask> m_task;

    StateStorageContext<GroundTask, StateStoragePolicyTag> m_context;
    FactStorageBackend<GroundTask, StateStoragePolicyTag> m_fluent_backend;
    AtomStorageBackend<GroundTask, StateStoragePolicyTag> m_derived_backend;
    NumericStorageBackend<GroundTask, StateStoragePolicyTag> m_numeric_backend;

    IndexedHashSet<State<GroundTask>> m_packed_states;
    SharedObjectPool<UnpackedState<GroundTask>> m_unpacked_state_pool;

    std::shared_ptr<AxiomEvaluator<GroundTask>> m_axiom_evaluator;
};

}

#endif