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

#ifndef TYR_DATALOG_ASSIGNMENT_SETS_HPP_
#define TYR_DATALOG_ASSIGNMENT_SETS_HPP_

#include "tyr/analysis/domains.hpp"
#include "tyr/common/closed_interval.hpp"
#include "tyr/common/config.hpp"
#include "tyr/datalog/assignment.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/datalog/formatter.hpp"
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"

#include <algorithm>
#include <boost/dynamic_bitset.hpp>
#include <cassert>
#include <limits>
#include <tuple>
#include <vector>

namespace tyr::datalog
{

struct PerfectAssignmentHash
{
    size_t m_num_assignments;                      ///< The number of type legal [i/o] including a sentinel for each i
    std::vector<std::vector<uint_t>> m_remapping;  ///< The remapping of o in O to index for each type legal [i/o]
    std::vector<uint_t> m_offsets;                 ///< The offsets of i
    analysis::DomainListList m_parameter_domains;

    PerfectAssignmentHash(const analysis::DomainListList& parameter_domains, size_t num_objects);

    /// @brief
    /// @tparam Checked = true enables an assertion that checks whether an assignment is part of the hash function.
    /// This assertion checks that the object in the vertex assignment is part of the parameter domain.
    /// @param assignment
    /// @return
    template<bool Checked>
    size_t get_rank(const VertexAssignment& assignment) const noexcept;
    /// @brief
    /// @tparam Checked = true enables an assertion that checks whether an assignment is part of the hash function.
    /// This assertion checks that each vertex assignment in the edge assignment is is part of the hash function.
    /// @param assignment
    /// @return
    template<bool Checked>
    size_t get_rank(const EdgeAssignment& assignment) const noexcept;

    size_t size() const noexcept;
};

template<formalism::FactKind T>
class PredicateAssignmentSet
{
private:
    formalism::datalog::PredicateView<T> m_predicate;
    Index<formalism::Predicate<T>> m_predicate_index;

    PerfectAssignmentHash m_hash;
    boost::dynamic_bitset<> m_set;

public:
    PredicateAssignmentSet(formalism::datalog::PredicateView<T> predicate, const analysis::DomainListList& parameter_domains, size_t num_objects);

    void reset() noexcept;

    void insert(formalism::datalog::PredicateBindingView<T> binding);

    bool operator[](const VertexAssignment& assignment) const noexcept;
    bool operator[](const EdgeAssignment& assignment) const noexcept;
    bool at(const VertexAssignment& assignment) const noexcept;
    bool at(const EdgeAssignment& assignment) const noexcept;

    size_t size() const noexcept;
    const PerfectAssignmentHash& get_hash() const noexcept;
    const boost::dynamic_bitset<>& get_set() const noexcept;
};

template<formalism::FactKind T>
class PredicateAssignmentSets
{
private:
    std::vector<PredicateAssignmentSet<T>> m_sets;

public:
    PredicateAssignmentSets();
    PredicateAssignmentSets(formalism::datalog::PredicateListView<T> predicates, const analysis::DomainListListList& predicate_domains, size_t num_objects);

    void reset() noexcept;

    void insert(formalism::datalog::GroundAtomView<T> ground_atom);
    void insert(formalism::datalog::PredicateBindingView<T> binding);
    void insert(formalism::datalog::PredicateBindingForwardRangeView<T> bindings);

    const PredicateAssignmentSet<T>& get_set(Index<formalism::Predicate<T>> index) const noexcept;

    size_t size() const noexcept;

    const std::vector<PredicateAssignmentSet<T>>& get_sets() const noexcept;
};

template<formalism::FactKind T>
class FunctionAssignmentSet
{
private:
    Index<formalism::Function<T>> m_function;

    PerfectAssignmentHash m_hash;
    std::vector<ClosedInterval<float_t>> m_set;

public:
    FunctionAssignmentSet(formalism::datalog::FunctionView<T> function, const analysis::DomainListList& parameter_domains, size_t num_objects);

    void reset() noexcept;

    void insert(formalism::datalog::FunctionBindingView<T> binding, float_t value);
    void insert(formalism::datalog::GroundFunctionTermValueView<T> fterm_value);

    ClosedInterval<float_t> operator[](const EmptyAssignment& assignment) const noexcept;
    ClosedInterval<float_t> operator[](const VertexAssignment& assignment) const noexcept;
    ClosedInterval<float_t> operator[](const EdgeAssignment& assignment) const noexcept;

    ClosedInterval<float_t> at(const EmptyAssignment& assignment) const noexcept;
    ClosedInterval<float_t> at(const VertexAssignment& assignment) const noexcept;
    ClosedInterval<float_t> at(const EdgeAssignment& assignment) const noexcept;

    size_t size() const noexcept;
    const PerfectAssignmentHash& get_hash() const noexcept;
};

template<formalism::FactKind T>
class FunctionAssignmentSets
{
private:
    std::vector<FunctionAssignmentSet<T>> m_sets;

public:
    FunctionAssignmentSets();
    FunctionAssignmentSets(formalism::datalog::FunctionListView<T> functions, const analysis::DomainListListList& function_domains, size_t num_objects);

    void reset() noexcept;

    void insert(formalism::datalog::GroundFunctionTermView<T> function_term, float_t value);
    void insert(formalism::datalog::GroundFunctionTermListView<T> function_terms, const std::vector<float_t>& values);
    void insert(formalism::datalog::GroundFunctionTermValueListView<T> fterm_values);

    const FunctionAssignmentSet<T>& get_set(Index<formalism::Function<T>> index) const noexcept;
    std::vector<FunctionAssignmentSet<T>>& get_sets() noexcept;
    const std::vector<FunctionAssignmentSet<T>>& get_sets() const noexcept;

    size_t size() const noexcept;
};

template<formalism::FactKind T>
struct TaggedAssignmentSets
{
    PredicateAssignmentSets<T> predicate;
    FunctionAssignmentSets<T> function;

    TaggedAssignmentSets();
    TaggedAssignmentSets(formalism::datalog::PredicateListView<T> predicates,
                         formalism::datalog::FunctionListView<T> functions,
                         const analysis::DomainListListList& predicate_domains,
                         const analysis::DomainListListList& function_domains,
                         size_t num_objects);
    TaggedAssignmentSets(formalism::datalog::PredicateListView<T> predicates,
                         formalism::datalog::FunctionListView<T> functions,
                         const analysis::DomainListListList& predicate_domains,
                         const analysis::DomainListListList& function_domains,
                         size_t num_objects,
                         const TaggedFactSets<T>& fact_sets);

    void insert(const TaggedFactSets<T>& fact_sets);

    void reset() noexcept;
};

struct AssignmentSets
{
    const TaggedAssignmentSets<formalism::StaticTag>& static_sets;
    const TaggedAssignmentSets<formalism::FluentTag>& fluent_sets;

    AssignmentSets(const TaggedAssignmentSets<formalism::StaticTag>& static_sets, const TaggedAssignmentSets<formalism::FluentTag>& fluent_sets);

    template<formalism::FactKind T>
    const TaggedAssignmentSets<T>& get() const noexcept;
};

}

#endif