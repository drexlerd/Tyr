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

#ifndef TYR_DATALOG_FACT_SETS_HPP_
#define TYR_DATALOG_FACT_SETS_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/formalism/datalog/views.hpp"

#include <boost/dynamic_bitset.hpp>

namespace tyr::datalog
{

template<formalism::FactKind T>
class PredicateFactSet
{
private:
    const formalism::datalog::Repository& m_context;
    IndexList<formalism::datalog::GroundAtom<T>> m_indices;

    boost::dynamic_bitset<> m_bitset;

public:
    explicit PredicateFactSet(View<IndexList<formalism::datalog::GroundAtom<T>>, formalism::datalog::Repository> view);

    void reset();

    void insert(View<Index<formalism::datalog::GroundAtom<T>>, formalism::datalog::Repository> view);

    void insert(View<IndexList<formalism::datalog::GroundAtom<T>>, formalism::datalog::Repository> view);

    bool contains(Index<formalism::datalog::GroundAtom<T>> index) const noexcept;

    bool contains(View<Index<formalism::datalog::GroundAtom<T>>, formalism::datalog::Repository> view) const noexcept;

    View<IndexList<formalism::datalog::GroundAtom<T>>, formalism::datalog::Repository> get_facts() const noexcept;

    const boost::dynamic_bitset<>& get_bitset() const noexcept;
};

template<formalism::FactKind T>
class FunctionFactSet
{
private:
    const formalism::datalog::Repository& m_context;
    IndexList<formalism::datalog::GroundFunctionTerm<T>> m_indices;
    UnorderedSet<Index<formalism::datalog::GroundFunctionTerm<T>>> m_unique;

    std::vector<float_t> m_values;

public:
    explicit FunctionFactSet(View<IndexList<formalism::datalog::GroundFunctionTermValue<T>>, formalism::datalog::Repository> view);

    void reset();

    void insert(View<Index<formalism::datalog::GroundFunctionTerm<T>>, formalism::datalog::Repository> function_term, float_t value);

    void insert(View<IndexList<formalism::datalog::GroundFunctionTerm<T>>, formalism::datalog::Repository> function_terms, const std::vector<float_t>& values);

    void insert(View<Index<formalism::datalog::GroundFunctionTermValue<T>>, formalism::datalog::Repository> view);

    void insert(View<IndexList<formalism::datalog::GroundFunctionTermValue<T>>, formalism::datalog::Repository> view);

    bool contains(Index<formalism::datalog::GroundFunctionTerm<T>> index) const noexcept;

    bool contains(View<Index<formalism::datalog::GroundFunctionTerm<T>>, formalism::datalog::Repository> view) const noexcept;

    float_t operator[](Index<formalism::datalog::GroundFunctionTerm<T>> index) const noexcept;

    View<IndexList<formalism::datalog::GroundFunctionTerm<T>>, formalism::datalog::Repository> get_fterms() const noexcept;
    const std::vector<float_t>& get_values() const noexcept;
};

template<formalism::FactKind T>
struct TaggedFactSets
{
    PredicateFactSet<T> predicate;
    FunctionFactSet<T> function;

    TaggedFactSets(View<IndexList<formalism::datalog::GroundAtom<T>>, formalism::datalog::Repository> atoms,
                   View<IndexList<formalism::datalog::GroundFunctionTermValue<T>>, formalism::datalog::Repository> function_terms) :
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

struct FactSets
{
    TaggedFactSets<formalism::StaticTag> static_sets;
    TaggedFactSets<formalism::FluentTag> fluent_sets;

    explicit FactSets(View<Index<formalism::datalog::Program>, formalism::datalog::Repository> program);

    FactSets(View<Index<formalism::datalog::Program>, formalism::datalog::Repository> program, TaggedFactSets<formalism::FluentTag> fluent_facts);

    template<formalism::FactKind T>
    void reset() noexcept
    {
        get<T>().reset();
    }

    void reset() noexcept
    {
        reset<formalism::StaticTag>();
        reset<formalism::FluentTag>();
    }

    template<formalism::FactKind T>
    void insert(View<IndexList<formalism::datalog::GroundAtom<T>>, formalism::datalog::Repository> view)
    {
        get<T>().predicate.insert(view);
    }

    template<formalism::FactKind T>
    void insert(View<IndexList<formalism::datalog::GroundFunctionTermValue<T>>, formalism::datalog::Repository> view)
    {
        get<T>().function.insert(view);
    }

    template<formalism::FactKind T>
    const TaggedFactSets<T>& get() const
    {
        if constexpr (std::is_same_v<T, formalism::StaticTag>)
            return static_sets;
        else if constexpr (std::is_same_v<T, formalism::FluentTag>)
            return fluent_sets;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }

    template<formalism::FactKind T>
    TaggedFactSets<T>& get()
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