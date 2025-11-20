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

#ifndef TYR_ANALYSIS_VARIABLE_DOMAINS_HPP_
#define TYR_ANALYSIS_VARIABLE_DOMAINS_HPP_

#include "tyr/common/formatter.hpp"
#include "tyr/common/unordered_set.hpp"
#include "tyr/formalism/formalism.hpp"

#include <unordered_set>
#include <vector>

namespace tyr::analysis
{
using DomainSet = std::unordered_set<formalism::ObjectIndex, Hash<formalism::ObjectIndex>, EqualTo<formalism::ObjectIndex>>;
using DomainSetList = std::vector<DomainSet>;
using DomainSetListList = std::vector<DomainSetList>;

using DomainList = std::vector<formalism::ObjectIndex>;
using DomainListList = std::vector<DomainList>;
using DomainListListList = std::vector<DomainListList>;

std::pair<DomainSetListList, DomainSetListList> compute_variable_list_per_predicate(formalism::ProgramProxy<> program)
{
    const auto num_static_predicates = program.get_predicates<formalism::StaticTag>().size();
    const auto num_fluent_predicates = program.get_predicates<formalism::FluentTag>().size();

    auto objects = std::vector<formalism::ObjectIndex> {};
    for (const auto object : program.get_objects())
        objects.push_back(object.get_index());
    auto universe = DomainSet(objects.begin(), objects.end());

    ///--- Step 1: Initialize static and fluent predicate parameter domains
    auto static_domains = DomainSetListList(num_static_predicates);
    for (const auto predicate : program.get_predicates<formalism::StaticTag>())
        static_domains[predicate.get_index().value].resize(predicate.get_arity());

    auto fluent_domains = DomainSetListList(num_fluent_predicates);
    for (const auto predicate : program.get_predicates<formalism::FluentTag>())
        fluent_domains[predicate.get_index().value].resize(predicate.get_arity());

    for (const auto atom : program.get_atoms<formalism::StaticTag>())
    {
        const auto predicate = atom.get_predicate();
        auto pos = size_t { 0 };
        for (const auto& object : atom.get_terms())
            static_domains[predicate.get_index().value][pos++].insert(object.get_index());
    }

    for (const auto atom : program.get_atoms<formalism::FluentTag>())
    {
        const auto predicate = atom.get_predicate();
        auto pos = size_t { 0 };
        for (const auto& object : atom.get_terms())
            fluent_domains[predicate.get_index().value][pos++].insert(object.get_index());
    }

    ///--- Step 2: Compute rule parameter domains as tightest bound from the previously computed domains of the static predicates.
    auto func_restrict_rule_parameter_domain = [&](auto&& atom, auto&& parameter_domains)
    {
        const auto predicate = atom.get_predicate();

        auto pos = size_t { 0 };
        for (const auto term : atom.get_terms())
        {
            term.visit(
                [&](auto&& arg)
                {
                    using ProxyType = std::decay_t<decltype(arg)>;

                    if constexpr (std::is_same_v<ProxyType, formalism::ObjectProxy<>>) {}
                    else if constexpr (std::is_same_v<ProxyType, formalism::ParameterIndex>)
                    {
                        const auto parameter_index = to_uint_t(arg);
                        auto& parameter_domain = parameter_domains[parameter_index];
                        auto& predicate_domain = static_domains[predicate.get_index().value][pos];

                        intersect_inplace(parameter_domain, predicate_domain);
                    }
                    else
                    {
                        static_assert(dependent_false<ProxyType>::value, "Missing case");
                    }
                });
            ++pos;
        }
    };

    auto parameter_domains_per_rule = DomainSetListList();
    {
        for (const auto rule : program.get_rules())
        {
            auto variables = rule.get_variables();
            auto parameter_domains = DomainSetList(variables.size(), universe);

            for (const auto literal : rule.get_static_body())
            {
                func_restrict_rule_parameter_domain(literal.get_atom(), parameter_domains);
            }

            parameter_domains_per_rule.push_back(std::move(parameter_domains));
        }
    }

    ///--- Step 3: Lift the fluent predicate domains given the variable relationships in the rules.

    auto func_lift_fluent_domain = [&](auto&& atom, auto&& parameter_domains)
    {
        const auto predicate = atom.get_predicate();

        auto pos = size_t { 0 };
        for (const auto term : atom.get_terms())
        {
            term.visit(
                [&](auto&& arg)
                {
                    using ProxyType = std::decay_t<decltype(arg)>;

                    if constexpr (std::is_same_v<ProxyType, formalism::ObjectProxy<>>) {}
                    else if constexpr (std::is_same_v<ProxyType, formalism::ParameterIndex>)
                    {
                        const auto parameter_index = to_uint_t(arg);
                        auto& parameter_domain = parameter_domains[parameter_index];
                        auto& predicate_domain = fluent_domains[predicate.get_index().value][pos];

                        union_inplace(predicate_domain, parameter_domain);
                    }
                    else
                    {
                        static_assert(dependent_false<ProxyType>::value, "Missing case");
                    }
                });
            ++pos;
        }
    };

    for (const auto rule : program.get_rules())
    {
        auto& parameter_domains = parameter_domains_per_rule[rule.get_index().value];

        for (const auto literal : rule.get_fluent_body())
        {
            func_lift_fluent_domain(literal.get_atom(), parameter_domains);
        }

        func_lift_fluent_domain(rule.get_head(), parameter_domains);
    }

    std::cout << "Static domains: " << "\n" << static_domains << std::endl;
    std::cout << "Fluent domains: " << "\n" << fluent_domains << std::endl;
    std::cout << "Rule domains: " << "\n" << parameter_domains_per_rule << std::endl;

    return { std::move(static_domains), std::move(fluent_domains) };
}

DomainListList compute_variable_list_per_function(formalism::ProgramProxy<> program) {}

DomainListList compute_variable_lists_per_rule(formalism::ProgramProxy<> program) {}
}

#endif