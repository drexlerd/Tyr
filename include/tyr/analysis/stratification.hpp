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

#include "tyr/formalism/formalism.hpp"

#include <vector>

namespace tyr::analysis
{

struct RuleStrata
{
    std::vector<IndexList<formalism::Rule>> strata;
};

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
    std::vector<UnorderedSet<Index<formalism::Predicate<formalism::FluentTag>>>> strata;
};

PredicateStrata compute_predicate_stratification(Proxy<formalism::Program, formalism::Repository> program)
{
    auto R = UnorderedMap<Index<formalism::Predicate<formalism::FluentTag>>, UnorderedMap<Index<formalism::Predicate<formalism::FluentTag>>, StratumStatus>> {};

    // lines 2-4
    for (const auto predicate_1 : program.get_predicates<formalism::FluentTag>())
    {
        for (const auto predicate_2 : program.get_predicates<formalism::FluentTag>())
        {
            R[predicate_1.get_index()][predicate_2.get_index()] = StratumStatus::UNCONSTRAINED;
        }
    }

    // lines 5-10
    for (const auto rule : program.get_rules())
    {
        const auto head_predicate = rule.get_head().get_predicate();

        for (const auto literal : rule.get_body().get_fluent_literals())
        {
            const auto body_predicate = literal.get_atom().get_predicate();

            if (!literal.get_polarity())
            {
                R[body_predicate.get_index()][head_predicate.get_index()] = StratumStatus::STRICTLY_LOWER;
            }
            else
            {
                R[body_predicate.get_index()][head_predicate.get_index()] = StratumStatus::LOWER;
            }
        }
    }

    // lines 11-15
    for (const auto& predicate_1 : program.get_predicates<formalism::FluentTag>())
    {
        for (const auto& predicate_2 : program.get_predicates<formalism::FluentTag>())
        {
            for (const auto& predicate_3 : program.get_predicates<formalism::FluentTag>())
            {
                if (std::min(static_cast<int>(R.at(predicate_2.get_index()).at(predicate_1.get_index())),
                             static_cast<int>(R.at(predicate_1.get_index()).at(predicate_3.get_index())))
                    > 0)
                {
                    R.at(predicate_2.get_index()).at(predicate_3.get_index()) =
                        static_cast<StratumStatus>(std::max({ static_cast<int>(R.at(predicate_2.get_index()).at(predicate_1.get_index())),
                                                              static_cast<int>(R.at(predicate_1.get_index()).at(predicate_3.get_index())),
                                                              static_cast<int>(R.at(predicate_2.get_index()).at(predicate_3.get_index())) }));
                }
            }
        }
    }

    // lines 16-27
    if (std::any_of(program.get_predicates<formalism::FluentTag>().begin(),
                    program.get_predicates<formalism::FluentTag>().end(),
                    [&R](const auto& predicate) { return R.at(predicate.get_index()).at(predicate.get_index()) == StratumStatus::STRICTLY_LOWER; }))
    {
        throw std::runtime_error("Set of rules is not stratifiable.");
    }

    auto predicate_strata = PredicateStrata {};
    auto remaining = UnorderedSet<Index<formalism::Predicate<formalism::FluentTag>>>(program.get().get_predicates<formalism::FluentTag>().begin(),
                                                                                     program.get().get_predicates<formalism::FluentTag>().end());
    while (!remaining.empty())
    {
        auto stratum = UnorderedSet<Index<formalism::Predicate<formalism::FluentTag>>> {};
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
RuleStrata compute_rule_stratification(Proxy<formalism::Program, formalism::Repository> program)
{
    const auto predicate_stratification = details::compute_predicate_stratification(program);

    auto rule_strata = RuleStrata {};

    auto remaining_rules = UnorderedSet<Index<formalism::Rule>>(program.get().rules.begin(), program.get().rules.end());

    for (const auto& predicate_stratum : predicate_stratification.strata)
    {
        auto stratum = UnorderedSet<Index<formalism::Rule>> {};

        for (const auto rule : remaining_rules)
        {
            if (predicate_stratum.count(Proxy<formalism::Rule, formalism::Repository>(rule, program.get_context()).get_head().get_predicate().get_index()))
            {
                stratum.insert(rule);
            }
        }

        for (const auto rule : stratum)
        {
            remaining_rules.erase(rule);
        }

        rule_strata.strata.push_back(IndexList<formalism::Rule>(stratum.begin(), stratum.end()));
    }

    return rule_strata;
}
}

#endif
