

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

#ifndef TYR_FORMALISM_CANONICALIZATION_HPP_
#define TYR_FORMALISM_CANONICALIZATION_HPP_

#include "tyr/formalism/arithmetic_operator_data.hpp"
#include "tyr/formalism/atom_data.hpp"
#include "tyr/formalism/binary_operator_data.hpp"
#include "tyr/formalism/boolean_operator_data.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function_data.hpp"
#include "tyr/formalism/function_expression_data.hpp"
#include "tyr/formalism/function_term_data.hpp"
#include "tyr/formalism/ground_atom_data.hpp"
#include "tyr/formalism/ground_function_expression_data.hpp"
#include "tyr/formalism/ground_function_term_data.hpp"
#include "tyr/formalism/ground_function_term_value_data.hpp"
#include "tyr/formalism/ground_literal_data.hpp"
#include "tyr/formalism/ground_rule_data.hpp"
#include "tyr/formalism/literal_data.hpp"
#include "tyr/formalism/multi_operator_data.hpp"
#include "tyr/formalism/object_data.hpp"
#include "tyr/formalism/predicate_data.hpp"
#include "tyr/formalism/program_data.hpp"
#include "tyr/formalism/rule_data.hpp"
#include "tyr/formalism/term_data.hpp"
#include "tyr/formalism/unary_operator_data.hpp"
#include "tyr/formalism/variable_data.hpp"

#include <algorithm>

namespace tyr::formalism
{

/**
 *
 */

template<typename T>
bool is_canonical(const IndexList<T>& list)
{
    return std::is_sorted(list.begin(), list.end());
}

template<typename T>
bool is_canonical(const DataList<T>& list)
{
    return std::is_sorted(list.begin(), list.end());
}

template<IsOp Op, typename T>
bool is_canonical(const Data<UnaryOperator<Op, T>>& data)
{
    return true;
}

template<IsOp Op, typename T>
bool is_canonical(const Data<BinaryOperator<Op, T>>& data)
{
    return true;
}

template<typename T>
bool is_canonical(const Data<BinaryOperator<OpAdd, T>>& data)
{
    return data.lhs <= data.rhs;
}

template<typename T>
bool is_canonical(const Data<BinaryOperator<OpMul, T>>& data)
{
    return data.lhs <= data.rhs;
}

template<IsOp Op, typename T>
bool is_canonical(const Data<MultiOperator<Op, T>>& data)
{
    return true;
}

template<typename T>
bool is_canonical(const Data<MultiOperator<OpAdd, T>>& data)
{
    return is_canonical(data.args);
}

template<typename T>
bool is_canonical(const Data<MultiOperator<OpMul, T>>& data)
{
    return is_canonical(data.args);
}

template<typename T>
bool is_canonical(const Data<BooleanOperator<T>>& data)
{
    return true;
}

template<typename T>
bool is_canonical(const Data<ArithmeticOperator<T>>& data)
{
    return true;
}

bool is_canonical(const Data<Variable>& data) { return true; }

bool is_canonical(const Data<Object>& data) { return true; }

bool is_canonical(const Data<Term>& data) { return true; }

template<IsFactTag T>
bool is_canonical(const Data<Predicate<T>>& data)
{
    return true;
}

template<IsFactTag T>
bool is_canonical(const Data<Atom<T>>& data)
{
    return true;
}

template<IsFactTag T>
bool is_canonical(const Data<Literal<T>>& data)
{
    return true;
}

template<IsFactTag T>
bool is_canonical(const Data<GroundAtom<T>>& data)
{
    return true;
}

template<IsFactTag T>
bool is_canonical(const Data<GroundLiteral<T>>& data)
{
    return true;
}

template<IsFactTag T>
bool is_canonical(const Data<Function<T>>& data)
{
    return true;
}

template<IsFactTag T>
bool is_canonical(const Data<FunctionTerm<T>>& data)
{
    return true;
}

bool is_canonical(const Data<FunctionExpression>& data) { return true; }

template<IsFactTag T>
bool is_canonical(const Data<GroundFunctionTerm<T>>& data)
{
    return true;
}

bool is_canonical(const Data<GroundFunctionExpression>& data) { return true; }

template<IsFactTag T>
bool is_canonical(const Data<GroundFunctionTermValue<T>>& data)
{
    return true;
}

bool is_canonical(const Data<ConjunctiveCondition>& data)
{
    return is_canonical(data.static_literals) && is_canonical(data.fluent_literals) && is_canonical(data.numeric_constraints);
}

bool is_canonical(const Data<GroundConjunctiveCondition>& data)
{
    return is_canonical(data.static_literals) && is_canonical(data.fluent_literals) && is_canonical(data.numeric_constraints);
}

bool is_canonical(const Data<Rule>& data) { return true; }

bool is_canonical(const Data<GroundRule>& data) { return true; }

bool is_canonical(const Data<Program>& data)
{
    return is_canonical(data.static_predicates) && is_canonical(data.fluent_predicates) && is_canonical(data.static_functions)
           && is_canonical(data.fluent_functions) && is_canonical(data.objects) && is_canonical(data.static_atoms) && is_canonical(data.fluent_atoms)
           && is_canonical(data.static_fterm_values) && is_canonical(data.fluent_fterm_values) && is_canonical(data.rules);
}

/**
 * Canonicalize
 */

template<typename T>
void canonicalize(IndexList<T>& list)
{
    if (!is_canonical(list))
        std::sort(list.begin(), list.end());
}

template<typename T>
void canonicalize(DataList<T>& list)
{
    if (!is_canonical(list))
        std::sort(list.begin(), list.end());
}

template<IsOp Op, typename T>
void canonicalize(Data<UnaryOperator<Op, T>>& data)
{
    // Trivially canonical
}

template<IsOp Op, typename T>
void canonicalize(Data<BinaryOperator<Op, T>>& data)
{
    // Canonicalization for commutative operator in spezializations
}

template<typename T>
void canonicalize(Data<BinaryOperator<OpAdd, T>>& data)
{
    if (data.lhs > data.rhs)
        std::swap(data.lhs, data.rhs);
}

template<typename T>
void canonicalize(Data<BinaryOperator<OpMul, T>>& data)
{
    if (data.lhs > data.rhs)
        std::swap(data.lhs, data.rhs);
}

template<IsOp Op, typename T>
void canonicalize(Data<MultiOperator<Op, T>>& data)
{
    // Canonicalization for commutative operator in spezializations
}

template<typename T>
void canonicalize(Data<MultiOperator<OpAdd, T>>& data)
{
    canonicalize(data.args);
}

template<typename T>
void canonicalize(Data<MultiOperator<OpMul, T>>& data)
{
    canonicalize(data.args);
}

template<typename T>
void canonicalize(Data<BooleanOperator<T>>& data)
{
    // Trivially canonical
}

template<typename T>
void canonicalize(Data<ArithmeticOperator<T>>& data)
{
    // Trivially canonical
}

void canonicalize(Data<Variable>& data)
{  // Trivially canonical
}

void canonicalize(Data<Object>& data)
{
    // Trivially canonical
}

void canonicalize(Data<Term>& data)
{
    // Trivially canonical
}

template<IsFactTag T>
void canonicalize(Data<Predicate<T>>& data)
{
    // Trivially canonical
}

template<IsFactTag T>
void canonicalize(Data<Atom<T>>& data)
{
    // Trivially canonical
}

template<IsFactTag T>
void canonicalize(Data<Literal<T>>& data)
{
    // Trivially canonical
}

template<IsFactTag T>
void canonicalize(Data<GroundAtom<T>>& data)
{
    // Trivially canonical
}

template<IsFactTag T>
void canonicalize(Data<GroundLiteral<T>>& data)
{
    // Trivially canonical
}

template<IsFactTag T>
void canonicalize(Data<Function<T>>& data)
{
    // Trivially canonical
}

template<IsFactTag T>
void canonicalize(Data<FunctionTerm<T>>& data)
{
    // Trivially canonical
}

void canonicalize(Data<FunctionExpression>& data)
{
    // Trivially canonical
}

template<IsFactTag T>
void canonicalize(Data<GroundFunctionTerm<T>>& data)
{
    // Trivially canonical
}

void canonicalize(Data<GroundFunctionExpression>& data)
{
    // Trivially canonical
}

template<IsFactTag T>
void canonicalize(Data<GroundFunctionTermValue<T>>& data)
{
    // Trivially canonical
}

void canonicalize(Data<ConjunctiveCondition>& data)
{
    canonicalize(data.static_literals);
    canonicalize(data.fluent_literals);
    canonicalize(data.numeric_constraints);
}

void canonicalize(Data<GroundConjunctiveCondition>& data)
{
    canonicalize(data.static_literals);
    canonicalize(data.fluent_literals);
    canonicalize(data.numeric_constraints);
}

void canonicalize(Data<Rule>& data)
{
    // Trivially canonical
}

void canonicalize(Data<GroundRule>& data)
{
    // Trivially canonical
}

void canonicalize(Data<Program>& data)
{
    canonicalize(data.static_predicates);
    canonicalize(data.fluent_predicates);
    canonicalize(data.static_functions);
    canonicalize(data.fluent_functions);
    canonicalize(data.objects);
    canonicalize(data.static_atoms);
    canonicalize(data.fluent_atoms);
    canonicalize(data.static_fterm_values);
    canonicalize(data.fluent_fterm_values);
    canonicalize(data.rules);
}
}

#endif
