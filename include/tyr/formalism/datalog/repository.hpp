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

    const Repository* m_parent;

    template<typename T>
        requires(IndexConcept<Index<T>> && !GroupIndexConcept<Index<T>>)
    std::optional<Index<T>> find_impl(const Data<T>& builder) const noexcept
    {
        const auto& repo = get_container<T>(m_repository);
        if (auto ptr = repo.find(builder))
            return ptr->index;
        return std::nullopt;
    }

    template<typename T>
        requires(GroupIndexConcept<Index<T>>)
    std::optional<Index<T>> find_impl(const Data<T>& builder) const noexcept
    {
        const auto& repos = get_container<T>(m_repository);
        const auto g = builder.index.group.value;

        if (g >= repos.size())
            return std::nullopt;

        if (auto ptr = repos[g].find(builder))
            return ptr->index;

        return std::nullopt;
    }

public:
    Repository(const Repository* parent = nullptr) : m_parent(parent) {}

    template<typename T>
    std::optional<Index<T>> find(const Data<T>& builder) const noexcept
    {
        if (!m_parent)
            return find_impl<T>(builder);

        if (auto ptr = find_impl<T>(builder))
            return ptr;

        return m_parent ? m_parent->template find<T>(builder) : std::nullopt;
    }

    template<typename T>
        requires(IndexConcept<Index<T>> && !GroupIndexConcept<Index<T>>)
    std::pair<Index<T>, bool> get_or_create(Data<T>& builder, buffer::Buffer& buf)
    {
        if (auto ptr = find<T>(builder))
            return { *ptr, false };

        auto& repo = get_container<T>(m_repository);

        const size_t parent_size = m_parent ? m_parent->template size<T>() : 0;
        builder.index.value = parent_size + repo.size();

        const auto [ptr, success] = repo.insert(builder, buf);
        return { ptr->index, success };
    }

    template<typename T>
        requires(GroupIndexConcept<Index<T>>)
    std::pair<Index<T>, bool> get_or_create(Data<T>& builder, buffer::Buffer& buf)
    {
        if (auto ptr = find<T>(builder))
            return { *ptr, false };

        auto& repos = get_container<T>(m_repository);
        const auto g = builder.index.group.value;

        if (g >= repos.size())
            repos.resize(g + 1);

        const size_t parent_size = m_parent ? m_parent->template size<T>(builder.index) : 0;
        builder.index.value = parent_size + repos[g].size();

        const auto [ptr, success] = repos[g].insert(builder, buf);
        return { ptr->index, success };
    }

    /// @brief Access the element with the given index.
    template<typename T>
        requires(IndexConcept<Index<T>> && !GroupIndexConcept<Index<T>>)
    const Data<T>& operator[](Index<T> index) const noexcept
    {
        assert(index != Index<T>::max() && "Unassigned index.");

        const size_t parent_size = m_parent ? m_parent->template size<T>() : 0;

        if (m_parent && index.value < parent_size)
            return (*m_parent)[index];

        index.value -= parent_size;

        const auto& repo = get_container<T>(m_repository);
        return repo[index];
    }

    template<typename T>
        requires(GroupIndexConcept<Index<T>>)
    const Data<T>& operator[](Index<T> index) const noexcept
    {
        assert(index != Index<T>::max() && "Unassigned index.");

        const size_t parent_size = m_parent ? m_parent->template size<T>(index) : 0;

        if (m_parent && index.value < parent_size)
            return (*m_parent)[index];

        index.value -= parent_size;

        const auto& repos = get_container<T>(m_repository);
        // Assuming index.group.value is valid for local access when present.
        return repos[index.group.value][index];
    }

    template<typename T>
        requires(IndexConcept<Index<T>> && !GroupIndexConcept<Index<T>>)
    const Data<T>& front() const
    {
        if (m_parent && m_parent->template size<T>() > 0)
            return m_parent->template front<T>();

        const auto& repo = get_container<T>(m_repository);
        return repo.front();
    }

    template<typename T>
        requires(GroupIndexConcept<Index<T>>)
    const Data<T>& front(Index<T> index) const
    {
        if (m_parent && m_parent->template size<T>(index) > 0)
            return m_parent->template front<T>(index);

        const auto& repos = get_container<T>(m_repository);
        assert(index.group.value < repos.size());
        assert(repos[index.group.value].size() > 0);
        return repos[index.group.value].front();
    }

    /// @brief Get the number of stored elements.
    template<typename T>
        requires(IndexConcept<Index<T>> && !GroupIndexConcept<Index<T>>)
    size_t size() const noexcept
    {
        const auto& repo = get_container<T>(m_repository);
        return (m_parent ? m_parent->template size<T>() : 0) + repo.size();
    }

    template<typename T>
        requires(GroupIndexConcept<Index<T>>)
    size_t size(Index<T> index) const noexcept
    {
        const auto& repos = get_container<T>(m_repository);
        const auto g = index.group.value;

        const size_t parent_size = m_parent ? m_parent->template size<T>(index) : 0;
        const size_t local_size = (g < repos.size()) ? repos[g].size() : 0;

        return parent_size + local_size;
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
