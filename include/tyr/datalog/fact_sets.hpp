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
    Index<formalism::Predicate<T>> m_predicate;

    const formalism::datalog::Repository& m_context;
    IndexList<formalism::datalog::GroundAtom<T>> m_indices;

    boost::dynamic_bitset<> m_bitset;

    std::vector<std::vector<Index<formalism::Object>>> m_columns;

public:
    explicit PredicateFactSet(View<Index<formalism::Predicate<T>>, formalism::datalog::Repository> predicate);

    void insert(const PredicateFactSet<T>& other) { insert(other.get_facts()); }

    void reset();

    void insert(Index<formalism::datalog::GroundAtom<T>> ground_atom);

    void insert(View<Index<formalism::datalog::GroundAtom<T>>, formalism::datalog::Repository> ground_atom);

    void insert(View<IndexList<formalism::datalog::GroundAtom<T>>, formalism::datalog::Repository> ground_atoms);

    bool contains(Index<formalism::datalog::GroundAtom<T>> ground_atom) const noexcept;

    bool contains(View<Index<formalism::datalog::GroundAtom<T>>, formalism::datalog::Repository> ground_atom) const noexcept;

    View<IndexList<formalism::datalog::GroundAtom<T>>, formalism::datalog::Repository> get_facts() const noexcept;

    const std::vector<Index<formalism::Object>>& get_column(formalism::ParameterIndex parameter) const noexcept;

    const boost::dynamic_bitset<>& get_bitset() const noexcept;
};

template<formalism::FactKind T>
class PredicateFactSets
{
private:
    std::vector<PredicateFactSet<T>> m_sets;

public:
    explicit PredicateFactSets(View<IndexList<formalism::Predicate<T>>, formalism::datalog::Repository> predicates) : m_sets()
    {
        /* Validate inputs. */
        for (uint_t i = 0; i < predicates.size(); ++i)
            assert(uint_t(predicates[i].get_index()) == i);

        /* Initialize sets. */
        for (const auto predicate : predicates)
            m_sets.emplace_back(PredicateFactSet<T>(predicate));
    }

    void insert(const PredicateFactSets<T>& other)
    {
        assert(m_sets.size() == other.m_sets.size());

        for (uint_t i = 0; i < m_sets.size(); ++i)
            m_sets[i].insert(other.m_sets[i]);
    }

    void reset()
    {
        for (auto& set : m_sets)
            set.reset();
    }

    void insert(Index<formalism::datalog::GroundAtom<T>> ground_atom)
    {
        assert(uint_t(ground_atom.group) < m_sets.size());
        m_sets[uint_t(ground_atom.group)].insert(ground_atom);
    }

    void insert(View<Index<formalism::datalog::GroundAtom<T>>, formalism::datalog::Repository> ground_atom) { insert(ground_atom.get_index()); }

    void insert(View<IndexList<formalism::datalog::GroundAtom<T>>, formalism::datalog::Repository> ground_atoms)
    {
        for (const auto ground_atom : ground_atoms)
            insert(ground_atom);
    }

    bool contains(Index<formalism::datalog::GroundAtom<T>> ground_atom) const noexcept { return m_sets[uint_t(ground_atom.get_group())].contains(ground_atom); }

    bool contains(View<Index<formalism::datalog::GroundAtom<T>>, formalism::datalog::Repository> ground_atom) const noexcept
    {
        return contains(ground_atom.get_index());
    }

    const std::vector<PredicateFactSet<T>>& get_sets() const noexcept { return m_sets; }
};

template<formalism::FactKind T>
class FunctionFactSet
{
private:
    Index<formalism::Function<T>> m_function;

    const formalism::datalog::Repository& m_context;

    IndexList<formalism::datalog::GroundFunctionTerm<T>> m_indices;
    UnorderedSet<Index<formalism::datalog::GroundFunctionTerm<T>>> m_unique;

    std::vector<float_t> m_values;

public:
    explicit FunctionFactSet(View<Index<formalism::Function<T>>, formalism::datalog::Repository> function);

    void insert(const FunctionFactSet& other) { insert(other.get_fterms(), other.get_values()); }

    void reset();

    void insert(View<Index<formalism::datalog::GroundFunctionTerm<T>>, formalism::datalog::Repository> function_term, float_t value);

    void insert(View<IndexList<formalism::datalog::GroundFunctionTerm<T>>, formalism::datalog::Repository> function_terms, const std::vector<float_t>& values);

    void insert(View<Index<formalism::datalog::GroundFunctionTermValue<T>>, formalism::datalog::Repository> fterm_value);

    void insert(View<IndexList<formalism::datalog::GroundFunctionTermValue<T>>, formalism::datalog::Repository> fterm_values);

    bool contains(Index<formalism::datalog::GroundFunctionTerm<T>> fterm) const noexcept;

    bool contains(View<Index<formalism::datalog::GroundFunctionTerm<T>>, formalism::datalog::Repository> fterm) const noexcept;

    float_t operator[](Index<formalism::datalog::GroundFunctionTerm<T>> fterm) const noexcept;

    View<IndexList<formalism::datalog::GroundFunctionTerm<T>>, formalism::datalog::Repository> get_fterms() const noexcept;
    const std::vector<float_t>& get_values() const noexcept;
};

template<formalism::FactKind T>
class FunctionFactSets
{
private:
    std::vector<FunctionFactSet<T>> m_sets;

public:
    explicit FunctionFactSets(View<IndexList<formalism::Function<T>>, formalism::datalog::Repository> functions) : m_sets()
    {
        /* Validate inputs. */
        for (uint_t i = 0; i < functions.size(); ++i)
            assert(uint_t(functions[i].get_index()) == i);

        /* Initialize sets. */
        for (const auto function : functions)
            m_sets.emplace_back(FunctionFactSet<T>(function));
    }

    void reset()
    {
        for (auto& set : m_sets)
            set.reset();
    }

    void insert(const FunctionFactSets& other)
    {
        assert(m_sets.size() == other.m_sets.size());

        for (uint_t i = 0; i < m_sets.size(); ++i)
            m_sets[i].insert(other.m_sets[i]);
    }

    void insert(View<Index<formalism::datalog::GroundFunctionTerm<T>>, formalism::datalog::Repository> function_term, float_t value)
    {
        m_sets[uint_t(function_term.get_index().get_group())].insert(function_term, value);
    }

    void insert(View<IndexList<formalism::datalog::GroundFunctionTerm<T>>, formalism::datalog::Repository> function_terms, const std::vector<float_t>& values)
    {
        assert(function_terms.size() == values.size());

        for (size_t i = 0; i < function_terms.size(); ++i)
            insert(function_terms[i], values[i]);
    }

    void insert(View<Index<formalism::datalog::GroundFunctionTermValue<T>>, formalism::datalog::Repository> fterm_value)
    {
        m_sets[uint_t(fterm_value.get_fterm().get_index().get_group())].insert(fterm_value.get_fterm(), fterm_value.get_value());
    }

    void insert(View<IndexList<formalism::datalog::GroundFunctionTermValue<T>>, formalism::datalog::Repository> fterm_values)
    {
        for (const auto fterm_value : fterm_values)
            insert(fterm_value);
    }

    bool contains(Index<formalism::datalog::GroundFunctionTerm<T>> fterm) const noexcept { return m_sets[uint_t(fterm.get_group())].contains(fterm); }

    bool contains(View<Index<formalism::datalog::GroundFunctionTerm<T>>, formalism::datalog::Repository> fterm) const noexcept
    {
        return contains(fterm.get_index());
    }

    float_t operator[](Index<formalism::datalog::GroundFunctionTerm<T>> fterm) const noexcept { return m_sets[uint_t(fterm.get_group())][fterm]; }

    const std::vector<FunctionFactSet<T>>& get_sets() const noexcept { return m_sets; }
};

template<formalism::FactKind T>
struct TaggedFactSets
{
    PredicateFactSets<T> predicate;
    FunctionFactSets<T> function;

    TaggedFactSets(View<IndexList<formalism::Predicate<T>>, formalism::datalog::Repository> predicates,
                   View<IndexList<formalism::Function<T>>, formalism::datalog::Repository> functions) :
        predicate(predicates),
        function(functions)
    {
    }

    TaggedFactSets(View<IndexList<formalism::Predicate<T>>, formalism::datalog::Repository> predicates,
                   View<IndexList<formalism::Function<T>>, formalism::datalog::Repository> functions,
                   View<IndexList<formalism::datalog::GroundAtom<T>>, formalism::datalog::Repository> atoms,
                   View<IndexList<formalism::datalog::GroundFunctionTermValue<T>>, formalism::datalog::Repository> fterm_values) :
        TaggedFactSets(predicates, functions)
    {
        predicate.insert(atoms);
        function.insert(fterm_values);
    }

    void insert(const TaggedFactSets<T>& other)
    {
        predicate.insert(other.predicate);
        function.insert(other.function);
    }

    void reset() noexcept
    {
        predicate.reset();
        function.reset();
    }
};

struct FactSets
{
    const TaggedFactSets<formalism::StaticTag>& static_sets;
    const TaggedFactSets<formalism::FluentTag>& fluent_sets;

    FactSets(const TaggedFactSets<formalism::StaticTag>& static_sets, const TaggedFactSets<formalism::FluentTag>& fluent_sets) noexcept :
        static_sets(static_sets),
        fluent_sets(fluent_sets)
    {
    }

    template<formalism::FactKind T>
    const auto& get() const noexcept
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