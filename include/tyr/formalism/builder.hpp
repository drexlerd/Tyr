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
#include "tyr/formalism/declarations.hpp"

namespace tyr::formalism
{
struct Builder
{
    Builder() = default;

    // ----- Operators over FunctionExpression -----
    Data<UnaryOperator<OpSub, Data<FunctionExpression>>> unary_sub;
    Data<BinaryOperator<OpAdd, Data<FunctionExpression>>> binary_add;
    Data<BinaryOperator<OpSub, Data<FunctionExpression>>> binary_sub;
    Data<BinaryOperator<OpMul, Data<FunctionExpression>>> binary_mul;
    Data<BinaryOperator<OpDiv, Data<FunctionExpression>>> binary_div;
    Data<BinaryOperator<OpEq, Data<FunctionExpression>>> binary_eq;
    Data<BinaryOperator<OpNe, Data<FunctionExpression>>> binary_ne;
    Data<BinaryOperator<OpLe, Data<FunctionExpression>>> binary_le;
    Data<BinaryOperator<OpLt, Data<FunctionExpression>>> binary_lt;
    Data<BinaryOperator<OpGe, Data<FunctionExpression>>> binary_ge;
    Data<BinaryOperator<OpGt, Data<FunctionExpression>>> binary_gt;
    Data<MultiOperator<OpAdd, Data<FunctionExpression>>> multi_add;
    Data<MultiOperator<OpMul, Data<FunctionExpression>>> multi_mul;
    Data<BooleanOperator<Data<FunctionExpression>>> boolean;
    Data<ArithmeticOperator<Data<FunctionExpression>>> arithmetic;

    Data<UnaryOperator<OpSub, Data<GroundFunctionExpression>>> ground_unary_sub;
    Data<BinaryOperator<OpAdd, Data<GroundFunctionExpression>>> ground_binary_add;
    Data<BinaryOperator<OpSub, Data<GroundFunctionExpression>>> ground_binary_sub;
    Data<BinaryOperator<OpMul, Data<GroundFunctionExpression>>> ground_binary_mul;
    Data<BinaryOperator<OpDiv, Data<GroundFunctionExpression>>> ground_binary_div;
    Data<BinaryOperator<OpEq, Data<GroundFunctionExpression>>> ground_binary_eq;
    Data<BinaryOperator<OpNe, Data<GroundFunctionExpression>>> ground_binary_ne;
    Data<BinaryOperator<OpLe, Data<GroundFunctionExpression>>> ground_binary_le;
    Data<BinaryOperator<OpLt, Data<GroundFunctionExpression>>> ground_binary_lt;
    Data<BinaryOperator<OpGe, Data<GroundFunctionExpression>>> ground_binary_ge;
    Data<BinaryOperator<OpGt, Data<GroundFunctionExpression>>> ground_binary_gt;
    Data<MultiOperator<OpAdd, Data<GroundFunctionExpression>>> ground_multi_add;
    Data<MultiOperator<OpMul, Data<GroundFunctionExpression>>> ground_multi_mul;
    Data<BooleanOperator<Data<GroundFunctionExpression>>> ground_boolean;
    Data<ArithmeticOperator<Data<GroundFunctionExpression>>> ground_arithmetic;

    // ----- Basic symbols -----
    Data<Variable> variable;
    Data<Object> object;
    Data<Term> term;

    // ----- Predicates / Atoms / Literals -----
    Data<Predicate<StaticTag>> static_predicate;
    Data<Predicate<FluentTag>> fluent_predicate;

    Data<Atom<StaticTag>> static_atom;
    Data<Atom<FluentTag>> fluent_atom;
    Data<Literal<StaticTag>> static_literal;
    Data<Literal<FluentTag>> fluent_literal;

    Data<GroundAtom<StaticTag>> ground_static_atom;
    Data<GroundAtom<FluentTag>> ground_fluent_atom;
    Data<GroundLiteral<StaticTag>> ground_static_literal;
    Data<GroundLiteral<FluentTag>> ground_fluent_literal;

    // ----- Functions / Function terms -----
    Data<Function<StaticTag>> static_function;
    Data<Function<FluentTag>> fluent_function;

    Data<FunctionTerm<StaticTag>> static_fterm;
    Data<FunctionTerm<FluentTag>> fluent_fterm;
    Data<GroundFunctionTerm<StaticTag>> ground_static_fterm;
    Data<GroundFunctionTerm<FluentTag>> ground_fluent_fterm;

    // ----- Function expressions -----
    Data<FunctionExpression> fexpr;
    Data<GroundFunctionExpression> ground_fexpr;

    // ----- Function term values -----
    Data<GroundFunctionTermValue<StaticTag>> ground_static_fterm_value;
    Data<GroundFunctionTermValue<FluentTag>> ground_fluent_fterm_value;

    // ----- Conditions, rules, program -----
    Data<ConjunctiveCondition> conj_cond;
    Data<GroundConjunctiveCondition> ground_conj_cond;

    Data<Rule> rule;
    Data<GroundRule> ground_rule;

    Data<Program> program;

    // ================== Operators ==================

    template<IsOp O>
    auto& get_unary() noexcept
    {
        if constexpr (std::is_same_v<O, OpSub>)
            return unary_sub;
        else
            static_assert(dependent_false<O>::value, "Missing Builder for the given types.");
    }

    template<IsOp O>
    auto& get_ground_unary() noexcept
    {
        if constexpr (std::is_same_v<O, OpSub>)
            return ground_unary_sub;
        else
            static_assert(dependent_false<O>::value, "Missing Builder for the given types.");
    }

    template<IsOp O>
    auto& get_binary() noexcept
    {
        if constexpr (std::is_same_v<O, OpAdd>)
            return binary_add;
        else if constexpr (std::is_same_v<O, OpSub>)
            return binary_sub;
        else if constexpr (std::is_same_v<O, OpMul>)
            return binary_mul;
        else if constexpr (std::is_same_v<O, OpDiv>)
            return binary_div;
        else if constexpr (std::is_same_v<O, OpEq>)
            return binary_eq;
        else if constexpr (std::is_same_v<O, OpNe>)
            return binary_ne;
        else if constexpr (std::is_same_v<O, OpLe>)
            return binary_le;
        else if constexpr (std::is_same_v<O, OpLt>)
            return binary_lt;
        else if constexpr (std::is_same_v<O, OpGe>)
            return binary_ge;
        else if constexpr (std::is_same_v<O, OpGt>)
            return binary_gt;
        else
            static_assert(dependent_false<O>::value, "Missing Builder for the given types.");
    }

    template<IsOp O>
    auto& get_ground_binary() noexcept
    {
        if constexpr (std::is_same_v<O, OpAdd>)
            return ground_binary_add;
        else if constexpr (std::is_same_v<O, OpSub>)
            return ground_binary_sub;
        else if constexpr (std::is_same_v<O, OpMul>)
            return ground_binary_mul;
        else if constexpr (std::is_same_v<O, OpDiv>)
            return ground_binary_div;
        else if constexpr (std::is_same_v<O, OpEq>)
            return ground_binary_eq;
        else if constexpr (std::is_same_v<O, OpNe>)
            return ground_binary_ne;
        else if constexpr (std::is_same_v<O, OpLe>)
            return ground_binary_le;
        else if constexpr (std::is_same_v<O, OpLt>)
            return ground_binary_lt;
        else if constexpr (std::is_same_v<O, OpGe>)
            return ground_binary_ge;
        else if constexpr (std::is_same_v<O, OpGt>)
            return ground_binary_gt;
        else
            static_assert(dependent_false<O>::value, "Missing Builder for the given types.");
    }

    template<IsOp O>
    auto& get_multi() noexcept
    {
        if constexpr (std::is_same_v<O, OpAdd>)
            return multi_add;
        else if constexpr (std::is_same_v<O, OpMul>)
            return multi_mul;
        else
            static_assert(dependent_false<O>::value, "Missing Builder for the given types.");
    }

    template<IsOp O>
    auto& get_ground_multi() noexcept
    {
        if constexpr (std::is_same_v<O, OpAdd>)
            return ground_multi_add;
        else if constexpr (std::is_same_v<O, OpMul>)
            return ground_multi_mul;
        else
            static_assert(dependent_false<O>::value, "Missing Builder for the given types.");
    }

    auto& get_boolean() noexcept { return boolean; }

    auto& get_ground_boolean() noexcept { return ground_boolean; }

    auto& get_arithmetic() noexcept { return arithmetic; }

    auto& get_ground_arithmetic() noexcept { return ground_arithmetic; }

    // ================== Basic symbols ==================

    auto& get_variable() noexcept { return variable; }

    auto& get_object() noexcept { return object; }

    auto& get_term() noexcept { return term; }

    // ================== Predicates / Atoms / Literals ==================

    template<IsFactTag T>
    auto& get_predicate() noexcept
    {
        if constexpr (std::is_same_v<T, StaticTag>)
            return static_predicate;
        else if constexpr (std::is_same_v<T, FluentTag>)
            return fluent_predicate;
        else
            static_assert(dependent_false<T>::value, "Missing Builder for the given types.");
    }

    template<IsFactTag T>
    auto& get_atom() noexcept
    {
        if constexpr (std::is_same_v<T, StaticTag>)
            return static_atom;
        else if constexpr (std::is_same_v<T, FluentTag>)
            return fluent_atom;
        else
            static_assert(dependent_false<T>::value, "Missing Builder for the given types.");
    }

    template<IsFactTag T>
    auto& get_ground_atom() noexcept
    {
        if constexpr (std::is_same_v<T, StaticTag>)
            return ground_static_atom;
        else if constexpr (std::is_same_v<T, FluentTag>)
            return ground_fluent_atom;
        else
            static_assert(dependent_false<T>::value, "Missing Builder for the given types.");
    }

    template<IsFactTag T>
    auto& get_literal() noexcept
    {
        if constexpr (std::is_same_v<T, StaticTag>)
            return static_literal;
        else if constexpr (std::is_same_v<T, FluentTag>)
            return fluent_literal;
        else
            static_assert(dependent_false<T>::value, "Missing Builder for the given types.");
    }

    template<IsFactTag T>
    auto& get_ground_literal() noexcept
    {
        if constexpr (std::is_same_v<T, StaticTag>)
            return ground_static_literal;
        else if constexpr (std::is_same_v<T, FluentTag>)
            return ground_fluent_literal;
        else
            static_assert(dependent_false<T>::value, "Missing Builder for the given types.");
    }

    // ================== Functions / Terms / Values ==================

    template<IsFactTag T>
    auto& get_function() noexcept
    {
        if constexpr (std::is_same_v<T, StaticTag>)
            return static_function;
        else if constexpr (std::is_same_v<T, FluentTag>)
            return fluent_function;
        else
            static_assert(dependent_false<T>::value, "Missing Builder for the given types.");
    }

    template<IsFactTag T>
    auto& get_fterm() noexcept
    {
        if constexpr (std::is_same_v<T, StaticTag>)
            return static_fterm;
        else if constexpr (std::is_same_v<T, FluentTag>)
            return fluent_fterm;
        else
            static_assert(dependent_false<T>::value, "Missing Builder for the given types.");
    }

    template<IsFactTag T>
    auto& get_ground_fterm() noexcept
    {
        if constexpr (std::is_same_v<T, StaticTag>)
            return ground_static_fterm;
        else if constexpr (std::is_same_v<T, FluentTag>)
            return ground_fluent_fterm;
        else
            static_assert(dependent_false<T>::value, "Missing Builder for the given types.");
    }

    template<IsFactTag T>
    auto& get_ground_fterm_value() noexcept
    {
        if constexpr (std::is_same_v<T, StaticTag>)
            return ground_static_fterm_value;
        else if constexpr (std::is_same_v<T, FluentTag>)
            return ground_fluent_fterm_value;
        else
            static_assert(dependent_false<T>::value, "Missing Builder for the given types.");
    }

    // ================== Expressions ==================

    auto& get_fexpr() noexcept { return fexpr; }
    auto& get_ground_fexpr() noexcept { return ground_fexpr; }

    // ================== Conditions / Rules / Program ==================

    auto& get_conj_cond() noexcept { return conj_cond; }
    auto& get_ground_conj_cond() noexcept { return ground_conj_cond; }

    auto& get_rule() noexcept { return rule; }
    auto& get_ground_rule() noexcept { return ground_rule; }

    auto& get_program() noexcept { return program; }
};

}

#endif
