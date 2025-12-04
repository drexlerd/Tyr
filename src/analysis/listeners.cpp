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

#include "tyr/analysis/listeners.hpp"

#include "tyr/analysis/stratification.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/views.hpp"

namespace tyr::analysis
{

Listeners compute_listeners(const RuleStrata& strata)
{
    auto listeners = Listeners();

    for (const auto& stratum : strata.strata)
    {
        auto listeners_in_stratum = ListenersPerStratum {};

        for (const auto rule : stratum)
        {
            for (const auto literal : rule.get_body().get_literals<formalism::FluentTag>())
            {
                listeners_in_stratum[literal.get_atom().get_predicate()].push_back(rule);
            }
        }

        listeners.positive_listeners_per_stratum.push_back(std::move(listeners_in_stratum));
    }

    std::cout << listeners.positive_listeners_per_stratum << std::endl;

    return listeners;
}
}
