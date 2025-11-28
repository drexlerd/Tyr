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
#include "tyr/common/formatter.hpp"
#include "tyr/formalism/formalism.hpp"

namespace tyr::analysis
{

using ListenersPerStratum = UnorderedMap<Index<formalism::Predicate<formalism::FluentTag>>, IndexList<formalism::Rule>>;

struct Listeners
{
    std::vector<ListenersPerStratum> positive_listeners_per_stratum;
};

Listeners compute_listeners(const RuleStrata& strata, const formalism::Repository& repository)
{
    auto listeners = Listeners();

    for (const auto& stratum : strata.strata)
    {
        auto listeners_in_stratum = ListenersPerStratum {};

        for (const auto rule_index : stratum)
        {
            const auto rule = View<Index<formalism::Rule>, formalism::Repository>(rule_index, repository);

            for (const auto literal : rule.get_body().get_literals<formalism::FluentTag>())
            {
                listeners_in_stratum[literal.get_atom().get_predicate().get_index()].push_back(rule_index);
            }
        }

        listeners.positive_listeners_per_stratum.push_back(std::move(listeners_in_stratum));
    }

    return listeners;
}
}

#endif
