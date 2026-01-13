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

#ifndef TYR_DATALOG_WORKSPACES_RULE_DELTA_HPP_
#define TYR_DATALOG_WORKSPACES_RULE_DELTA_HPP_

#include "tyr/common/declarations.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/unique_object_pool.hpp"
#include "tyr/datalog/applicability.hpp"
#include "tyr/datalog/policies/annotation.hpp"
#include "tyr/formalism/binding_index.hpp"
#include "tyr/formalism/datalog/ground_atom_index.hpp"
#include "tyr/formalism/datalog/merge.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/overlay_repository.hpp"

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
                    formalism::datalog::GrounderContext<C>& context)
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
    bool is_dynamically_applicable(const FactSets& fact_sets, formalism::datalog::GrounderContext<C>& context)
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
                    formalism::datalog::GrounderContext<C>& context)
    {
        m_nullary.initialize(nullary, fact_sets);
        m_conflicting.initialize(conflicting, fact_sets, context);
    }

    bool is_statically_applicable() const noexcept { return m_nullary.is_statically_applicable() && m_conflicting.is_statically_applicable(); }

    template<formalism::datalog::Context C>
    bool is_dynamically_applicable(const FactSets& fact_sets, formalism::datalog::GrounderContext<C>& context) noexcept
    {
        return m_nullary.is_dynamically_applicable(fact_sets) && m_conflicting.is_dynamically_applicable(fact_sets, context);
    }
};

struct PendingRule
{
    Index<formalism::Binding> binding;
    Index<formalism::datalog::GroundAtom<formalism::FluentTag>> head;

    auto identifying_members() const noexcept { return binding; }
};

struct RuleDeltaWorkspace
{
    /// Merge thread into staging area
    formalism::datalog::RepositoryPtr repository;

    /// Results across iterations
    IndexList<formalism::Object> binding;
    formalism::datalog::MergeCache merge_cache;

    UnorderedSet<IndexList<formalism::Object>> seen_bindings_dbg;

    /// Pool applicability checks since we dont know how many are needed.
    UniqueObjectPool<ApplicabilityCheck> applicability_check_pool;
    UnorderedMap<PendingRule, UniqueObjectPoolPtr<ApplicabilityCheck>> pending_rules;

    RuleDeltaWorkspace();

    void clear() noexcept;
};
}

#endif
