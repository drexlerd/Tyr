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

#include "tyr/analysis/domains.hpp"
#include "tyr/common/closed_interval.hpp"
#include "tyr/common/config.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/grounder/assignment.hpp"
#include "tyr/grounder/assignment_sets.hpp"
#include "tyr/grounder/fact_sets.hpp"

#include <boost/dynamic_bitset.hpp>
#include <cassert>
#include <limits>
#include <tuple>
#include <vector>

namespace tyr::grounder
{

PerfectAssignmentHash::PerfectAssignmentHash(const analysis::DomainListList& parameter_domains, size_t num_objects) :
    m_num_assignments(0),
    m_remapping(),
    m_offsets()
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

size_t PerfectAssignmentHash::get_assignment_rank(const VertexAssignment& assignment) const noexcept
{
    assert(assignment.is_valid());

    const auto o = m_remapping[uint_t(assignment.index) + 1][uint_t(assignment.object) + 1];

    const auto result = m_offsets[uint_t(assignment.index) + 1] + o;

    assert(result < m_num_assignments);

    return result;
}

size_t PerfectAssignmentHash::get_assignment_rank(const EdgeAssignment& assignment) const noexcept
{
    assert(assignment.is_valid());

    const auto o1 = m_remapping[uint_t(assignment.first_index) + 1][uint_t(assignment.first_object) + 1];
    const auto o2 = m_remapping[uint_t(assignment.second_index) + 1][uint_t(assignment.second_object) + 1];

    const auto j1 = m_offsets[uint_t(assignment.first_index) + 1] + o1;
    const auto j2 = m_offsets[uint_t(assignment.second_index) + 1] + o2;

    const auto result = j1 * m_num_assignments + j2;

    assert(result < m_num_assignments * m_num_assignments);

    return result;
}

size_t PerfectAssignmentHash::size() const noexcept { return m_num_assignments * m_num_assignments; }

template<formalism::FactKind T>
PredicateAssignmentSet<T>::PredicateAssignmentSet(View<Index<formalism::Predicate<T>>, formalism::Repository> predicate,
                                                  const analysis::DomainListList& parameter_domains,
                                                  size_t num_objects) :
    m_predicate(predicate.get_index()),
    m_hash(PerfectAssignmentHash(parameter_domains, num_objects)),
    m_set(m_hash.size(), false)
{
}

template<formalism::FactKind T>
void PredicateAssignmentSet<T>::reset() noexcept
{
    m_set.reset();
}

template<formalism::FactKind T>
void PredicateAssignmentSet<T>::insert(View<Index<formalism::GroundAtom<T>>, formalism::Repository> ground_atom)
{
    const auto arity = ground_atom.get_predicate().get_arity();
    const auto objects = ground_atom.get_objects();

    assert(ground_atom.get_predicate().get_index() == m_predicate);

    for (uint_t first_index = 0; first_index < arity; ++first_index)
    {
        const auto first_object = objects[first_index];

        // Complete vertex.
        m_set.set(m_hash.get_assignment_rank(VertexAssignment(formalism::ParameterIndex(first_index), first_object.get_index())));

        for (uint_t second_index = first_index + 1; second_index < arity; ++second_index)
        {
            const auto second_object = objects[second_index];

            // Ordered complete edge.
            m_set.set(m_hash.get_assignment_rank(EdgeAssignment(formalism::ParameterIndex(first_index),
                                                                first_object.get_index(),
                                                                formalism::ParameterIndex(second_index),
                                                                second_object.get_index())));
        }
    }
}

template<formalism::FactKind T>
bool PredicateAssignmentSet<T>::operator[](const VertexAssignment& assignment) const noexcept
{
    return m_set.test(m_hash.get_assignment_rank(assignment));
}

template<formalism::FactKind T>
bool PredicateAssignmentSet<T>::operator[](const EdgeAssignment& assignment) const noexcept
{
    return m_set.test(m_hash.get_assignment_rank(assignment));
}

template<formalism::FactKind T>
size_t PredicateAssignmentSet<T>::size() const noexcept
{
    return m_set.size();
}

template class PredicateAssignmentSet<formalism::StaticTag>;
template class PredicateAssignmentSet<formalism::FluentTag>;
template class PredicateAssignmentSet<formalism::DerivedTag>;

template<formalism::FactKind T>
PredicateAssignmentSets<T>::PredicateAssignmentSets(View<IndexList<formalism::Predicate<T>>, formalism::Repository> predicates,
                                                    const analysis::DomainListListList& predicate_domains,
                                                    size_t num_objects) :
    m_sets()
{
    /* Validate inputs. */
    for (uint_t i = 0; i < predicates.size(); ++i)
    {
        assert(predicates[i].get_index().get_value() == i);
    }

    /* Initialize sets. */
    for (const auto predicate : predicates)
        m_sets.emplace_back(PredicateAssignmentSet<T>(predicate, predicate_domains[predicate.get_index().get_value()], num_objects));
}

template<formalism::FactKind T>
void PredicateAssignmentSets<T>::reset() noexcept
{
    for (auto& set : m_sets)
        set.reset();
}

template<formalism::FactKind T>
void PredicateAssignmentSets<T>::insert(View<IndexList<formalism::GroundAtom<T>>, formalism::Repository> ground_atoms)
{
    for (const auto ground_atom : ground_atoms)
        m_sets[ground_atom.get_predicate().get_index().get_value()].insert(ground_atom);
}

template<formalism::FactKind T>
void PredicateAssignmentSets<T>::insert(View<Index<formalism::GroundAtom<T>>, formalism::Repository> ground_atom)
{
    m_sets[ground_atom.get_predicate().get_index().get_value()].insert(ground_atom);
}

template<formalism::FactKind T>
const PredicateAssignmentSet<T>& PredicateAssignmentSets<T>::get_set(Index<formalism::Predicate<T>> index) const noexcept
{
    return m_sets[index.get_value()];
}

template<formalism::FactKind T>
size_t PredicateAssignmentSets<T>::size() const noexcept
{
    return std::accumulate(m_sets.begin(), m_sets.end(), size_t { 0 }, [](auto&& lhs, auto&& rhs) { return lhs + rhs.size(); });
}

template class PredicateAssignmentSets<formalism::StaticTag>;
template class PredicateAssignmentSets<formalism::FluentTag>;

template<formalism::FactKind T>
FunctionAssignmentSet<T>::FunctionAssignmentSet(View<Index<formalism::Function<T>>, formalism::Repository> function,
                                                const analysis::DomainListList& parameter_domains,
                                                size_t num_objects) :
    m_function(function.get_index()),
    m_hash(PerfectAssignmentHash(parameter_domains, num_objects)),
    m_set(m_hash.size(), ClosedInterval<float_t>())
{
}

template<formalism::FactKind T>
void FunctionAssignmentSet<T>::reset() noexcept
{
    std::fill(m_set.begin(), m_set.end(), ClosedInterval<float_t>());
}

template<formalism::FactKind T>
void FunctionAssignmentSet<T>::insert(View<Index<formalism::GroundFunctionTerm<T>>, formalism::Repository> function_term, float_t value)
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
        auto& single_assignment_bound = m_set[m_hash.get_assignment_rank(VertexAssignment(formalism::ParameterIndex(first_index), first_object.get_index()))];
        single_assignment_bound = hull(single_assignment_bound, ClosedInterval<float_t>(value, value));

        for (uint_t second_index = first_index + 1; second_index < arity; ++second_index)
        {
            const auto second_object = arguments[second_index];

            // Ordered complete edge.
            auto& double_assignment_bound = m_set[m_hash.get_assignment_rank(EdgeAssignment(formalism::ParameterIndex(first_index),
                                                                                            first_object.get_index(),
                                                                                            formalism::ParameterIndex(second_index),
                                                                                            second_object.get_index()))];
            double_assignment_bound = hull(double_assignment_bound, ClosedInterval<float_t>(value, value));
        }
    }
}

template<formalism::FactKind T>
void FunctionAssignmentSet<T>::insert(View<Index<formalism::GroundFunctionTermValue<T>>, formalism::Repository> fterm_value)
{
    insert(fterm_value.get_fterm(), fterm_value.get_value());
}

template<formalism::FactKind T>
ClosedInterval<float_t> FunctionAssignmentSet<T>::operator[](const EmptyAssignment& assignment) const noexcept
{
    return m_set[EmptyAssignment::rank];
}

template<formalism::FactKind T>
ClosedInterval<float_t> FunctionAssignmentSet<T>::operator[](const VertexAssignment& assignment) const noexcept
{
    return m_set[m_hash.get_assignment_rank(assignment)];
}

template<formalism::FactKind T>
ClosedInterval<float_t> FunctionAssignmentSet<T>::operator[](const EdgeAssignment& assignment) const noexcept
{
    return m_set[m_hash.get_assignment_rank(assignment)];
}

template<formalism::FactKind T>
size_t FunctionAssignmentSet<T>::size() const noexcept
{
    return m_set.size();
}

template class FunctionAssignmentSet<formalism::StaticTag>;
template class FunctionAssignmentSet<formalism::FluentTag>;

template<formalism::FactKind T>
FunctionAssignmentSets<T>::FunctionAssignmentSets(View<IndexList<formalism::Function<T>>, formalism::Repository> functions,
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

template<formalism::FactKind T>
void FunctionAssignmentSets<T>::reset() noexcept
{
    for (auto& set : m_sets)
        set.reset();
}

template<formalism::FactKind T>
void FunctionAssignmentSets<T>::insert(View<Index<formalism::GroundFunctionTerm<T>>, formalism::Repository> function_term, float_t value)
{
    m_sets[function_term.get_function().get_index().get_value()].insert(function_term, value);
}

template<formalism::FactKind T>
void FunctionAssignmentSets<T>::insert(View<IndexList<formalism::GroundFunctionTerm<T>>, formalism::Repository> function_terms,
                                       const std::vector<float_t>& values)
{
    for (size_t i = 0; i < function_terms.size(); ++i)
        m_sets[function_terms[i].get_function().get_index().get_value()].insert(function_terms[i], values[i]);
}

template<formalism::FactKind T>
void FunctionAssignmentSets<T>::insert(View<IndexList<formalism::GroundFunctionTermValue<T>>, formalism::Repository> fterm_values)
{
    for (size_t i = 0; i < fterm_values.size(); ++i)
        m_sets[fterm_values[i].get_fterm().get_function().get_index().get_value()].insert(fterm_values[i]);
}

template<formalism::FactKind T>
const FunctionAssignmentSet<T>& FunctionAssignmentSets<T>::get_set(Index<formalism::Function<T>> index) const noexcept
{
    return m_sets[index.get_value()];
}

template<formalism::FactKind T>
size_t FunctionAssignmentSets<T>::size() const noexcept
{
    return std::accumulate(m_sets.begin(), m_sets.end(), size_t { 0 }, [](auto&& lhs, auto&& rhs) { return lhs + rhs.size(); });
}

template class FunctionAssignmentSets<formalism::StaticTag>;
template class FunctionAssignmentSets<formalism::FluentTag>;

template<formalism::FactKind T>
TaggedAssignmentSets<T>::TaggedAssignmentSets(View<IndexList<formalism::Predicate<T>>, formalism::Repository> predicates,
                                              View<IndexList<formalism::Function<T>>, formalism::Repository> functions,
                                              const analysis::DomainListListList& predicate_domains,
                                              const analysis::DomainListListList& function_domains,
                                              size_t num_objects) :
    predicate(predicates, predicate_domains, num_objects),
    function(functions, function_domains, num_objects)
{
}

template<formalism::FactKind T>
void TaggedAssignmentSets<T>::insert(const TaggedFactSets<T>& fact_sets)
{
    predicate.insert(fact_sets.predicate.get_facts());
    function.insert(fact_sets.function.get_fterms(), fact_sets.function.get_values());
}

template<formalism::FactKind T>
void TaggedAssignmentSets<T>::reset()
{
    predicate.reset();
    function.reset();
}

template class TaggedAssignmentSets<formalism::StaticTag>;
template class TaggedAssignmentSets<formalism::FluentTag>;

AssignmentSets::AssignmentSets(View<Index<formalism::Program>, formalism::Repository> program, const analysis::VariableDomains& domains) :
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

AssignmentSets::AssignmentSets(View<Index<formalism::Program>, formalism::Repository> program,
                               const analysis::VariableDomains& domains,
                               const FactSets& fact_sets) :
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
    insert(fact_sets);
}

template<formalism::FactKind T>
void AssignmentSets::reset() noexcept
{
    get<T>().reset();
}

template void AssignmentSets::reset<formalism::StaticTag>() noexcept;
template void AssignmentSets::reset<formalism::FluentTag>() noexcept;

void AssignmentSets::reset() noexcept
{
    reset<formalism::StaticTag>();
    reset<formalism::FluentTag>();
}

template<formalism::FactKind T>
void AssignmentSets::insert(const TaggedFactSets<T>& fact_set)
{
    get<T>().insert(fact_set);
}

template void AssignmentSets::insert<formalism::StaticTag>(const TaggedFactSets<formalism::StaticTag>& fact_set);
template void AssignmentSets::insert<formalism::FluentTag>(const TaggedFactSets<formalism::FluentTag>& fact_set);

void AssignmentSets::insert(const FactSets& fact_sets)
{
    insert(fact_sets.template get<formalism::StaticTag>());
    insert(fact_sets.template get<formalism::FluentTag>());
}

template<formalism::FactKind T>
TaggedAssignmentSets<T>& AssignmentSets::get()
{
    if constexpr (std::is_same_v<T, formalism::StaticTag>)
        return static_sets;
    else if constexpr (std::is_same_v<T, formalism::FluentTag>)
        return fluent_sets;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template TaggedAssignmentSets<formalism::StaticTag>& AssignmentSets::get<formalism::StaticTag>();
template TaggedAssignmentSets<formalism::FluentTag>& AssignmentSets::get<formalism::FluentTag>();

template<formalism::FactKind T>
const TaggedAssignmentSets<T>& AssignmentSets::get() const
{
    if constexpr (std::is_same_v<T, formalism::StaticTag>)
        return static_sets;
    else if constexpr (std::is_same_v<T, formalism::FluentTag>)
        return fluent_sets;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template const TaggedAssignmentSets<formalism::StaticTag>& AssignmentSets::get<formalism::StaticTag>() const;
template const TaggedAssignmentSets<formalism::FluentTag>& AssignmentSets::get<formalism::FluentTag>() const;
}
