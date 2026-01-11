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

FactsWorkspace::FactsWorkspace(View<IndexList<f::Predicate<f::FluentTag>>, fd::Repository> predicates,
                               View<IndexList<f::Function<f::FluentTag>>, fd::Repository> functions,
                               const a::DomainListListList& predicate_domains,
                               const a::DomainListListList& function_domains,
                               size_t num_objects,
                               View<IndexList<fd::GroundAtom<formalism::FluentTag>>, fd::Repository> atoms,
                               View<IndexList<fd::GroundFunctionTermValue<formalism::FluentTag>>, fd::Repository> fterm_values) :
    fact_sets(predicates, functions, atoms, fterm_values),
    assignment_sets(predicates, functions, predicate_domains, function_domains, num_objects, fact_sets),
    goal_fact_sets(predicates)
{
}

void FactsWorkspace::reset()
{
    fact_sets.reset();
    assignment_sets.reset();
}

ConstFactsWorkspace::ConstFactsWorkspace(View<IndexList<f::Predicate<f::StaticTag>>, fd::Repository> predicates,
                                         View<IndexList<f::Function<f::StaticTag>>, fd::Repository> functions,
                                         const a::DomainListListList& predicate_domains,
                                         const a::DomainListListList& function_domains,
                                         size_t num_objects,
                                         View<IndexList<fd::GroundAtom<f::StaticTag>>, fd::Repository> atoms,
                                         View<IndexList<fd::GroundFunctionTermValue<f::StaticTag>>, fd::Repository> fterm_values) :
    fact_sets(predicates, functions, atoms, fterm_values),
    assignment_sets(predicates, functions, predicate_domains, function_domains, num_objects, fact_sets)
{
}

}
