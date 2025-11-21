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

template<typename T>
using MappedType = boost::hana::pair<boost::hana::type<T>, cista::IndexedHashSet<T>>;

template<typename T>
using MappedTypePerIndex = boost::hana::pair<boost::hana::type<T>, cista::IndexedHashSetList<T>>;

template<typename T>
struct TypeProperties
{
    using PairType = MappedType<T>;
};

template<IsStaticOrFluentTag T>
struct TypeProperties<Atom<T>>
{
    using PairType = MappedTypePerIndex<Atom<T>>;
    static auto get_index(AtomIndex<T>& self) noexcept { return self.predicate_index; }
};

template<IsStaticOrFluentTag T>
struct TypeProperties<GroundAtom<T>>
{
    using PairType = MappedTypePerIndex<GroundAtom<T>>;
    static auto get_index(GroundAtomIndex<T> self) noexcept { return self.predicate_index; }
};

template<IsStaticOrFluentTag T>
struct TypeProperties<Literal<T>>
{
    using PairType = MappedTypePerIndex<Literal<T>>;
    static auto get_index(LiteralIndex<T> self) noexcept { return self.predicate_index; }
};

template<IsStaticOrFluentTag T>
struct TypeProperties<FunctionTerm<T>>
{
    using PairType = MappedTypePerIndex<FunctionTerm<T>>;
    static auto get_index(FunctionTermIndex<T> self) noexcept { return self.function_index; }
};

template<IsStaticOrFluentTag T>
struct TypeProperties<GroundFunctionTerm<T>>
{
    using PairType = MappedTypePerIndex<GroundFunctionTerm<T>>;
    static auto get_index(GroundFunctionTermIndex<T>& self) noexcept { return self.function_index; }
};

template<IsStaticOrFluentTag T>
struct TypeProperties<GroundFunctionTermValue<T>>
{
    using PairType = MappedTypePerIndex<GroundFunctionTermValue<T>>;
    static auto get_index(GroundFunctionTermValueIndex<T>& self) noexcept { return self.function_index; }
};

template<>
struct TypeProperties<GroundRule>
{
    using PairType = MappedTypePerIndex<GroundRule>;
    static auto get_index(GroundRuleIndex self) noexcept { return self.rule_index; }
};

template<typename T>
concept IsMappedType = std::same_as<typename TypeProperties<T>::PairType, MappedType<T>>;

template<typename T>
concept IsMappedTypePerIndex = std::same_as<typename TypeProperties<T>::PairType, MappedTypePerIndex<T>>;

class Repository
{
private:
    using HanaRepository = boost::hana::map<TypeProperties<Variable>::PairType,
                                            TypeProperties<Object>::PairType,
                                            TypeProperties<Predicate<StaticTag>>::PairType,
                                            TypeProperties<Predicate<FluentTag>>::PairType,
                                            TypeProperties<Atom<StaticTag>>::PairType,
                                            TypeProperties<Atom<FluentTag>>::PairType,
                                            TypeProperties<GroundAtom<StaticTag>>::PairType,
                                            TypeProperties<GroundAtom<FluentTag>>::PairType,
                                            TypeProperties<Literal<StaticTag>>::PairType,
                                            TypeProperties<Literal<FluentTag>>::PairType,
                                            TypeProperties<GroundLiteral<StaticTag>>::PairType,
                                            TypeProperties<GroundLiteral<FluentTag>>::PairType,
                                            TypeProperties<Function<StaticTag>>::PairType,
                                            TypeProperties<Function<FluentTag>>::PairType,
                                            TypeProperties<FunctionTerm<StaticTag>>::PairType,
                                            TypeProperties<FunctionTerm<FluentTag>>::PairType,
                                            TypeProperties<GroundFunctionTerm<StaticTag>>::PairType,
                                            TypeProperties<GroundFunctionTerm<FluentTag>>::PairType,
                                            TypeProperties<GroundFunctionTermValue<StaticTag>>::PairType,
                                            TypeProperties<GroundFunctionTermValue<FluentTag>>::PairType,
                                            TypeProperties<UnaryOperator<OpSub, FunctionExpression>>::PairType,
                                            TypeProperties<BinaryOperator<OpAdd, FunctionExpression>>::PairType,
                                            TypeProperties<BinaryOperator<OpSub, FunctionExpression>>::PairType,
                                            TypeProperties<BinaryOperator<OpMul, FunctionExpression>>::PairType,
                                            TypeProperties<BinaryOperator<OpDiv, FunctionExpression>>::PairType,
                                            TypeProperties<MultiOperator<OpAdd, FunctionExpression>>::PairType,
                                            TypeProperties<MultiOperator<OpMul, FunctionExpression>>::PairType,
                                            TypeProperties<BinaryOperator<OpEq, FunctionExpression>>::PairType,
                                            TypeProperties<BinaryOperator<OpLe, FunctionExpression>>::PairType,
                                            TypeProperties<BinaryOperator<OpLt, FunctionExpression>>::PairType,
                                            TypeProperties<BinaryOperator<OpGe, FunctionExpression>>::PairType,
                                            TypeProperties<BinaryOperator<OpGt, FunctionExpression>>::PairType,
                                            TypeProperties<UnaryOperator<OpSub, GroundFunctionExpression>>::PairType,
                                            TypeProperties<BinaryOperator<OpAdd, GroundFunctionExpression>>::PairType,
                                            TypeProperties<BinaryOperator<OpSub, GroundFunctionExpression>>::PairType,
                                            TypeProperties<BinaryOperator<OpMul, GroundFunctionExpression>>::PairType,
                                            TypeProperties<BinaryOperator<OpDiv, GroundFunctionExpression>>::PairType,
                                            TypeProperties<MultiOperator<OpAdd, GroundFunctionExpression>>::PairType,
                                            TypeProperties<MultiOperator<OpMul, GroundFunctionExpression>>::PairType,
                                            TypeProperties<BinaryOperator<OpEq, GroundFunctionExpression>>::PairType,
                                            TypeProperties<BinaryOperator<OpLe, GroundFunctionExpression>>::PairType,
                                            TypeProperties<BinaryOperator<OpLt, GroundFunctionExpression>>::PairType,
                                            TypeProperties<BinaryOperator<OpGe, GroundFunctionExpression>>::PairType,
                                            TypeProperties<BinaryOperator<OpGt, GroundFunctionExpression>>::PairType,
                                            TypeProperties<Rule>::PairType,
                                            TypeProperties<GroundRule>::PairType,
                                            TypeProperties<Program>::PairType>;

    HanaRepository m_repository;

public:
    Repository() = default;

    // nullptr signals that the object does not exist.
    template<IsMappedTypePerIndex T>
    const T* find(const T& builder) const
    {
        const auto& list = boost::hana::at_key(m_repository, boost::hana::type<T> {});

        const auto i = TypeProperties<T>::get_index(builder.index).get();

        if (i >= list.size())
            return nullptr;

        const auto& indexed_hash_set = list[i];

        return indexed_hash_set.find(builder);
    }

    // nullptr signals that the object does not exist.
    template<IsMappedType T>
    const T* find(const T& builder) const
    {
        const auto& indexed_hash_set = boost::hana::at_key(m_repository, boost::hana::type<T> {});

        return indexed_hash_set.find(builder);
    }

    // const T* always points to a valid instantiation of the class.
    // We return const T* here to avoid bugs when using structured bindings.
    template<IsMappedTypePerIndex T, bool AssignIndex = true>
    std::pair<const T*, bool> get_or_create(T& builder, cista::Buffer& buf)
    {
        auto& list = boost::hana::at_key(m_repository, boost::hana::type<T> {});

        const auto i = TypeProperties<T>::get_index(builder.index).get();

        if (i >= list.size())
            list.resize(i + 1);

        auto& indexed_hash_set = list[i];

        if constexpr (AssignIndex)
            builder.index.value = indexed_hash_set.size();

        return indexed_hash_set.insert(builder, buf);
    }

    // const T* always points to a valid instantiation of the class.
    // We return const T* here to avoid bugs when using structured bindings.
    template<IsMappedType T, bool AssignIndex = true>
    std::pair<const T*, bool> get_or_create(T& builder, cista::Buffer& buf)
    {
        auto& indexed_hash_set = boost::hana::at_key(m_repository, boost::hana::type<T> {});

        if constexpr (AssignIndex)
            builder.index.value = indexed_hash_set.size();

        return indexed_hash_set.insert(builder, buf);
    }

    template<IsIndexType T>
        requires IsMappedTypePerIndex<typename IndexTraits<T>::DataType>
    const auto& operator[](T index) const
    {
        using DataType = typename IndexTraits<T>::DataType;

        const auto& list = boost::hana::at_key(m_repository, boost::hana::type<DataType> {});

        const auto i = TypeProperties<DataType>::get_index(index).get();

        assert(i < list.size());

        const auto& repository = list[i];

        return repository[index];
    }

    template<IsIndexType T>
        requires IsMappedType<typename IndexTraits<T>::DataType>
    const auto& operator[](T index) const
    {
        using DataType = typename IndexTraits<T>::DataType;

        const auto& repository = boost::hana::at_key(m_repository, boost::hana::type<DataType> {});

        return repository[index];
    }
};

static_assert(IsRepository<Repository>);
static_assert(IsContext<Repository>);
}

#endif
