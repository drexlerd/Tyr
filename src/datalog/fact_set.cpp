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

using namespace tyr::formalism;
using namespace tyr::formalism::datalog;

namespace tyr::datalog
{

/**
 * PredicateFactSet
 */

template<FactKind T>
PredicateFactSet<T>::PredicateFactSet(View<IndexList<GroundAtom<T>>, Repository> view) : m_context(view.get_context()), m_indices()
{
    insert(view);
}

template<FactKind T>
void PredicateFactSet<T>::reset()
{
    m_indices.clear();
    m_bitset.reset();
}

template<FactKind T>
void PredicateFactSet<T>::insert(View<Index<GroundAtom<T>>, Repository> view)
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

template<FactKind T>
void PredicateFactSet<T>::insert(View<IndexList<GroundAtom<T>>, Repository> view)
{
    for (const auto atom : view)
        insert(atom);
}

template<FactKind T>
bool PredicateFactSet<T>::contains(Index<GroundAtom<T>> index) const noexcept
{
    if (index.get_value() >= m_bitset.size())
        return false;
    return m_bitset.test(index.get_value());
}

template<FactKind T>
bool PredicateFactSet<T>::contains(View<Index<GroundAtom<T>>, Repository> view) const noexcept
{
    return contains(view.get_index());
}

template<FactKind T>
View<IndexList<GroundAtom<T>>, Repository> PredicateFactSet<T>::get_facts() const noexcept
{
    return make_view(m_indices, m_context);
}

template<FactKind T>
const boost::dynamic_bitset<>& PredicateFactSet<T>::get_bitset() const noexcept
{
    return m_bitset;
}

template class PredicateFactSet<StaticTag>;
template class PredicateFactSet<FluentTag>;

/**
 * FunctionFactSet
 */

template<FactKind T>
FunctionFactSet<T>::FunctionFactSet(View<IndexList<GroundFunctionTermValue<T>>, Repository> view) : m_context(view.get_context()), m_indices(), m_unique()
{
    insert(view);
}

template<FactKind T>
void FunctionFactSet<T>::reset()
{
    m_indices.clear();
    m_unique.clear();
    std::fill(m_values.begin(), m_values.end(), std::numeric_limits<float_t>::quiet_NaN());
}

template<FactKind T>
void FunctionFactSet<T>::insert(View<Index<GroundFunctionTerm<T>>, Repository> function_term, float_t value)
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

template<FactKind T>
void FunctionFactSet<T>::insert(View<IndexList<GroundFunctionTerm<T>>, Repository> function_terms, const std::vector<float_t>& values)
{
    assert(function_terms.size() == values.size());

    for (uint_t i = 0; i < function_terms.size(); ++i)
        insert(function_terms[i], values[i]);
}

template<FactKind T>
void FunctionFactSet<T>::insert(View<Index<GroundFunctionTermValue<T>>, Repository> view)
{
    insert(view.get_fterm(), view.get_value());
}

template<FactKind T>
void FunctionFactSet<T>::insert(View<IndexList<GroundFunctionTermValue<T>>, Repository> view)
{
    for (const auto fterm_value : view)
        insert(fterm_value);
}

template<FactKind T>
bool FunctionFactSet<T>::contains(Index<GroundFunctionTerm<T>> index) const noexcept
{
    return m_unique.contains(index);
}

template<FactKind T>
bool FunctionFactSet<T>::contains(View<Index<GroundFunctionTerm<T>>, Repository> view) const noexcept
{
    return contains(view.get_index());
}

template<FactKind T>
float_t FunctionFactSet<T>::operator[](Index<GroundFunctionTerm<T>> index) const noexcept
{
    return m_values[index.get_value()];
}

template<FactKind T>
View<IndexList<GroundFunctionTerm<T>>, Repository> FunctionFactSet<T>::get_fterms() const noexcept
{
    return make_view(m_indices, m_context);
}

template<FactKind T>
const std::vector<float_t>& FunctionFactSet<T>::get_values() const noexcept
{
    return m_values;
}

template class FunctionFactSet<StaticTag>;
template class FunctionFactSet<FluentTag>;

/**
 * FactSets
 */

FactSets::FactSets(View<Index<Program>, Repository> program) :
    static_sets(program.template get_atoms<StaticTag>(), program.template get_fterm_values<StaticTag>()),
    fluent_sets(program.template get_atoms<FluentTag>(), program.template get_fterm_values<FluentTag>())
{
}

FactSets::FactSets(View<Index<Program>, Repository> program, TaggedFactSets<FluentTag> fluent_facts) :
    static_sets(program.template get_atoms<StaticTag>(), program.template get_fterm_values<StaticTag>()),
    fluent_sets(std::move(fluent_facts))
{
}

}
