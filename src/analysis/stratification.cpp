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

#include "tyr/analysis/stratification.hpp"

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/views.hpp"

using namespace tyr::formalism;

namespace tyr::analysis
{

namespace details
{
enum class StratumStatus
{
    UNCONSTRAINED = 0,
    LOWER = 1,
    STRICTLY_LOWER = 2,
};

struct PredicateStrata
{
    std::vector<UnorderedSet<Index<Predicate<FluentTag>>>> strata;
};

static PredicateStrata compute_predicate_stratification(View<Index<Program>, Repository> program)
{
    auto R = UnorderedMap<Index<Predicate<FluentTag>>, UnorderedMap<Index<Predicate<FluentTag>>, StratumStatus>> {};

    const auto& predicates = program.get_predicates<FluentTag>().get_data();

    // lines 2-4
    for (const auto predicate_1 : predicates)
    {
        for (const auto predicate_2 : predicates)
        {
            R[predicate_1][predicate_2] = StratumStatus::UNCONSTRAINED;
        }
    }

    // lines 5-10
    for (const auto rule : program.get_rules())
    {
        const auto head_predicate = rule.get_head().get_predicate().get_index();

        for (const auto literal : rule.get_body().get_literals<FluentTag>())
        {
            const auto body_predicate = literal.get_atom().get_predicate().get_index();

            if (!literal.get_polarity())
            {
                R[body_predicate][head_predicate] = StratumStatus::STRICTLY_LOWER;
            }
            else
            {
                R[body_predicate][head_predicate] = StratumStatus::LOWER;
            }
        }
    }

    // lines 11-15
    for (const auto& predicate_1 : predicates)
    {
        for (const auto& predicate_2 : predicates)
        {
            for (const auto& predicate_3 : predicates)
            {
                if (std::min(static_cast<int>(R.at(predicate_2).at(predicate_1)), static_cast<int>(R.at(predicate_1).at(predicate_3))) > 0)
                {
                    R.at(predicate_2).at(predicate_3) = static_cast<StratumStatus>(std::max({ static_cast<int>(R.at(predicate_2).at(predicate_1)),
                                                                                              static_cast<int>(R.at(predicate_1).at(predicate_3)),
                                                                                              static_cast<int>(R.at(predicate_2).at(predicate_3)) }));
                }
            }
        }
    }

    // lines 16-27
    if (std::any_of(predicates.begin(),
                    predicates.end(),
                    [&R](const auto& predicate) { return R.at(predicate).at(predicate) == StratumStatus::STRICTLY_LOWER; }))
    {
        throw std::runtime_error("Set of rules is not stratifiable.");
    }

    auto predicate_strata = PredicateStrata {};
    auto remaining = UnorderedSet<Index<Predicate<FluentTag>>>(predicates.begin(), predicates.end());
    while (!remaining.empty())
    {
        auto stratum = UnorderedSet<Index<Predicate<FluentTag>>> {};
        for (const auto& predicate_1 : remaining)
        {
            if (std::all_of(remaining.begin(),
                            remaining.end(),
                            [&R, &predicate_1](const auto& predicate_2) { return R.at(predicate_2).at(predicate_1) != StratumStatus::STRICTLY_LOWER; }))
            {
                stratum.insert(predicate_1);
            }
        }

        for (const auto& predicate : stratum)
        {
            remaining.erase(predicate);
        }

        predicate_strata.strata.push_back(std::move(stratum));
    }

    return predicate_strata;
}
}

/// @brief Compute the rule stratification for the rules in the given program.
/// An implementation of Algorithm 1 by Thiébaux-et-al-ijcai2003
/// Source: https://users.cecs.anu.edu.au/~thiebaux/papers/ijcai03.pdf
/// @param program is the program
/// @return is the RuleStrata
RuleStrata compute_rule_stratification(View<Index<Program>, Repository> program)
{
    const auto predicate_stratification = details::compute_predicate_stratification(program);

    auto rule_strata = RuleStrata {};

    auto remaining_rules = UnorderedSet<Index<Rule>>(program.get_rules().get_data().begin(), program.get_rules().get_data().end());

    for (const auto& predicate_stratum : predicate_stratification.strata)
    {
        auto stratum = UnorderedSet<Index<Rule>> {};

        for (const auto rule : remaining_rules)
        {
            if (predicate_stratum.count(make_view(rule, program.get_context()).get_head().get_predicate().get_index()))
            {
                stratum.insert(rule);
            }
        }

        for (const auto rule : stratum)
        {
            remaining_rules.erase(rule);
        }

        rule_strata.data.push_back(RuleStratum(stratum.begin(), stratum.end()));
    }

    // std::cout << rule_strata.data << std::endl;

    return rule_strata;
}
}
