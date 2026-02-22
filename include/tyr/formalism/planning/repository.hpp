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

#ifndef TYR_FORMALISM_PLANNING_REPOSITORY_HPP_
#define TYR_FORMALISM_PLANNING_REPOSITORY_HPP_

// Include specialization headers first
#include "tyr/formalism/planning/datas.hpp"
#include "tyr/formalism/planning/indices.hpp"
//
#include "tyr/buffer/declarations.hpp"
#include "tyr/buffer/indexed_hash_set.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/planning/declarations.hpp"

#include <cassert>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace tyr::formalism::planning
{

class Repository
{
private:
    template<typename T>
    struct Slot
    {
        buffer::IndexedHashSet<T> container {};
        size_t parent_size {};
    };

    template<typename T>
    struct RepositoryEntry
    {
        using value_type = T;
        using slot_type = Slot<T>;

        slot_type slot = slot_type {};
    };

    using RepositoryStorage = std::tuple<RepositoryEntry<Variable>,
                                         RepositoryEntry<Object>,
                                         RepositoryEntry<Binding>,
                                         RepositoryEntry<Predicate<StaticTag>>,
                                         RepositoryEntry<Predicate<FluentTag>>,
                                         RepositoryEntry<Predicate<DerivedTag>>,
                                         RepositoryEntry<Atom<StaticTag>>,
                                         RepositoryEntry<Atom<FluentTag>>,
                                         RepositoryEntry<Atom<DerivedTag>>,
                                         RepositoryEntry<GroundAtom<StaticTag>>,
                                         RepositoryEntry<GroundAtom<FluentTag>>,
                                         RepositoryEntry<GroundAtom<DerivedTag>>,
                                         RepositoryEntry<Literal<StaticTag>>,
                                         RepositoryEntry<Literal<FluentTag>>,
                                         RepositoryEntry<Literal<DerivedTag>>,
                                         RepositoryEntry<GroundLiteral<StaticTag>>,
                                         RepositoryEntry<GroundLiteral<FluentTag>>,
                                         RepositoryEntry<GroundLiteral<DerivedTag>>,
                                         RepositoryEntry<Function<StaticTag>>,
                                         RepositoryEntry<Function<FluentTag>>,
                                         RepositoryEntry<Function<AuxiliaryTag>>,
                                         RepositoryEntry<FunctionTerm<StaticTag>>,
                                         RepositoryEntry<FunctionTerm<FluentTag>>,
                                         RepositoryEntry<FunctionTerm<AuxiliaryTag>>,
                                         RepositoryEntry<GroundFunctionTerm<StaticTag>>,
                                         RepositoryEntry<GroundFunctionTerm<FluentTag>>,
                                         RepositoryEntry<GroundFunctionTerm<AuxiliaryTag>>,
                                         RepositoryEntry<GroundFunctionTermValue<StaticTag>>,
                                         RepositoryEntry<GroundFunctionTermValue<FluentTag>>,
                                         RepositoryEntry<GroundFunctionTermValue<AuxiliaryTag>>,
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
                                         RepositoryEntry<NumericEffect<OpAssign, FluentTag>>,
                                         RepositoryEntry<NumericEffect<OpIncrease, FluentTag>>,
                                         RepositoryEntry<NumericEffect<OpDecrease, FluentTag>>,
                                         RepositoryEntry<NumericEffect<OpScaleUp, FluentTag>>,
                                         RepositoryEntry<NumericEffect<OpScaleDown, FluentTag>>,
                                         RepositoryEntry<NumericEffect<OpIncrease, AuxiliaryTag>>,
                                         RepositoryEntry<GroundNumericEffect<OpAssign, FluentTag>>,
                                         RepositoryEntry<GroundNumericEffect<OpIncrease, FluentTag>>,
                                         RepositoryEntry<GroundNumericEffect<OpDecrease, FluentTag>>,
                                         RepositoryEntry<GroundNumericEffect<OpScaleUp, FluentTag>>,
                                         RepositoryEntry<GroundNumericEffect<OpScaleDown, FluentTag>>,
                                         RepositoryEntry<GroundNumericEffect<OpIncrease, AuxiliaryTag>>,
                                         RepositoryEntry<ConditionalEffect>,
                                         RepositoryEntry<GroundConditionalEffect>,
                                         RepositoryEntry<ConjunctiveEffect>,
                                         RepositoryEntry<GroundConjunctiveEffect>,
                                         RepositoryEntry<Action>,
                                         RepositoryEntry<GroundAction>,
                                         RepositoryEntry<Axiom>,
                                         RepositoryEntry<GroundAxiom>,
                                         RepositoryEntry<Metric>,
                                         RepositoryEntry<Domain>,
                                         RepositoryEntry<Task>,
                                         RepositoryEntry<FDRVariable<FluentTag>>,
                                         RepositoryEntry<FDRVariable<DerivedTag>>,
                                         RepositoryEntry<FDRFact<FluentTag>>,
                                         RepositoryEntry<FDRFact<DerivedTag>>,
                                         RepositoryEntry<ConjunctiveCondition>,
                                         RepositoryEntry<GroundConjunctiveCondition>,
                                         RepositoryEntry<FDRTask>>;

    RepositoryStorage m_repository;

    const Repository* m_parent;

    template<typename T>
    void initialize_entry(RepositoryEntry<T>& entry)
    {
        entry.slot.parent_size = m_parent ? m_parent->template size<T>() : 0;
    }

    void initialize_entries()
    {
        std::apply([&](auto&... e) { (initialize_entry(e), ...); }, m_repository);
    }

    void clear_entries() noexcept
    {
        std::apply([](auto&... entry) { (entry.slot.container.clear(), ...); }, m_repository);
    }

public:
    Repository(const Repository* parent = nullptr) : m_parent(parent) { initialize_entries(); }

    template<typename T>
    std::optional<Index<T>> find_with_hash(const Data<T>& builder, size_t h) const noexcept
    {
        const auto& entry = std::get<RepositoryEntry<T>>(m_repository);
        const auto& indexed_hash_set = entry.slot.container;
        assert(h == indexed_hash_set.hash(builder) && "The given hash does not match container internal's hash.");

        if (auto ptr = indexed_hash_set.find_with_hash(builder, h))
        {
            return ptr->index;
        }

        return m_parent ? m_parent->template find_with_hash<T>(builder, h) : std::nullopt;
    }

    template<typename T>
    std::optional<Index<T>> find(const Data<T>& builder) const noexcept
    {
        const auto& entry = std::get<RepositoryEntry<T>>(m_repository);
        const auto& indexed_hash_set = entry.slot.container;
        const auto h = indexed_hash_set.hash(builder);

        return find_with_hash<T>(builder, h);
    }

    template<typename T>
    std::pair<Index<T>, bool> get_or_create(Data<T>& builder, buffer::Buffer& buf)
    {
        auto& entry = std::get<RepositoryEntry<T>>(m_repository);
        auto& indexed_hash_set = entry.slot.container;
        const auto h = indexed_hash_set.hash(builder);

        if (m_parent)
        {
            if (auto ptr = m_parent->template find_with_hash<T>(builder, h))
            {
                return { *ptr, false };
            }
        }

        // Manually assign index to continue indexing.
        builder.index.value = entry.slot.parent_size + indexed_hash_set.size();

        const auto [ptr, success] = indexed_hash_set.insert_with_hash(h, builder, buf);

        return { ptr->index, success };
    }

    /// @brief Access the element with the given index.
    template<typename T>
    const Data<T>& operator[](Index<T> index) const noexcept
    {
        assert(index != Index<T>::max() && "Unassigned index.");

        const auto& entry = std::get<RepositoryEntry<T>>(m_repository);
        const auto parent_size = entry.slot.parent_size;

        // In parent range -> delegate
        if (index.value < parent_size)
        {
            assert(m_parent);
            return (*m_parent)[index];
        }

        // Local range -> shift down
        index.value -= parent_size;

        return entry.slot.container[index];
    }

    template<typename T>
    const Data<T>& front() const
    {
        const auto& entry = std::get<RepositoryEntry<T>>(m_repository);

        if (entry.slot.parent_size > 0)
        {
            assert(m_parent);
            return m_parent->template front<T>();  // recurse to root-most non-empty
        }

        return entry.slot.container.front();
    }

    /// @brief Get the number of stored elements.
    template<typename T>
    size_t size() const noexcept
    {
        const auto& entry = std::get<RepositoryEntry<T>>(m_repository);

        return entry.slot.parent_size + entry.slot.container.size();
    }

    /// @brief Clear the repository but keep memory allocated.
    void clear() noexcept
    {
        clear_entries();
        initialize_entries();
    }
};

static_assert(RepositoryConcept<Repository>);

static_assert(Context<Repository>);

}

#endif
