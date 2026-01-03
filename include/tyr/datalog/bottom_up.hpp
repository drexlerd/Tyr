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

#ifndef TYR_SOLVER_BOTTOM_UP_HPP_
#define TYR_SOLVER_BOTTOM_UP_HPP_

#include "tyr/common/config.hpp"
#include "tyr/common/types.hpp"
#include "tyr/datalog/declarations.hpp"
#include "tyr/datalog/policies/annotation.hpp"
#include "tyr/datalog/policies/termination.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/ground_atom_index.hpp"

#include <concepts>
#include <vector>

namespace tyr::datalog
{

template<OrAnnotationPolicyConcept OrAP = NoOrAnnotationPolicy,
         AndAnnotationPolicyConcept AndAP = NoAndAnnotationPolicy,
         TerminationPolicy TP = NoTerminationPolicy>
void solve_bottom_up(datalog::ProgramWorkspace& ws, const datalog::ConstProgramWorkspace& cws, AnnotationPolicies<OrAP, AndAP>& aps, TP& tp);

}

#endif
