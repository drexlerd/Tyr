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

#ifndef TYR_DATALOG_WORKSPACES_FACTS_HPP_
#define TYR_DATALOG_WORKSPACES_FACTS_HPP_

#include "tyr/datalog/assignment_sets.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function_view.hpp"
#include "tyr/formalism/predicate_view.hpp"

namespace tyr::datalog
{
struct FactsWorkspace
{
    TaggedFactSets<formalism::FluentTag> fact_sets;
    TaggedAssignmentSets<formalism::FluentTag> assignment_sets;

    TaggedFactSets<formalism::FluentTag> delta_fact_sets;

    PredicateFactSets<formalism::FluentTag> goal_fact_sets;

    explicit FactsWorkspace(formalism::datalog::PredicateListView<formalism::FluentTag> predicates,
                            formalism::datalog::FunctionListView<formalism::FluentTag> functions,
                            const analysis::DomainListListList& predicate_domains,
                            const analysis::DomainListListList& function_domains,
                            size_t num_objects,
                            formalism::datalog::GroundAtomListView<formalism::FluentTag> atoms,
                            formalism::datalog::GroundFunctionTermValueListView<formalism::FluentTag> fterm_values,
                            const formalism::datalog::Repository& workspace_repository);

    void reset();
};

struct ConstFactsWorkspace
{
    const TaggedFactSets<formalism::StaticTag> fact_sets;
    const TaggedAssignmentSets<formalism::StaticTag> assignment_sets;

    explicit ConstFactsWorkspace(formalism::datalog::PredicateListView<formalism::StaticTag> predicates,
                                 formalism::datalog::FunctionListView<formalism::StaticTag> functions,
                                 const analysis::DomainListListList& predicate_domains,
                                 const analysis::DomainListListList& function_domains,
                                 size_t num_objects,
                                 formalism::datalog::GroundAtomListView<formalism::StaticTag> atoms,
                                 formalism::datalog::GroundFunctionTermValueListView<formalism::StaticTag> fterm_values,
                                 const formalism::datalog::Repository& program_repository);
};

}

#endif
