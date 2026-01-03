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
#include "tyr/datalog/kpkc_utils.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/canonicalization.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/rule_view.hpp"

#include <chrono>
#include <vector>

namespace f = tyr::formalism;
namespace fd = tyr::formalism::datalog;

namespace tyr::datalog
{
/**
 * RuleWorkspace
 */

RuleWorkspace::RuleWorkspace(const formalism::datalog::Repository& parent, const StaticConsistencyGraph& static_consistency_graph) :
    consistency_graph(datalog::kpkc::allocate_dense_graph(static_consistency_graph)),
    kpkc_workspace(datalog::kpkc::allocate_workspace(static_consistency_graph)),
    repository(std::make_shared<fd::Repository>()),  // we have to use pointer, since the RuleExecutionContext is moved into a vector
    overlay_repository(parent, *repository),
    binding(),
    heads(),
    statistics()
{
}

void RuleWorkspace::clear() noexcept
{
    repository->clear();
    heads.clear();
}

void RuleWorkspace::initialize(const StaticConsistencyGraph& static_consistency_graph, const AssignmentSets& assignment_sets)
{
    datalog::kpkc::initialize_dense_graph_and_workspace(static_consistency_graph, assignment_sets, consistency_graph, kpkc_workspace);
}

/**
 * ConstRuleWorkspace
 */

static std::pair<Index<fd::Rule>, bool> create_fluent_rule(View<Index<fd::Rule>, fd::Repository> element, fd::Repository& context)
{
    auto builder = fd::Builder {};
    auto rule_ptr = builder.get_builder<fd::Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    auto conj_cond_ptr = builder.get_builder<fd::ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    conj_cond.variables = element.get_variables().get_data();
    conj_cond.fluent_literals = element.get_body().get_data().fluent_literals;

    canonicalize(conj_cond);
    const auto new_conj_cond = context.get_or_create(conj_cond, builder.get_buffer()).first;

    rule.variables = element.get_variables().get_data();
    rule.body = new_conj_cond;
    rule.head = element.get_head().get_index();

    canonicalize(rule);
    return context.get_or_create(rule, builder.get_buffer());
}

ConstRuleWorkspace::ConstRuleWorkspace(Index<formalism::datalog::Rule> rule,
                                       formalism::datalog::Repository& repository,
                                       const analysis::DomainListList& parameter_domains,
                                       const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets) :
    rule(rule),
    repository(repository),
    fluent_rule(create_fluent_rule(get_rule(), repository).first),
    nullary_condition(create_ground_nullary_condition(get_rule().get_body(), repository).first),
    unary_overapproximation_condition(create_overapproximation_conjunctive_condition(1, get_rule().get_body(), repository).first),
    binary_overapproximation_condition(create_overapproximation_conjunctive_condition(2, get_rule().get_body(), repository).first),
    unary_conflicting_overapproximation_condition(create_overapproximation_conflicting_conjunctive_condition(1, get_rule().get_body(), repository).first),
    binary_conflicting_overapproximation_condition(create_overapproximation_conflicting_conjunctive_condition(2, get_rule().get_body(), repository).first),
    static_consistency_graph(get_rule().get_body(),
                             get_unary_overapproximation_condition(),
                             get_binary_overapproximation_condition(),
                             parameter_domains,
                             0,
                             get_rule().get_arity(),
                             static_assignment_sets)
{
}

}
