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
#include "tyr/common/config.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_atom_proxy.hpp"
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

    PerfectAssignmentHash(const analysis::DomainListList& parameter_domains, size_t num_objects)
    {
        const auto num_parameters = parameter_domains.size();

        m_remapping.resize(num_parameters + 1);
        m_offsets.resize(num_parameters + 1);

        m_remapping[0].resize(1, 0);  // 0 is sentinel to map to 0
        m_offsets[0] = m_num_assignments++;

        for (uint_t i = 0; i < num_parameters; ++i)
        {
            m_remapping[i + 1].resize(num_objects + 1, 0);  // 0 is sentinel to map to 0
            m_offsets[i + 1] = m_num_assignments++;

            const auto& parameter_domain = parameter_domains[i];
            auto new_index = uint_t { 0 };
            for (const auto object_index : parameter_domain)
            {
                m_remapping[i + 1][object_index.value + 1] = ++new_index;
                ++m_num_assignments;
            }
        }
    }

    size_t get_assignment_rank(const VertexAssignment& assignment) const noexcept
    {
        assert(assignment.is_valid());

        const auto o = m_remapping[assignment.index + 1][assignment.object + 1];

        const auto result = m_offsets[assignment.index + 1] + o;

        assert(result < m_num_assignments);

        return result;
    }

    size_t get_assignment_rank(const EdgeAssignment& assignment) const noexcept
    {
        assert(assignment.is_valid());

        const auto o1 = m_remapping[assignment.first_index + 1][assignment.first_object + 1];
        const auto o2 = m_remapping[assignment.second_index + 1][assignment.second_object + 1];

        const auto j1 = m_offsets[assignment.first_index + 1] + o1;
        const auto j2 = m_offsets[assignment.second_index + 1] + o2;

        const auto result = j1 * m_num_assignments + j2;

        assert(result < m_num_assignments * m_num_assignments);

        return result;
    }

    size_t size() const noexcept { return m_num_assignments * m_num_assignments; }
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
    PredicateAssignmentSet(Proxy<formalism::Predicate<T>, C> predicate, const analysis::DomainListList& predicate_parameter_domains, size_t num_objects) :
        m_predicate(predicate.get_index()),
        m_hash(PerfectAssignmentHash(predicate_parameter_domains, num_objects)),
        m_set(m_hash.size(), false)
    {
    }

    void reset() noexcept { m_set.reset(); }

    template<formalism::IsContext C>
    void insert_ground_atom(Proxy<formalism::GroundAtom<T>, C> ground_atom)
    {
        const auto arity = ground_atom.get_predicate().get_arity();
        const auto objects = ground_atom.get_terms();

        assert(ground_atom.get_predicate().get_index() == m_predicate);

        for (size_t first_index = 0; first_index < arity; ++first_index)
        {
            const auto first_object = objects[first_index];

            // Complete vertex.
            m_set.set(m_hash.get_assignment_rank(VertexAssignment(first_index, first_object.get_index().value)));

            for (size_t second_index = first_index + 1; second_index < arity; ++second_index)
            {
                const auto second_object = objects[second_index];

                // Ordered complete edge.
                m_set.set(
                    m_hash.get_assignment_rank(EdgeAssignment(first_index, first_object.get_index().value, second_index, second_object.get_index().value)));
            }
        }
    }

    bool operator[](const VertexAssignment& assignment) const noexcept { return m_set.test(m_hash.get_assignment_rank(assignment)); }
    bool operator[](const EdgeAssignment& assignment) const noexcept { return m_set.test(m_hash.get_assignment_rank(assignment)); }

    size_t size() const noexcept { return m_set.size(); }
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
    FunctionSkeletonAssignmentSets(SpanProxy<formalism::Function<T>, C>& functions, size_t num_objects);

    void reset() noexcept;

    template<formalism::IsContext C>
    void insert_ground_function_values(SpanProxy<formalism::GroundFunctionTerm<T>, C>& terms, const std::vector<float_t>& values);

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