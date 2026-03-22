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
    const formalism::datalog::Repository& m_repository;

    Index<formalism::Predicate<T>> m_predicate_index;
    std::vector<Index<formalism::Row>> m_bindings;

    boost::dynamic_bitset<> m_bitset;

public:
    explicit PredicateFactSet(formalism::datalog::PredicateView<T> predicate, const formalism::datalog::Repository& repository);

    auto get_predicate() const noexcept { return m_predicate; }

    void reset() noexcept;

    void insert(const PredicateFactSet<T>& other);
    void insert(formalism::datalog::GroundAtomView<T> ground_atom);
    void insert(formalism::datalog::PredicateBindingView<T> binding);
    void insert(formalism::datalog::PredicateBindingForwardRangeView<T> bindings);
    void insert(const std::vector<formalism::datalog::PredicateBindingView<T>>& bindings);

    bool contains(formalism::datalog::PredicateBindingView<T> binding) const noexcept;

    formalism::datalog::PredicateBindingForwardRangeView<T> get_bindings() const noexcept;
};

template<formalism::FactKind T>
class PredicateFactSets
{
private:
    std::vector<PredicateFactSet<T>> m_sets;

public:
    explicit PredicateFactSets(formalism::datalog::PredicateListView<T> predicates, const formalism::datalog::Repository& repository);

    void reset() noexcept;

    void insert(const PredicateFactSets<T>& other);
    void insert(formalism::datalog::GroundAtomView<T> ground_atom);
    void insert(formalism::datalog::PredicateBindingView<T> binding);
    void insert(formalism::datalog::PredicateBindingForwardRangeView<T> bindings);

    bool contains(formalism::datalog::PredicateBindingView<T> binding) const noexcept;

    const std::vector<PredicateFactSet<T>>& get_sets() const noexcept;
};

template<formalism::FactKind T>
class FunctionFactSet
{
private:
    formalism::datalog::FunctionView<T> m_function;
    const formalism::datalog::Repository& m_repository;

    Index<formalism::Function<T>> m_function_index;
    std::vector<uint_t> m_remap;
    std::vector<Index<formalism::Row>> m_bindings;
    std::vector<float_t> m_values;

public:
    explicit FunctionFactSet(formalism::datalog::FunctionView<T> function, const formalism::datalog::Repository& repository);

    void reset() noexcept;

    void insert(const FunctionFactSet& other);
    void insert(formalism::datalog::FunctionBindingView<T> binding, float_t value);
    void insert(formalism::datalog::FunctionBindingRandomAccessRangeView<T> bindings, const std::vector<float_t>& values);
    void insert(const std::vector<formalism::datalog::FunctionBindingView<T>>& bindings, const std::vector<float_t>& values);
    void insert(formalism::datalog::GroundFunctionTermView<T> fterm, float_t value);
    void insert(formalism::datalog::GroundFunctionTermValueView<T> fterm_value);
    void insert(formalism::datalog::GroundFunctionTermValueListView<T> fterm_values);

    float_t operator[](formalism::datalog::GroundFunctionTermView<T> fterm) const noexcept;

    const std::vector<uint_t>& get_remap() const noexcept;
    formalism::datalog::FunctionBindingRandomAccessRangeView<T> get_bindings() const noexcept;
    const std::vector<float_t>& get_values() const noexcept;
};

template<formalism::FactKind T>
class FunctionFactSets
{
private:
    std::vector<FunctionFactSet<T>> m_sets;

public:
    explicit FunctionFactSets(formalism::datalog::FunctionListView<T> functions, const formalism::datalog::Repository& repository);

    void reset() noexcept;

    void insert(const FunctionFactSets& other);
    void insert(formalism::datalog::GroundFunctionTermView<T> function_term, float_t value);
    void insert(formalism::datalog::GroundFunctionTermListView<T> function_terms, const std::vector<float_t>& values);
    void insert(formalism::datalog::GroundFunctionTermValueView<T> fterm_value);
    void insert(formalism::datalog::GroundFunctionTermValueListView<T> fterm_values);

    float_t operator[](formalism::datalog::GroundFunctionTermView<T> fterm) const noexcept;

    const std::vector<FunctionFactSet<T>>& get_sets() const noexcept;
};

template<formalism::FactKind T>
struct TaggedFactSets
{
    PredicateFactSets<T> predicate;
    FunctionFactSets<T> function;

    TaggedFactSets(formalism::datalog::PredicateListView<T> predicates,
                   formalism::datalog::FunctionListView<T> functions,
                   const formalism::datalog::Repository& repository);

    TaggedFactSets(formalism::datalog::PredicateListView<T> predicates,
                   formalism::datalog::FunctionListView<T> functions,
                   formalism::datalog::GroundAtomListView<T> atoms,
                   formalism::datalog::GroundFunctionTermValueListView<T> fterm_values,
                   const formalism::datalog::Repository& repository);

    void insert(const TaggedFactSets<T>& other);

    void reset() noexcept;
};

struct FactSets
{
    const TaggedFactSets<formalism::StaticTag>& static_sets;
    const TaggedFactSets<formalism::FluentTag>& fluent_sets;

    FactSets(const TaggedFactSets<formalism::StaticTag>& static_sets, const TaggedFactSets<formalism::FluentTag>& fluent_sets) noexcept;

    template<formalism::FactKind T>
    const TaggedFactSets<T>& get() const noexcept;
};

}

#endif