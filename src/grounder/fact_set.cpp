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

#include "tyr/grounder/fact_set.hpp"

#include <boost/dynamic_bitset.hpp>
#include <limits>

namespace tyr::grounder
{

/**
 * PredicateFactSet
 */

template<formalism::FactKind T>
PredicateFactSet<T>::PredicateFactSet(View<IndexList<formalism::GroundAtom<T>>, formalism::Repository> view) : m_context(view.get_context()), m_indices()
{
    insert(view);
}

template<formalism::FactKind T>
void PredicateFactSet<T>::reset()
{
    m_indices.clear();
    m_bitset.reset();
}

template<formalism::FactKind T>
void PredicateFactSet<T>::insert(View<Index<formalism::GroundAtom<T>>, formalism::Repository> view)
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

template<formalism::FactKind T>
void PredicateFactSet<T>::insert(View<IndexList<formalism::GroundAtom<T>>, formalism::Repository> view)
{
    for (const auto atom : view)
        insert(atom);
}

template<formalism::FactKind T>
bool PredicateFactSet<T>::contains(Index<formalism::GroundAtom<T>> index) const noexcept
{
    if (index.get_value() >= m_bitset.size())
        return false;
    return m_bitset.test(index.get_value());
}

template<formalism::FactKind T>
View<IndexList<formalism::GroundAtom<T>>, formalism::Repository> PredicateFactSet<T>::get_facts() const noexcept
{
    return View<IndexList<formalism::GroundAtom<T>>, formalism::Repository>(m_indices, m_context);
}

template class PredicateFactSet<formalism::StaticTag>;
template class PredicateFactSet<formalism::FluentTag>;

/**
 * FunctionFactSet
 */

template<formalism::FactKind T>
FunctionFactSet<T>::FunctionFactSet(View<IndexList<formalism::GroundFunctionTermValue<T>>, formalism::Repository> view) :
    m_context(view.get_context()),
    m_indices(),
    m_unique()
{
    insert(view);
}

template<formalism::FactKind T>
void FunctionFactSet<T>::reset()
{
    m_indices.clear();
    m_unique.clear();
    std::fill(m_vector.begin(), m_vector.end(), std::numeric_limits<float_t>::quiet_NaN());
}

template<formalism::FactKind T>
void FunctionFactSet<T>::insert(View<Index<formalism::GroundFunctionTermValue<T>>, formalism::Repository> view)
{
    if (&m_context != &view.get_context())
        throw std::runtime_error("Incompatible contexts.");

    const auto fterm_index = view.get_fterm().get_index();

    if (m_unique.contains(fterm_index))
        throw std::runtime_error("Multiple value assignments to a ground function term.");

    m_indices.push_back(view.get_index());
    m_unique.insert(fterm_index);
    if (fterm_index.get_value() >= m_vector.size())
        m_vector.resize(fterm_index.get_value() + 1, std::numeric_limits<float_t>::quiet_NaN());
    m_vector[fterm_index.get_value()] = view.get_value();
}

template<formalism::FactKind T>
void FunctionFactSet<T>::insert(View<IndexList<formalism::GroundFunctionTermValue<T>>, formalism::Repository> view)
{
    for (const auto fterm_value : view)
        insert(fterm_value);
}

template<formalism::FactKind T>
bool FunctionFactSet<T>::contains(Index<formalism::GroundFunctionTerm<T>> index) const noexcept
{
    return m_unique.contains(index);
}

template<formalism::FactKind T>
float_t FunctionFactSet<T>::operator[](Index<formalism::GroundFunctionTerm<T>> index) const noexcept
{
    return m_vector[index.get_value()];
}

template<formalism::FactKind T>
View<IndexList<formalism::GroundFunctionTermValue<T>>, formalism::Repository> FunctionFactSet<T>::get_facts() const noexcept
{
    return View<IndexList<formalism::GroundFunctionTermValue<T>>, formalism::Repository>(m_indices, m_context);
}

template class FunctionFactSet<formalism::StaticTag>;
template class FunctionFactSet<formalism::FluentTag>;

/**
 * TaggedFactSets
 */

template<formalism::FactKind T>
TaggedFactSets<T>::TaggedFactSets(View<IndexList<formalism::GroundAtom<T>>, formalism::Repository> atoms,
                                  View<IndexList<formalism::GroundFunctionTermValue<T>>, formalism::Repository> function_terms) :
    predicate(atoms),
    function(function_terms)
{
}

template<formalism::FactKind T>
void TaggedFactSets<T>::reset() noexcept
{
    predicate.reset();
    function.reset();
}

template class TaggedFactSets<formalism::StaticTag>;
template class TaggedFactSets<formalism::FluentTag>;

/**
 * FactSets
 */

FactSets::FactSets(View<Index<formalism::Program>, formalism::Repository> program) :
    static_sets(program.template get_atoms<formalism::StaticTag>(), program.template get_fterm_values<formalism::StaticTag>()),
    fluent_sets(program.template get_atoms<formalism::FluentTag>(), program.template get_fterm_values<formalism::FluentTag>())
{
}

FactSets::FactSets(View<Index<formalism::Program>, formalism::Repository> program, TaggedFactSets<formalism::FluentTag> fluent_facts) :
    static_sets(program.template get_atoms<formalism::StaticTag>(), program.template get_fterm_values<formalism::StaticTag>()),
    fluent_sets(std::move(fluent_facts))
{
}

template<formalism::FactKind T>
void FactSets::reset() noexcept
{
    get<T>().template reset();
}

template void FactSets::reset<formalism::StaticTag>() noexcept;
template void FactSets::reset<formalism::FluentTag>() noexcept;

void FactSets::reset() noexcept
{
    reset<formalism::StaticTag>();
    reset<formalism::FluentTag>();
}

template<formalism::FactKind T>
void FactSets::insert(View<IndexList<formalism::GroundAtom<T>>, formalism::Repository> view)
{
    get<T>().predicate.insert(view);
}

template void FactSets::insert<formalism::StaticTag>(View<IndexList<formalism::GroundAtom<formalism::StaticTag>>, formalism::Repository> view);
template void FactSets::insert<formalism::FluentTag>(View<IndexList<formalism::GroundAtom<formalism::FluentTag>>, formalism::Repository> view);

template<formalism::FactKind T>
void FactSets::insert(View<IndexList<formalism::GroundFunctionTermValue<T>>, formalism::Repository> view)
{
    get<T>().function.insert(view);
}

template void FactSets::insert<formalism::StaticTag>(View<IndexList<formalism::GroundFunctionTermValue<formalism::StaticTag>>, formalism::Repository> view);
template void FactSets::insert<formalism::FluentTag>(View<IndexList<formalism::GroundFunctionTermValue<formalism::FluentTag>>, formalism::Repository> view);

template<formalism::FactKind T>
const TaggedFactSets<T>& FactSets::get() const
{
    if constexpr (std::is_same_v<T, formalism::StaticTag>)
        return static_sets;
    else if constexpr (std::is_same_v<T, formalism::FluentTag>)
        return fluent_sets;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template const TaggedFactSets<formalism::StaticTag>& FactSets::get<formalism::StaticTag>() const;
template const TaggedFactSets<formalism::FluentTag>& FactSets::get<formalism::FluentTag>() const;

template<formalism::FactKind T>
TaggedFactSets<T>& FactSets::get()
{
    if constexpr (std::is_same_v<T, formalism::StaticTag>)
        return static_sets;
    else if constexpr (std::is_same_v<T, formalism::FluentTag>)
        return fluent_sets;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template TaggedFactSets<formalism::StaticTag>& FactSets::get<formalism::StaticTag>();
template TaggedFactSets<formalism::FluentTag>& FactSets::get<formalism::FluentTag>();

}
