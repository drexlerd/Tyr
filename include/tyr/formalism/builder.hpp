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

#ifndef TYR_FORMALISM_BUILDER_HPP_
#define TYR_FORMALISM_BUILDER_HPP_

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
#include "tyr/formalism/planning/action_view.hpp"
#include "tyr/formalism/planning/axiom_data.hpp"
#include "tyr/formalism/planning/axiom_index.hpp"
#include "tyr/formalism/planning/axiom_view.hpp"
#include "tyr/formalism/planning/conditional_effect_data.hpp"
#include "tyr/formalism/planning/conditional_effect_index.hpp"
#include "tyr/formalism/planning/conditional_effect_view.hpp"
#include "tyr/formalism/planning/conjunctive_effect_data.hpp"
#include "tyr/formalism/planning/conjunctive_effect_index.hpp"
#include "tyr/formalism/planning/conjunctive_effect_view.hpp"
#include "tyr/formalism/planning/domain_data.hpp"
#include "tyr/formalism/planning/domain_index.hpp"
#include "tyr/formalism/planning/domain_view.hpp"
#include "tyr/formalism/planning/ground_action_data.hpp"
#include "tyr/formalism/planning/ground_action_index.hpp"
#include "tyr/formalism/planning/ground_action_view.hpp"
#include "tyr/formalism/planning/ground_axiom_data.hpp"
#include "tyr/formalism/planning/ground_axiom_index.hpp"
#include "tyr/formalism/planning/ground_axiom_view.hpp"
#include "tyr/formalism/planning/ground_conditional_effect_data.hpp"
#include "tyr/formalism/planning/ground_conditional_effect_index.hpp"
#include "tyr/formalism/planning/ground_conditional_effect_view.hpp"
#include "tyr/formalism/planning/ground_conjunctive_effect_data.hpp"
#include "tyr/formalism/planning/ground_conjunctive_effect_index.hpp"
#include "tyr/formalism/planning/ground_conjunctive_effect_view.hpp"
#include "tyr/formalism/planning/ground_numeric_effect_data.hpp"
#include "tyr/formalism/planning/ground_numeric_effect_index.hpp"
#include "tyr/formalism/planning/ground_numeric_effect_view.hpp"
#include "tyr/formalism/planning/metric_data.hpp"
#include "tyr/formalism/planning/metric_index.hpp"
#include "tyr/formalism/planning/metric_view.hpp"
#include "tyr/formalism/planning/numeric_effect_data.hpp"
#include "tyr/formalism/planning/numeric_effect_index.hpp"
#include "tyr/formalism/planning/numeric_effect_view.hpp"
#include "tyr/formalism/planning/task_data.hpp"
#include "tyr/formalism/planning/task_index.hpp"
#include "tyr/formalism/planning/task_view.hpp"
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
#include "tyr/common/unique_object_pool.hpp"
#include "tyr/formalism/declarations.hpp"

namespace tyr::formalism
{
struct Builder
{
    Builder() = default;

    /**
     * Datalog
     */

    template<typename T>
    using BuilderEntry = boost::hana::pair<boost::hana::type<T>, UniqueObjectPool<Data<T>>>;

    using HanaBuilder = boost::hana::map<BuilderEntry<Variable>,
                                         BuilderEntry<Object>,
                                         BuilderEntry<Predicate<StaticTag>>,
                                         BuilderEntry<Predicate<FluentTag>>,
                                         BuilderEntry<Predicate<DerivedTag>>,
                                         BuilderEntry<Atom<StaticTag>>,
                                         BuilderEntry<Atom<FluentTag>>,
                                         BuilderEntry<Atom<DerivedTag>>,
                                         BuilderEntry<GroundAtom<StaticTag>>,
                                         BuilderEntry<GroundAtom<FluentTag>>,
                                         BuilderEntry<GroundAtom<DerivedTag>>,
                                         BuilderEntry<Literal<StaticTag>>,
                                         BuilderEntry<Literal<FluentTag>>,
                                         BuilderEntry<Literal<DerivedTag>>,
                                         BuilderEntry<GroundLiteral<StaticTag>>,
                                         BuilderEntry<GroundLiteral<FluentTag>>,
                                         BuilderEntry<GroundLiteral<DerivedTag>>,
                                         BuilderEntry<Function<StaticTag>>,
                                         BuilderEntry<Function<FluentTag>>,
                                         BuilderEntry<Function<AuxiliaryTag>>,
                                         BuilderEntry<FunctionTerm<StaticTag>>,
                                         BuilderEntry<FunctionTerm<FluentTag>>,
                                         BuilderEntry<FunctionTerm<AuxiliaryTag>>,
                                         BuilderEntry<GroundFunctionTerm<StaticTag>>,
                                         BuilderEntry<GroundFunctionTerm<FluentTag>>,
                                         BuilderEntry<GroundFunctionTerm<AuxiliaryTag>>,
                                         BuilderEntry<GroundFunctionTermValue<StaticTag>>,
                                         BuilderEntry<GroundFunctionTermValue<FluentTag>>,
                                         BuilderEntry<GroundFunctionTermValue<AuxiliaryTag>>,
                                         BuilderEntry<UnaryOperator<OpSub, Data<FunctionExpression>>>,
                                         BuilderEntry<BinaryOperator<OpAdd, Data<FunctionExpression>>>,
                                         BuilderEntry<BinaryOperator<OpSub, Data<FunctionExpression>>>,
                                         BuilderEntry<BinaryOperator<OpMul, Data<FunctionExpression>>>,
                                         BuilderEntry<BinaryOperator<OpDiv, Data<FunctionExpression>>>,
                                         BuilderEntry<MultiOperator<OpAdd, Data<FunctionExpression>>>,
                                         BuilderEntry<MultiOperator<OpMul, Data<FunctionExpression>>>,
                                         BuilderEntry<BinaryOperator<OpEq, Data<FunctionExpression>>>,
                                         BuilderEntry<BinaryOperator<OpLe, Data<FunctionExpression>>>,
                                         BuilderEntry<BinaryOperator<OpLt, Data<FunctionExpression>>>,
                                         BuilderEntry<BinaryOperator<OpGe, Data<FunctionExpression>>>,
                                         BuilderEntry<BinaryOperator<OpGt, Data<FunctionExpression>>>,
                                         BuilderEntry<UnaryOperator<OpSub, Data<GroundFunctionExpression>>>,
                                         BuilderEntry<BinaryOperator<OpAdd, Data<GroundFunctionExpression>>>,
                                         BuilderEntry<BinaryOperator<OpSub, Data<GroundFunctionExpression>>>,
                                         BuilderEntry<BinaryOperator<OpMul, Data<GroundFunctionExpression>>>,
                                         BuilderEntry<BinaryOperator<OpDiv, Data<GroundFunctionExpression>>>,
                                         BuilderEntry<MultiOperator<OpAdd, Data<GroundFunctionExpression>>>,
                                         BuilderEntry<MultiOperator<OpMul, Data<GroundFunctionExpression>>>,
                                         BuilderEntry<BinaryOperator<OpEq, Data<GroundFunctionExpression>>>,
                                         BuilderEntry<BinaryOperator<OpLe, Data<GroundFunctionExpression>>>,
                                         BuilderEntry<BinaryOperator<OpLt, Data<GroundFunctionExpression>>>,
                                         BuilderEntry<BinaryOperator<OpGe, Data<GroundFunctionExpression>>>,
                                         BuilderEntry<BinaryOperator<OpGt, Data<GroundFunctionExpression>>>,
                                         BuilderEntry<ConjunctiveCondition>,
                                         BuilderEntry<Rule>,
                                         BuilderEntry<GroundConjunctiveCondition>,
                                         BuilderEntry<GroundRule>,
                                         BuilderEntry<Program>,
                                         BuilderEntry<NumericEffect<OpAssign, FluentTag>>,
                                         BuilderEntry<NumericEffect<OpIncrease, FluentTag>>,
                                         BuilderEntry<NumericEffect<OpDecrease, FluentTag>>,
                                         BuilderEntry<NumericEffect<OpScaleUp, FluentTag>>,
                                         BuilderEntry<NumericEffect<OpScaleDown, FluentTag>>,
                                         BuilderEntry<NumericEffect<OpIncrease, AuxiliaryTag>>,
                                         BuilderEntry<GroundNumericEffect<OpAssign, FluentTag>>,
                                         BuilderEntry<GroundNumericEffect<OpIncrease, FluentTag>>,
                                         BuilderEntry<GroundNumericEffect<OpDecrease, FluentTag>>,
                                         BuilderEntry<GroundNumericEffect<OpScaleUp, FluentTag>>,
                                         BuilderEntry<GroundNumericEffect<OpScaleDown, FluentTag>>,
                                         BuilderEntry<GroundNumericEffect<OpIncrease, AuxiliaryTag>>,
                                         BuilderEntry<ConditionalEffect>,
                                         BuilderEntry<GroundConditionalEffect>,
                                         BuilderEntry<ConjunctiveEffect>,
                                         BuilderEntry<GroundConjunctiveEffect>,
                                         BuilderEntry<Action>,
                                         BuilderEntry<GroundAction>,
                                         BuilderEntry<Axiom>,
                                         BuilderEntry<GroundAxiom>,
                                         BuilderEntry<Metric>,
                                         BuilderEntry<Domain>,
                                         BuilderEntry<Task>>;

    HanaBuilder m_builder;

    template<typename T>
    [[nodiscard]] auto get_builder()
    {
        return boost::hana::at_key(m_builder, boost::hana::type<T> {}).get_or_allocate();
    }

    buffer::Buffer buffer;

    auto& get_buffer() noexcept { return buffer; }
};

}

#endif
