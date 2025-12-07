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

#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/grounder/fact_sets.hpp"

#include <boost/dynamic_bitset.hpp>
#include <limits>

namespace tyr::grounder
{

/**
 * PredicateFactSet
 */

template<formalism::FactKind T, formalism::Context C>
PredicateFactSet<T, C>::PredicateFactSet(View<IndexList<formalism::GroundAtom<T>>, C> view) : m_context(view.get_context()), m_indices()
{
    insert(view);
}

template<formalism::FactKind T, formalism::Context C>
void PredicateFactSet<T, C>::reset()
{
    m_indices.clear();
    m_bitset.reset();
}

template<formalism::FactKind T, formalism::Context C>
void PredicateFactSet<T, C>::insert(View<Index<formalism::GroundAtom<T>>, C> view)
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

template<formalism::FactKind T, formalism::Context C>
void PredicateFactSet<T, C>::insert(View<IndexList<formalism::GroundAtom<T>>, C> view)
{
    for (const auto atom : view)
        insert(atom);
}

template<formalism::FactKind T, formalism::Context C>
bool PredicateFactSet<T, C>::contains(Index<formalism::GroundAtom<T>> index) const noexcept
{
    if (index.get_value() >= m_bitset.size())
        return false;
    return m_bitset.test(index.get_value());
}

template<formalism::FactKind T, formalism::Context C>
bool PredicateFactSet<T, C>::contains(View<Index<formalism::GroundAtom<T>>, C> view) const noexcept
{
    return contains(view.get_index());
}

template<formalism::FactKind T, formalism::Context C>
View<IndexList<formalism::GroundAtom<T>>, C> PredicateFactSet<T, C>::get_facts() const noexcept
{
    return View<IndexList<formalism::GroundAtom<T>>, C>(m_indices, m_context);
}

template<formalism::FactKind T, formalism::Context C>
const boost::dynamic_bitset<>& PredicateFactSet<T, C>::get_bitset() const noexcept
{
    return m_bitset;
}

template class PredicateFactSet<formalism::StaticTag, formalism::Repository>;
template class PredicateFactSet<formalism::FluentTag, formalism::Repository>;
template class PredicateFactSet<formalism::StaticTag, formalism::OverlayRepository<formalism::Repository>>;
template class PredicateFactSet<formalism::FluentTag, formalism::OverlayRepository<formalism::Repository>>;

/**
 * FunctionFactSet
 */

template<formalism::FactKind T, formalism::Context C>
FunctionFactSet<T, C>::FunctionFactSet(View<IndexList<formalism::GroundFunctionTermValue<T>>, C> view) : m_context(view.get_context()), m_indices(), m_unique()
{
    insert(view);
}

template<formalism::FactKind T, formalism::Context C>
void FunctionFactSet<T, C>::reset()
{
    m_indices.clear();
    m_unique.clear();
    std::fill(m_values.begin(), m_values.end(), std::numeric_limits<float_t>::quiet_NaN());
}

template<formalism::FactKind T, formalism::Context C>
void FunctionFactSet<T, C>::insert(View<Index<formalism::GroundFunctionTerm<T>>, C> function_term, float_t value)
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

template<formalism::FactKind T, formalism::Context C>
void FunctionFactSet<T, C>::insert(View<IndexList<formalism::GroundFunctionTerm<T>>, C> function_terms, const std::vector<float_t>& values)
{
    assert(function_terms.size() == values.size());

    for (uint_t i = 0; i < function_terms.size(); ++i)
        insert(function_terms[i], values[i]);
}

template<formalism::FactKind T, formalism::Context C>
void FunctionFactSet<T, C>::insert(View<Index<formalism::GroundFunctionTermValue<T>>, C> view)
{
    insert(view.get_fterm(), view.get_value());
}

template<formalism::FactKind T, formalism::Context C>
void FunctionFactSet<T, C>::insert(View<IndexList<formalism::GroundFunctionTermValue<T>>, C> view)
{
    for (const auto fterm_value : view)
        insert(fterm_value);
}

template<formalism::FactKind T, formalism::Context C>
bool FunctionFactSet<T, C>::contains(Index<formalism::GroundFunctionTerm<T>> index) const noexcept
{
    return m_unique.contains(index);
}

template<formalism::FactKind T, formalism::Context C>
bool FunctionFactSet<T, C>::contains(View<Index<formalism::GroundFunctionTerm<T>>, C> view) const noexcept
{
    return contains(view.get_index());
}

template<formalism::FactKind T, formalism::Context C>
float_t FunctionFactSet<T, C>::operator[](Index<formalism::GroundFunctionTerm<T>> index) const noexcept
{
    return m_values[index.get_value()];
}

template<formalism::FactKind T, formalism::Context C>
View<IndexList<formalism::GroundFunctionTerm<T>>, C> FunctionFactSet<T, C>::get_fterms() const noexcept
{
    return View<IndexList<formalism::GroundFunctionTerm<T>>, C>(m_indices, m_context);
}

template<formalism::FactKind T, formalism::Context C>
const std::vector<float_t>& FunctionFactSet<T, C>::get_values() const noexcept
{
    return m_values;
}

template class FunctionFactSet<formalism::StaticTag, formalism::Repository>;
template class FunctionFactSet<formalism::FluentTag, formalism::Repository>;
template class FunctionFactSet<formalism::StaticTag, formalism::OverlayRepository<formalism::Repository>>;
template class FunctionFactSet<formalism::FluentTag, formalism::OverlayRepository<formalism::Repository>>;

/**
 * TaggedFactSets
 */

template<formalism::FactKind T, formalism::Context C>
TaggedFactSets<T, C>::TaggedFactSets(View<IndexList<formalism::GroundAtom<T>>, C> atoms,
                                     View<IndexList<formalism::GroundFunctionTermValue<T>>, C> function_terms) :
    predicate(atoms),
    function(function_terms)
{
}

template<formalism::FactKind T, formalism::Context C>
void TaggedFactSets<T, C>::reset() noexcept
{
    predicate.reset();
    function.reset();
}

template class TaggedFactSets<formalism::StaticTag, formalism::Repository>;
template class TaggedFactSets<formalism::FluentTag, formalism::Repository>;
template class TaggedFactSets<formalism::StaticTag, formalism::OverlayRepository<formalism::Repository>>;
template class TaggedFactSets<formalism::FluentTag, formalism::OverlayRepository<formalism::Repository>>;

/**
 * FactSets
 */

template<formalism::Context C>
FactSets<C>::FactSets(View<Index<formalism::Program>, C> program) :
    static_sets(program.template get_atoms<formalism::StaticTag>(), program.template get_fterm_values<formalism::StaticTag>()),
    fluent_sets(program.template get_atoms<formalism::FluentTag>(), program.template get_fterm_values<formalism::FluentTag>())
{
}

template<formalism::Context C>
FactSets<C>::FactSets(View<Index<formalism::Program>, C> program, TaggedFactSets<formalism::FluentTag, C> fluent_facts) :
    static_sets(program.template get_atoms<formalism::StaticTag>(), program.template get_fterm_values<formalism::StaticTag>()),
    fluent_sets(std::move(fluent_facts))
{
}

template<formalism::Context C>
template<formalism::FactKind T>
void FactSets<C>::reset() noexcept
{
    get<T>().template reset();
}

template void FactSets<formalism::Repository>::reset<formalism::StaticTag>() noexcept;
template void FactSets<formalism::Repository>::reset<formalism::FluentTag>() noexcept;
template void FactSets<formalism::OverlayRepository<formalism::Repository>>::reset<formalism::StaticTag>() noexcept;
template void FactSets<formalism::OverlayRepository<formalism::Repository>>::reset<formalism::FluentTag>() noexcept;

template<formalism::Context C>
void FactSets<C>::reset() noexcept
{
    reset<formalism::StaticTag>();
    reset<formalism::FluentTag>();
}

template<formalism::Context C>
template<formalism::FactKind T>
void FactSets<C>::insert(View<IndexList<formalism::GroundAtom<T>>, C> view)
{
    get<T>().predicate.insert(view);
}

template void FactSets<formalism::Repository>::insert(View<IndexList<formalism::GroundAtom<formalism::StaticTag>>, formalism::Repository> view);
template void FactSets<formalism::Repository>::insert(View<IndexList<formalism::GroundAtom<formalism::FluentTag>>, formalism::Repository> view);
template void FactSets<formalism::OverlayRepository<formalism::Repository>>::insert(
    View<IndexList<formalism::GroundAtom<formalism::StaticTag>>, formalism::OverlayRepository<formalism::Repository>> view);
template void FactSets<formalism::OverlayRepository<formalism::Repository>>::insert(
    View<IndexList<formalism::GroundAtom<formalism::FluentTag>>, formalism::OverlayRepository<formalism::Repository>> view);

template<formalism::Context C>
template<formalism::FactKind T>
void FactSets<C>::insert(View<IndexList<formalism::GroundFunctionTermValue<T>>, C> view)
{
    get<T>().function.insert(view);
}

template void FactSets<formalism::Repository>::insert(View<IndexList<formalism::GroundFunctionTermValue<formalism::StaticTag>>, formalism::Repository> view);
template void FactSets<formalism::Repository>::insert(View<IndexList<formalism::GroundFunctionTermValue<formalism::FluentTag>>, formalism::Repository> view);
template void FactSets<formalism::OverlayRepository<formalism::Repository>>::insert(
    View<IndexList<formalism::GroundFunctionTermValue<formalism::StaticTag>>, formalism::OverlayRepository<formalism::Repository>> view);
template void FactSets<formalism::OverlayRepository<formalism::Repository>>::insert(
    View<IndexList<formalism::GroundFunctionTermValue<formalism::FluentTag>>, formalism::OverlayRepository<formalism::Repository>> view);

template<formalism::Context C>
template<formalism::FactKind T>
const TaggedFactSets<T, C>& FactSets<C>::get() const
{
    if constexpr (std::is_same_v<T, formalism::StaticTag>)
        return static_sets;
    else if constexpr (std::is_same_v<T, formalism::FluentTag>)
        return fluent_sets;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template const TaggedFactSets<formalism::StaticTag, formalism::Repository>& FactSets<formalism::Repository>::get<formalism::StaticTag>() const;
template const TaggedFactSets<formalism::FluentTag, formalism::Repository>& FactSets<formalism::Repository>::get<formalism::FluentTag>() const;
template const TaggedFactSets<formalism::StaticTag, formalism::OverlayRepository<formalism::Repository>>&
FactSets<formalism::OverlayRepository<formalism::Repository>>::get<formalism::StaticTag>() const;
template const TaggedFactSets<formalism::FluentTag, formalism::OverlayRepository<formalism::Repository>>&
FactSets<formalism::OverlayRepository<formalism::Repository>>::get<formalism::FluentTag>() const;

template<formalism::Context C>
template<formalism::FactKind T>
TaggedFactSets<T, C>& FactSets<C>::get()
{
    if constexpr (std::is_same_v<T, formalism::StaticTag>)
        return static_sets;
    else if constexpr (std::is_same_v<T, formalism::FluentTag>)
        return fluent_sets;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template TaggedFactSets<formalism::StaticTag, formalism::Repository>& FactSets<formalism::Repository>::get<formalism::StaticTag>();
template TaggedFactSets<formalism::FluentTag, formalism::Repository>& FactSets<formalism::Repository>::get<formalism::FluentTag>();
template TaggedFactSets<formalism::StaticTag, formalism::OverlayRepository<formalism::Repository>>&
FactSets<formalism::OverlayRepository<formalism::Repository>>::get<formalism::StaticTag>();
template TaggedFactSets<formalism::FluentTag, formalism::OverlayRepository<formalism::Repository>>&
FactSets<formalism::OverlayRepository<formalism::Repository>>::get<formalism::FluentTag>();

}
