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
#include "tyr/formalism/arithmetic_operator_data.hpp"
#include "tyr/formalism/arithmetic_operator_proxy.hpp"
#include "tyr/formalism/atom_data.hpp"
#include "tyr/formalism/atom_index.hpp"
#include "tyr/formalism/atom_proxy.hpp"
#include "tyr/formalism/binary_operator_data.hpp"
#include "tyr/formalism/binary_operator_index.hpp"
#include "tyr/formalism/binary_operator_proxy.hpp"
#include "tyr/formalism/boolean_operator_data.hpp"
#include "tyr/formalism/boolean_operator_proxy.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function_data.hpp"
#include "tyr/formalism/function_expression_data.hpp"
#include "tyr/formalism/function_expression_proxy.hpp"
#include "tyr/formalism/function_index.hpp"
#include "tyr/formalism/function_proxy.hpp"
#include "tyr/formalism/function_term_data.hpp"
#include "tyr/formalism/function_term_index.hpp"
#include "tyr/formalism/function_term_proxy.hpp"
#include "tyr/formalism/ground_atom_data.hpp"
#include "tyr/formalism/ground_atom_index.hpp"
#include "tyr/formalism/ground_atom_proxy.hpp"
#include "tyr/formalism/ground_function_expression_data.hpp"
#include "tyr/formalism/ground_function_expression_proxy.hpp"
#include "tyr/formalism/ground_function_term_data.hpp"
#include "tyr/formalism/ground_function_term_index.hpp"
#include "tyr/formalism/ground_function_term_proxy.hpp"
#include "tyr/formalism/ground_function_term_value_data.hpp"
#include "tyr/formalism/ground_function_term_value_index.hpp"
#include "tyr/formalism/ground_function_term_value_proxy.hpp"
#include "tyr/formalism/ground_literal_data.hpp"
#include "tyr/formalism/ground_literal_index.hpp"
#include "tyr/formalism/ground_literal_proxy.hpp"
#include "tyr/formalism/ground_rule_data.hpp"
#include "tyr/formalism/ground_rule_index.hpp"
#include "tyr/formalism/ground_rule_proxy.hpp"
#include "tyr/formalism/literal_data.hpp"
#include "tyr/formalism/literal_index.hpp"
#include "tyr/formalism/literal_proxy.hpp"
#include "tyr/formalism/multi_operator_data.hpp"
#include "tyr/formalism/multi_operator_index.hpp"
#include "tyr/formalism/multi_operator_proxy.hpp"
#include "tyr/formalism/object_data.hpp"
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/object_proxy.hpp"
#include "tyr/formalism/parameter_index.hpp"
#include "tyr/formalism/predicate_data.hpp"
#include "tyr/formalism/predicate_index.hpp"
#include "tyr/formalism/predicate_proxy.hpp"
#include "tyr/formalism/program_data.hpp"
#include "tyr/formalism/program_index.hpp"
#include "tyr/formalism/program_proxy.hpp"
#include "tyr/formalism/rule_data.hpp"
#include "tyr/formalism/rule_index.hpp"
#include "tyr/formalism/rule_proxy.hpp"
#include "tyr/formalism/term_data.hpp"
#include "tyr/formalism/term_proxy.hpp"
#include "tyr/formalism/unary_operator_data.hpp"
#include "tyr/formalism/unary_operator_index.hpp"
#include "tyr/formalism/unary_operator_proxy.hpp"
#include "tyr/formalism/variable_data.hpp"
#include "tyr/formalism/variable_index.hpp"
#include "tyr/formalism/variable_proxy.hpp"

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <ostream>

namespace tyr
{
inline std::ostream& print(std::ostream& os, const formalism::ParameterIndex& el)
{
    fmt::print(os, "{}", to_uint_t(el));
    return os;
}

inline std::ostream& print(std::ostream& os, formalism::OpEq el)
{
    fmt::print(os, "=");
    return os;
}

inline std::ostream& print(std::ostream& os, formalism::OpLe el)
{
    fmt::print(os, "<=");
    return os;
}

inline std::ostream& print(std::ostream& os, formalism::OpLt el)
{
    fmt::print(os, "<");
    return os;
}

inline std::ostream& print(std::ostream& os, formalism::OpGe el)
{
    fmt::print(os, ">=");
    return os;
}

inline std::ostream& print(std::ostream& os, formalism::OpGt el)
{
    fmt::print(os, ">");
    return os;
}

inline std::ostream& print(std::ostream& os, formalism::OpAdd el)
{
    fmt::print(os, "+");
    return os;
}

inline std::ostream& print(std::ostream& os, formalism::OpSub el)
{
    fmt::print(os, "-");
    return os;
}

inline std::ostream& print(std::ostream& os, formalism::OpMul el)
{
    fmt::print(os, "*");
    return os;
}

inline std::ostream& print(std::ostream& os, formalism::OpDiv el)
{
    fmt::print(os, "/");
    return os;
}

template<formalism::IsOp Op, typename T>
inline std::ostream& print(std::ostream& os, const Data<formalism::UnaryOperator<Op, T>>& el)
{
    fmt::print(os, "({} {})", to_string(Op {}), to_string(el.arg));
    return os;
}

template<formalism::IsOp Op, typename T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const Proxy<formalism::UnaryOperator<Op, T>, C>& el)
{
    fmt::print(os, "({} {})", to_string(Op {}), to_string(el.get_arg()));
    return os;
}

template<formalism::IsOp Op, typename T>
inline std::ostream& print(std::ostream& os, const Data<formalism::BinaryOperator<Op, T>>& el)
{
    fmt::print(os, "({} {} {})", to_string(Op {}), to_string(el.lhs), to_string(el.rhs));
    return os;
}

template<formalism::IsOp Op, typename T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const Proxy<formalism::BinaryOperator<Op, T>, C>& el)
{
    fmt::print(os, "({} {} {})", to_string(Op {}), to_string(el.get_lhs()), to_string(el.get_rhs()));
    return os;
}

template<formalism::IsOp Op, typename T>
inline std::ostream& print(std::ostream& os, const Data<formalism::MultiOperator<Op, T>>& el)
{
    fmt::print(os, "({} {})", to_string(Op {}), to_strings(el.args));
    return os;
}

template<formalism::IsOp Op, typename T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const Proxy<formalism::MultiOperator<Op, T>, C>& el)
{
    fmt::print(os, "({} {})", to_string(Op {}), to_strings(el.get_args()));
    return os;
}

template<typename T>
inline std::ostream& print(std::ostream& os, const Data<formalism::ArithmeticOperator<T>>& el)
{
    // fmt::print(os, "{}", to_string(el.value));
    return os;
}

template<typename T>
inline std::ostream& print(std::ostream& os, const Data<formalism::BooleanOperator<T>>& el)
{
    // fmt::print(os, "{}", to_string(el.value));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Variable>& el)
{
    fmt::print(os, "{}", to_string(el.name));
    return os;
}

template<formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const Proxy<formalism::Variable, C>& el)
{
    fmt::print(os, "{}", to_string(el.get_name()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Object>& el)
{
    fmt::print(os, "{}", to_string(el.name));
    return os;
}

template<formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const Proxy<formalism::Object, C>& el)
{
    fmt::print(os, "{}", to_string(el.get_name()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Term>& el)
{
    // fmt::print(os, "{}", to_string(el.value));
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const Data<formalism::Predicate<T>>& el)
{
    fmt::print(os, "{}/{}", to_string(el.name), to_string(el.arity));
    return os;
}

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const Proxy<formalism::Predicate<T>, C>& el)
{
    fmt::print(os, "{}/{}", to_string(el.get_name()), to_string(el.get_arity()));
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const Data<formalism::Atom<T>>& el)
{
    fmt::print(os, "({} {})", to_string(el.index.group), to_strings(el.terms));
    return os;
}

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const Proxy<formalism::Atom<T>, C>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const Data<formalism::Literal<T>>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const Proxy<formalism::Literal<T>, C>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const Data<formalism::GroundAtom<T>>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const Proxy<formalism::GroundAtom<T>, C>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const Data<formalism::GroundLiteral<T>>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const Proxy<formalism::GroundLiteral<T>, C>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const Data<formalism::Function<T>>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const Proxy<formalism::Function<T>, C>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const Data<formalism::FunctionTerm<T>>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const Proxy<formalism::FunctionTerm<T>, C>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const Data<formalism::GroundFunctionTerm<T>>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const Proxy<formalism::GroundFunctionTerm<T>, C>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T>
inline std::ostream& print(std::ostream& os, const Data<formalism::GroundFunctionTermValue<T>>& el)
{
    return os;
}

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const Proxy<formalism::GroundFunctionTermValue<T>, C>& el)
{
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::FunctionExpression>& el)
{
    // fmt::print(os, "{}", to_string(el.value));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::GroundFunctionExpression>& el)
{
    // fmt::print(os, "{}", to_string(el.value));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Rule>& el) { return os; }

template<formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const Proxy<formalism::Rule, C>& el)
{
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::GroundRule>& el) { return os; }

template<formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const Proxy<formalism::GroundRule, C>& el)
{
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Program>& el) { return os; }

template<formalism::IsContext C>
inline std::ostream& print(std::ostream& os, const Proxy<formalism::Program, C>& el)
{
    return os;
}

namespace formalism
{
inline std::ostream& operator<<(std::ostream& os, const ParameterIndex& el) { return tyr::print(os, el); }

template<IsOp Op, typename T>
inline std::ostream& operator<<(std::ostream& os, const Data<UnaryOperator<Op, T>>& el)
{
    return tyr::print(os, el);
}

template<IsOp Op, typename T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const Proxy<UnaryOperator<Op, T>, C>& el)
{
    return tyr::print(os, el);
}

template<IsOp Op, typename T>
inline std::ostream& operator<<(std::ostream& os, const Data<BinaryOperator<Op, T>>& el)
{
    return tyr::print(os, el);
}

template<IsOp Op, typename T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const Proxy<BinaryOperator<Op, T>, C>& el)
{
    return tyr::print(os, el);
}

template<IsOp Op, typename T>
inline std::ostream& operator<<(std::ostream& os, const Data<MultiOperator<Op, T>>& el)
{
    return tyr::print(os, el);
}

template<IsOp Op, typename T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const Proxy<MultiOperator<Op, T>, C>& el)
{
    return tyr::print(os, el);
}

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const Data<ArithmeticOperator<T>>& el)
{
    return os;
}

template<typename T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const Proxy<ArithmeticOperator<T>, C>& el)
{
    return os;
}

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const Data<BooleanOperator<T>>& el)
{
    return os;
}

template<typename T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const Proxy<BooleanOperator<T>, C>& el)
{
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Data<Variable>& el) { return tyr::print(os, el); }

template<IsContext C>
inline std::ostream& operator<<(std::ostream& os, const Proxy<Variable, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<Object>& el) { return tyr::print(os, el); }

template<IsContext C>
inline std::ostream& operator<<(std::ostream& os, const Proxy<Object, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<Term>& el) { return tyr::print(os, el); }

template<IsContext C>
inline std::ostream& operator<<(std::ostream& os, const Proxy<Term, C>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const Data<Predicate<T>>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const Proxy<Predicate<T>, C>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const Data<Atom<T>>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const Proxy<Atom<T>, C>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const Data<Literal<T>>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const Proxy<Literal<T>, C>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const Data<GroundAtom<T>>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const Proxy<GroundAtom<T>, C>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const Data<GroundLiteral<T>>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const Proxy<GroundLiteral<T>, C>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const Data<Function<T>>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const Proxy<Function<T>, C>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const Data<FunctionTerm<T>>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const Proxy<FunctionTerm<T>, C>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const Data<GroundFunctionTerm<T>>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const Proxy<GroundFunctionTerm<T>, C>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T>
inline std::ostream& operator<<(std::ostream& os, const Data<GroundFunctionTermValue<T>>& el)
{
    return tyr::print(os, el);
}

template<IsStaticOrFluentTag T, IsContext C>
inline std::ostream& operator<<(std::ostream& os, const Proxy<GroundFunctionTermValue<T>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<FunctionExpression>& el) { return tyr::print(os, el); }

template<IsContext C>
inline std::ostream& operator<<(std::ostream& os, const Proxy<FunctionExpression, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<GroundFunctionExpression>& el) { return tyr::print(os, el); }

template<IsContext C>
inline std::ostream& operator<<(std::ostream& os, const Proxy<GroundFunctionExpression, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<Rule>& el) { return tyr::print(os, el); }

template<IsContext C>
inline std::ostream& operator<<(std::ostream& os, const Proxy<Rule, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<GroundRule>& el) { return tyr::print(os, el); }

template<IsContext C>
inline std::ostream& operator<<(std::ostream& os, const Proxy<GroundRule, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<Program>& el) { return tyr::print(os, el); }

template<IsContext C>
inline std::ostream& operator<<(std::ostream& os, const Proxy<Program, C>& el)
{
    return tyr::print(os, el);
}
}
}
#endif
