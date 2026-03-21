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
#include "tyr/formalism/datalog/repository.hpp"

#include <boost/dynamic_bitset.hpp>
#include <iostream>

namespace tyr::datalog
{

template<formalism::FactKind T>
class PredicateFactSet
{
private:
    formalism::datalog::PredicateView<T> m_predicate;

    Index<formalism::Predicate<T>> m_predicate_index;
    std::vector<Index<formalism::Binding>> m_bindings;

    boost::dynamic_bitset<> m_bitset;

public:
    explicit PredicateFactSet(formalism::datalog::PredicateView<T> predicate);

    auto get_predicate() const noexcept { return m_predicate; }

    void reset();

    void insert(const PredicateFactSet<T>& other);
    void insert(formalism::datalog::GroundAtomView<T> ground_atom);
    void insert(formalism::datalog::PredicateBindingView<T> binding);
    void insert(formalism::datalog::PredicateBindingVecView<T> bindings);
    void insert(const std::vector<formalism::datalog::PredicateBindingView<T>>& bindings);

    bool contains(formalism::datalog::PredicateBindingView<T> binding) const noexcept;

    formalism::datalog::PredicateBindingVecView<T> get_bindings() const noexcept;
};

template<formalism::FactKind T>
class PredicateFactSets
{
private:
    std::vector<PredicateFactSet<T>> m_sets;

public:
    explicit PredicateFactSets(formalism::datalog::PredicateListView<T> predicates);

    void reset();

    void insert(const PredicateFactSets<T>& other);
    void insert(formalism::datalog::GroundAtomView<T> ground_atom);
    void insert(formalism::datalog::PredicateBindingView<T> binding);
    void insert(formalism::datalog::PredicateBindingVecView<T> bindings);

    bool contains(formalism::datalog::PredicateBindingView<T> binding) const noexcept;

    const std::vector<PredicateFactSet<T>>& get_sets() const noexcept;
};

template<formalism::FactKind T>
class FunctionFactSet
{
private:
    formalism::datalog::FunctionView<T> m_function;

    std::vector<uint_t> m_remap;
    std::vector<formalism::datalog::FunctionBindingView<T>> m_bindings;
    std::vector<float_t> m_values;

public:
    explicit FunctionFactSet(formalism::datalog::FunctionView<T> function);

    void insert(const FunctionFactSet& other) { insert(other.get_bindings(), other.get_values()); }

    void reset();

    void insert(formalism::datalog::FunctionBindingView<T> binding, float_t value);

    void insert(formalism::datalog::GroundFunctionTermView<T> fterm, float_t value);

    void insert(const std::vector<formalism::datalog::FunctionBindingView<T>>& bindings, const std::vector<float_t>& values);

    void insert(formalism::datalog::GroundFunctionTermValueView<T> fterm_value);

    void insert(formalism::datalog::GroundFunctionTermValueListView<T> fterm_values);

    float_t operator[](formalism::datalog::GroundFunctionTermView<T> fterm) const noexcept;

    const std::vector<uint_t>& get_remap() const noexcept;
    const std::vector<formalism::datalog::FunctionBindingView<T>>& get_bindings() const noexcept;
    const std::vector<float_t>& get_values() const noexcept;
};

template<formalism::FactKind T>
class FunctionFactSets
{
private:
    std::vector<FunctionFactSet<T>> m_sets;

public:
    explicit FunctionFactSets(formalism::datalog::FunctionListView<T> functions) : m_sets()
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

    void insert(formalism::datalog::GroundFunctionTermView<T> function_term, float_t value)
    {
        m_sets[uint_t(function_term.get_function().get_index())].insert(function_term, value);
    }

    void insert(formalism::datalog::GroundFunctionTermListView<T> function_terms, const std::vector<float_t>& values)
    {
        assert(function_terms.size() == values.size());

        for (size_t i = 0; i < function_terms.size(); ++i)
            insert(function_terms[i], values[i]);
    }

    void insert(formalism::datalog::GroundFunctionTermValueView<T> fterm_value)
    {
        m_sets[uint_t(fterm_value.get_fterm().get_function().get_index())].insert(fterm_value.get_fterm(), fterm_value.get_value());
    }

    void insert(formalism::datalog::GroundFunctionTermValueListView<T> fterm_values)
    {
        for (const auto fterm_value : fterm_values)
            insert(fterm_value);
    }

    float_t operator[](formalism::datalog::GroundFunctionTermView<T> fterm) const noexcept { return m_sets[uint_t(fterm.get_function().get_index())][fterm]; }

    const std::vector<FunctionFactSet<T>>& get_sets() const noexcept { return m_sets; }
};

template<formalism::FactKind T>
struct TaggedFactSets
{
    PredicateFactSets<T> predicate;
    FunctionFactSets<T> function;

    TaggedFactSets(formalism::datalog::PredicateListView<T> predicates, formalism::datalog::FunctionListView<T> functions) :
        predicate(predicates),
        function(functions)
    {
    }

    TaggedFactSets(formalism::datalog::PredicateListView<T> predicates,
                   formalism::datalog::FunctionListView<T> functions,
                   formalism::datalog::GroundAtomListView<T> atoms,
                   formalism::datalog::GroundFunctionTermValueListView<T> fterm_values) :
        TaggedFactSets(predicates, functions)
    {
        for (const auto atom : atoms)
            predicate.insert(atom);
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