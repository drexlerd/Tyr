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

#include "tyr/datalog/fact_sets.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/overlay_repository.hpp"

#include <boost/dynamic_bitset.hpp>
#include <limits>

namespace f = tyr::formalism;
namespace fd = tyr::formalism::datalog;

namespace tyr::datalog
{

/**
 * PredicateFactSet
 */

template<f::FactKind T>
PredicateFactSet<T>::PredicateFactSet(View<Index<formalism::Predicate<T>>, formalism::datalog::Repository> predicate) :
    m_predicate(predicate.get_index()),
    m_context(predicate.get_context()),
    m_indices(),
    m_bitset()
{
}

template<f::FactKind T>
void PredicateFactSet<T>::reset()
{
    m_indices.clear();
    m_bitset.reset();
}

template<f::FactKind T>
void PredicateFactSet<T>::insert(View<Index<fd::GroundAtom<T>>, fd::Repository> view)
{
    if (&m_context != &view.get_context())
        throw std::runtime_error("Incompatible contexts.");

    const auto index = view.get_index();

    if (index.get_value() >= m_bitset.size())
        m_bitset.resize(index.get_value() + 1, false);

    if (!m_bitset.test(index.get_value()))
        m_indices.push_back(index);

    m_bitset.set(index.get_value());
}

template<f::FactKind T>
void PredicateFactSet<T>::insert(View<IndexList<fd::GroundAtom<T>>, fd::Repository> view)
{
    for (const auto atom : view)
        insert(atom);
}

template<f::FactKind T>
bool PredicateFactSet<T>::contains(Index<fd::GroundAtom<T>> index) const noexcept
{
    if (index.get_value() >= m_bitset.size())
        return false;
    return m_bitset.test(index.get_value());
}

template<f::FactKind T>
bool PredicateFactSet<T>::contains(View<Index<fd::GroundAtom<T>>, fd::Repository> view) const noexcept
{
    return contains(view.get_index());
}

template<f::FactKind T>
View<IndexList<fd::GroundAtom<T>>, fd::Repository> PredicateFactSet<T>::get_facts() const noexcept
{
    return make_view(m_indices, m_context);
}

template<f::FactKind T>
const boost::dynamic_bitset<>& PredicateFactSet<T>::get_bitset() const noexcept
{
    return m_bitset;
}

template class PredicateFactSet<f::StaticTag>;
template class PredicateFactSet<f::FluentTag>;

/**
 * FunctionFactSet
 */

template<f::FactKind T>
FunctionFactSet<T>::FunctionFactSet(View<Index<formalism::Function<T>>, formalism::datalog::Repository> function) :
    m_function(function.get_index()),
    m_context(function.get_context()),
    m_indices(),
    m_unique()
{
}

template<f::FactKind T>
void FunctionFactSet<T>::reset()
{
    m_indices.clear();
    m_unique.clear();
    std::fill(m_values.begin(), m_values.end(), std::numeric_limits<float_t>::quiet_NaN());
}

template<f::FactKind T>
void FunctionFactSet<T>::insert(View<Index<fd::GroundFunctionTerm<T>>, fd::Repository> function_term, float_t value)
{
    const auto fterm_index = function_term.get_index();

    if (m_unique.contains(fterm_index))
        throw std::runtime_error("Multiple value assignments to a ground function term.");

    m_indices.push_back(fterm_index);
    m_unique.insert(fterm_index);
    if (fterm_index.get_value() >= m_values.size())
        m_values.resize(fterm_index.get_value() + 1, std::numeric_limits<float_t>::quiet_NaN());
    m_values[fterm_index.get_value()] = value;
}

template<f::FactKind T>
void FunctionFactSet<T>::insert(View<IndexList<fd::GroundFunctionTerm<T>>, fd::Repository> function_terms, const std::vector<float_t>& values)
{
    assert(function_terms.size() == values.size());

    for (uint_t i = 0; i < function_terms.size(); ++i)
        insert(function_terms[i], values[i]);
}

template<f::FactKind T>
void FunctionFactSet<T>::insert(View<Index<fd::GroundFunctionTermValue<T>>, fd::Repository> fterm_value)
{
    insert(fterm_value.get_fterm(), fterm_value.get_value());
}

template<f::FactKind T>
void FunctionFactSet<T>::insert(View<IndexList<fd::GroundFunctionTermValue<T>>, fd::Repository> fterm_values)
{
    for (const auto fterm_value : fterm_values)
        insert(fterm_value);
}

template<f::FactKind T>
bool FunctionFactSet<T>::contains(Index<fd::GroundFunctionTerm<T>> fterm) const noexcept
{
    return m_unique.contains(fterm);
}

template<f::FactKind T>
bool FunctionFactSet<T>::contains(View<Index<fd::GroundFunctionTerm<T>>, fd::Repository> fterm) const noexcept
{
    return contains(fterm.get_index());
}

template<f::FactKind T>
float_t FunctionFactSet<T>::operator[](Index<fd::GroundFunctionTerm<T>> fterm) const noexcept
{
    return m_values[fterm.get_value()];
}

template<f::FactKind T>
View<IndexList<fd::GroundFunctionTerm<T>>, fd::Repository> FunctionFactSet<T>::get_fterms() const noexcept
{
    return make_view(m_indices, m_context);
}

template<f::FactKind T>
const std::vector<float_t>& FunctionFactSet<T>::get_values() const noexcept
{
    return m_values;
}

template class FunctionFactSet<f::StaticTag>;
template class FunctionFactSet<f::FluentTag>;

/**
 * FactSets
 */

FactSets::FactSets(View<Index<fd::Program>, fd::Repository> program) :
    static_sets(program.template get_predicates<f::StaticTag>(), program.template get_functions<f::StaticTag>()),
    fluent_sets(program.template get_predicates<f::FluentTag>(), program.template get_functions<f::FluentTag>())
{
    static_sets.predicate.insert(program.template get_atoms<f::StaticTag>());
    static_sets.function.insert(program.template get_fterm_values<f::StaticTag>());
    fluent_sets.predicate.insert(program.template get_atoms<f::FluentTag>());
    fluent_sets.function.insert(program.template get_fterm_values<f::FluentTag>());
}

FactSets::FactSets(View<Index<fd::Program>, fd::Repository> program, TaggedFactSets<f::FluentTag> fluent_facts) :
    static_sets(program.template get_predicates<f::StaticTag>(), program.template get_functions<f::StaticTag>()),
    fluent_sets(program.template get_predicates<f::FluentTag>(), program.template get_functions<f::FluentTag>())
{
    static_sets.predicate.insert(program.template get_atoms<f::StaticTag>());
    static_sets.function.insert(program.template get_fterm_values<f::StaticTag>());

    for (const auto& set : fluent_facts.predicate.get_sets())
        fluent_sets.predicate.insert(set.get_facts());
    for (const auto& set : fluent_facts.function.get_sets())
        fluent_sets.function.insert(set.get_fterms(), set.get_values());
}

}
