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
#include "tyr/common/declarations.hpp"  // for UnorderedMap, UnorderedSet
#include "tyr/common/equal_to.hpp"      // for EqualTo
#include "tyr/common/hash.hpp"          // for Hash
#include "tyr/common/types.hpp"
#include "tyr/formalism/declarations.hpp"     // for FluentTag, Predicate, Rule
#include "tyr/formalism/predicate_index.hpp"  // for Index
#include "tyr/formalism/rule_index.hpp"       // for Index

#include <vector>  // for vector

namespace tyr::analysis
{

using ListenerStratum = UnorderedMap<Index<formalism::Predicate<formalism::FluentTag>>, UnorderedSet<Index<formalism::Rule>>>;

struct ListenerStrata
{
    std::vector<ListenerStratum> data;
};

extern ListenerStrata compute_listeners(const RuleStrata& strata, const formalism::Repository& context);
}

#endif
