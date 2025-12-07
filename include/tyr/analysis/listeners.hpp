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

#ifndef TYR_ANALYSIS_LISTENERS_HPP_
#define TYR_ANALYSIS_LISTENERS_HPP_

#include "tyr/analysis/declarations.hpp"
#include "tyr/analysis/stratification.hpp"
#include "tyr/common/declarations.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/formatter.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/types.hpp"
#include "tyr/formalism/declarations.hpp"

namespace tyr::analysis
{

using ListenersPerStratum = UnorderedMap<View<Index<formalism::Predicate<formalism::FluentTag>>, formalism::Repository>,
                                         std::vector<View<Index<formalism::Rule>, formalism::Repository>>>;

struct Listeners
{
    std::vector<ListenersPerStratum> positive_listeners_per_stratum;
};

extern Listeners compute_listeners(const RuleStrata& strata);
}

#endif
