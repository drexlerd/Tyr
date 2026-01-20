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

#ifndef TYR_DATALOG_WORKSPACES_RULE_HPP_
#define TYR_DATALOG_WORKSPACES_RULE_HPP_

#include "tyr/datalog/consistency_graph.hpp"
#include "tyr/datalog/delta_kpkc.hpp"
#include "tyr/datalog/statistics/rule.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/overlay_repository.hpp"

#include <chrono>
#include <vector>

namespace tyr::datalog
{
struct RuleIterationWorkspace
{
    kpkc::DeltaKPKC kpkc;
    kpkc::Workspace kpkc_workspace;

    /// Merge stage into rule execution context
    std::shared_ptr<formalism::datalog::Repository> repository;
    formalism::OverlayRepository<formalism::datalog::Repository> overlay_repository;

    /// Bindings kept from iteration in stage
    IndexList<formalism::Object> binding;
    UnorderedSet<Index<formalism::datalog::GroundAtom<formalism::FluentTag>>> heads;

    RuleStatistics statistics;

    RuleIterationWorkspace(const formalism::datalog::Repository& parent, const ConstRuleWorkspace& cws);

    void clear() noexcept;

    void initialize(const StaticConsistencyGraph& static_consistency_graph, const AssignmentSets& assignment_sets);
};

struct RulePersistentWorkspace
{
    formalism::datalog::RepositoryPtr repository;
    formalism::OverlayRepository<formalism::datalog::Repository> overlay_repository;

    explicit RulePersistentWorkspace(const formalism::datalog::Repository& parent);

    void clear() noexcept;
};

struct ConstRuleWorkspace
{
    Index<formalism::datalog::Rule> rule;
    const formalism::datalog::Repository& repository;

    Index<formalism::datalog::ConjunctiveCondition> witness_condition;
    Index<formalism::datalog::GroundConjunctiveCondition> nullary_condition;
    Index<formalism::datalog::ConjunctiveCondition> unary_overapproximation_condition;
    Index<formalism::datalog::ConjunctiveCondition> binary_overapproximation_condition;
    Index<formalism::datalog::ConjunctiveCondition> conflicting_overapproximation_condition;

    StaticConsistencyGraph static_consistency_graph;

    auto get_rule() const noexcept { return make_view(rule, repository); }
    auto get_witness_condition() const noexcept { return make_view(witness_condition, repository); }
    auto get_nullary_condition() const noexcept { return make_view(nullary_condition, repository); }
    auto get_unary_overapproximation_condition() const noexcept { return make_view(unary_overapproximation_condition, repository); }
    auto get_binary_overapproximation_condition() const noexcept { return make_view(binary_overapproximation_condition, repository); }
    auto get_conflicting_overapproximation_condition() const noexcept { return make_view(conflicting_overapproximation_condition, repository); }

    ConstRuleWorkspace(Index<formalism::datalog::Rule> rule,
                       formalism::datalog::Repository& repository,
                       const analysis::DomainListList& parameter_domains,
                       const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets);
};
}

#endif
