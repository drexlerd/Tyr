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

#include "tyr/formalism/formalism.hpp"

#include <boost/dynamic_bitset.hpp>

namespace tyr::grounder
{

template<formalism::FactKind T>
class PredicateFactSet
{
private:
    const formalism::Repository& m_context;
    IndexList<formalism::GroundAtom<T>> m_indices;

    boost::dynamic_bitset<> m_bitset;

public:
    explicit PredicateFactSet(View<IndexList<formalism::GroundAtom<T>>, formalism::Repository> view);

    void reset();

    void insert(View<Index<formalism::GroundAtom<T>>, formalism::Repository> view);

    void insert(View<IndexList<formalism::GroundAtom<T>>, formalism::Repository> view);

    bool contains(Index<formalism::GroundAtom<T>> index) const noexcept;

    bool contains(View<Index<formalism::GroundAtom<T>>, formalism::Repository> view) const noexcept;

    View<IndexList<formalism::GroundAtom<T>>, formalism::Repository> get_facts() const noexcept;
};

template<formalism::FactKind T>
class FunctionFactSet
{
private:
    const formalism::Repository& m_context;
    IndexList<formalism::GroundFunctionTerm<T>> m_indices;
    UnorderedSet<Index<formalism::GroundFunctionTerm<T>>> m_unique;

    std::vector<float_t> m_vector;

public:
    explicit FunctionFactSet(View<IndexList<formalism::GroundFunctionTermValue<T>>, formalism::Repository> view);

    void reset();

    void insert(View<Index<formalism::GroundFunctionTerm<T>>, formalism::Repository> function_term, float_t value);

    void insert(View<IndexList<formalism::GroundFunctionTerm<T>>, formalism::Repository> function_terms, const std::vector<float_t>& values);

    void insert(View<Index<formalism::GroundFunctionTermValue<T>>, formalism::Repository> view);

    void insert(View<IndexList<formalism::GroundFunctionTermValue<T>>, formalism::Repository> view);

    bool contains(Index<formalism::GroundFunctionTerm<T>> index) const noexcept;

    bool contains(View<Index<formalism::GroundFunctionTerm<T>>, formalism::Repository> view) const noexcept;

    float_t operator[](Index<formalism::GroundFunctionTerm<T>> index) const noexcept;

    View<IndexList<formalism::GroundFunctionTerm<T>>, formalism::Repository> get_fterms() const noexcept;
    const std::vector<float_t>& get_values() const noexcept;
};

template<formalism::FactKind T>
struct TaggedFactSets
{
    PredicateFactSet<T> predicate;
    FunctionFactSet<T> function;

    TaggedFactSets(View<IndexList<formalism::GroundAtom<T>>, formalism::Repository> atoms,
                   View<IndexList<formalism::GroundFunctionTermValue<T>>, formalism::Repository> function_terms);

    void reset() noexcept;
};

struct FactSets
{
    TaggedFactSets<formalism::StaticTag> static_sets;
    TaggedFactSets<formalism::FluentTag> fluent_sets;

    explicit FactSets(View<Index<formalism::Program>, formalism::Repository> program);

    FactSets(View<Index<formalism::Program>, formalism::Repository> program, TaggedFactSets<formalism::FluentTag> fluent_facts);

    template<formalism::FactKind T>
    void reset() noexcept;

    void reset() noexcept;

    template<formalism::FactKind T>
    void insert(View<IndexList<formalism::GroundAtom<T>>, formalism::Repository> view);

    template<formalism::FactKind T>
    void insert(View<IndexList<formalism::GroundFunctionTermValue<T>>, formalism::Repository> view);

    template<formalism::FactKind T>
    const TaggedFactSets<T>& get() const;

    template<formalism::FactKind T>
    TaggedFactSets<T>& get();
};
}

#endif