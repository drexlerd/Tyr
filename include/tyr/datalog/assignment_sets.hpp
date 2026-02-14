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

[[maybe_unused]] inline bool contains(const analysis::DomainListList& parameter_domains, const VertexAssignment& assignment)
{
    const auto& domain = parameter_domains[uint_t(assignment.index)];
    return std::find(domain.begin(), domain.end(), assignment.object) != domain.end();
}

[[maybe_unused]] inline bool contains(const analysis::DomainListList& parameter_domains, const EdgeAssignment& assignment)
{
    return contains(parameter_domains, VertexAssignment(assignment.first_index, assignment.first_object))
           && contains(parameter_domains, VertexAssignment(assignment.second_index, assignment.second_object));
}

struct PerfectAssignmentHash
{
    size_t m_num_assignments;                      ///< The number of type legal [i/o] including a sentinel for each i
    std::vector<std::vector<uint_t>> m_remapping;  ///< The remapping of o in O to index for each type legal [i/o]
    std::vector<uint_t> m_offsets;                 ///< The offsets of i
    analysis::DomainListList m_parameter_domains;

    PerfectAssignmentHash(const analysis::DomainListList& parameter_domains, size_t num_objects) :
        m_num_assignments(0),
        m_remapping(),
        m_offsets(),
        m_parameter_domains(parameter_domains)
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
                m_remapping[i + 1][uint_t(object_index) + 1] = ++new_index;
                ++m_num_assignments;
            }
        }
    }

    /// @brief
    /// @tparam Checked = true enables an assertion that checks whether an assignment is part of the hash function.
    /// This assertion checks that the object in the vertex assignment is part of the parameter domain.
    /// @param assignment
    /// @return
    template<bool Checked>
    size_t get_rank(const VertexAssignment& assignment) const noexcept
    {
        assert(assignment.is_valid());
        if constexpr (Checked)
            assert(contains(m_parameter_domains, assignment));

        const auto o = m_remapping[uint_t(assignment.index) + 1][uint_t(assignment.object) + 1];

        const auto result = m_offsets[uint_t(assignment.index) + 1] + o;

        assert(result < m_num_assignments);

        return result;
    }
    /// @brief
    /// @tparam Checked = true enables an assertion that checks whether an assignment is part of the hash function.
    /// This assertion checks that each vertex assignment in the edge assignment is is part of the hash function.
    /// @param assignment
    /// @return
    template<bool Checked>
    size_t get_rank(const EdgeAssignment& assignment) const noexcept
    {
        assert(assignment.is_valid());
        if constexpr (Checked)
            assert(contains(m_parameter_domains, assignment));

        const auto o1 = m_remapping[uint_t(assignment.first_index) + 1][uint_t(assignment.first_object) + 1];
        const auto o2 = m_remapping[uint_t(assignment.second_index) + 1][uint_t(assignment.second_object) + 1];

        const auto j1 = m_offsets[uint_t(assignment.first_index) + 1] + o1;
        const auto j2 = m_offsets[uint_t(assignment.second_index) + 1] + o2;

        const auto result = j1 * m_num_assignments + j2;

        assert(result < m_num_assignments * m_num_assignments);

        return result;
    }

    size_t size() const noexcept { return m_num_assignments * m_num_assignments; }
};

template<formalism::FactKind T>
class PredicateAssignmentSet
{
private:
    Index<formalism::Predicate<T>> m_predicate;

    PerfectAssignmentHash m_hash;
    boost::dynamic_bitset<> m_set;

public:
    PredicateAssignmentSet(View<Index<formalism::Predicate<T>>, formalism::datalog::Repository> predicate,
                           const analysis::DomainListList& parameter_domains,
                           size_t num_objects) :
        m_predicate(predicate.get_index()),
        m_hash(PerfectAssignmentHash(parameter_domains, num_objects)),
        m_set(m_hash.size(), false)
    {
    }

    void reset() noexcept { m_set.reset(); }

    void insert(View<Index<formalism::datalog::GroundAtom<T>>, formalism::datalog::Repository> ground_atom)
    {
        const auto arity = ground_atom.get_predicate().get_arity();
        const auto objects = ground_atom.get_objects();

        assert(ground_atom.get_predicate().get_index() == m_predicate);

        for (uint_t first_index = 0; first_index < arity; ++first_index)
        {
            const auto first_object = objects[first_index];

            // Complete vertex.
            m_set.set(m_hash.get_rank<false>(VertexAssignment(formalism::ParameterIndex(first_index), first_object.get_index())));

            for (uint_t second_index = first_index + 1; second_index < arity; ++second_index)
            {
                const auto second_object = objects[second_index];

                // Ordered complete edge.
                m_set.set(m_hash.get_rank<false>(EdgeAssignment(formalism::ParameterIndex(first_index),
                                                                first_object.get_index(),
                                                                formalism::ParameterIndex(second_index),
                                                                second_object.get_index())));
            }
        }
    }

    bool operator[](const VertexAssignment& assignment) const noexcept { return m_set.test(m_hash.template get_rank<false>(assignment)); }
    bool operator[](const EdgeAssignment& assignment) const noexcept { return m_set.test(m_hash.template get_rank<false>(assignment)); }
    bool at(const VertexAssignment& assignment) const noexcept { return m_set.test(m_hash.template get_rank<true>(assignment)); }
    bool at(const EdgeAssignment& assignment) const noexcept { return m_set.test(m_hash.template get_rank<true>(assignment)); }

    size_t size() const noexcept { return m_set.size(); }
    const PerfectAssignmentHash& get_hash() const noexcept { return m_hash; }
    const boost::dynamic_bitset<>& get_set() const noexcept { return m_set; }
};

template<formalism::FactKind T>
class PredicateAssignmentSets
{
private:
    std::vector<PredicateAssignmentSet<T>> m_sets;

public:
    PredicateAssignmentSets() = default;

    PredicateAssignmentSets(View<IndexList<formalism::Predicate<T>>, formalism::datalog::Repository> predicates,
                            const analysis::DomainListListList& predicate_domains,
                            size_t num_objects) :
        m_sets()
    {
        /* Validate inputs. */
        for (uint_t i = 0; i < predicates.size(); ++i)
        {
            assert(uint_t(predicates[i].get_index()) == i);
        }

        /* Initialize sets. */
        for (const auto predicate : predicates)
            m_sets.emplace_back(PredicateAssignmentSet<T>(predicate, predicate_domains[uint_t(predicate.get_index())], num_objects));
    }

    void reset() noexcept
    {
        for (auto& set : m_sets)
            set.reset();
    }

    void insert(View<IndexList<formalism::datalog::GroundAtom<T>>, formalism::datalog::Repository> ground_atoms)
    {
        for (const auto ground_atom : ground_atoms)
            insert(ground_atom);
    }

    void insert(View<Index<formalism::datalog::GroundAtom<T>>, formalism::datalog::Repository> ground_atom)
    {
        m_sets[uint_t(ground_atom.get_predicate().get_index())].insert(ground_atom);
    }

    const PredicateAssignmentSet<T>& get_set(Index<formalism::Predicate<T>> index) const noexcept { return m_sets[uint_t(index)]; }

    size_t size() const noexcept
    {
        return std::accumulate(m_sets.begin(), m_sets.end(), size_t { 0 }, [](auto&& lhs, auto&& rhs) { return lhs + rhs.size(); });
    }

    const auto& get_sets() const noexcept { return m_sets; }
};

template<formalism::FactKind T>
class FunctionAssignmentSet
{
private:
    Index<formalism::Function<T>> m_function;

    PerfectAssignmentHash m_hash;
    std::vector<ClosedInterval<float_t>> m_set;

public:
    FunctionAssignmentSet() = default;

    FunctionAssignmentSet(View<Index<formalism::Function<T>>, formalism::datalog::Repository> function,
                          const analysis::DomainListList& parameter_domains,
                          size_t num_objects) :
        m_function(function.get_index()),
        m_hash(PerfectAssignmentHash(parameter_domains, num_objects)),
        m_set(m_hash.size(), ClosedInterval<float_t>())
    {
    }

    void reset() noexcept { std::fill(m_set.begin(), m_set.end(), ClosedInterval<float_t>()); }

    void insert(View<Index<formalism::datalog::GroundFunctionTerm<T>>, formalism::datalog::Repository> function_term, float_t value)
    {
        const auto arity = function_term.get_function().get_arity();
        const auto arguments = function_term.get_objects();

        assert(function_term.get_function().get_index() == m_function);

        auto& empty_assignment_bound = m_set[EmptyAssignment::rank];
        empty_assignment_bound = hull(empty_assignment_bound, ClosedInterval<float_t>(value, value));

        for (uint_t first_index = 0; first_index < arity; ++first_index)
        {
            const auto first_object = arguments[first_index];

            // Complete vertex.
            auto& single_assignment_bound = m_set[m_hash.get_rank<false>(VertexAssignment(formalism::ParameterIndex(first_index), first_object.get_index()))];
            single_assignment_bound = hull(single_assignment_bound, ClosedInterval<float_t>(value, value));

            for (uint_t second_index = first_index + 1; second_index < arity; ++second_index)
            {
                const auto second_object = arguments[second_index];

                // Ordered complete edge.
                auto& double_assignment_bound = m_set[m_hash.get_rank<false>(EdgeAssignment(formalism::ParameterIndex(first_index),
                                                                                            first_object.get_index(),
                                                                                            formalism::ParameterIndex(second_index),
                                                                                            second_object.get_index()))];
                double_assignment_bound = hull(double_assignment_bound, ClosedInterval<float_t>(value, value));
            }
        }
    }

    void insert(View<Index<formalism::datalog::GroundFunctionTermValue<T>>, formalism::datalog::Repository> fterm_value)
    {
        insert(fterm_value.get_fterm(), fterm_value.get_value());
    }

    ClosedInterval<float_t> operator[](const EmptyAssignment& assignment) const noexcept { return m_set[EmptyAssignment::rank]; }
    ClosedInterval<float_t> operator[](const VertexAssignment& assignment) const noexcept { return m_set[m_hash.template get_rank<false>(assignment)]; }
    ClosedInterval<float_t> operator[](const EdgeAssignment& assignment) const noexcept { return m_set[m_hash.template get_rank<false>(assignment)]; }

    ClosedInterval<float_t> at(const EmptyAssignment& assignment) const noexcept { return m_set[EmptyAssignment::rank]; }
    ClosedInterval<float_t> at(const VertexAssignment& assignment) const noexcept { return m_set[m_hash.template get_rank<true>(assignment)]; }
    ClosedInterval<float_t> at(const EdgeAssignment& assignment) const noexcept { return m_set[m_hash.template get_rank<true>(assignment)]; }

    size_t size() const noexcept { return m_set.size(); }
    const PerfectAssignmentHash& get_hash() const noexcept { return m_hash; }
};

template<formalism::FactKind T>
class FunctionAssignmentSets
{
private:
    std::vector<FunctionAssignmentSet<T>> m_sets;

public:
    FunctionAssignmentSets() = default;

    FunctionAssignmentSets(View<IndexList<formalism::Function<T>>, formalism::datalog::Repository> functions,
                           const analysis::DomainListListList& function_domains,
                           size_t num_objects) :
        m_sets()
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

    void insert(View<Index<formalism::datalog::GroundFunctionTerm<T>>, formalism::datalog::Repository> function_term, float_t value)
    {
        m_sets[function_term.get_function().get_index().get_value()].insert(function_term, value);
    }

    void insert(View<IndexList<formalism::datalog::GroundFunctionTerm<T>>, formalism::datalog::Repository> function_terms, const std::vector<float_t>& values)
    {
        for (size_t i = 0; i < function_terms.size(); ++i)
            m_sets[function_terms[i].get_function().get_index().get_value()].insert(function_terms[i], values[i]);
    }

    void insert(View<IndexList<formalism::datalog::GroundFunctionTermValue<T>>, formalism::datalog::Repository> fterm_values)
    {
        for (size_t i = 0; i < fterm_values.size(); ++i)
            m_sets[fterm_values[i].get_fterm().get_function().get_index().get_value()].insert(fterm_values[i]);
    }

    const FunctionAssignmentSet<T>& get_set(Index<formalism::Function<T>> index) const noexcept { return m_sets[index.get_value()]; }

    size_t size() const noexcept
    {
        return std::accumulate(m_sets.begin(), m_sets.end(), size_t { 0 }, [](auto&& lhs, auto&& rhs) { return lhs + rhs.size(); });
    }
};

template<formalism::FactKind T>
struct TaggedAssignmentSets
{
    PredicateAssignmentSets<T> predicate;
    FunctionAssignmentSets<T> function;

    TaggedAssignmentSets() = default;

    TaggedAssignmentSets(View<IndexList<formalism::Predicate<T>>, formalism::datalog::Repository> predicates,
                         View<IndexList<formalism::Function<T>>, formalism::datalog::Repository> functions,
                         const analysis::DomainListListList& predicate_domains,
                         const analysis::DomainListListList& function_domains,
                         size_t num_objects) :
        predicate(predicates, predicate_domains, num_objects),
        function(functions, function_domains, num_objects)
    {
    }

    TaggedAssignmentSets(View<IndexList<formalism::Predicate<T>>, formalism::datalog::Repository> predicates,
                         View<IndexList<formalism::Function<T>>, formalism::datalog::Repository> functions,
                         const analysis::DomainListListList& predicate_domains,
                         const analysis::DomainListListList& function_domains,
                         size_t num_objects,
                         const TaggedFactSets<T>& fact_sets) :
        TaggedAssignmentSets(predicates, functions, predicate_domains, function_domains, num_objects)
    {
        insert(fact_sets);
    }

    void insert(const TaggedFactSets<T>& fact_sets)
    {
        for (const auto& set : fact_sets.predicate.get_sets())
            predicate.insert(set.get_facts());
        for (const auto& set : fact_sets.function.get_sets())
            function.insert(set.get_fterms(), set.get_values());
    }

    void reset()
    {
        predicate.reset();
        function.reset();
    }
};

struct AssignmentSets
{
    const TaggedAssignmentSets<formalism::StaticTag>& static_sets;
    const TaggedAssignmentSets<formalism::FluentTag>& fluent_sets;

    AssignmentSets(const TaggedAssignmentSets<formalism::StaticTag>& static_sets, const TaggedAssignmentSets<formalism::FluentTag>& fluent_sets) :
        static_sets(static_sets),
        fluent_sets(fluent_sets)
    {
    }

    template<formalism::FactKind T>
    const auto& get() const
    {
        if constexpr (std::is_same_v<T, formalism::StaticTag>)
            return static_sets;
        else if constexpr (std::is_same_v<T, formalism::FluentTag>)
            return fluent_sets;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }
};

}

#endif