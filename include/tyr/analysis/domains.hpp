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

#ifndef TYR_ANALYSIS_DOMAINS_HPP_
#define TYR_ANALYSIS_DOMAINS_HPP_

#include "tyr/common/declarations.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/types.hpp"
#include "tyr/formalism/declarations.hpp"

#include <unordered_set>
#include <vector>

namespace tyr::analysis
{
using DomainSet = UnorderedSet<Index<formalism::Object>>;
using DomainSetList = std::vector<DomainSet>;
using DomainSetListList = std::vector<DomainSetList>;

using DomainList = std::vector<Index<formalism::Object>>;
using DomainListList = std::vector<DomainList>;
using DomainListListList = std::vector<DomainListList>;

struct ProgramVariableDomains
{
    DomainListListList static_predicate_domains;
    DomainListListList fluent_predicate_domains;
    DomainListListList static_function_domains;
    DomainListListList fluent_function_domains;
    DomainListListList rule_domains;
};

struct TaskVariableDomains
{
    DomainListListList static_predicate_domains;
    DomainListListList fluent_predicate_domains;
    DomainListListList derived_predicate_domains;
    DomainListListList static_function_domains;
    DomainListListList fluent_function_domains;
    std::vector<std::pair<DomainListList, DomainListListList>> action_domains;
    DomainListListList axiom_domains;
};

extern ProgramVariableDomains compute_variable_domains(View<Index<formalism::Program>, formalism::Repository> program);

extern TaskVariableDomains compute_variable_domains(View<Index<formalism::Task>, formalism::OverlayRepository<formalism::Repository>> task);
}

#endif