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

#ifndef TYR_FORMALISM_DATALOG_REPOSITORY_HPP_
#define TYR_FORMALISM_DATALOG_REPOSITORY_HPP_

// Include specialization headers first
#include "tyr/formalism/datalog/datas.hpp"
#include "tyr/formalism/datalog/indices.hpp"
//
#include "tyr/buffer/declarations.hpp"
#include "tyr/buffer/indexed_hash_set.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/datalog/declarations.hpp"

#include <cassert>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace tyr::formalism::datalog
{

class Repository
{
private:
    template<typename T>
    struct RepositoryEntry;

    template<typename T>
        requires(IndexConcept<Index<T>> && !GroupIndexConcept<Index<T>>)
    struct RepositoryEntry<T>
    {
        using value_type = T;
        using container_type = buffer::IndexedHashSet<T>;

        container_type container;
    };

    template<typename T>
        requires(GroupIndexConcept<Index<T>>)
    struct RepositoryEntry<T>
    {
        using value_type = T;
        using container_type = std::vector<buffer::IndexedHashSet<T>>;

        container_type container;
    };

    using RepositoryStorage = std::tuple<RepositoryEntry<Variable>,
                                         RepositoryEntry<Object>,
                                         RepositoryEntry<Binding>,
                                         RepositoryEntry<Predicate<StaticTag>>,
                                         RepositoryEntry<Predicate<FluentTag>>,
                                         RepositoryEntry<Atom<StaticTag>>,
                                         RepositoryEntry<Atom<FluentTag>>,
                                         RepositoryEntry<GroundAtom<StaticTag>>,
                                         RepositoryEntry<GroundAtom<FluentTag>>,
                                         RepositoryEntry<Literal<StaticTag>>,
                                         RepositoryEntry<Literal<FluentTag>>,
                                         RepositoryEntry<GroundLiteral<StaticTag>>,
                                         RepositoryEntry<GroundLiteral<FluentTag>>,
                                         RepositoryEntry<Function<StaticTag>>,
                                         RepositoryEntry<Function<FluentTag>>,
                                         RepositoryEntry<FunctionTerm<StaticTag>>,
                                         RepositoryEntry<FunctionTerm<FluentTag>>,
                                         RepositoryEntry<GroundFunctionTerm<StaticTag>>,
                                         RepositoryEntry<GroundFunctionTerm<FluentTag>>,
                                         RepositoryEntry<GroundFunctionTermValue<StaticTag>>,
                                         RepositoryEntry<GroundFunctionTermValue<FluentTag>>,
                                         RepositoryEntry<UnaryOperator<OpSub, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpAdd, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpSub, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpMul, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpDiv, Data<FunctionExpression>>>,
                                         RepositoryEntry<MultiOperator<OpAdd, Data<FunctionExpression>>>,
                                         RepositoryEntry<MultiOperator<OpMul, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpEq, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpNe, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpLe, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpLt, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpGe, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpGt, Data<FunctionExpression>>>,
                                         RepositoryEntry<UnaryOperator<OpSub, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpAdd, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpSub, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpMul, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpDiv, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<MultiOperator<OpAdd, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<MultiOperator<OpMul, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpEq, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpNe, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpLe, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpLt, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpGe, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpGt, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<ConjunctiveCondition>,
                                         RepositoryEntry<Rule>,
                                         RepositoryEntry<GroundConjunctiveCondition>,
                                         RepositoryEntry<GroundRule>,
                                         RepositoryEntry<Program>>;

    RepositoryStorage m_repository;

public:
    Repository() = default;

    template<typename T>
        requires(IndexConcept<Index<T>> && !GroupIndexConcept<Index<T>>)
    std::optional<Index<T>> find(const Data<T>& builder) const noexcept
    {
        const auto& repository = get_container<T>(m_repository);

        if (const auto ptr = repository.find(builder))
            return ptr->index;

        return std::nullopt;
    }

    template<typename T>
        requires(GroupIndexConcept<Index<T>>)
    std::optional<Index<T>> find(const Data<T>& builder) const noexcept
    {
        const auto& repositories = get_container<T>(m_repository);

        if (builder.index.group.value >= repositories.size())
            return std::nullopt;

        if (const auto ptr = repositories[builder.index.group.value].find(builder))
            return ptr->index;

        return std::nullopt;
    }

    template<typename T, bool AssignIndex = true>
        requires(IndexConcept<Index<T>> && !GroupIndexConcept<Index<T>>)
    std::pair<Index<T>, bool> get_or_create(Data<T>& builder, buffer::Buffer& buf)
    {
        auto& repository = get_container<T>(m_repository);

        if constexpr (AssignIndex)
            builder.index.value = repository.size();

        const auto [ptr, success] = repository.insert(builder, buf);

        return std::make_pair(ptr->index, success);
    }

    template<typename T, bool AssignIndex = true>
        requires(GroupIndexConcept<Index<T>>)
    std::pair<Index<T>, bool> get_or_create(Data<T>& builder, buffer::Buffer& buf)
    {
        auto& repositories = get_container<T>(m_repository);

        if (builder.index.group.value >= repositories.size())
            repositories.resize(builder.index.group.value + 1);

        if constexpr (AssignIndex)
            builder.index.value = repositories[builder.index.group.value].size();

        const auto [ptr, success] = repositories[builder.index.group.value].insert(builder, buf);

        return std::make_pair(ptr->index, success);
    }

    /// @brief Access the element with the given index.
    template<typename T>
        requires(IndexConcept<Index<T>> && !GroupIndexConcept<Index<T>>)
    const Data<T>& operator[](Index<T> index) const noexcept
    {
        assert(index != Index<T>::max() && "Unassigned index.");

        const auto& repository = get_container<T>(m_repository);

        return repository[index];
    }

    template<typename T>
        requires(GroupIndexConcept<Index<T>>)
    const Data<T>& operator[](Index<T> index) const noexcept
    {
        assert(index != Index<T>::max() && "Unassigned index.");

        const auto& repositories = get_container<T>(m_repository);

        return repositories[index.group.value][index];
    }

    template<typename T>
    const Data<T>& front() const
    {
        const auto& repository = get_container<T>(m_repository);

        return repository.front();
    }

    template<typename T>
        requires(GroupIndexConcept<Index<T>>)
    const Data<T>& front(Index<T> index) const
    {
        const auto& repositories = get_container<T>(m_repository);

        assert(size(index) > 0);

        return repositories[index.group.value].front();
    }

    /// @brief Get the number of stored elements.
    template<typename T>
    size_t size() const noexcept
    {
        const auto& repository = get_container<T>(m_repository);

        return repository.size();
    }

    template<typename T>
        requires(GroupIndexConcept<Index<T>>)
    size_t size(Index<T> index) const noexcept
    {
        const auto& repositories = get_container<T>(m_repository);

        if (index.group.value >= repositories.size())
            return 0;

        return repositories[index.group.value].size();
    }

    /// @brief Clear the repository but keep memory allocated.
    void clear() noexcept
    {
        std::apply([](auto&... slots) { (slots.container.clear(), ...); }, m_repository);
    }
};

static_assert(RepositoryConcept<Repository>);

static_assert(Context<Repository>);

}

#endif
