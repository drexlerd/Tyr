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

#include "tyr/analysis/analysis.hpp"
#include "tyr/formalism/formalism.hpp"
#include "tyr/grounder/grounder.hpp"

namespace tyr::solver
{
void solve_bottom_up(View<Index<formalism::Program>, formalism::Repository> program,
                     formalism::Repository& repository,
                     grounder::FactSets<formalism::Repository>& fact_sets,
                     const analysis::Listeners& listeners,
                     const analysis::RuleStrata& strata,
                     std::vector<View<formalism::GroundAtom<formalism::FluentTag>, formalism::Repository>>& out_facts);

}

#endif
