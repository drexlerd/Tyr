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

#include "tyr/analysis/stratification.hpp"               // for RuleStrata
#include "tyr/common/index_mixins.hpp"                   // for operator!=
#include "tyr/common/types.hpp"                          // for make_view
#include "tyr/common/vector.hpp"                         // for View
#include "tyr/formalism/atom_view.hpp"                   // for View
#include "tyr/formalism/conjunctive_condition_view.hpp"  // for View
#include "tyr/formalism/literal_index.hpp"               // for Index
#include "tyr/formalism/literal_view.hpp"                // for View
#include "tyr/formalism/predicate_view.hpp"              // for View
#include "tyr/formalism/repository.hpp"                  // for Repository
#include "tyr/formalism/rule_view.hpp"                   // for View

#include <cista/containers/vector.h>  // for basic_vector
#include <gtl/phmap.hpp>              // for flat_hash_set
#include <utility>                    // for move

using namespace tyr::formalism;

namespace tyr::analysis
{

ListenerStrata compute_listeners(const RuleStrata& strata, const formalism::Repository& context)
{
    auto listeners = ListenerStrata();

    for (const auto& stratum : strata.data)
    {
        auto listeners_in_stratum = ListenerStratum {};

        for (const auto rule : stratum)
            for (const auto literal : make_view(rule, context).get_body().get_literals<FluentTag>())
                if (literal.get_polarity())
                    listeners_in_stratum[literal.get_atom().get_predicate().get_index()].insert(rule);

        listeners.data.push_back(std::move(listeners_in_stratum));
    }

    // std::cout << listeners.data << std::endl;

    return listeners;
}
}
