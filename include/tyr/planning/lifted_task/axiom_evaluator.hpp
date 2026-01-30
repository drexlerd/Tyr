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

#ifndef TYR_PLANNING_LIFTED_TASK_AXIOM_EVALUATOR_HPP_
#define TYR_PLANNING_LIFTED_TASK_AXIOM_EVALUATOR_HPP_

#include "tyr/planning/lifted_task/unpacked_state.hpp"
//
#include "tyr/datalog/policies/annotation.hpp"
#include "tyr/datalog/policies/termination.hpp"
#include "tyr/datalog/workspaces/program.hpp"
#include "tyr/planning/axiom_evaluator.hpp"
#include "tyr/planning/declarations.hpp"

#include <memory>

namespace tyr::planning
{

template<>
class AxiomEvaluator<LiftedTask>
{
public:
    explicit AxiomEvaluator(std::shared_ptr<LiftedTask> task);

    void compute_extended_state(UnpackedState<LiftedTask>& unpacked_state);

    const auto& get_workspace() const noexcept { return m_workspace; }

private:
    std::shared_ptr<LiftedTask> m_task;

    datalog::ProgramWorkspace<datalog::NoOrAnnotationPolicy, datalog::NoAndAnnotationPolicy, datalog::NoTerminationPolicy> m_workspace;
};

}

#endif
