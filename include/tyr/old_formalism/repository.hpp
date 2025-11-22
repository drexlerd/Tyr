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

#ifndef TYR_FORMALISM_REPOSITORY_HPP_
#define TYR_FORMALISM_REPOSITORY_HPP_

// Include specialization headers first
#include "tyr/formalism/data_traits.hpp"
#include "tyr/formalism/index_traits.hpp"
#include "tyr/formalism/proxy_traits.hpp"
//
#include "tyr/cista/declarations.hpp"
#include "tyr/cista/indexed_hash_set.hpp"
#include "tyr/formalism/atom.hpp"
#include "tyr/formalism/binary_operator.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function.hpp"
#include "tyr/formalism/function_expression.hpp"
#include "tyr/formalism/function_term.hpp"
#include "tyr/formalism/ground_atom.hpp"
#include "tyr/formalism/ground_function_expression.hpp"
#include "tyr/formalism/ground_function_term.hpp"
#include "tyr/formalism/ground_function_term_value.hpp"
#include "tyr/formalism/ground_literal.hpp"
#include "tyr/formalism/ground_rule.hpp"
#include "tyr/formalism/literal.hpp"
#include "tyr/formalism/multi_operator.hpp"
#include "tyr/formalism/object.hpp"
#include "tyr/formalism/predicate.hpp"
#include "tyr/formalism/program.hpp"
#include "tyr/formalism/rule.hpp"
#include "tyr/formalism/term.hpp"
#include "tyr/formalism/unary_operator.hpp"
#include "tyr/formalism/variable.hpp"

#include <boost/hana.hpp>

namespace tyr::formalism
{
class Repository
{
private:
    /// @brief `FlatRepositoryEntry` is the mapping from data type to an indexed hash set.
    template<typename T>
    using FlatRepositoryEntry = boost::hana::pair<boost::hana::type<T>, cista::IndexedHashSet<T>>;

    /// @brief `GroupRepositoryEntry` is the mapping from data type to a list of indexed hash sets.
    template<typename T>
    using GroupRepositoryEntry = boost::hana::pair<boost::hana::type<T>, cista::IndexedHashSetList<T>>;

    template<typename T>
    struct RepositoryTraits
    {
        using EntryType = std::conditional_t<IsGroupDataType<T>, GroupRepositoryEntry<T>, FlatRepositoryEntry<T>>;
    };

    using HanaRepository = boost::hana::map<RepositoryTraits<Variable>::EntryType,
                                            RepositoryTraits<Object>::EntryType,
                                            RepositoryTraits<Predicate<StaticTag>>::EntryType,
                                            RepositoryTraits<Predicate<FluentTag>>::EntryType,
                                            RepositoryTraits<Atom<StaticTag>>::EntryType,
                                            RepositoryTraits<Atom<FluentTag>>::EntryType,
                                            RepositoryTraits<GroundAtom<StaticTag>>::EntryType,
                                            RepositoryTraits<GroundAtom<FluentTag>>::EntryType,
                                            RepositoryTraits<Literal<StaticTag>>::EntryType,
                                            RepositoryTraits<Literal<FluentTag>>::EntryType,
                                            RepositoryTraits<GroundLiteral<StaticTag>>::EntryType,
                                            RepositoryTraits<GroundLiteral<FluentTag>>::EntryType,
                                            RepositoryTraits<Function<StaticTag>>::EntryType,
                                            RepositoryTraits<Function<FluentTag>>::EntryType,
                                            RepositoryTraits<FunctionTerm<StaticTag>>::EntryType,
                                            RepositoryTraits<FunctionTerm<FluentTag>>::EntryType,
                                            RepositoryTraits<GroundFunctionTerm<StaticTag>>::EntryType,
                                            RepositoryTraits<GroundFunctionTerm<FluentTag>>::EntryType,
                                            RepositoryTraits<GroundFunctionTermValue<StaticTag>>::EntryType,
                                            RepositoryTraits<GroundFunctionTermValue<FluentTag>>::EntryType,
                                            RepositoryTraits<UnaryOperator<OpSub, FunctionExpression>>::EntryType,
                                            RepositoryTraits<BinaryOperator<OpAdd, FunctionExpression>>::EntryType,
                                            RepositoryTraits<BinaryOperator<OpSub, FunctionExpression>>::EntryType,
                                            RepositoryTraits<BinaryOperator<OpMul, FunctionExpression>>::EntryType,
                                            RepositoryTraits<BinaryOperator<OpDiv, FunctionExpression>>::EntryType,
                                            RepositoryTraits<MultiOperator<OpAdd, FunctionExpression>>::EntryType,
                                            RepositoryTraits<MultiOperator<OpMul, FunctionExpression>>::EntryType,
                                            RepositoryTraits<BinaryOperator<OpEq, FunctionExpression>>::EntryType,
                                            RepositoryTraits<BinaryOperator<OpLe, FunctionExpression>>::EntryType,
                                            RepositoryTraits<BinaryOperator<OpLt, FunctionExpression>>::EntryType,
                                            RepositoryTraits<BinaryOperator<OpGe, FunctionExpression>>::EntryType,
                                            RepositoryTraits<BinaryOperator<OpGt, FunctionExpression>>::EntryType,
                                            RepositoryTraits<UnaryOperator<OpSub, GroundFunctionExpression>>::EntryType,
                                            RepositoryTraits<BinaryOperator<OpAdd, GroundFunctionExpression>>::EntryType,
                                            RepositoryTraits<BinaryOperator<OpSub, GroundFunctionExpression>>::EntryType,
                                            RepositoryTraits<BinaryOperator<OpMul, GroundFunctionExpression>>::EntryType,
                                            RepositoryTraits<BinaryOperator<OpDiv, GroundFunctionExpression>>::EntryType,
                                            RepositoryTraits<MultiOperator<OpAdd, GroundFunctionExpression>>::EntryType,
                                            RepositoryTraits<MultiOperator<OpMul, GroundFunctionExpression>>::EntryType,
                                            RepositoryTraits<BinaryOperator<OpEq, GroundFunctionExpression>>::EntryType,
                                            RepositoryTraits<BinaryOperator<OpLe, GroundFunctionExpression>>::EntryType,
                                            RepositoryTraits<BinaryOperator<OpLt, GroundFunctionExpression>>::EntryType,
                                            RepositoryTraits<BinaryOperator<OpGe, GroundFunctionExpression>>::EntryType,
                                            RepositoryTraits<BinaryOperator<OpGt, GroundFunctionExpression>>::EntryType,
                                            RepositoryTraits<Rule>::EntryType,
                                            RepositoryTraits<GroundRule>::EntryType,
                                            RepositoryTraits<Program>::EntryType>;

    HanaRepository m_repository;

public:
    Repository() = default;

    // nullptr signals that the object does not exist.
    template<IsGroupDataType T>
    const T* find(const T& builder) const
    {
        const auto& list = boost::hana::at_key(m_repository, boost::hana::type<T> {});

        const auto i = builder.index.get_group().get_value();

        if (i >= list.size())
            return nullptr;

        const auto& indexed_hash_set = list[i];

        return indexed_hash_set.find(builder);
    }

    // nullptr signals that the object does not exist.
    template<IsFlatDataType T>
    const T* find(const T& builder) const
    {
        const auto& indexed_hash_set = boost::hana::at_key(m_repository, boost::hana::type<T> {});

        return indexed_hash_set.find(builder);
    }

    // const T* always points to a valid instantiation of the class.
    // We return const T* here to avoid bugs when using structured bindings.
    template<IsGroupDataType T, bool AssignIndex = true>
    std::pair<const T*, bool> get_or_create(T& builder, cista::Buffer& buf)
    {
        auto& list = boost::hana::at_key(m_repository, boost::hana::type<T> {});

        const auto i = builder.index.get_group().get_value();

        if (i >= list.size())
            list.resize(i + 1);

        auto& indexed_hash_set = list[i];

        if constexpr (AssignIndex)
            builder.index.value = indexed_hash_set.size();

        return indexed_hash_set.insert(builder, buf);
    }

    // const T* always points to a valid instantiation of the class.
    // We return const T* here to avoid bugs when using structured bindings.
    template<IsFlatDataType T, bool AssignIndex = true>
    std::pair<const T*, bool> get_or_create(T& builder, cista::Buffer& buf)
    {
        auto& indexed_hash_set = boost::hana::at_key(m_repository, boost::hana::type<T> {});

        if constexpr (AssignIndex)
            builder.index.value = indexed_hash_set.size();

        return indexed_hash_set.insert(builder, buf);
    }

    /// @brief Access the element with the given index.
    template<IsGroupIndexType T>
    const auto& operator[](T index) const
    {
        using DataType = typename IndexTraits<T>::DataType;

        const auto& list = boost::hana::at_key(m_repository, boost::hana::type<DataType> {});

        const auto i = index.get_group().get_value();

        assert(i < list.size());

        const auto& repository = list[i];

        return repository[index];
    }

    /// @brief Access the element with the given index.
    template<IsFlatIndexType T>
    const auto& operator[](T index) const
    {
        using DataType = typename IndexTraits<T>::DataType;

        const auto& repository = boost::hana::at_key(m_repository, boost::hana::type<DataType> {});

        return repository[index];
    }

    /// @brief Get the number of stored elements.
    template<IsGroupIndexType T>
    size_t size(T index) const
    {
        using DataType = typename IndexTraits<T>::DataType;

        const auto& list = boost::hana::at_key(m_repository, boost::hana::type<DataType> {});

        const auto i = index.get_group().get_value();

        assert(i < list.size());

        const auto& repository = list[i];

        return repository.size();
    }

    /// @brief Get the number of stored elements.
    template<IsFlatIndexType T>
    size_t size() const
    {
        using DataType = typename IndexTraits<T>::DataType;

        const auto& repository = boost::hana::at_key(m_repository, boost::hana::type<DataType> {});

        return repository.size();
    }
};

static_assert(IsRepository<Repository>);
static_assert(IsContext<Repository>);
}

#endif
