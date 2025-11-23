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

#ifndef TYR_GROUNDER_ASSIGNMENT_SET_HPP_
#define TYR_GROUNDER_ASSIGNMENT_SET_HPP_

#include "tyr/analysis/domains.hpp"
#include "tyr/common/closed_interval.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/grounder/assignment.hpp"

#include <boost/dynamic_bitset.hpp>
#include <cassert>
#include <limits>
#include <tuple>
#include <vector>

namespace tyr::grounder
{

struct PerfectAssignmentHash
{
    size_t m_num_assignments;                        ///< The number of type legal [i/o] including a sentinel for each i
    std::vector<std::vector<uint32_t>> m_remapping;  ///< The remapping of o in O to index for each type legal [i/o]
    std::vector<uint32_t> m_offsets;                 ///< The offsets of i

    PerfectAssignmentHash(const analysis::DomainList& parameter_domains, size_t num_objects);

    size_t get_assignment_rank(const VertexAssignment& assignment) const noexcept;

    size_t get_assignment_rank(const EdgeAssignment& assignment) const noexcept;

    size_t size() const noexcept;
};

template<formalism::IsStaticOrFluentTag T>
class PredicateAssignmentSet
{
private:
    Index<formalism::Predicate<T>> m_predicate;

    PerfectAssignmentHash m_hash;
    boost::dynamic_bitset<> m_set;

public:
    template<formalism::IsContext C>
    PredicateAssignmentSet(Proxy<formalism::Predicate<T>, C> predicate, size_t num_objects);

    void reset() noexcept;

    template<formalism::IsContext C>
    void insert_ground_atom(Proxy<formalism::GroundAtom<T>, C> ground_atom);

    bool operator[](const VertexAssignment& assignment) const noexcept;
    bool operator[](const EdgeAssignment& assignment) const noexcept;

    size_t size() const noexcept;
};

template<formalism::IsStaticOrFluentTag T>
class PredicateAssignmentSets
{
private:
    std::vector<PredicateAssignmentSet<T>> m_sets;

public:
    PredicateAssignmentSets() = default;

    template<formalism::IsContext C>
    PredicateAssignmentSets(SpanProxy<formalism::Predicate<T>, C>& predicates, size_t num_objects);

    void reset() noexcept;

    template<formalism::IsContext C>
    void insert_ground_atoms(SpanProxy<formalism::GroundAtom<T>, C>& ground_atoms);

    template<formalism::IsContext C>
    void insert_ground_atom(Proxy<formalism::GroundAtom<T>, C> ground_atom);

    const PredicateAssignmentSet<T>& get_set(Index<formalism::Predicate<T>> predicate) const noexcept;

    size_t size() const noexcept;
};

template<formalism::IsStaticOrFluentTag T>
class FunctionSkeletonAssignmentSet
{
private:
    Index<formalism::Function<T>> m_function;

    PerfectAssignmentHash m_hash;
    std::vector<ClosedInterval<float_t>> m_set;

public:
    FunctionSkeletonAssignmentSet() = default;

    template<formalism::IsContext C>
    FunctionSkeletonAssignmentSet(Proxy<formalism::Function<T>, C> function, size_t num_objects);

    void reset() noexcept;

    template<formalism::IsContext C>
    void insert_ground_function_term_value(Proxy<formalism::GroundFunctionTerm<T>, C> term, float_t value);

    ClosedInterval<float_t> operator[](const EmptyAssignment& assignment) const noexcept;
    ClosedInterval<float_t> operator[](const VertexAssignment& assignment) const noexcept;
    ClosedInterval<float_t> operator[](const EdgeAssignment& assignment) const noexcept;

    size_t size() const noexcept;
};

template<formalism::IsStaticOrFluentTag T>
class FunctionSkeletonAssignmentSets
{
private:
    std::vector<FunctionSkeletonAssignmentSet<T>> m_sets;

public:
    FunctionSkeletonAssignmentSets() = default;

    template<formalism::IsContext C>
    FunctionSkeletonAssignmentSets(SpanProxy<formalism::FunctionSkeleton<T>, C>& functions, size_t num_objects);

    void reset() noexcept;

    template<formalism::IsContext C>
    void insert_ground_function_values(SpanProxy<formalism::GroundFunctionTerm<T>, C>& terms, const std::vector<float_T>& values);

    const FunctionSkeletonAssignmentSet<T>& get_set(Index<formalism::Function<T>> function) const noexcept;

    size_t size() const noexcept;
};

template<formalism::IsStaticOrFluentTag T>
struct AssignmentSets;

template<>
struct AssignmentSets<formalism::StaticTag>
{
    PredicateAssignmentSets<formalism::StaticTag> static_predicate_assignment_sets;

    AssignmentSets();

    template<formalism::IsContext C>
    AssignmentSets(Proxy<formalism::Program, C> program);
};

template<>
struct AssignmentSets<formalism::FluentTag>
{
    PredicateAssignmentSets<formalism::FluentTag> fluent_predicate_assignment_sets;

    AssignmentSets();

    template<formalism::IsContext C>
    AssignmentSets(Proxy<formalism::Program, C> program);
};

}

#endif