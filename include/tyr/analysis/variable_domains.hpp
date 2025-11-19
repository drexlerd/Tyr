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

#include "tyr/formalism/formalism.hpp"

#include <unordered_set>
#include <vector>

namespace tyr::analysis
{
using DomainSetList = std::vector<std::unordered_set<formalism::ObjectIndex, Hash<formalism::ObjectIndex>, EqualTo<formalism::ObjectIndex>>>;
using DomainSetListList = std::vector<DomainSetList>;

using DomainListList = std::vector<std::vector<formalism::ObjectIndex>>;
using DomainListListList = std::vector<DomainListList>;

DomainListList compute_variable_list_per_predicate(formalism::ProgramProxy program)
{
    const auto num_static_predicates = program.get_predicates<formalism::StaticTag>().size();
    const auto num_fluent_predicates = program.get_predicates<formalism::FluentTag>().size();

    auto static_domains = DomainSetListList(num_static_predicates);
    for (const auto predicate : program.get_predicates<formalism::StaticTag>())
        static_domains[predicate.get_index().value].resize(predicate.get_arity());

    auto fluent_domains = DomainSetListList(num_fluent_predicates);
    for (const auto predicate : program.get_predicates<formalism::FluentTag>())
        fluent_domains[predicate.get_index().value].resize(predicate.get_arity());

    for (const auto atom : program.get_atoms<formalism::StaticTag>()) {}
    for (const auto atom : program.get_atoms<formalism::FluentTag>()) {}

    for (const auto lhs_rule : program.get_rules())
    {
        for (const auto rhs_rule : program.get_rules()) {}
    }
}

DomainListList compute_variable_list_per_function(formalism::ProgramProxy program) {}

DomainListList compute_variable_lists_per_rule(formalism::ProgramProxy program) {}

}

#endif