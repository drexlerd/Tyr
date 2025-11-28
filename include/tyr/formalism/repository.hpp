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
#include "tyr/formalism/arithmetic_operator_data.hpp"
#include "tyr/formalism/arithmetic_operator_view.hpp"
#include "tyr/formalism/atom_data.hpp"
#include "tyr/formalism/atom_index.hpp"
#include "tyr/formalism/binary_operator_data.hpp"
#include "tyr/formalism/binary_operator_index.hpp"
#include "tyr/formalism/boolean_operator_data.hpp"
#include "tyr/formalism/boolean_operator_view.hpp"
#include "tyr/formalism/conjunctive_condition_data.hpp"
#include "tyr/formalism/conjunctive_condition_index.hpp"
#include "tyr/formalism/function_data.hpp"
#include "tyr/formalism/function_expression_data.hpp"
#include "tyr/formalism/function_expression_view.hpp"
#include "tyr/formalism/function_index.hpp"
#include "tyr/formalism/function_term_data.hpp"
#include "tyr/formalism/function_term_index.hpp"
#include "tyr/formalism/ground_atom_data.hpp"
#include "tyr/formalism/ground_atom_index.hpp"
#include "tyr/formalism/ground_conjunctive_condition_data.hpp"
#include "tyr/formalism/ground_conjunctive_condition_index.hpp"
#include "tyr/formalism/ground_function_expression_data.hpp"
#include "tyr/formalism/ground_function_expression_view.hpp"
#include "tyr/formalism/ground_function_term_data.hpp"
#include "tyr/formalism/ground_function_term_index.hpp"
#include "tyr/formalism/ground_function_term_value_data.hpp"
#include "tyr/formalism/ground_function_term_value_index.hpp"
#include "tyr/formalism/ground_literal_data.hpp"
#include "tyr/formalism/ground_literal_index.hpp"
#include "tyr/formalism/ground_rule_data.hpp"
#include "tyr/formalism/ground_rule_index.hpp"
#include "tyr/formalism/literal_data.hpp"
#include "tyr/formalism/literal_index.hpp"
#include "tyr/formalism/multi_operator_data.hpp"
#include "tyr/formalism/multi_operator_index.hpp"
#include "tyr/formalism/object_data.hpp"
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/planning/action_data.hpp"
#include "tyr/formalism/planning/action_index.hpp"
#include "tyr/formalism/planning/axiom_data.hpp"
#include "tyr/formalism/planning/axiom_index.hpp"
#include "tyr/formalism/planning/conditional_effect_data.hpp"
#include "tyr/formalism/planning/conditional_effect_index.hpp"
#include "tyr/formalism/planning/conjunctive_effect_data.hpp"
#include "tyr/formalism/planning/conjunctive_effect_index.hpp"
#include "tyr/formalism/planning/ground_action_data.hpp"
#include "tyr/formalism/planning/ground_action_index.hpp"
#include "tyr/formalism/planning/ground_axiom_data.hpp"
#include "tyr/formalism/planning/ground_axiom_index.hpp"
#include "tyr/formalism/planning/ground_conditional_effect_data.hpp"
#include "tyr/formalism/planning/ground_conditional_effect_index.hpp"
#include "tyr/formalism/planning/ground_conjunctive_effect_data.hpp"
#include "tyr/formalism/planning/ground_conjunctive_effect_index.hpp"
#include "tyr/formalism/planning/ground_numeric_effect_data.hpp"
#include "tyr/formalism/planning/ground_numeric_effect_index.hpp"
#include "tyr/formalism/planning/numeric_effect_data.hpp"
#include "tyr/formalism/planning/numeric_effect_index.hpp"
#include "tyr/formalism/planning/task_data.hpp"
#include "tyr/formalism/planning/task_index.hpp"
#include "tyr/formalism/predicate_data.hpp"
#include "tyr/formalism/predicate_index.hpp"
#include "tyr/formalism/program_data.hpp"
#include "tyr/formalism/program_index.hpp"
#include "tyr/formalism/rule_data.hpp"
#include "tyr/formalism/rule_index.hpp"
#include "tyr/formalism/unary_operator_data.hpp"
#include "tyr/formalism/unary_operator_index.hpp"
#include "tyr/formalism/variable_data.hpp"
#include "tyr/formalism/variable_index.hpp"
//
#include "tyr/buffer/declarations.hpp"
#include "tyr/buffer/indexed_hash_set.hpp"
#include "tyr/formalism/declarations.hpp"

#include <boost/hana.hpp>

namespace tyr::formalism
{

class Repository
{
private:
    template<typename T>
    using RepositoryEntry = boost::hana::pair<boost::hana::type<T>, buffer::IndexedHashSet<T>>;

    using HanaRepository = boost::hana::map<RepositoryEntry<Variable>,
                                            RepositoryEntry<Object>,
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
                                            RepositoryEntry<BinaryOperator<OpLe, Data<GroundFunctionExpression>>>,
                                            RepositoryEntry<BinaryOperator<OpLt, Data<GroundFunctionExpression>>>,
                                            RepositoryEntry<BinaryOperator<OpGe, Data<GroundFunctionExpression>>>,
                                            RepositoryEntry<BinaryOperator<OpGt, Data<GroundFunctionExpression>>>,
                                            RepositoryEntry<ConjunctiveCondition>,
                                            RepositoryEntry<Rule>,
                                            RepositoryEntry<GroundConjunctiveCondition>,
                                            RepositoryEntry<GroundRule>,
                                            RepositoryEntry<Program>>;

    HanaRepository m_repository;

public:
    Repository() = default;

    template<typename T>
    std::optional<View<Index<T>, Repository>> find(const Data<T>& builder) const
    {
        const auto& indexed_hash_set = boost::hana::at_key(m_repository, boost::hana::type<T> {});

        if (const auto ptr = indexed_hash_set.find(builder))
            return View<Index<T>, Repository>(ptr->index, *this);

        return std::nullopt;
    }

    template<typename T, bool AssignIndex = true>
    std::pair<View<Index<T>, Repository>, bool> get_or_create(Data<T>& builder, buffer::Buffer& buf)
    {
        auto& indexed_hash_set = boost::hana::at_key(m_repository, boost::hana::type<T> {});

        if constexpr (AssignIndex)
            builder.index.value = indexed_hash_set.size();

        const auto [ptr, success] = indexed_hash_set.insert(builder, buf);

        return std::make_pair(View<Index<T>, Repository>(ptr->index, *this), success);
    }

    /// @brief Access the element with the given index.
    template<typename T>
    const Data<T>& operator[](Index<T> index) const
    {
        const auto& repository = boost::hana::at_key(m_repository, boost::hana::type<T> {});

        return repository[index];
    }

    /// @brief Get the number of stored elements.
    template<typename T>
    size_t size() const
    {
        const auto& repository = boost::hana::at_key(m_repository, boost::hana::type<T> {});

        return repository.size();
    }

    /// @brief Clear the repository but keep memory allocated.
    void clear() noexcept
    {
        boost::hana::for_each(m_repository,
                              [](auto&& pair)
                              {
                                  auto& indexed_hash_set = boost::hana::second(pair);
                                  indexed_hash_set.clear();
                              });
    }
};

/// @brief Make Repository a trivial context.
/// @param context
/// @return
inline const Repository& get_repository(const Repository& context) noexcept { return context; }

static_assert(IsRepository<Repository>);

static_assert(IsContext<Repository>);
}

#endif
