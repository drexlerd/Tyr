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

#ifndef TYR_PLANNING_GROUND_TASK_AXIOM_EVALUATOR_HPP_
#define TYR_PLANNING_GROUND_TASK_AXIOM_EVALUATOR_HPP_

#include "tyr/common/formatter.hpp"                       // for oper...
#include "tyr/common/types.hpp"                           // for Inde...
#include "tyr/formalism/planning/ground_axiom_index.hpp"  // for Index
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task/match_tree/declarations.hpp"  // for Matc...
#include "tyr/planning/ground_task/match_tree/match_tree.hpp"

#include <memory>  // for shar...
#include <vector>  // for vector

namespace tyr::planning
{
template<>
class AxiomEvaluator<GroundTask>
{
public:
    explicit AxiomEvaluator(std::shared_ptr<GroundTask> task);

    void compute_extended_state(UnpackedState<GroundTask>& unpacked_state);

private:
    std::shared_ptr<GroundTask> m_task;

    std::vector<match_tree::MatchTreePtr<formalism::GroundAxiom>> m_axiom_match_tree_strata;
    IndexList<formalism::GroundAxiom> m_applicable_axioms;
};
}

#endif
