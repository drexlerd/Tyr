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

#include "tyr/grounder/execution_contexts.hpp"
#include "tyr/planning/axiom_evaluator.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/programs/axiom.hpp"

#include <memory>

namespace tyr::planning
{

template<>
class AxiomEvaluator<LiftedTask>
{
public:
    explicit AxiomEvaluator(std::shared_ptr<LiftedTask> task);

    void compute_extended_state(UnpackedState<LiftedTask>& unpacked_state);

private:
    std::shared_ptr<LiftedTask> m_task;

    AxiomEvaluatorProgram m_axiom_program;
    grounder::ProgramExecutionContext m_axiom_context;
};

}

#endif
