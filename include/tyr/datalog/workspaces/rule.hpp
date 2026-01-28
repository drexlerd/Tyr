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

#include "tyr/common/declarations.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/unique_object_pool.hpp"
#include "tyr/datalog/applicability.hpp"
#include "tyr/datalog/consistency_graph.hpp"
#include "tyr/datalog/delta_kpkc.hpp"
#include "tyr/datalog/delta_kpkc2.hpp"
#include "tyr/datalog/policies/annotation.hpp"
#include "tyr/datalog/statistics/rule.hpp"
#include "tyr/formalism/binding_index.hpp"
#include "tyr/formalism/datalog/ground_atom_index.hpp"
#include "tyr/formalism/datalog/merge.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/overlay_repository.hpp"

#include <chrono>
#include <vector>

namespace tyr::datalog
{

struct NullaryApplicabilityCheck
{
private:
    std::optional<View<Index<formalism::datalog::GroundConjunctiveCondition>, formalism::datalog::Repository>> m_condition;

    boost::dynamic_bitset<> m_unsat_fluent_literals;
    boost::dynamic_bitset<> m_unsat_numeric_constraints;

    bool m_statically_applicable;

public:
    NullaryApplicabilityCheck() : m_condition(std::nullopt), m_unsat_fluent_literals(), m_unsat_numeric_constraints(), m_statically_applicable(false) {}

    void initialize(View<Index<formalism::datalog::GroundConjunctiveCondition>, formalism::datalog::Repository> condition, const FactSets& fact_sets)
    {
        m_condition = condition;
        m_unsat_fluent_literals.clear();
        m_unsat_fluent_literals.resize(condition.get_literals<formalism::FluentTag>().size(), true);
        m_unsat_numeric_constraints.clear();
        m_unsat_numeric_constraints.resize(condition.get_numeric_constraints().size(), true);
        m_statically_applicable = true;

        for (const auto literal : condition.get_literals<formalism::StaticTag>())
            if (!tyr::datalog::is_applicable(literal, fact_sets))
                m_statically_applicable = false;
    }

    bool is_statically_applicable() const noexcept { return m_statically_applicable; }

    bool is_dynamically_applicable(const FactSets& fact_sets)
    {
        for (auto i = m_unsat_fluent_literals.find_first(); i != boost::dynamic_bitset<>::npos; i = m_unsat_fluent_literals.find_next(i))
            if (is_applicable(m_condition->get_literals<formalism::FluentTag>()[i], fact_sets))
                m_unsat_fluent_literals.reset(i);

        for (auto i = m_unsat_numeric_constraints.find_first(); i != boost::dynamic_bitset<>::npos; i = m_unsat_numeric_constraints.find_next(i))
            if (evaluate(m_condition->get_numeric_constraints()[i], fact_sets))
                m_unsat_numeric_constraints.reset(i);

        return m_unsat_fluent_literals.none() && m_unsat_numeric_constraints.none();
    }
};

class ConflictingApplicabilityCheck
{
private:
    std::optional<View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository>> m_condition;

    boost::dynamic_bitset<> m_unsat_fluent_literals;
    boost::dynamic_bitset<> m_unsat_numeric_constraints;

    bool m_statically_applicable;

public:
    ConflictingApplicabilityCheck() : m_condition(std::nullopt), m_unsat_fluent_literals(), m_unsat_numeric_constraints(), m_statically_applicable(false) {}

    template<formalism::datalog::Context C>
    void initialize(View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> condition,
                    const FactSets& fact_sets,
                    formalism::datalog::ConstGrounderContext<C>& context)
    {
        m_condition = condition;
        m_unsat_fluent_literals.clear();
        m_unsat_fluent_literals.resize(condition.get_literals<formalism::FluentTag>().size(), true);
        m_unsat_numeric_constraints.clear();
        m_unsat_numeric_constraints.resize(condition.get_numeric_constraints().size(), true);
        m_statically_applicable = true;

        for (const auto literal : condition.get_literals<formalism::StaticTag>())
            if (!is_valid_binding(literal, fact_sets, context))
                m_statically_applicable = false;
    }

    bool is_statically_applicable() const noexcept { return m_statically_applicable; }

    template<formalism::datalog::Context C>
    bool is_dynamically_applicable(const FactSets& fact_sets, formalism::datalog::ConstGrounderContext<C>& context)
    {
        for (auto i = m_unsat_fluent_literals.find_first(); i != boost::dynamic_bitset<>::npos; i = m_unsat_fluent_literals.find_next(i))
            if (is_valid_binding(m_condition->get_literals<formalism::FluentTag>()[i], fact_sets, context))
                m_unsat_fluent_literals.reset(i);

        for (auto i = m_unsat_numeric_constraints.find_first(); i != boost::dynamic_bitset<>::npos; i = m_unsat_numeric_constraints.find_next(i))
            if (is_valid_binding(m_condition->get_numeric_constraints()[i], fact_sets, context))
                m_unsat_numeric_constraints.reset(i);

        return m_unsat_fluent_literals.none() && m_unsat_numeric_constraints.none();
    }
};

struct ApplicabilityCheck
{
private:
    NullaryApplicabilityCheck m_nullary;
    ConflictingApplicabilityCheck m_conflicting;

public:
    ApplicabilityCheck() : m_nullary(), m_conflicting() {}

    template<formalism::datalog::Context C>
    void initialize(View<Index<formalism::datalog::GroundConjunctiveCondition>, formalism::datalog::Repository> nullary,
                    View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository> conflicting,
                    const FactSets& fact_sets,
                    formalism::datalog::ConstGrounderContext<C>& context)
    {
        m_nullary.initialize(nullary, fact_sets);
        m_conflicting.initialize(conflicting, fact_sets, context);
    }

    bool is_statically_applicable() const noexcept { return m_nullary.is_statically_applicable() && m_conflicting.is_statically_applicable(); }

    template<formalism::datalog::Context C>
    bool is_dynamically_applicable(const FactSets& fact_sets, formalism::datalog::ConstGrounderContext<C>& context) noexcept
    {
        return m_nullary.is_dynamically_applicable(fact_sets) && m_conflicting.is_dynamically_applicable(fact_sets, context);
    }
};

/*
struct RuleWorkspace
{
    struct Common
    {
        kpkc::DeltaKPKC kpkc;
    };

    struct WorkerIteration
    {
        kpkc::Workspace kpkc_workspace;

        /// Merge stage into rule execution context
        std::shared_ptr<formalism::datalog::Repository> repository;
        formalism::OverlayRepository<formalism::datalog::Repository> overlay_repository;

        /// Bindings kept from iteration in stage
        IndexList<formalism::Object> binding;
        UnorderedSet<Index<formalism::datalog::GroundAtom<formalism::FluentTag>>> heads;

        RuleStatistics statistics;
    };

    struct WorkerSolve
    {
        /// Program repository to ground witnesses for which ground entities must already exist and we can simply call find.
        formalism::datalog::RepositoryPtr program_repository;

        /// Merge thread into staging area
        formalism::datalog::RepositoryPtr stage_repository;

        /// Memory for grounding
        IndexList<formalism::Object> binding;

        /// Debugging
        UnorderedSet<IndexList<formalism::Object>> seen_bindings_dbg;

        /// Pool applicability checks since we dont know how many are needed.
        UniqueObjectPool<ApplicabilityCheck> applicability_check_pool;
        UnorderedMap<Index<formalism::Binding>, UniqueObjectPoolPtr<ApplicabilityCheck>> pending_rules;
    };

    struct Worker
    {
        WorkerIteration iteration;
        WorkerSolve solve;
    };

    Common common;
    Worker worker;
};*/

struct RuleIterationWorkspace
{
    kpkc::DeltaKPKC kpkc;
    kpkc::Workspace kpkc_workspace;

    kpkc2::DeltaKPKC kpkc2;
    kpkc2::Workspace kpkc2_workspace;
    kpkc2::AnchorsSet kpkc2_anchors_set;
    std::vector<uint_t> kpkc2_anchors_order;
    std::vector<kpkc2::Vertex> kpkc2_vertices;
    std::vector<details::Vertex> static_vertices;

    /// Unique heads constructed in this iteration
    std::shared_ptr<formalism::datalog::Repository> repository;
    formalism::OverlayRepository<formalism::datalog::Repository> overlay_repository;
    UnorderedSet<Index<formalism::datalog::GroundAtom<formalism::FluentTag>>> heads;

    RuleIterationWorkspace(const formalism::datalog::Repository& parent, const ConstRuleWorkspace& cws);

    void clear() noexcept;

    void initialize(const StaticConsistencyGraph& static_consistency_graph, const AssignmentSets& assignment_sets);
};

struct RuleSolveWorkspace
{
    /// Merge thread into staging area
    formalism::datalog::RepositoryPtr repository;

    /// In debug mode, we accumulate all bindings to verify the correctness of delta-kpkc
    UnorderedSet<IndexList<formalism::Object>> seen_bindings_dbg;

    /// Pool applicability checks since we dont know how many are needed.
    UniqueObjectPool<ApplicabilityCheck> applicability_check_pool;
    UnorderedMap<Index<formalism::Binding>, UniqueObjectPoolPtr<ApplicabilityCheck>> pending_rules;

    RuleSolveWorkspace();

    void clear() noexcept;
};

struct RuleWorkspace
{
    /// Accumulated statistics
    RuleStatistics statistics;

    RuleWorkspace() = default;
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
                       size_t num_objects,
                       size_t num_fluent_predicates,
                       const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets);
};
}

#endif
