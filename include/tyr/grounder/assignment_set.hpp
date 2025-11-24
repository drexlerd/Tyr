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
#include "tyr/grounder/fact_set.hpp"

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

    PerfectAssignmentHash(const analysis::DomainListList& parameter_domains, size_t num_objects) : m_num_assignments(0), m_remapping(), m_offsets()
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
                m_remapping[i + 1][object_index.get_value() + 1] = ++new_index;
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
    PredicateAssignmentSet(Proxy<formalism::Predicate<T>, C> predicate, const analysis::DomainListList& parameter_domains, size_t num_objects) :
        m_predicate(predicate.get_index()),
        m_hash(PerfectAssignmentHash(parameter_domains, num_objects)),
        m_set(m_hash.size(), false)
    {
    }

    void reset() noexcept { m_set.reset(); }

    template<formalism::IsContext C>
    void insert(Proxy<formalism::GroundAtom<T>, C> ground_atom)
    {
        const auto arity = ground_atom.get_predicate().get_arity();
        const auto objects = ground_atom.get_terms();

        assert(ground_atom.get_index().get_group() == m_predicate);

        for (size_t first_index = 0; first_index < arity; ++first_index)
        {
            const auto first_object = objects[first_index];

            // Complete vertex.
            m_set.set(m_hash.get_assignment_rank(VertexAssignment(first_index, first_object.get_index().get_value())));

            for (size_t second_index = first_index + 1; second_index < arity; ++second_index)
            {
                const auto second_object = objects[second_index];

                // Ordered complete edge.
                m_set.set(m_hash.get_assignment_rank(
                    EdgeAssignment(first_index, first_object.get_index().get_value(), second_index, second_object.get_index().get_value())));
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
    PredicateAssignmentSets(SpanProxy<formalism::Predicate<T>, C> predicates, const analysis::DomainListListList& predicate_domains, size_t num_objects) :
        m_sets()
    {
        /* Validate inputs. */
        for (uint_t i = 0; i < predicates.size(); ++i)
            assert(predicates[i].get_index().get_value() == i);

        /* Initialize sets. */
        for (const auto predicate : predicates)
            m_sets.emplace_back(PredicateAssignmentSet<T>(predicate, predicate_domains[predicate.get_index().get_value()], num_objects));
    }

    void reset() noexcept
    {
        for (auto& set : m_sets)
            set.reset();
    }

    template<formalism::IsContext C>
    void insert(SpanProxy<formalism::GroundAtom<T>, C> ground_atoms)
    {
        for (const auto ground_atom : ground_atoms)
            m_sets[ground_atom.get_index().get_group().get_value()].insert(ground_atom);
    }

    template<formalism::IsContext C>
    void insert(Proxy<formalism::GroundAtom<T>, C> ground_atom)
    {
        m_sets[ground_atom.get_index().get_group().get_value()].insert(ground_atom);
    }

    const PredicateAssignmentSet<T>& get_set(Index<formalism::Predicate<T>> predicate) const noexcept { return m_sets[predicate.get_value()]; }

    size_t size() const noexcept
    {
        return std::accumulate(m_sets.begin(), m_sets.end(), size_t { 0 }, [](auto&& lhs, auto&& rhs) { return lhs + rhs.size(); });
    }
};

template<formalism::IsStaticOrFluentTag T>
class FunctionAssignmentSet
{
private:
    Index<formalism::Function<T>> m_function;

    PerfectAssignmentHash m_hash;
    std::vector<ClosedInterval<float_t>> m_set;

public:
    FunctionAssignmentSet() = default;

    template<formalism::IsContext C>
    FunctionAssignmentSet(Proxy<formalism::Function<T>, C> function, const analysis::DomainListList& parameter_domains, size_t num_objects) :
        m_function(function.get_index()),
        m_hash(PerfectAssignmentHash(parameter_domains, num_objects)),
        m_set(m_hash.size(), ClosedInterval<float_t>())
    {
    }

    void reset() noexcept { std::fill(m_set.begin(), m_set.end(), ClosedInterval<float_t>()); }

    template<formalism::IsContext C>
    void insert(Proxy<formalism::GroundFunctionTerm<T>, C> function_term, float_t value)
    {
        const auto arity = function_term.get_function().get_arity();
        const auto arguments = function_term.get_terms();

        assert(function_term.get_index().get_group() == m_function);

        auto& empty_assignment_bound = m_set[EmptyAssignment::rank];
        empty_assignment_bound = hull(empty_assignment_bound, ClosedInterval<float_t>(value, value));

        for (size_t first_index = 0; first_index < arity; ++first_index)
        {
            const auto first_object = arguments[first_index];

            // Complete vertex.
            auto& single_assignment_bound = m_set[m_hash.get_assignment_rank(VertexAssignment(first_index, first_object.get_index().get_value()))];
            single_assignment_bound = hull(single_assignment_bound, ClosedInterval<float_t>(value, value));

            for (size_t second_index = first_index + 1; second_index < arity; ++second_index)
            {
                const auto second_object = arguments[second_index];

                // Ordered complete edge.
                auto& double_assignment_bound = m_set[m_hash.get_assignment_rank(
                    EdgeAssignment(first_index, first_object.get_index().get_value(), second_index, second_object.get_index().get_value()))];
                double_assignment_bound = hull(double_assignment_bound, ClosedInterval<float_t>(value, value));
            }
        }
    }

    template<formalism::IsContext C>
    void insert(Proxy<formalism::GroundFunctionTermValue<T>, C> function_term_value)
    {
        insert(function_term_value.get_term(), function_term_value.get_value());
    }

    ClosedInterval<float_t> operator[](const EmptyAssignment& assignment) const noexcept { return m_set[EmptyAssignment::rank]; }
    ClosedInterval<float_t> operator[](const VertexAssignment& assignment) const noexcept { return m_set[m_hash.get_assignment_rank(assignment)]; }
    ClosedInterval<float_t> operator[](const EdgeAssignment& assignment) const noexcept { return m_set[m_hash.get_assignment_rank(assignment)]; }

    size_t size() const noexcept { return m_set.size(); }
};

template<formalism::IsStaticOrFluentTag T>
class FunctionAssignmentSets
{
private:
    std::vector<FunctionAssignmentSet<T>> m_sets;

public:
    FunctionAssignmentSets() = default;

    template<formalism::IsContext C>
    FunctionAssignmentSets(SpanProxy<formalism::Function<T>, C> functions, const analysis::DomainListListList& function_domains, size_t num_objects) : m_sets()
    {
        /* Validate inputs. */
        for (uint_t i = 0; i < functions.size(); ++i)
            assert(functions[i].get_index().get_value() == i);

        /* Initialize sets. */
        for (const auto function : functions)
            m_sets.emplace_back(FunctionAssignmentSet<T>(function, function_domains[function.get_index().get_value()], num_objects));
    }

    void reset() noexcept
    {
        for (auto& set : m_sets)
            set.reset();
    }

    template<formalism::IsContext C>
    void insert(SpanProxy<formalism::GroundFunctionTerm<T>, C> function_terms, const std::vector<float_t>& values)
    {
        for (size_t i = 0; i < function_terms.size(); ++i)
            m_sets[function_terms[i].get_index().get_group().get_value()].insert(function_terms[i], values[i]);
    }

    template<formalism::IsContext C>
    void insert(SpanProxy<formalism::GroundFunctionTermValue<T>, C> function_term_values)
    {
        for (size_t i = 0; i < function_term_values.size(); ++i)
            m_sets[function_term_values[i].get_index().get_group().get_value()].insert(function_term_values[i]);
    }

    const FunctionAssignmentSet<T>& get_set(Index<formalism::Function<T>> function) const noexcept { return m_sets[function.get_index().get_value()]; }

    size_t size() const noexcept
    {
        return std::accumulate(m_sets.begin(), m_sets.end(), size_t { 0 }, [](auto&& lhs, auto&& rhs) { return lhs + rhs.size(); });
    }
};

template<formalism::IsStaticOrFluentTag T>
struct TaggedAssignmentSets;

template<>
struct TaggedAssignmentSets<formalism::StaticTag>
{
    PredicateAssignmentSets<formalism::StaticTag> predicate;
    FunctionAssignmentSets<formalism::StaticTag> function;

    TaggedAssignmentSets() = default;

    template<formalism::IsContext C>
    TaggedAssignmentSets(SpanProxy<formalism::Predicate<formalism::StaticTag>, C> static_predicates,
                         SpanProxy<formalism::Function<formalism::StaticTag>, C> static_functions,
                         const analysis::DomainListListList& static_predicate_domains,
                         const analysis::DomainListListList& static_function_domains,
                         size_t num_objects) :
        predicate(static_predicates, static_predicate_domains, num_objects),
        function(static_functions, static_function_domains, num_objects)
    {
    }
};

template<>
struct TaggedAssignmentSets<formalism::FluentTag>
{
    PredicateAssignmentSets<formalism::FluentTag> predicate;
    FunctionAssignmentSets<formalism::FluentTag> function;

    TaggedAssignmentSets() = default;

    template<formalism::IsContext C>
    TaggedAssignmentSets(SpanProxy<formalism::Predicate<formalism::FluentTag>, C> fluent_predicates,
                         SpanProxy<formalism::Function<formalism::FluentTag>, C> fluent_functions,
                         const analysis::DomainListListList& fluent_predicate_domains,
                         const analysis::DomainListListList& fluent_function_domains,
                         size_t num_objects) :
        predicate(fluent_predicates, fluent_predicate_domains, num_objects),
        function(fluent_functions, fluent_function_domains, num_objects)
    {
    }

    void reset()
    {
        predicate.reset();
        function.reset();
    }
};

struct AssignmentSets
{
    TaggedAssignmentSets<formalism::StaticTag> static_sets;
    TaggedAssignmentSets<formalism::FluentTag> fluent_sets;

    template<formalism::IsContext C>
    AssignmentSets(Proxy<formalism::Program, C> program, const analysis::VariableDomains& domains) :
        static_sets(program.template get_predicates<formalism::StaticTag>(),
                    program.template get_functions<formalism::StaticTag>(),
                    domains.static_predicate_domains,
                    domains.static_function_domains,
                    program.get_objects().size()),
        fluent_sets(program.template get_predicates<formalism::FluentTag>(),
                    program.template get_functions<formalism::FluentTag>(),
                    domains.fluent_predicate_domains,
                    domains.fluent_function_domains,
                    program.get_objects().size())
    {
    }

    void reset() { fluent_sets.reset(); }

    template<formalism::IsContext C>
    void insert(const FactSets<C>& fact_sets)
    {
        fluent_sets.predicate.insert(fact_sets.predicate);
        fluent_sets.function.insert(fact_sets.function);
    }
};

}

#endif