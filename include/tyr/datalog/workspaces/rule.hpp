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
#include "tyr/datalog/policies/annotation.hpp"
#include "tyr/datalog/statistics/rule.hpp"
#include "tyr/formalism/binding_index.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/ground_atom_index.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/object_index.hpp"

#include <atomic>
#include <chrono>
#include <oneapi/tbb/enumerable_thread_specific.h>
#include <oneapi/tbb/spin_mutex.h>
#include <vector>

namespace tyr::datalog
{

struct NullaryApplicabilityCheck
{
private:
    std::optional<formalism::datalog::GroundConjunctiveConditionView> m_condition;

    boost::dynamic_bitset<> m_unsat_fluent_literals;
    boost::dynamic_bitset<> m_unsat_numeric_constraints;

    bool m_statically_applicable;

public:
    NullaryApplicabilityCheck() : m_condition(std::nullopt), m_unsat_fluent_literals(), m_unsat_numeric_constraints(), m_statically_applicable(false) {}

    void initialize(formalism::datalog::GroundConjunctiveConditionView condition, const FactSets& fact_sets)
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
    std::optional<formalism::datalog::ConjunctiveConditionView> m_condition;

    boost::dynamic_bitset<> m_unsat_fluent_literals;
    boost::dynamic_bitset<> m_unsat_numeric_constraints;

    bool m_statically_applicable;

public:
    ConflictingApplicabilityCheck() : m_condition(std::nullopt), m_unsat_fluent_literals(), m_unsat_numeric_constraints(), m_statically_applicable(false) {}

    void initialize(formalism::datalog::ConjunctiveConditionView condition, const FactSets& fact_sets, formalism::datalog::GrounderContext& context)
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

    bool is_dynamically_applicable(const FactSets& fact_sets, formalism::datalog::GrounderContext& context)
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

    void initialize(formalism::datalog::GroundConjunctiveConditionView nullary,
                    formalism::datalog::ConjunctiveConditionView conflicting,
                    const FactSets& fact_sets,
                    formalism::datalog::GrounderContext& context)
    {
        m_nullary.initialize(nullary, fact_sets);
        m_conflicting.initialize(conflicting, fact_sets, context);
    }

    bool is_statically_applicable() const noexcept { return m_nullary.is_statically_applicable() && m_conflicting.is_statically_applicable(); }

    bool is_dynamically_applicable(const FactSets& fact_sets, formalism::datalog::GrounderContext& context) noexcept
    {
        return m_nullary.is_dynamically_applicable(fact_sets) && m_conflicting.is_dynamically_applicable(fact_sets, context);
    }
};

template<typename AndAP>
struct RuleWorkspace
{
    struct Common
    {
        explicit Common(const formalism::datalog::Repository& program_repository,
                        const formalism::datalog::Repository& workspace_repository,
                        const StaticConsistencyGraph& static_consistency_graph);

        void initialize_iteration(const StaticConsistencyGraph& static_consistency_graph,
                                  const TaggedFactSets<formalism::FluentTag>& delta_fact_sets,
                                  const AssignmentSets& assignment_sets);

        void clear() noexcept;

        /// Program repository to ground witnesses for which ground entities must already exist and we can simply call find.
        const formalism::datalog::Repository& program_repository;
        const formalism::datalog::Repository& workspace_repository;

        /// KPKC
        kpkc::DeltaKPKC kpkc;

        /// Statistics
        RuleStatistics statistics;
    };

    /// @brief Each iteration consists of
    /// - generate all k-cliques
    /// - ground witnesses
    /// - annotate witnesses
    struct Iteration
    {
        explicit Iteration(formalism::datalog::RepositoryFactory& factory, const ConstRuleWorkspace& cws, const Common& common);

        void clear() noexcept;

        /// Merge stage into rule execution context
        formalism::datalog::Repository workspace_overlay_repository;

        /// Heads
        Index<formalism::Predicate<formalism::FluentTag>> head_predicate;
        UnorderedSet<Index<formalism::Row>> head_rows;

        // Annotations stored in program_overlay_repository
        AndAnnotationsMap and_annot;

        /// KPKC
        kpkc::Workspace kpkc_workspace;
    };

    struct Solve
    {
        explicit Solve(formalism::datalog::RepositoryFactory& factory,
                       const formalism::datalog::Repository& program_repository,
                       const formalism::datalog::Repository& workspace_repository,
                       const AndAP& and_ap);

        void clear() noexcept;

        AndAP and_ap;

        /// Persistent memory
        formalism::datalog::Repository program_overlay_repository;

        /// In debug mode, we accumulate all bindings to verify the correctness of delta-kpkc
        UnorderedSet<IndexList<formalism::Object>> seen_bindings_dbg;

        /// Pool applicability checks since we dont know how many are needed.
        UniqueObjectPool<ApplicabilityCheck> applicability_check_pool;
        UnorderedMap<formalism::datalog::RuleBindingView, UniqueObjectPoolPtr<ApplicabilityCheck>> pending_rules;

        /// Statistics
        RuleWorkerStatistics statistics;
    };

    struct Worker
    {
        explicit Worker(formalism::datalog::RepositoryFactory& factory,
                        const formalism::datalog::Repository& program_repository,
                        const formalism::datalog::Repository& workspace_repository,
                        const ConstRuleWorkspace& cws,
                        const Common& common,
                        const AndAP& and_ap);

        void clear() noexcept;

        formalism::datalog::Builder builder;
        IndexList<formalism::Object> binding;

        Iteration iteration;
        Solve solve;
    };

    RuleWorkspace(formalism::datalog::RepositoryFactory& factory,
                  const formalism::datalog::Repository& program_repository,
                  const formalism::datalog::Repository& workspace_repository,
                  const ConstRuleWorkspace& cws,
                  const AndAP& and_ap);
    RuleWorkspace(const RuleWorkspace& other) = delete;
    RuleWorkspace& operator=(const RuleWorkspace& other) = delete;
    RuleWorkspace(RuleWorkspace&& other) = delete;
    RuleWorkspace& operator=(RuleWorkspace&& other) = delete;

    void clear() noexcept;

    Common common;

    oneapi::tbb::enumerable_thread_specific<Worker> worker;
};

struct ConstRuleWorkspace
{
public:
    auto get_rule() const noexcept { return rule; }
    auto get_witness_rule() const noexcept { return witness_rule; }
    auto get_nullary_condition() const noexcept { return nullary_condition; }
    auto get_unary_overapproximation_rule() const noexcept { return unary_overapproximation_rule; }
    auto get_binary_overapproximation_rule() const noexcept { return binary_overapproximation_rule; }
    auto get_static_binary_overapproximation_rule() const noexcept { return static_binary_overapproximation_rule; }
    auto get_conflicting_overapproximation_rule() const noexcept { return conflicting_overapproximation_rule; }
    const auto& get_static_consistency_graph() const noexcept { return static_consistency_graph; }

    ConstRuleWorkspace(formalism::datalog::RuleView rule,
                       formalism::datalog::Repository& repository,
                       const analysis::DomainListList& parameter_domains,
                       size_t num_objects,
                       size_t num_fluent_predicates,
                       const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets);

private:
    formalism::datalog::RuleView rule;
    formalism::datalog::RuleView witness_rule;
    formalism::datalog::GroundConjunctiveConditionView nullary_condition;
    formalism::datalog::RuleView unary_overapproximation_rule;
    formalism::datalog::RuleView binary_overapproximation_rule;
    formalism::datalog::RuleView static_binary_overapproximation_rule;
    formalism::datalog::RuleView conflicting_overapproximation_rule;

    StaticConsistencyGraph static_consistency_graph;
};

/**
 * Implementations
 */

template<typename AndAP>
RuleWorkspace<AndAP>::Common::Common(const formalism::datalog::Repository& program_repository,
                                     const formalism::datalog::Repository& workspace_repository,
                                     const StaticConsistencyGraph& static_consistency_graph) :
    program_repository(program_repository),
    workspace_repository(workspace_repository),
    kpkc(static_consistency_graph),
    statistics()
{
}

template<typename AndAP>
void RuleWorkspace<AndAP>::Common::clear() noexcept
{
    kpkc.reset();
}

template<typename AndAP>
void RuleWorkspace<AndAP>::Common::initialize_iteration(const StaticConsistencyGraph& static_consistency_graph,
                                                        const TaggedFactSets<formalism::FluentTag>& delta_fact_sets,
                                                        const AssignmentSets& assignment_sets)
{
    kpkc.set_next_assignment_sets(static_consistency_graph, delta_fact_sets, assignment_sets);
}

template<typename AndAP>
RuleWorkspace<AndAP>::Iteration::Iteration(formalism::datalog::RepositoryFactory& factory, const ConstRuleWorkspace& cws, const Common& common) :
    workspace_overlay_repository(factory.create(&common.workspace_repository)),
    head_predicate(cws.get_rule().get_head().get_predicate().get_index()),
    head_rows(),
    and_annot(),
    kpkc_workspace(common.kpkc.get_graph_layout())
{
}

template<typename AndAP>
void RuleWorkspace<AndAP>::Iteration::clear() noexcept
{
    workspace_overlay_repository.clear();
    head_rows.clear();
    and_annot.clear();
}

template<typename AndAP>
RuleWorkspace<AndAP>::Solve::Solve(formalism::datalog::RepositoryFactory& factory,
                                   const formalism::datalog::Repository& program_repository,
                                   const formalism::datalog::Repository& workspace_repository,
                                   const AndAP& and_ap) :
    and_ap(and_ap),
    program_overlay_repository(factory.create(&program_repository)),
    seen_bindings_dbg(),
    applicability_check_pool(),
    pending_rules(),
    statistics()
{
}

template<typename AndAP>
void RuleWorkspace<AndAP>::Solve::clear() noexcept
{
    program_overlay_repository.clear();
    seen_bindings_dbg.clear();
    pending_rules.clear();
}

template<typename AndAP>
RuleWorkspace<AndAP>::Worker::Worker(formalism::datalog::RepositoryFactory& factory,
                                     const formalism::datalog::Repository& program_repository,
                                     const formalism::datalog::Repository& workspace_repository,
                                     const ConstRuleWorkspace& cws,
                                     const Common& common,
                                     const AndAP& and_ap) :
    builder(),
    binding(),
    iteration(factory, cws, common),
    solve(factory, program_repository, workspace_repository, and_ap)
{
}

template<typename AndAP>
void RuleWorkspace<AndAP>::Worker::clear() noexcept
{
    iteration.clear();
    solve.clear();
}

template<typename AndAP>
RuleWorkspace<AndAP>::RuleWorkspace(formalism::datalog::RepositoryFactory& factory_,
                                    const formalism::datalog::Repository& program_repository_,
                                    const formalism::datalog::Repository& workspace_repository_,
                                    const ConstRuleWorkspace& cws_,
                                    const AndAP& and_ap_) :
    common(program_repository_, workspace_repository_, cws_.get_static_consistency_graph()),
    worker([this, program_repository = &program_repository_, workspace_repository = &workspace_repository_, factory = &factory_, cws = &cws_, and_ap = &and_ap_]
           { return Worker(*factory, *program_repository, *workspace_repository, *cws, this->common, *and_ap); })
{
}

template<typename AndAP>
void RuleWorkspace<AndAP>::clear() noexcept
{
    common.clear();
    for (auto& w : worker)
        w.clear();
}

}

#endif
