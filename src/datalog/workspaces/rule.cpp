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

#include "tyr/datalog/workspaces/rule.hpp"

#include "tyr/datalog/assignment_sets.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/canonicalization.hpp"
#include "tyr/formalism/datalog/expression_arity.hpp"
#include "tyr/formalism/datalog/grounder.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/rule_view.hpp"

#include <chrono>
#include <vector>

namespace f = tyr::formalism;
namespace fd = tyr::formalism::datalog;

namespace tyr::datalog
{

/**
 * ConstRuleWorkspace
 */

static auto create_witness_condition(View<Index<fd::ConjunctiveCondition>, fd::Repository> element, fd::Repository& context)
{
    auto builder = fd::Builder {};
    auto conj_cond_ptr = builder.get_builder<fd::ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    conj_cond.variables = element.get_variables().get_data();
    for (const auto& literal : element.get_literals<f::FluentTag>())
        if (literal.get_polarity())
            conj_cond.fluent_literals.push_back(literal.get_index());

    canonicalize(conj_cond);
    return context.get_or_create(conj_cond, builder.get_buffer());
}

ConstRuleWorkspace::ConstRuleWorkspace(Index<formalism::datalog::Rule> rule,
                                       formalism::datalog::Repository& repository,
                                       const analysis::DomainListList& parameter_domains,
                                       size_t num_objects,
                                       size_t num_fluent_predicates,
                                       const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets) :
    rule(rule),
    repository(repository),
    witness_condition(create_witness_condition(get_rule().get_body(), repository).first),
    nullary_condition(create_ground_nullary_condition(get_rule().get_body(), repository).first),
    unary_overapproximation_condition(create_overapproximation_conjunctive_condition(1, get_rule().get_body(), repository).first),
    binary_overapproximation_condition(create_overapproximation_conjunctive_condition(2, get_rule().get_body(), repository).first),
    conflicting_overapproximation_condition(
        create_overapproximation_conflicting_conjunctive_condition(get_rule().get_arity() == 1 ? 1 : 2, get_rule().get_body(), repository).first),
    static_consistency_graph(get_rule(),
                             get_rule().get_body(),
                             get_unary_overapproximation_condition(),
                             get_binary_overapproximation_condition(),
                             parameter_domains,
                             num_objects,
                             num_fluent_predicates,
                             0,
                             get_rule().get_arity(),
                             static_assignment_sets)
{
}

}
