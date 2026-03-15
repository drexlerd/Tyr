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

#ifndef TYR_ANALYSIS_STRATIFICATION_HPP_
#define TYR_ANALYSIS_STRATIFICATION_HPP_

#include "tyr/common/formatter.hpp"                // for operator<<
#include "tyr/common/types.hpp"                    // for IndexList
#include "tyr/formalism/datalog/declarations.hpp"  // for Program (ptr only), Rule
#include "tyr/formalism/datalog/repository.hpp"

#include <vector>  // for vector

namespace tyr::analysis
{

using RuleStratum = IndexList<formalism::datalog::Rule>;

struct RuleStrata
{
    std::vector<RuleStratum> data;
};

extern RuleStrata compute_rule_stratification(formalism::datalog::ProgramView program);
}

#endif
