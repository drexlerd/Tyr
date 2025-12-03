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
    std::vector<UnorderedSet<View<Index<formalism::Predicate<formalism::FluentTag>>, formalism::Repository>>> strata;
};

static PredicateStrata compute_predicate_stratification(View<Index<formalism::Program>, formalism::Repository> program)
{
    auto R = UnorderedMap<View<Index<formalism::Predicate<formalism::FluentTag>>, formalism::Repository>,
                          UnorderedMap<View<Index<formalism::Predicate<formalism::FluentTag>>, formalism::Repository>, StratumStatus>> {};

    // lines 2-4
    for (const auto predicate_1 : program.get_predicates<formalism::FluentTag>())
    {
        for (const auto predicate_2 : program.get_predicates<formalism::FluentTag>())
        {
            R[predicate_1][predicate_2] = StratumStatus::UNCONSTRAINED;
        }
    }

    // lines 5-10
    for (const auto rule : program.get_rules())
    {
        const auto head_predicate = rule.get_head().get_predicate();

        for (const auto literal : rule.get_body().get_literals<formalism::FluentTag>())
        {
            const auto body_predicate = literal.get_atom().get_predicate();

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
    for (const auto& predicate_1 : program.get_predicates<formalism::FluentTag>())
    {
        for (const auto& predicate_2 : program.get_predicates<formalism::FluentTag>())
        {
            for (const auto& predicate_3 : program.get_predicates<formalism::FluentTag>())
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
    if (std::any_of(program.get_predicates<formalism::FluentTag>().begin(),
                    program.get_predicates<formalism::FluentTag>().end(),
                    [&R](const auto& predicate) { return R.at(predicate).at(predicate) == StratumStatus::STRICTLY_LOWER; }))
    {
        throw std::runtime_error("Set of rules is not stratifiable.");
    }

    auto predicate_strata = PredicateStrata {};
    auto remaining =
        UnorderedSet<View<Index<formalism::Predicate<formalism::FluentTag>>, formalism::Repository>>(program.get_predicates<formalism::FluentTag>().begin(),
                                                                                                     program.get_predicates<formalism::FluentTag>().end());
    while (!remaining.empty())
    {
        auto stratum = UnorderedSet<View<Index<formalism::Predicate<formalism::FluentTag>>, formalism::Repository>> {};
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
RuleStrata compute_rule_stratification(View<Index<formalism::Program>, formalism::Repository> program)
{
    const auto predicate_stratification = details::compute_predicate_stratification(program);

    auto rule_strata = RuleStrata {};

    auto remaining_rules = UnorderedSet<View<Index<formalism::Rule>, formalism::Repository>>(program.get_rules().begin(), program.get_rules().end());

    for (const auto& predicate_stratum : predicate_stratification.strata)
    {
        auto stratum = UnorderedSet<View<Index<formalism::Rule>, formalism::Repository>> {};

        for (const auto rule : remaining_rules)
        {
            if (predicate_stratum.count(rule.get_head().get_predicate()))
            {
                stratum.insert(rule);
            }
        }

        for (const auto rule : stratum)
        {
            remaining_rules.erase(rule);
        }

        rule_strata.strata.push_back(std::vector<View<Index<formalism::Rule>, formalism::Repository>>(stratum.begin(), stratum.end()));
    }

    return rule_strata;
}
}
