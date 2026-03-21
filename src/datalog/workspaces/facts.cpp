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

#include "tyr/datalog/workspaces/facts.hpp"

namespace a = tyr::analysis;
namespace f = tyr::formalism;
namespace fd = tyr::formalism::datalog;

namespace tyr::datalog
{

FactsWorkspace::FactsWorkspace(fd::PredicateListView<f::FluentTag> predicates,
                               fd::FunctionListView<f::FluentTag> functions,
                               const a::DomainListListList& predicate_domains,
                               const a::DomainListListList& function_domains,
                               size_t num_objects,
                               fd::GroundAtomListView<formalism::FluentTag> atoms,
                               fd::GroundFunctionTermValueListView<formalism::FluentTag> fterm_values,
                               const formalism::datalog::Repository& workspace_repository) :
    fact_sets(predicates, functions, atoms, fterm_values, workspace_repository),
    assignment_sets(predicates, functions, predicate_domains, function_domains, num_objects, fact_sets),
    delta_fact_sets(predicates, functions, workspace_repository),
    goal_fact_sets(predicates, workspace_repository)
{
}

void FactsWorkspace::reset()
{
    fact_sets.reset();
    assignment_sets.reset();
    delta_fact_sets.reset();
}

ConstFactsWorkspace::ConstFactsWorkspace(fd::PredicateListView<f::StaticTag> predicates,
                                         fd::FunctionListView<f::StaticTag> functions,
                                         const a::DomainListListList& predicate_domains,
                                         const a::DomainListListList& function_domains,
                                         size_t num_objects,
                                         fd::GroundAtomListView<f::StaticTag> atoms,
                                         fd::GroundFunctionTermValueListView<f::StaticTag> fterm_values,
                                         const formalism::datalog::Repository& program_repository) :
    fact_sets(predicates, functions, atoms, fterm_values, program_repository),
    assignment_sets(predicates, functions, predicate_domains, function_domains, num_objects, fact_sets)
{
}

}
