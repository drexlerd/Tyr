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

#include "tyr/datalog/assignment_sets.hpp"

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

namespace f = tyr::formalism;
namespace fd = tyr::formalism::datalog;

namespace tyr::datalog
{
namespace
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
}

/**
 * PerfectAssignmentHash
 */

PerfectAssignmentHash::PerfectAssignmentHash(const analysis::DomainListList& parameter_domains, size_t num_objects) :
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

template<bool Checked>
size_t PerfectAssignmentHash::get_rank(const VertexAssignment& assignment) const noexcept
{
    assert(assignment.is_valid());
    if constexpr (Checked)
        assert(contains(m_parameter_domains, assignment));

    const auto o = m_remapping[uint_t(assignment.index) + 1][uint_t(assignment.object) + 1];

    const auto result = m_offsets[uint_t(assignment.index) + 1] + o;

    assert(result < m_num_assignments);

    return result;
}

template size_t PerfectAssignmentHash::get_rank<true>(const VertexAssignment& assignment) const noexcept;
template size_t PerfectAssignmentHash::get_rank<false>(const VertexAssignment& assignment) const noexcept;

template<bool Checked>
size_t PerfectAssignmentHash::get_rank(const EdgeAssignment& assignment) const noexcept
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

template size_t PerfectAssignmentHash::get_rank<true>(const EdgeAssignment& assignment) const noexcept;
template size_t PerfectAssignmentHash::get_rank<false>(const EdgeAssignment& assignment) const noexcept;

size_t PerfectAssignmentHash::size() const noexcept { return m_num_assignments * m_num_assignments; }

/**
 * PredicateAssignmentSet
 */

template<formalism::FactKind T>
PredicateAssignmentSet<T>::PredicateAssignmentSet(formalism::datalog::PredicateView<T> predicate,
                                                  const analysis::DomainListList& parameter_domains,
                                                  size_t num_objects) :
    m_predicate(predicate),
    m_predicate_index(predicate.get_index()),
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
void PredicateAssignmentSet<T>::insert(formalism::datalog::PredicateBindingView<T> binding)
{
    const auto arity = m_predicate.get_arity();
    const auto objects = binding.get_objects();

    assert(binding.get_index().relation == m_predicate_index);

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

template<formalism::FactKind T>
bool PredicateAssignmentSet<T>::operator[](const VertexAssignment& assignment) const noexcept
{
    return m_set.test(m_hash.template get_rank<false>(assignment));
}

template<formalism::FactKind T>
bool PredicateAssignmentSet<T>::operator[](const EdgeAssignment& assignment) const noexcept
{
    return m_set.test(m_hash.template get_rank<false>(assignment));
}

template<formalism::FactKind T>
bool PredicateAssignmentSet<T>::at(const VertexAssignment& assignment) const noexcept
{
    return m_set.test(m_hash.template get_rank<true>(assignment));
}

template<formalism::FactKind T>
bool PredicateAssignmentSet<T>::at(const EdgeAssignment& assignment) const noexcept
{
    return m_set.test(m_hash.template get_rank<true>(assignment));
}

template<formalism::FactKind T>
size_t PredicateAssignmentSet<T>::size() const noexcept
{
    return m_set.size();
}

template<formalism::FactKind T>
const PerfectAssignmentHash& PredicateAssignmentSet<T>::get_hash() const noexcept
{
    return m_hash;
}

template<formalism::FactKind T>
const boost::dynamic_bitset<>& PredicateAssignmentSet<T>::get_set() const noexcept
{
    return m_set;
}

template class PredicateAssignmentSet<f::StaticTag>;
template class PredicateAssignmentSet<f::FluentTag>;

/**
 * PredicateAssignmentSets
 */

template<formalism::FactKind T>
PredicateAssignmentSets<T>::PredicateAssignmentSets()
{
}

template<formalism::FactKind T>
PredicateAssignmentSets<T>::PredicateAssignmentSets(formalism::datalog::PredicateListView<T> predicates,
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

template<formalism::FactKind T>
void PredicateAssignmentSets<T>::reset() noexcept
{
    for (auto& set : m_sets)
        set.reset();
}

template<formalism::FactKind T>
void PredicateAssignmentSets<T>::insert(formalism::datalog::GroundAtomView<T> ground_atom)
{
    insert(ground_atom.get_row());
}

template<formalism::FactKind T>
void PredicateAssignmentSets<T>::insert(formalism::datalog::PredicateBindingView<T> binding)
{
    m_sets[uint_t(binding.get_index().relation)].insert(binding);
}

template<formalism::FactKind T>
void PredicateAssignmentSets<T>::insert(formalism::datalog::PredicateBindingForwardRangeView<T> bindings)
{
    for (const auto binding : bindings)
        insert(binding);
}

template<formalism::FactKind T>
const PredicateAssignmentSet<T>& PredicateAssignmentSets<T>::get_set(Index<formalism::Predicate<T>> index) const noexcept
{
    return m_sets[uint_t(index)];
}

template<formalism::FactKind T>
size_t PredicateAssignmentSets<T>::size() const noexcept
{
    return std::accumulate(m_sets.begin(), m_sets.end(), size_t { 0 }, [](auto&& lhs, auto&& rhs) { return lhs + rhs.size(); });
}

template<formalism::FactKind T>
const std::vector<PredicateAssignmentSet<T>>& PredicateAssignmentSets<T>::get_sets() const noexcept
{
    return m_sets;
}

template class PredicateAssignmentSets<f::StaticTag>;
template class PredicateAssignmentSets<f::FluentTag>;

/**
 * FunctionAssignmentSet
 */

template<formalism::FactKind T>
FunctionAssignmentSet<T>::FunctionAssignmentSet(formalism::datalog::FunctionView<T> function,
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
void FunctionAssignmentSet<T>::insert(formalism::datalog::FunctionBindingView<T> binding, float_t value)
{
    const auto objects = binding.get_objects();
    const auto arity = objects.size();

    auto& empty_assignment_bound = m_set[EmptyAssignment::rank];
    empty_assignment_bound = hull(empty_assignment_bound, ClosedInterval<float_t>(value, value));

    for (uint_t first_index = 0; first_index < arity; ++first_index)
    {
        const auto first_object = objects[first_index];

        // Complete vertex.
        auto& single_assignment_bound = m_set[m_hash.get_rank<false>(VertexAssignment(formalism::ParameterIndex(first_index), first_object.get_index()))];
        single_assignment_bound = hull(single_assignment_bound, ClosedInterval<float_t>(value, value));

        for (uint_t second_index = first_index + 1; second_index < arity; ++second_index)
        {
            const auto second_object = objects[second_index];

            // Ordered complete edge.
            auto& double_assignment_bound = m_set[m_hash.get_rank<false>(EdgeAssignment(formalism::ParameterIndex(first_index),
                                                                                        first_object.get_index(),
                                                                                        formalism::ParameterIndex(second_index),
                                                                                        second_object.get_index()))];
            double_assignment_bound = hull(double_assignment_bound, ClosedInterval<float_t>(value, value));
        }
    }
}

template<formalism::FactKind T>
void FunctionAssignmentSet<T>::insert(formalism::datalog::GroundFunctionTermValueView<T> fterm_value)
{
    insert(fterm_value.get_fterm().get_row(), fterm_value.get_value());
}

template<formalism::FactKind T>
ClosedInterval<float_t> FunctionAssignmentSet<T>::operator[](const EmptyAssignment& assignment) const noexcept
{
    return m_set[EmptyAssignment::rank];
}

template<formalism::FactKind T>
ClosedInterval<float_t> FunctionAssignmentSet<T>::operator[](const VertexAssignment& assignment) const noexcept
{
    return m_set[m_hash.template get_rank<false>(assignment)];
}

template<formalism::FactKind T>
ClosedInterval<float_t> FunctionAssignmentSet<T>::operator[](const EdgeAssignment& assignment) const noexcept
{
    return m_set[m_hash.template get_rank<false>(assignment)];
}

template<formalism::FactKind T>
ClosedInterval<float_t> FunctionAssignmentSet<T>::at(const EmptyAssignment& assignment) const noexcept
{
    return m_set[EmptyAssignment::rank];
}

template<formalism::FactKind T>
ClosedInterval<float_t> FunctionAssignmentSet<T>::at(const VertexAssignment& assignment) const noexcept
{
    return m_set[m_hash.template get_rank<true>(assignment)];
}

template<formalism::FactKind T>
ClosedInterval<float_t> FunctionAssignmentSet<T>::at(const EdgeAssignment& assignment) const noexcept
{
    return m_set[m_hash.template get_rank<true>(assignment)];
}

template<formalism::FactKind T>
size_t FunctionAssignmentSet<T>::size() const noexcept
{
    return m_set.size();
}

template<formalism::FactKind T>
const PerfectAssignmentHash& FunctionAssignmentSet<T>::get_hash() const noexcept
{
    return m_hash;
}

template class FunctionAssignmentSet<f::StaticTag>;
template class FunctionAssignmentSet<f::FluentTag>;

/**
 * FunctionAssignmentSets
 */

template<formalism::FactKind T>
FunctionAssignmentSets<T>::FunctionAssignmentSets()
{
}

template<formalism::FactKind T>
FunctionAssignmentSets<T>::FunctionAssignmentSets(formalism::datalog::FunctionListView<T> functions,
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
void FunctionAssignmentSets<T>::insert(formalism::datalog::GroundFunctionTermView<T> function_term, float_t value)
{
    m_sets[function_term.get_function().get_index().get_value()].insert(function_term.get_row(), value);
}

template<formalism::FactKind T>
void FunctionAssignmentSets<T>::insert(formalism::datalog::GroundFunctionTermListView<T> function_terms, const std::vector<float_t>& values)
{
    assert(function_terms.size() == values.size());

    for (size_t i = 0; i < function_terms.size(); ++i)
        insert(function_terms[i], values[i]);
}

template<formalism::FactKind T>
void FunctionAssignmentSets<T>::insert(formalism::datalog::GroundFunctionTermValueListView<T> fterm_values)
{
    for (const auto fterm_value : fterm_values)
        insert(fterm_value.get_fterm(), fterm_value.get_value());
}

template<formalism::FactKind T>
const FunctionAssignmentSet<T>& FunctionAssignmentSets<T>::get_set(Index<formalism::Function<T>> index) const noexcept
{
    return m_sets[index.get_value()];
}

template<formalism::FactKind T>
std::vector<FunctionAssignmentSet<T>>& FunctionAssignmentSets<T>::get_sets() noexcept
{
    return m_sets;
}

template<formalism::FactKind T>
const std::vector<FunctionAssignmentSet<T>>& FunctionAssignmentSets<T>::get_sets() const noexcept
{
    return m_sets;
}

template<formalism::FactKind T>
size_t FunctionAssignmentSets<T>::size() const noexcept
{
    return std::accumulate(m_sets.begin(), m_sets.end(), size_t { 0 }, [](auto&& lhs, auto&& rhs) { return lhs + rhs.size(); });
}

template class FunctionAssignmentSets<f::StaticTag>;
template class FunctionAssignmentSets<f::FluentTag>;

/**
 * TaggedAssignmentSets
 */

template<formalism::FactKind T>
TaggedAssignmentSets<T>::TaggedAssignmentSets()
{
}

template<formalism::FactKind T>
TaggedAssignmentSets<T>::TaggedAssignmentSets(formalism::datalog::PredicateListView<T> predicates,
                                              formalism::datalog::FunctionListView<T> functions,
                                              const analysis::DomainListListList& predicate_domains,
                                              const analysis::DomainListListList& function_domains,
                                              size_t num_objects) :
    predicate(predicates, predicate_domains, num_objects),
    function(functions, function_domains, num_objects)
{
}

template<formalism::FactKind T>
TaggedAssignmentSets<T>::TaggedAssignmentSets(formalism::datalog::PredicateListView<T> predicates,
                                              formalism::datalog::FunctionListView<T> functions,
                                              const analysis::DomainListListList& predicate_domains,
                                              const analysis::DomainListListList& function_domains,
                                              size_t num_objects,
                                              const TaggedFactSets<T>& fact_sets) :
    TaggedAssignmentSets(predicates, functions, predicate_domains, function_domains, num_objects)
{
    insert(fact_sets);
}

template<formalism::FactKind T>
void TaggedAssignmentSets<T>::insert(const TaggedFactSets<T>& fact_sets)
{
    for (const auto& set : fact_sets.predicate.get_sets())
        predicate.insert(set.get_bindings());

    for (uint_t i = 0; i < fact_sets.function.get_sets().size(); ++i)
    {
        auto& self = function.get_sets()[i];
        const auto& other = fact_sets.function.get_sets()[i];

        const auto& remap = other.get_remap();
        const auto& bindings = other.get_bindings();
        const auto& values = other.get_values();
        for (uint_t j = 0; j < remap.size(); ++j)
        {
            if (remap[j] == std::numeric_limits<uint_t>::max())
                continue;

            const auto pos = remap[j];
            self.insert(bindings[pos], values[pos]);
        }
    }
}

template<formalism::FactKind T>
void TaggedAssignmentSets<T>::reset() noexcept
{
    predicate.reset();
    function.reset();
}

template class TaggedAssignmentSets<f::StaticTag>;
template class TaggedAssignmentSets<f::FluentTag>;

/**
 * AssignmentSets
 */

AssignmentSets::AssignmentSets(const TaggedAssignmentSets<formalism::StaticTag>& static_sets, const TaggedAssignmentSets<formalism::FluentTag>& fluent_sets) :
    static_sets(static_sets),
    fluent_sets(fluent_sets)
{
}

template<formalism::FactKind T>
const TaggedAssignmentSets<T>& AssignmentSets::get() const noexcept
{
    if constexpr (std::is_same_v<T, formalism::StaticTag>)
        return static_sets;
    else if constexpr (std::is_same_v<T, formalism::FluentTag>)
        return fluent_sets;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template const TaggedAssignmentSets<f::StaticTag>& AssignmentSets::get<f::StaticTag>() const noexcept;
template const TaggedAssignmentSets<f::FluentTag>& AssignmentSets::get<f::FluentTag>() const noexcept;

}
