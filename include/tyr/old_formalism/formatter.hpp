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

#ifndef TYR_FORMALISM_FORMATTER_HPP_
#define TYR_FORMALISM_FORMATTER_HPP_

#include "tyr/common/formatter.hpp"
#include "tyr/formalism/arithmetic_operator.hpp"
#include "tyr/formalism/arithmetic_operator_proxy.hpp"
#include "tyr/formalism/atom.hpp"
#include "tyr/formalism/atom_index.hpp"
#include "tyr/formalism/atom_proxy.hpp"
#include "tyr/formalism/binary_operator.hpp"
#include "tyr/formalism/binary_operator_index.hpp"
#include "tyr/formalism/binary_operator_proxy.hpp"
#include "tyr/formalism/boolean_operator.hpp"
#include "tyr/formalism/boolean_operator_proxy.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/double.hpp"
#include "tyr/formalism/function.hpp"
#include "tyr/formalism/function_expression.hpp"
#include "tyr/formalism/function_expression_proxy.hpp"
#include "tyr/formalism/function_index.hpp"
#include "tyr/formalism/function_proxy.hpp"
#include "tyr/formalism/function_term.hpp"
#include "tyr/formalism/function_term_index.hpp"
#include "tyr/formalism/function_term_proxy.hpp"
#include "tyr/formalism/ground_atom.hpp"
#include "tyr/formalism/ground_atom_index.hpp"
#include "tyr/formalism/ground_atom_proxy.hpp"
#include "tyr/formalism/ground_function_expression.hpp"
#include "tyr/formalism/ground_function_expression_proxy.hpp"
#include "tyr/formalism/ground_function_term.hpp"
#include "tyr/formalism/ground_function_term_index.hpp"
#include "tyr/formalism/ground_function_term_proxy.hpp"
#include "tyr/formalism/ground_function_term_value.hpp"
#include "tyr/formalism/ground_function_term_value_index.hpp"
#include "tyr/formalism/ground_function_term_value_proxy.hpp"
#include "tyr/formalism/ground_literal.hpp"
#include "tyr/formalism/ground_literal_index.hpp"
#include "tyr/formalism/ground_literal_proxy.hpp"
#include "tyr/formalism/ground_rule.hpp"
#include "tyr/formalism/ground_rule_index.hpp"
#include "tyr/formalism/ground_rule_proxy.hpp"
#include "tyr/formalism/literal.hpp"
#include "tyr/formalism/literal_index.hpp"
#include "tyr/formalism/literal_proxy.hpp"
#include "tyr/formalism/multi_operator.hpp"
#include "tyr/formalism/multi_operator_index.hpp"
#include "tyr/formalism/multi_operator_proxy.hpp"
#include "tyr/formalism/object.hpp"
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/object_proxy.hpp"
#include "tyr/formalism/parameter_index.hpp"
#include "tyr/formalism/predicate.hpp"
#include "tyr/formalism/predicate_index.hpp"
#include "tyr/formalism/predicate_proxy.hpp"
#include "tyr/formalism/program.hpp"
#include "tyr/formalism/program_index.hpp"
#include "tyr/formalism/program_proxy.hpp"
#include "tyr/formalism/rule.hpp"
#include "tyr/formalism/rule_index.hpp"
#include "tyr/formalism/rule_proxy.hpp"
#include "tyr/formalism/term.hpp"
#include "tyr/formalism/term_proxy.hpp"
#include "tyr/formalism/unary_operator.hpp"
#include "tyr/formalism/unary_operator_index.hpp"
#include "tyr/formalism/unary_operator_proxy.hpp"
#include "tyr/formalism/variable.hpp"
#include "tyr/formalism/variable_index.hpp"
#include "tyr/formalism/variable_proxy.hpp"

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <ostream>

namespace tyr
{
inline std::ostream& print(std::ostream& os, const formalism::Double& el) { return os; }

inline std::ostream& print(std::ostream& os, const formalism::ParameterIndex& el)
{
    fmt::print(os, "{}", to_uint_t(el));
    return os;
}

template<formalism::IsOp Op, typename T>
inline std::ostream& print(std::ostream& os, const formalism::UnaryOperatorIndex<Op, T>& el)
{
    return os;
}

template<formalism::IsOp Op, typename T>
inline std::ostream& print(std::ostream& os, const formalism::UnaryOperator<Op, T>& el)
{
    return os;
}

template<formalism::IsOp Op, typename T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const formalism::UnaryOperatorProxy<Op, T, C>& el)
{
    return os;
}

template<formalism::IsOp Op, typename T>
inline std::ostream& print(std::ostream& os, const formalism::BinaryOperatorIndex<Op, T>& el)
{
    return os;
}

template<formalism::IsOp Op, typename T>
inline std::ostream& print(std::ostream& os, const formalism::BinaryOperator<Op, T>& el)
{
    return os;
}

template<formalism::IsOp Op, typename T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const formalism::BinaryOperatorProxy<Op, T, C>& el)
{
    return os;
}

template<formalism::IsOp Op, typename T>
inline std::ostream& print(std::ostream& os, const formalism::MultiOperatorIndex<Op, T>& el)
{
    return os;
}

template<formalism::IsOp Op, typename T>
inline std::ostream& print(std::ostream& os, const formalism::MultiOperator<Op, T>& el)
{
    return os;
}

template<formalism::IsOp Op, typename T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const formalism::MultiOperatorProxy<Op, T, C>& el)
{
    return os;
}

template<typename T>
inline std::ostream& print(std::ostream& os, const formalism::ArithmeticOperator<T>& el)
{
    return os;
}

template<typename T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const formalism::ArithmeticOperatorProxy<T, C>& el)
{
    return os;
}

template<typename T>
inline std::ostream& print(std::ostream& os, const formalism::BooleanOperator<T>& el)
{
    return os;
}

template<typename T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const formalism::BooleanOperatorProxy<T, C>& el)
{
    return os;
}

inline std::ostream& print(std::ostream& os, const formalism::VariableIndex& el) { return os; }

inline std::ostream& print(std::ostream& os, const formalism::Variable& el) { return os; }

template<formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const formalism::VariableProxy<C>& el)
{
    return os;
}

inline std::ostream& print(std::ostream& os, const formalism::ObjectIndex& el)
{
    fmt::print(os, "{}", el.value);
    return os;
}

inline std::ostream& print(std::ostream& os, const formalism::Object& el) { return os; }

template<formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const formalism::ObjectProxy<C>& el)
{
    return os;
}

inline std::ostream& print(std::ostream& os, const formalism::Term& el) { return os; }

template<formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const formalism::TermProxy<C>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const formalism::PredicateIndex<T>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const formalism::Predicate<T>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const formalism::PredicateProxy<T, C>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const formalism::AtomIndex<T>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const formalism::Atom<T>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const formalism::AtomProxy<T, C>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const formalism::LiteralIndex<T>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const formalism::Literal<T>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const formalism::LiteralProxy<T, C>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const formalism::GroundAtomIndex<T>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const formalism::GroundAtom<T>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const formalism::GroundAtomProxy<T, C>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const formalism::GroundLiteralIndex<T>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const formalism::GroundLiteral<T>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const formalism::GroundLiteralProxy<T, C>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const formalism::FunctionIndex<T>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const formalism::Function<T>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const formalism::FunctionProxy<T, C>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const formalism::FunctionTermIndex<T>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const formalism::FunctionTerm<T>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const formalism::FunctionTermProxy<T, C>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const formalism::GroundFunctionTermIndex<T>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const formalism::GroundFunctionTerm<T>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const formalism::GroundFunctionTermProxy<T, C>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const formalism::GroundFunctionTermValueIndex<T>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const formalism::GroundFunctionTermValue<T>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const formalism::GroundFunctionTermValueProxy<T, C>& el)
{
    return os;
}

inline std::ostream& print(std::ostream& os, const formalism::FunctionExpression& el) { return os; }

template<formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const formalism::FunctionExpressionProxy<C>& el)
{
    return os;
}

inline std::ostream& print(std::ostream& os, const formalism::GroundFunctionExpression& el) { return os; }

template<formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const formalism::GroundFunctionExpressionProxy<C>& el)
{
    return os;
}

inline std::ostream& print(std::ostream& os, const formalism::RuleIndex& el) { return os; }

inline std::ostream& print(std::ostream& os, const formalism::Rule& el) { return os; }

template<formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const formalism::RuleProxy<C>& el)
{
    return os;
}

inline std::ostream& print(std::ostream& os, const formalism::GroundRuleIndex& el) { return os; }

inline std::ostream& print(std::ostream& os, const formalism::GroundRule& el) { return os; }

template<formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const formalism::GroundRuleProxy<C>& el)
{
    return os;
}

inline std::ostream& print(std::ostream& os, const formalism::ProgramIndex& el) { return os; }

inline std::ostream& print(std::ostream& os, const formalism::Program& el) { return os; }

template<formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const formalism::ProgramProxy<C>& el)
{
    return os;
}

namespace formalism
{
inline std::ostream& operator<<(std::ostream& os, const Double& el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, const ParameterIndex& el) { return tyr::print(os, el); }

template<IsOp Op, typename T>
inline std::ostream& operator<<(std::ostream& os, const UnaryOperatorIndex<Op, T>& el)
{
    return tyr::print(os, el);
}

template<IsOp Op, typename T>
inline std::ostream& operator<<(std::ostream& os, const UnaryOperator<Op, T>& el)
{
    return tyr::print(os, el);
}

template<IsOp Op, typename T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const UnaryOperatorProxy<Op, T, C>& el)
{
    return tyr::print(os, el);
}

template<IsOp Op, typename T>
inline std::ostream& operator<<(std::ostream& os, const BinaryOperatorIndex<Op, T>& el)
{
    return tyr::print(os, el);
}

template<IsOp Op, typename T>
inline std::ostream& operator<<(std::ostream& os, const BinaryOperator<Op, T>& el)
{
    return tyr::print(os, el);
}

template<IsOp Op, typename T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const BinaryOperatorProxy<Op, T, C>& el)
{
    return tyr::print(os, el);
}

template<IsOp Op, typename T>
inline std::ostream& operator<<(std::ostream& os, const MultiOperatorIndex<Op, T>& el)
{
    return tyr::print(os, el);
}

template<IsOp Op, typename T>
inline std::ostream& operator<<(std::ostream& os, const MultiOperator<Op, T>& el)
{
    return tyr::print(os, el);
}

template<IsOp Op, typename T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const MultiOperatorProxy<Op, T, C>& el)
{
    return tyr::print(os, el);
}

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const ArithmeticOperator<T>& el)
{
    return os;
}

template<typename T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const ArithmeticOperatorProxy<T, C>& el)
{
    return os;
}

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const BooleanOperator<T>& el)
{
    return os;
}

template<typename T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const BooleanOperatorProxy<T, C>& el)
{
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const VariableIndex& el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, const Variable& el) { return tyr::print(os, el); }

template<IsContext C>
inline std::ostream& operator<<(std::ostream& os, const VariableProxy<C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const ObjectIndex& el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, const Object& el) { return tyr::print(os, el); }

template<IsContext C>
inline std::ostream& operator<<(std::ostream& os, const ObjectProxy<C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Term& el) { return tyr::print(os, el); }

template<IsContext C>
inline std::ostream& operator<<(std::ostream& os, const TermProxy<C>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const PredicateIndex<T>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const Predicate<T>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const PredicateProxy<T, C>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const AtomIndex<T>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const Atom<T>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const AtomProxy<T, C>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const LiteralIndex<T>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const Literal<T>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const LiteralProxy<T, C>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const GroundAtomIndex<T>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const GroundAtom<T>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const GroundAtomProxy<T, C>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const GroundLiteralIndex<T>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const GroundLiteral<T>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const GroundLiteralProxy<T, C>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const FunctionIndex<T>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const Function<T>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const FunctionProxy<T, C>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const FunctionTermIndex<T>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const FunctionTerm<T>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const FunctionTermProxy<T, C>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const GroundFunctionTermIndex<T>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const GroundFunctionTerm<T>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const GroundFunctionTermProxy<T, C>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const GroundFunctionTermValueIndex<T>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const GroundFunctionTermValue<T>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const GroundFunctionTermValueProxy<T, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const FunctionExpression& el) { return tyr::print(os, el); }

template<IsContext C>
inline std::ostream& operator<<(std::ostream& os, const FunctionExpressionProxy<C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const GroundFunctionExpression& el) { return tyr::print(os, el); }

template<IsContext C>
inline std::ostream& operator<<(std::ostream& os, const GroundFunctionExpressionProxy<C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const RuleIndex& el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, const Rule& el) { return tyr::print(os, el); }

template<IsContext C>
inline std::ostream& operator<<(std::ostream& os, const RuleProxy<C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const GroundRuleIndex& el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, const GroundRule& el) { return tyr::print(os, el); }

template<IsContext C>
inline std::ostream& operator<<(std::ostream& os, const GroundRuleProxy<C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const ProgramIndex& el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, const Program& el) { return tyr::print(os, el); }

template<IsContext C>
inline std::ostream& operator<<(std::ostream& os, const ProgramProxy<C>& el)
{
    return tyr::print(os, el);
}
}
}
#endif
