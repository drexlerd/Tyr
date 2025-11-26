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

#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_atom_proxy.hpp"
#include "tyr/formalism/ground_function_term_value_proxy.hpp"

#include <boost/dynamic_bitset.hpp>
#include <limits>

namespace tyr::grounder
{

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
class PredicateFactSet
{
private:
    Proxy<IndexList<formalism::GroundAtom<T>>, C> m_view;

    using BitsetType = std::conditional_t<IsGroupType<formalism::GroundAtom<T>>, GroupDynamicBitset, FlatDynamicBitset>;

    BitsetType m_bitset;

public:
    explicit PredicateFactSet(Proxy<IndexList<formalism::GroundAtom<T>>, C> view) : m_view(view) { initialize(view); }

    void initialize(Proxy<IndexList<formalism::GroundAtom<T>>, C> view)
    {
        //        m_view = view;
        //        m_bitset.reset();
        //
        //        for (const auto atom : m_view)
        //        {
        //            m_bitset.resize_to_fit(atom.get_index());
        //
        //            m_bitset.set(atom.get_index());
        //        }
    }

    bool contains(Index<formalism::GroundAtom<T>> index) const noexcept { return m_bitset.test(index); }

    auto get_facts() const noexcept { return m_view; }
};

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
class FunctionFactSet
{
private:
    Proxy<IndexList<formalism::GroundFunctionTermValue<T>>, C> m_view;

    using VectorType = std::conditional_t<IsGroupType<formalism::GroundFunctionTermValue<T>>, GroupVector<float_t>, FlatVector<float_t>>;

    VectorType m_vector;

public:
    explicit FunctionFactSet(Proxy<IndexList<formalism::GroundFunctionTermValue<T>>, C> view) : m_view(view) { initialize(view); }

    void initialize(Proxy<IndexList<formalism::GroundFunctionTermValue<T>>, C> view)
    {
        //         m_view = view;
        //         m_vector.reset(std::numeric_limits<float_t>::quiet_NaN());
        //
        //         for (const auto function_value : m_view)
        //         {
        //             m_vector.resize_to_fit(function_value.get_function().get_index());
        //
        //             m_vector[function_value.get_function().get_index()] = function_value.get_value();
        //         }
    }

    float_t at(Index<formalism::GroundFunctionTerm<T>> index) const noexcept { return m_vector.at(index); }
    float_t operator[](Index<formalism::GroundFunctionTerm<T>> index) const noexcept { return m_vector[index]; }

    auto get_facts() const noexcept { return m_view; }
};

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
struct TaggedFactSets
{
    PredicateFactSet<T, C> predicate;
    FunctionFactSet<T, C> function;

    TaggedFactSets(Proxy<IndexList<formalism::GroundAtom<T>>, C> atoms, Proxy<IndexList<formalism::GroundFunctionTermValue<T>>, C> function_terms) :
        predicate(atoms),
        function(function_terms)
    {
    }
};

template<formalism::IsContext C>
struct FactSets
{
    TaggedFactSets<formalism::StaticTag, C> static_sets;
    TaggedFactSets<formalism::FluentTag, C> fluent_sets;

    // Convenience constructor
    explicit FactSets(Proxy<Index<formalism::Program>, C> program) :
        static_sets(program.template get_atoms<formalism::StaticTag>(), program.template get_function_values<formalism::StaticTag>()),
        fluent_sets(program.template get_atoms<formalism::FluentTag>(), program.template get_function_values<formalism::FluentTag>())
    {
    }
};
}

#endif