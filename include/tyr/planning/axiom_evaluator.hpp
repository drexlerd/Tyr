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

#ifndef TYR_PLANNING_AXIOM_EVALUATOR_HPP_
#define TYR_PLANNING_AXIOM_EVALUATOR_HPP_

#include "tyr/common/declarations.hpp"
#include "tyr/common/onetbb.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/unpacked_state.hpp"

namespace tyr::planning
{

template<TaskKind Kind>
class AxiomEvaluator
{
    static_assert(dependent_false<Kind>::value, "AxiomEvaluator is not defined for type T.");
};

template<typename T, typename Kind>
concept AxiomEvaluatorConcept =
    requires(T& r, std::shared_ptr<Task<Kind>> task, std::shared_ptr<ExecutionContext> execution_context, UnpackedState<Kind>& unpacked_state) {
        requires TaskKind<Kind>;
        { T(task, execution_context) };
        { r.compute_extended_state(unpacked_state) } -> std::same_as<void>;
    };

}

#endif
