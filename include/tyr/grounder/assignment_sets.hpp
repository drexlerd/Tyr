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

#ifndef TYR_GROUNDER_ASSIGNMENT_SETS_HPP_
#define TYR_GROUNDER_ASSIGNMENT_SETS_HPP_

#include "tyr/analysis/domains.hpp"
#include "tyr/common/closed_interval.hpp"
#include "tyr/common/config.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/grounder/assignment.hpp"
#include "tyr/grounder/fact_sets.hpp"

#include <boost/dynamic_bitset.hpp>
#include <cassert>
#include <limits>
#include <tuple>
#include <vector>

namespace tyr::grounder
{

struct PerfectAssignmentHash
{
    size_t m_num_assignments;                      ///< The number of type legal [i/o] including a sentinel for each i
    std::vector<std::vector<uint_t>> m_remapping;  ///< The remapping of o in O to index for each type legal [i/o]
    std::vector<uint_t> m_offsets;                 ///< The offsets of i

    PerfectAssignmentHash(const analysis::DomainListList& parameter_domains, size_t num_objects);

    size_t get_assignment_rank(const VertexAssignment& assignment) const noexcept;

    size_t get_assignment_rank(const EdgeAssignment& assignment) const noexcept;

    size_t size() const noexcept;
};

template<formalism::FactKind T, formalism::Context C>
class PredicateAssignmentSet
{
private:
    Index<formalism::Predicate<T>> m_predicate;

    PerfectAssignmentHash m_hash;
    boost::dynamic_bitset<> m_set;

public:
    PredicateAssignmentSet(View<Index<formalism::Predicate<T>>, C> predicate, const analysis::DomainListList& parameter_domains, size_t num_objects);

    void reset() noexcept;

    void insert(View<Index<formalism::GroundAtom<T>>, C> ground_atom);

    bool operator[](const VertexAssignment& assignment) const noexcept;
    bool operator[](const EdgeAssignment& assignment) const noexcept;

    size_t size() const noexcept;
};

template<formalism::FactKind T, formalism::Context C>
class PredicateAssignmentSets
{
private:
    std::vector<PredicateAssignmentSet<T>> m_sets;

public:
    PredicateAssignmentSets() = default;

    PredicateAssignmentSets(View<IndexList<formalism::Predicate<T>>, C> predicates, const analysis::DomainListListList& predicate_domains, size_t num_objects);

    void reset() noexcept;

    void insert(View<IndexList<formalism::GroundAtom<T>>, C> ground_atoms);

    void insert(View<Index<formalism::GroundAtom<T>>, C> ground_atom);

    const PredicateAssignmentSet<T, C>& get_set(Index<formalism::Predicate<T>> index) const noexcept;

    size_t size() const noexcept;
};

template<formalism::FactKind T, formalism::Context C>
class FunctionAssignmentSet
{
private:
    Index<formalism::Function<T>> m_function;

    PerfectAssignmentHash m_hash;
    std::vector<ClosedInterval<float_t>> m_set;

public:
    FunctionAssignmentSet() = default;

    FunctionAssignmentSet(View<Index<formalism::Function<T>>, C> function, const analysis::DomainListList& parameter_domains, size_t num_objects);

    void reset() noexcept;

    void insert(View<Index<formalism::GroundFunctionTerm<T>>, C> function_term, float_t value);

    void insert(View<Index<formalism::GroundFunctionTermValue<T>>, C> fterm_value);

    ClosedInterval<float_t> operator[](const EmptyAssignment& assignment) const noexcept;
    ClosedInterval<float_t> operator[](const VertexAssignment& assignment) const noexcept;
    ClosedInterval<float_t> operator[](const EdgeAssignment& assignment) const noexcept;

    size_t size() const noexcept;
};

template<formalism::FactKind T, formalism::Context C>
class FunctionAssignmentSets
{
private:
    std::vector<FunctionAssignmentSet<T>> m_sets;

public:
    FunctionAssignmentSets() = default;

    FunctionAssignmentSets(View<IndexList<formalism::Function<T>>, C> functions, const analysis::DomainListListList& function_domains, size_t num_objects);

    void reset() noexcept;

    void insert(View<Index<formalism::GroundFunctionTerm<T>>, C> function_term, float_t value);

    void insert(View<IndexList<formalism::GroundFunctionTerm<T>>, C> function_terms, const std::vector<float_t>& values);

    void insert(View<IndexList<formalism::GroundFunctionTermValue<T>>, C> fterm_values);

    const FunctionAssignmentSet<T, C>& get_set(Index<formalism::Function<T>> index) const noexcept;

    size_t size() const noexcept;
};

template<formalism::FactKind T, formalism::Context C>
struct TaggedAssignmentSets
{
    PredicateAssignmentSets<T> predicate;
    FunctionAssignmentSets<T> function;

    TaggedAssignmentSets() = default;

    TaggedAssignmentSets(View<IndexList<formalism::Predicate<T>>, C> predicates,
                         View<IndexList<formalism::Function<T>>, C> functions,
                         const analysis::DomainListListList& predicate_domains,
                         const analysis::DomainListListList& function_domains,
                         size_t num_objects);

    void insert(const TaggedFactSets<T, C>& fact_sets);

    void reset();
};

template<formalism::Context C>
struct AssignmentSets
{
    TaggedAssignmentSets<formalism::StaticTag, C> static_sets;
    TaggedAssignmentSets<formalism::FluentTag, C> fluent_sets;

    AssignmentSets(View<Index<formalism::Program>, C> program, const analysis::ProgramVariableDomains& domains);

    AssignmentSets(View<Index<formalism::Program>, C> program, const analysis::ProgramVariableDomains& domains, const FactSets<C>& fact_sets);

    template<formalism::FactKind T>
    void reset() noexcept;

    void reset() noexcept;

    template<formalism::FactKind T>
    void insert(const TaggedFactSets<T, C>& fact_set);

    void insert(const FactSets<C>& fact_sets);

    template<formalism::FactKind T>
    TaggedAssignmentSets<T, C>& get();

    template<formalism::FactKind T>
    const TaggedAssignmentSets<T, C>& get() const;
};

}

#endif