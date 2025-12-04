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

#ifndef TYR_GROUNDER_FACT_SET_HPP_
#define TYR_GROUNDER_FACT_SET_HPP_

#include "tyr/formalism/views.hpp"

#include <boost/dynamic_bitset.hpp>
#include <limits>

namespace tyr::grounder
{

template<formalism::FactKind T, formalism::Context C>
class PredicateFactSet
{
private:
    const C& m_context;
    IndexList<formalism::GroundAtom<T>> m_indices;

    boost::dynamic_bitset<> m_bitset;

public:
    explicit PredicateFactSet(View<IndexList<formalism::GroundAtom<T>>, C> view) : m_context(view.get_context()), m_indices() { insert(view); }

    void reset()
    {
        m_indices.clear();
        m_bitset.reset();
    }

    void insert(View<Index<formalism::GroundAtom<T>>, C> view)
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

    void insert(View<IndexList<formalism::GroundAtom<T>>, C> view)
    {
        for (const auto atom : view)
            insert(atom);
    }

    bool contains(Index<formalism::GroundAtom<T>> index) const noexcept { return m_bitset.test(index.get_value()); }

    auto get_facts() const noexcept { return View<IndexList<formalism::GroundAtom<T>>, C>(m_indices, m_context); }
};

template<formalism::FactKind T, formalism::Context C>
class FunctionFactSet
{
private:
    const C& m_context;
    IndexList<formalism::GroundFunctionTermValue<T>> m_indices;
    UnorderedSet<Index<formalism::GroundFunctionTerm<T>>> m_unique;

    std::vector<float_t> m_vector;

public:
    explicit FunctionFactSet(View<IndexList<formalism::GroundFunctionTermValue<T>>, C> view) : m_context(view.get_context()), m_indices(), m_unique()
    {
        insert(view);
    }

    void reset()
    {
        m_indices.clear();
        m_unique.clear();
        std::fill(m_vector.begin(), m_vector.end(), std::numeric_limits<float_t>::quiet_NaN());
    }

    void insert(View<Index<formalism::GroundFunctionTermValue<T>>, C> view)
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

    void insert(View<IndexList<formalism::GroundFunctionTermValue<T>>, C> view)
    {
        for (const auto fterm_value : view)
            insert(fterm_value);
    }

    bool contains(Index<formalism::GroundFunctionTerm<T>> index) const noexcept { return m_unique.contains(index); }

    float_t operator[](Index<formalism::GroundFunctionTerm<T>> index) const noexcept { return m_vector[index.get_value()]; }

    auto get_facts() const noexcept { return View<IndexList<formalism::GroundFunctionTermValue<T>>, C>(m_indices, m_context); }
};

template<formalism::FactKind T, formalism::Context C>
struct TaggedFactSets
{
    PredicateFactSet<T, C> predicate;
    FunctionFactSet<T, C> function;

    TaggedFactSets(View<IndexList<formalism::GroundAtom<T>>, C> atoms, View<IndexList<formalism::GroundFunctionTermValue<T>>, C> function_terms) :
        predicate(atoms),
        function(function_terms)
    {
    }

    void reset() noexcept
    {
        predicate.reset();
        function.reset();
    }
};

template<formalism::Context C>
struct FactSets
{
    TaggedFactSets<formalism::StaticTag, C> static_sets;
    TaggedFactSets<formalism::FluentTag, C> fluent_sets;

    explicit FactSets(View<Index<formalism::Program>, C> program) :
        static_sets(program.template get_atoms<formalism::StaticTag>(), program.template get_fterm_values<formalism::StaticTag>()),
        fluent_sets(program.template get_atoms<formalism::FluentTag>(), program.template get_fterm_values<formalism::FluentTag>())
    {
    }

    FactSets(View<Index<formalism::Program>, C> program, TaggedFactSets<formalism::FluentTag, C> fluent_facts) :
        static_sets(program.template get_atoms<formalism::StaticTag>(), program.template get_fterm_values<formalism::StaticTag>()),
        fluent_sets(std::move(fluent_facts))
    {
    }

    template<formalism::FactKind T>
    void reset() noexcept
    {
        get<T>().template reset();
    }

    void reset() noexcept
    {
        reset<formalism::StaticTag>();
        reset<formalism::FluentTag>();
    }

    template<formalism::FactKind T>
    void insert(View<IndexList<formalism::GroundAtom<T>>, C> view)
    {
        get<T>().predicate.insert(view);
    }

    template<formalism::FactKind T>
    void insert(View<IndexList<formalism::GroundFunctionTermValue<T>>, C> view)
    {
        get<T>().function.insert(view);
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

    template<formalism::FactKind T>
    auto& get()
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