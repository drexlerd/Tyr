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

#include "tyr/common/config.hpp"
#include "tyr/common/indexed_hash_set.hpp"
#include "tyr/common/onetbb.hpp"
#include "tyr/common/shared_object_pool.hpp"
#include "tyr/planning/lifted_task/state_data.hpp"
#include "tyr/planning/lifted_task/state_view.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"
#include "tyr/planning/state_index.hpp"
//
#include "tyr/planning/lifted_task/state_storage/hash_set/atom.hpp"
#include "tyr/planning/lifted_task/state_storage/hash_set/fact.hpp"
#include "tyr/planning/lifted_task/state_storage/tree_compression/atom.hpp"
#include "tyr/planning/lifted_task/state_storage/tree_compression/fact.hpp"
#include "tyr/planning/state_storage/config.hpp"
#include "tyr/planning/state_storage/hash_set/numeric.hpp"
#include "tyr/planning/state_storage/tree_compression/numeric.hpp"

#include <memory>
#include <valla/valla.hpp>
#include <vector>

namespace tyr::planning
{

template<>
class StateRepository<LiftedTag> : public std::enable_shared_from_this<StateRepository<LiftedTag>>
{
public:
    explicit StateRepository(std::shared_ptr<Task<LiftedTag>> task, ExecutionContextPtr execution_context);

    static std::shared_ptr<StateRepository<LiftedTag>> create(std::shared_ptr<Task<LiftedTag>> task, ExecutionContextPtr execution_context);

    StateView<LiftedTag> get_initial_state();

    StateView<LiftedTag> get_registered_state(Index<State<LiftedTag>> state_index);

    StateView<LiftedTag>
    create_state(const std::vector<Data<formalism::planning::FDRFact<formalism::FluentTag>>>& fluent_facts,
                 const std::vector<std::pair<Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>>, float_t>>& fterm_values);

    StateView<LiftedTag> create_state(const std::vector<formalism::planning::FDRFactView<formalism::FluentTag>>& fluent_facts,
                                      const std::vector<formalism::planning::GroundFunctionTermViewValuePair<formalism::FluentTag>>& fterm_values);

    SharedObjectPoolPtr<UnpackedState<LiftedTag>> get_unregistered_state();

    StateView<LiftedTag> register_state(SharedObjectPoolPtr<UnpackedState<LiftedTag>> state);

    size_t memory_usage() const noexcept;

    const auto& get_task() const noexcept { return m_task; }
    const auto& get_axiom_evaluator() const noexcept { return m_axiom_evaluator; }

private:
    std::shared_ptr<Task<LiftedTag>> m_task;

    StateStorageContext<LiftedTag, StateStoragePolicyTag> m_context;
    FactStorageBackend<LiftedTag, StateStoragePolicyTag> m_fluent_backend;
    AtomStorageBackend<LiftedTag, StateStoragePolicyTag> m_derived_backend;
    NumericStorageBackend<LiftedTag, StateStoragePolicyTag> m_numeric_backend;

    IndexedHashSet<State<LiftedTag>> m_packed_states;
    SharedObjectPool<UnpackedState<LiftedTag>> m_unpacked_state_pool;

    std::shared_ptr<AxiomEvaluator<LiftedTag>> m_axiom_evaluator;
};

}

#endif