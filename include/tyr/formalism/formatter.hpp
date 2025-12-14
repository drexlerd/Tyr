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
#include "tyr/common/iostream.hpp"
#include "tyr/formalism/datas.hpp"
#include "tyr/formalism/views.hpp"

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <ostream>

namespace tyr
{
/**
 * Forward declarations
 */
namespace formalism
{
inline std::ostream& operator<<(std::ostream& os, const ParameterIndex& el);

inline std::ostream& operator<<(std::ostream& os, OpEq el);

inline std::ostream& operator<<(std::ostream& os, OpNe el);

inline std::ostream& operator<<(std::ostream& os, OpLe el);

inline std::ostream& operator<<(std::ostream& os, OpLt el);

inline std::ostream& operator<<(std::ostream& os, OpGe el);

inline std::ostream& operator<<(std::ostream& os, OpGt el);

inline std::ostream& operator<<(std::ostream& os, OpAdd el);

inline std::ostream& operator<<(std::ostream& os, OpSub el);

inline std::ostream& operator<<(std::ostream& os, OpMul el);

inline std::ostream& operator<<(std::ostream& os, OpDiv el);

template<OpKind Op, typename T>
inline std::ostream& operator<<(std::ostream& os, const Data<UnaryOperator<Op, T>>& el);

template<OpKind Op, typename T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<UnaryOperator<Op, T>>, C>& el);

template<OpKind Op, typename T>
inline std::ostream& operator<<(std::ostream& os, const Data<BinaryOperator<Op, T>>& el);

template<OpKind Op, typename T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<BinaryOperator<Op, T>>, C>& el);

template<OpKind Op, typename T>
inline std::ostream& operator<<(std::ostream& os, const Data<MultiOperator<Op, T>>& el);

template<OpKind Op, typename T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<MultiOperator<Op, T>>, C>& el);

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const Data<ArithmeticOperator<T>>& el);

template<typename T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Data<ArithmeticOperator<T>>, C>& el);

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const Data<BooleanOperator<T>>& el);

template<typename T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Data<BooleanOperator<T>>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<Variable>& el);

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Variable>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<Object>& el);

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Object>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<Binding>& el);

template<formalism::Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Binding>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<Term>& el);

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Data<Term>, C>& el);

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<Predicate<T>>& el);

template<FactKind T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Predicate<T>>, C>& el);

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<Atom<T>>& el);

template<FactKind T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Atom<T>>, C>& el);

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<Literal<T>>& el);

template<FactKind T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Literal<T>>, C>& el);

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<GroundAtom<T>>& el);

template<FactKind T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<GroundAtom<T>>, C>& el);

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<GroundLiteral<T>>& el);

template<FactKind T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<GroundLiteral<T>>, C>& el);

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<Function<T>>& el);

template<FactKind T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Function<T>>, C>& el);

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<FunctionTerm<T>>& el);

template<FactKind T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<FunctionTerm<T>>, C>& el);

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<GroundFunctionTerm<T>>& el);

template<FactKind T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<GroundFunctionTerm<T>>, C>& el);

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<GroundFunctionTermValue<T>>& el);

template<FactKind T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<GroundFunctionTermValue<T>>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<FunctionExpression>& el);

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Data<FunctionExpression>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<GroundFunctionExpression>& el);

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Data<GroundFunctionExpression>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<ConjunctiveCondition>& el);

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<ConjunctiveCondition>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<Rule>& el);

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Rule>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<GroundConjunctiveCondition>& el);

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<GroundConjunctiveCondition>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<GroundRule>& el);

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<GroundRule>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<Program>& el);

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Program>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const OpAssign& el);

inline std::ostream& operator<<(std::ostream& os, const OpIncrease& el);

inline std::ostream& operator<<(std::ostream& os, const OpDecrease& el);

inline std::ostream& operator<<(std::ostream& os, const OpScaleUp& el);

inline std::ostream& operator<<(std::ostream& os, const OpScaleDown& el);

inline std::ostream& operator<<(std::ostream& os, const Minimize& el);

inline std::ostream& operator<<(std::ostream& os, const Maximize& el);

template<NumericEffectOpKind Op, FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<NumericEffect<Op, T>>& el);

template<NumericEffectOpKind Op, FactKind T, Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<NumericEffect<Op, T>>, C>& el);

template<NumericEffectOpKind Op, FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundNumericEffect<Op, T>>& el);

template<NumericEffectOpKind Op, FactKind T, Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<GroundNumericEffect<Op, T>>, C>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<NumericEffectOperator<T>>& el);

template<FactKind T, Context C>
std::ostream& operator<<(std::ostream& os, const View<Data<NumericEffectOperator<T>>, C>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundNumericEffectOperator<T>>& el);

template<FactKind T, Context C>
std::ostream& operator<<(std::ostream& os, const View<Data<GroundNumericEffectOperator<T>>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<ConditionalEffect>& el);

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<ConditionalEffect>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<GroundConditionalEffect>& el);

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<GroundConditionalEffect>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<ConjunctiveEffect>& el);

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<ConjunctiveEffect>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<GroundConjunctiveEffect>& el);

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<GroundConjunctiveEffect>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<Action>& el);

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<Action>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<GroundAction>& el);

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<GroundAction>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<Axiom>& el);

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<Axiom>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<GroundAxiom>& el);

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<GroundAxiom>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<Metric>& el);

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<Metric>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<Task>& el);

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<Task>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<Domain>& el);

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<Domain>, C>& el);

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<FDRVariable<T>>& el);

template<FactKind T, Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<FDRVariable<T>>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const FDRValue& el);

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<FDRFact<T>>& el);

template<FactKind T, Context C>
std::ostream& operator<<(std::ostream& os, const View<Data<FDRFact<T>>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<GroundFDRConjunctiveCondition>& el);

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<GroundFDRConjunctiveCondition>, C>& el);

inline std::ostream& operator<<(std::ostream& os, const Data<FDRTask>& el);

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<FDRTask>, C>& el);
}  // end namespace formalism

/**
 * Definitions
 */

inline std::ostream& print(std::ostream& os, const formalism::ParameterIndex& el)
{
    fmt::print(os, "V{}", uint_t(el));
    return os;
}

inline std::ostream& print(std::ostream& os, formalism::OpEq el)
{
    fmt::print(os, "=");
    return os;
}

inline std::ostream& print(std::ostream& os, formalism::OpNe el)
{
    fmt::print(os, "!=");
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

template<formalism::OpKind Op, typename T>
inline std::ostream& print(std::ostream& os, const Data<formalism::UnaryOperator<Op, T>>& el)
{
    fmt::print(os, "({} {})", to_string(Op {}), to_string(el.arg));
    return os;
}

template<formalism::OpKind Op, typename T, formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::UnaryOperator<Op, T>>, C>& el)
{
    fmt::print(os, "({} {})", to_string(Op {}), to_string(el.get_arg()));
    return os;
}

template<formalism::OpKind Op, typename T>
inline std::ostream& print(std::ostream& os, const Data<formalism::BinaryOperator<Op, T>>& el)
{
    fmt::print(os, "({} {} {})", to_string(Op {}), to_string(el.lhs), to_string(el.rhs));
    return os;
}

template<formalism::OpKind Op, typename T, formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::BinaryOperator<Op, T>>, C>& el)
{
    fmt::print(os, "({} {} {})", to_string(Op {}), to_string(el.get_lhs()), to_string(el.get_rhs()));
    return os;
}

template<formalism::OpKind Op, typename T>
inline std::ostream& print(std::ostream& os, const Data<formalism::MultiOperator<Op, T>>& el)
{
    fmt::print(os, "({} {})", to_string(Op {}), fmt::format("{}", fmt::join(to_strings(el.args), " ")));
    return os;
}

template<formalism::OpKind Op, typename T, formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::MultiOperator<Op, T>>, C>& el)
{
    fmt::print(os, "({} {})", to_string(Op {}), fmt::format("{}", fmt::join(to_strings(el.get_args()), " ")));
    return os;
}

template<typename T>
inline std::ostream& print(std::ostream& os, const Data<formalism::ArithmeticOperator<T>>& el)
{
    fmt::print(os, "{}", to_string(el.value));
    return os;
}

template<typename T, formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Data<formalism::ArithmeticOperator<T>>, C>& el)
{
    fmt::print(os, "{}", to_string(el.get_variant()));
    return os;
}

template<typename T>
inline std::ostream& print(std::ostream& os, const Data<formalism::BooleanOperator<T>>& el)
{
    fmt::print(os, "{}", to_string(el.value));
    return os;
}

template<typename T, formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Data<formalism::BooleanOperator<T>>, C>& el)
{
    fmt::print(os, "{}", to_string(el.get_variant()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Variable>& el)
{
    fmt::print(os, "{}", to_string(el.name));
    return os;
}

template<formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::Variable>, C>& el)
{
    fmt::print(os, "{}", to_string(el.get_name()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Object>& el)
{
    fmt::print(os, "{}", to_string(el.name));
    return os;
}

template<formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::Object>, C>& el)
{
    fmt::print(os, "{}", to_string(el.get_name()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Binding>& el)
{
    fmt::print(os, "{}", fmt::join(to_strings(el.objects), " "));
    return os;
}

template<formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::Binding>, C>& el)
{
    fmt::print(os, "{}", fmt::join(to_strings(el.get_objects()), " "));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Term>& el)
{
    fmt::print(os, "{}", to_string(el.value));
    return os;
}

template<formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Data<formalism::Term>, C>& el)
{
    fmt::print(os, "{}", to_string(el.get_variant()));
    return os;
}

template<formalism::FactKind T>
inline std::ostream& print(std::ostream& os, const Data<formalism::Predicate<T>>& el)
{
    fmt::print(os, "{}/{}", to_string(el.name), to_string(el.arity));
    return os;
}

template<formalism::FactKind T, formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::Predicate<T>>, C>& el)
{
    fmt::print(os, "{}/{}", to_string(el.get_name()), to_string(el.get_arity()));
    return os;
}

template<formalism::FactKind T>
inline std::ostream& print(std::ostream& os, const Data<formalism::Atom<T>>& el)
{
    fmt::print(os, "({} {})", to_string(el.predicate), fmt::format("{}", fmt::join(to_strings(el.terms), " ")));
    return os;
}

template<formalism::FactKind T, formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::Atom<T>>, C>& el)
{
    fmt::print(os, "({} {})", to_string(el.get_predicate().get_name()), fmt::format("{}", fmt::join(to_strings(el.get_terms()), " ")));
    return os;
}

template<formalism::FactKind T>
inline std::ostream& print(std::ostream& os, const Data<formalism::Literal<T>>& el)
{
    if (el.polarity)
        print(os, to_string(el.atom));
    else
        fmt::print(os, "(not {})", to_string(el.atom));
    return os;
}

template<formalism::FactKind T, formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::Literal<T>>, C>& el)
{
    if (el.get_polarity())
        print(os, to_string(el.get_atom()));
    else
        fmt::print(os, "(not {})", to_string(el.get_atom()));
    return os;
}

template<formalism::FactKind T>
inline std::ostream& print(std::ostream& os, const Data<formalism::GroundAtom<T>>& el)
{
    fmt::print(os, "({} {})", to_string(el.predicate), to_string(el.binding));
    return os;
}

template<formalism::FactKind T, formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::GroundAtom<T>>, C>& el)
{
    fmt::print(os, "({} {})", to_string(el.get_predicate().get_name()), to_string(el.get_binding()));
    return os;
}

template<formalism::FactKind T>
inline std::ostream& print(std::ostream& os, const Data<formalism::GroundLiteral<T>>& el)
{
    if (el.polarity)
        print(os, to_string(el.atom));
    else
        fmt::print(os, "(not {})", to_string(el.atom));
    return os;
}

template<formalism::FactKind T, formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::GroundLiteral<T>>, C>& el)
{
    if (el.get_polarity())
        print(os, to_string(el.get_atom()));
    else
        fmt::print(os, "(not {})", to_string(el.get_atom()));
    return os;
}

template<formalism::FactKind T>
inline std::ostream& print(std::ostream& os, const Data<formalism::Function<T>>& el)
{
    fmt::print(os, "{}/{}", to_string(el.name), to_string(el.arity));
    return os;
}

template<formalism::FactKind T, formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::Function<T>>, C>& el)
{
    fmt::print(os, "{}/{}", to_string(el.get_name()), to_string(el.get_arity()));
    return os;
}

template<formalism::FactKind T>
inline std::ostream& print(std::ostream& os, const Data<formalism::FunctionTerm<T>>& el)
{
    fmt::print(os, "({} {})", to_string(el.function), fmt::format("{}", fmt::join(to_strings(el.terms), " ")));
    return os;
}

template<formalism::FactKind T, formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::FunctionTerm<T>>, C>& el)
{
    fmt::print(os, "({} {})", to_string(el.get_function().get_name()), fmt::format("{}", fmt::join(to_strings(el.get_terms()), " ")));
    return os;
}

template<formalism::FactKind T>
inline std::ostream& print(std::ostream& os, const Data<formalism::GroundFunctionTerm<T>>& el)
{
    fmt::print(os, "({} {})", to_string(el.function), to_string(el.binding));
    return os;
}

template<formalism::FactKind T, formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::GroundFunctionTerm<T>>, C>& el)
{
    fmt::print(os, "({} {})", to_string(el.get_function().get_name()), to_string(el.get_binding()));
    return os;
}

template<formalism::FactKind T>
inline std::ostream& print(std::ostream& os, const Data<formalism::GroundFunctionTermValue<T>>& el)
{
    fmt::print(os, "(= {} {})", to_string(el.fterm), to_string(el.value));
    return os;
}

template<formalism::FactKind T, formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::GroundFunctionTermValue<T>>, C>& el)
{
    fmt::print(os, "(= {} {})", to_string(el.get_fterm()), to_string(el.get_value()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::FunctionExpression>& el)
{
    fmt::print(os, "{}", to_string(el.value));
    return os;
}

template<formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Data<formalism::FunctionExpression>, C>& el)
{
    fmt::print(os, "{}", to_string(el.get_variant()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::GroundFunctionExpression>& el)
{
    fmt::print(os, "{}", to_string(el.value));
    return os;
}

template<formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Data<formalism::GroundFunctionExpression>, C>& el)
{
    fmt::print(os, "{}", to_string(el.get_variant()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::ConjunctiveCondition>& el)
{
    os << "ConjunctiveCondition(\n";
    {
        IndentScope scope(os);

        os << print_indent << "variables = " << el.variables << "\n";

        os << print_indent << "static literals = " << el.static_literals << "\n";

        os << print_indent << "fluent literals = " << el.fluent_literals << "\n";

        os << print_indent << "derived literals = " << el.derived_literals << "\n";

        os << print_indent << "numeric constraints = " << el.numeric_constraints << "\n";
    }
    os << print_indent << ")";

    return os;
}

template<formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::ConjunctiveCondition>, C>& el)
{
    os << "ConjunctiveCondition(\n";
    {
        IndentScope scope(os);

        os << print_indent << "variables = " << el.get_variables() << "\n";

        os << print_indent << "static literals = " << el.template get_literals<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent literals = " << el.template get_literals<formalism::FluentTag>() << "\n";

        os << print_indent << "derived literals = " << el.template get_literals<formalism::DerivedTag>() << "\n";

        os << print_indent << "numeric constraints = " << el.get_numeric_constraints() << "\n";
    }
    os << print_indent << ")";

    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Rule>& el)
{
    os << "Rule(\n";
    {
        IndentScope scope(os);

        os << print_indent << "head = " << el.head << "\n";

        os << print_indent << "body = " << el.body << "\n";
    }
    os << print_indent << ")";

    return os;
}

template<formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::Rule>, C>& el)
{
    os << "Rule(\n";
    {
        IndentScope scope(os);

        os << print_indent << "head = " << el.get_head() << "\n";

        os << print_indent << "body = " << el.get_body() << "\n";
    }
    os << print_indent << ")";

    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::GroundConjunctiveCondition>& el)
{
    os << "GroundConjunctiveCondition(\n";
    {
        IndentScope scope(os);

        os << print_indent << "static literals = " << el.static_literals << "\n";

        os << print_indent << "fluent literals = " << el.fluent_literals << "\n";

        os << print_indent << "derived literals = " << el.derived_literals << "\n";

        os << print_indent << "numeric constraints = " << el.numeric_constraints << "\n";
    }
    os << print_indent << ")";

    return os;
}

template<formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::GroundConjunctiveCondition>, C>& el)
{
    os << "GroundConjunctiveCondition(\n";
    {
        IndentScope scope(os);

        os << print_indent << "static literals = " << el.template get_literals<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent literals = " << el.template get_literals<formalism::FluentTag>() << "\n";

        os << print_indent << "derived literals = " << el.template get_literals<formalism::DerivedTag>() << "\n";

        os << print_indent << "numeric constraints = " << el.get_numeric_constraints() << "\n";
    }
    os << print_indent << ")";

    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::GroundRule>& el)
{
    os << "GroundRule(\n";
    {
        IndentScope scope(os);

        os << print_indent << "head = " << el.head << "\n";

        os << print_indent << "body = " << el.body << "\n";
    }
    os << print_indent << ")";

    return os;
}

template<formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::GroundRule>, C>& el)
{
    os << "GroundRule(\n";
    {
        IndentScope scope(os);

        os << print_indent << "head = " << el.get_head() << "\n";

        os << print_indent << "body = " << el.get_body() << "\n";
    }
    os << print_indent << ")";

    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Program>& el)
{
    os << "Program(\n";
    {
        IndentScope scope(os);

        os << print_indent << "static predicates =" << el.static_predicates << "\n";

        os << print_indent << "fluent predicates = " << el.fluent_predicates << "\n";

        os << print_indent << "static functions = " << el.static_functions << "\n";

        os << print_indent << "fluent functions = " << el.fluent_functions << "\n";

        os << print_indent << "objects = " << el.objects << "\n";

        os << print_indent << "static atoms = " << el.static_atoms << "\n";

        os << print_indent << "fluent atoms = " << el.fluent_atoms << "\n";

        os << print_indent << "static fterms = " << el.static_fterm_values << "\n";

        os << print_indent << "fluent fterms = " << el.fluent_fterm_values << "\n";

        os << print_indent << "rules = " << el.rules << "\n";
    }
    os << print_indent << ")";

    return os;
}

template<formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::Program>, C>& el)
{
    os << "Program(\n";
    {
        IndentScope scope(os);

        os << print_indent << "static predicates =" << el.template get_predicates<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent predicates = " << el.template get_predicates<formalism::FluentTag>() << "\n";

        os << print_indent << "static functions = " << el.template get_functions<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent functions = " << el.template get_functions<formalism::FluentTag>() << "\n";

        os << print_indent << "objects = " << el.get_objects() << "\n";

        os << print_indent << "static atoms = " << el.template get_atoms<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent atoms = " << el.template get_atoms<formalism::FluentTag>() << "\n";

        os << print_indent << "static fterms = " << el.template get_fterm_values<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent fterms = " << el.template get_fterm_values<formalism::FluentTag>() << "\n";

        os << print_indent << "rules = " << el.get_rules() << "\n";
    }
    os << print_indent << ")";

    return os;
}

inline std::ostream& print(std::ostream& os, const formalism::OpAssign& el)
{
    fmt::print(os, "assign");
    return os;
}

inline std::ostream& print(std::ostream& os, const formalism::OpIncrease& el)
{
    fmt::print(os, "increase");
    return os;
}

inline std::ostream& print(std::ostream& os, const formalism::OpDecrease& el)
{
    fmt::print(os, "decrease");
    return os;
}

inline std::ostream& print(std::ostream& os, const formalism::OpScaleUp& el)
{
    fmt::print(os, "scale-up");
    return os;
}

inline std::ostream& print(std::ostream& os, const formalism::OpScaleDown& el)
{
    fmt::print(os, "scale-down");
    return os;
}

inline std::ostream& print(std::ostream& os, const formalism::Minimize& el)
{
    fmt::print(os, "minimize");
    return os;
}

inline std::ostream& print(std::ostream& os, const formalism::Maximize& el)
{
    fmt::print(os, "maximize");
    return os;
}

template<formalism::NumericEffectOpKind Op, formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::NumericEffect<Op, T>>& el)
{
    fmt::print(os, "({} {} {})", to_string(Op {}), to_string(el.fterm), to_string(el.fexpr));
    return os;
}

template<formalism::NumericEffectOpKind Op, formalism::FactKind T, formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::NumericEffect<Op, T>>, C>& el)
{
    fmt::print(os, "({} {} {})", to_string(Op {}), to_string(el.get_fterm()), to_string(el.get_fexpr()));
    return os;
}

template<formalism::NumericEffectOpKind Op, formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::GroundNumericEffect<Op, T>>& el)
{
    fmt::print(os, "({} {} {})", to_string(Op {}), to_string(el.fterm), to_string(el.fexpr));
    return os;
}

template<formalism::NumericEffectOpKind Op, formalism::FactKind T, formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::GroundNumericEffect<Op, T>>, C>& el)
{
    fmt::print(os, "({} {} {})", to_string(Op {}), to_string(el.get_fterm()), to_string(el.get_fexpr()));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::NumericEffectOperator<T>>& el)
{
    return tyr::print(os, el.value);
}

template<formalism::FactKind T, formalism::Context C>
std::ostream& print(std::ostream& os, const View<Data<formalism::NumericEffectOperator<T>>, C>& el)
{
    return tyr::print(os, el.get_variant());
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::GroundNumericEffectOperator<T>>& el)
{
    return tyr::print(os, el.value);
}

template<formalism::FactKind T, formalism::Context C>
std::ostream& print(std::ostream& os, const View<Data<formalism::GroundNumericEffectOperator<T>>, C>& el)
{
    return tyr::print(os, el.get_variant());
}

inline std::ostream& print(std::ostream& os, const Data<formalism::ConditionalEffect>& el)
{
    os << "ConditionalEffect(\n";
    {
        IndentScope scope(os);

        os << print_indent << "condition = " << el.condition << "\n";

        os << print_indent << "effect = " << el.effect << "\n";
    }
    os << print_indent << ")";

    return os;
}

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::ConditionalEffect>, C>& el)
{
    os << "ConditionalEffect(\n";
    {
        IndentScope scope(os);

        os << print_indent << "condition = " << el.get_condition() << "\n";

        os << print_indent << "effect = " << el.get_effect() << "\n";
    }
    os << print_indent << ")";

    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::GroundConditionalEffect>& el)
{
    os << "GroundConditionalEffect(\n";
    {
        IndentScope scope(os);

        os << print_indent << "condition = " << el.condition << "\n";

        os << print_indent << "effect = " << el.effect << "\n";
    }
    os << print_indent << ")";

    return os;
}

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::GroundConditionalEffect>, C>& el)
{
    os << "GroundConditionalEffect(\n";
    {
        IndentScope scope(os);

        os << print_indent << "condition = " << el.get_condition() << "\n";

        os << print_indent << "effect = " << el.get_effect() << "\n";
    }
    os << print_indent << ")";

    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::ConjunctiveEffect>& el)
{
    os << "ConjunctiveEffect(\n";
    {
        IndentScope scope(os);

        os << print_indent << "fluent literals = " << el.literals << "\n";

        os << print_indent << "fluent numeric effects = " << el.numeric_effects << "\n";

        os << print_indent << "auxiliary numeric effect = " << el.auxiliary_numeric_effect << "\n";
    }
    os << print_indent << ")";

    return os;
}

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::ConjunctiveEffect>, C>& el)
{
    os << "ConjunctiveEffect(\n";
    {
        IndentScope scope(os);

        os << print_indent << "fluent literals = " << el.get_literals() << "\n";

        os << print_indent << "fluent numeric effects = " << el.get_numeric_effects() << "\n";

        os << print_indent << "auxiliary numeric effect = " << el.get_auxiliary_numeric_effect() << "\n";
    }
    os << print_indent << ")";

    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::GroundConjunctiveEffect>& el)
{
    os << "GroundConjunctiveEffect(\n";
    {
        IndentScope scope(os);

        os << print_indent << "fluent literals = " << el.facts << "\n";

        os << print_indent << "fluent numeric effects = " << el.numeric_effects << "\n";

        os << print_indent << "auxiliary numeric effect = " << el.auxiliary_numeric_effect << "\n";
    }
    os << print_indent << ")";

    return os;
}

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::GroundConjunctiveEffect>, C>& el)
{
    os << "GroundConjunctiveEffect(\n";
    {
        IndentScope scope(os);

        os << print_indent << "fluent literals = " << el.get_facts() << "\n";

        os << print_indent << "fluent numeric effects = " << el.get_numeric_effects() << "\n";

        os << print_indent << "auxiliary numeric effect = " << el.get_auxiliary_numeric_effect() << "\n";
    }
    os << print_indent << ")";

    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Action>& el)
{
    os << "Action(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.index << "\n";

        os << print_indent << "name = " << el.name << "\n";

        os << print_indent << "condition = " << el.condition << "\n";

        os << print_indent << "effects = " << el.effects << "\n";
    }
    os << print_indent << ")";

    return os;
}

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::Action>, C>& el)
{
    os << "Action(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.get_index() << "\n";

        os << print_indent << "name = " << el.get_name() << "\n";

        os << print_indent << "condition = " << el.get_condition() << "\n";

        os << print_indent << "effects = " << el.get_effects() << "\n";
    }
    os << print_indent << ")";

    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::GroundAction>& el)
{
    os << "GroundAction(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.index << "\n";

        os << print_indent << "action index = " << el.action << "\n";

        os << print_indent << "condition = " << el.condition << "\n";

        os << print_indent << "effects = " << el.effects << "\n";
    }
    os << print_indent << ")";

    return os;
}

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::GroundAction>, C>& el)
{
    os << "GroundAction(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.get_index() << "\n";

        os << print_indent << "action index = " << el.get_action().get_index() << "\n";

        os << print_indent << "condition = " << el.get_condition() << "\n";

        os << print_indent << "effects = " << el.get_effects() << "\n";
    }
    os << print_indent << ")";

    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Axiom>& el)
{
    os << "Axiom(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.index << "\n";

        os << print_indent << "body = " << el.body << "\n";

        os << print_indent << "head = " << el.head << "\n";
    }
    os << print_indent << ")";

    return os;
}

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::Axiom>, C>& el)
{
    os << "Axiom(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.get_index() << "\n";

        os << print_indent << "body = " << el.get_body() << "\n";

        os << print_indent << "head = " << el.get_head() << "\n";
    }
    os << print_indent << ")";

    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::GroundAxiom>& el)
{
    os << "GroundAxiom(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.index << "\n";

        os << print_indent << "axiom index = " << el.axiom << "\n";

        os << print_indent << "body = " << el.body << "\n";

        os << print_indent << "head = " << el.head << "\n";
    }
    os << print_indent << ")";

    return os;
}

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::GroundAxiom>, C>& el)
{
    os << "GroundAxiom(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.get_index() << "\n";

        os << print_indent << "axiom index = " << el.get_axiom().get_index() << "\n";

        os << print_indent << "body = " << el.get_body() << "\n";

        os << print_indent << "head = " << el.get_head() << "\n";
    }
    os << print_indent << ")";

    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Metric>& el)
{
    fmt::print(os, "({} {})", to_string(el.objective), to_string(el.fexpr));
    return os;
}

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::Metric>, C>& el)
{
    fmt::print(os, "({} {})", to_string(el.get_objective()), to_string(el.get_fexpr()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Task>& el)
{
    os << "Task(\n";
    {
        IndentScope scope(os);

        os << print_indent << "name = " << el.name << "\n";

        os << print_indent << "derived predicates = " << el.derived_predicates << "\n";

        os << print_indent << "objects = " << el.objects << "\n";

        os << print_indent << "static atoms = " << el.static_atoms << "\n";

        os << print_indent << "fluent atoms = " << el.fluent_atoms << "\n";

        os << print_indent << "static numeric variables = " << el.static_fterm_values << "\n";

        os << print_indent << "fluent numeric variables = " << el.fluent_fterm_values << "\n";

        os << print_indent << "auxiliary numeric variable = " << el.auxiliary_fterm_value << "\n";

        os << print_indent << "goal = " << el.goal << "\n";

        os << print_indent << "metric = " << el.metric << "\n";

        os << print_indent << "axioms = " << el.axioms << "\n";
    }
    os << print_indent << ")";

    return os;
}

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::Task>, C>& el)
{
    os << "Task(\n";
    {
        IndentScope scope(os);

        os << print_indent << "name = " << el.get_name() << "\n";

        os << print_indent << "derived predicates = " << el.get_derived_predicates() << "\n";

        os << print_indent << "objects = " << el.get_objects() << "\n";

        os << print_indent << "static atoms = " << el.template get_atoms<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent atoms = " << el.template get_atoms<formalism::FluentTag>() << "\n";

        os << print_indent << "static numeric variables = " << el.template get_fterm_values<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent numeric variables = " << el.template get_fterm_values<formalism::FluentTag>() << "\n";

        os << print_indent << "auxiliary numeric variable = " << el.get_auxiliary_fterm_value() << "\n";

        os << print_indent << "goal = " << el.get_goal() << "\n";

        os << print_indent << "metric = " << el.get_metric() << "\n";

        os << print_indent << "axioms = " << el.get_axioms() << "\n";
    }
    os << print_indent << ")";

    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Domain>& el)
{
    os << "Domain(\n";
    {
        IndentScope scope(os);

        os << print_indent << "name = " << el.name << "\n";

        os << print_indent << "static predicates = " << el.static_predicates << "\n";

        os << print_indent << "fluent predicates = " << el.fluent_predicates << "\n";

        os << print_indent << "derived predicates = " << el.derived_predicates << "\n";

        os << print_indent << "static functions = " << el.static_functions << "\n";

        os << print_indent << "fluent functions = " << el.fluent_functions << "\n";

        os << print_indent << "auxiliary function = " << el.auxiliary_function << "\n";

        os << print_indent << "constants = " << el.constants << "\n";

        os << print_indent << "actions = " << el.actions << "\n";

        os << print_indent << "axioms = " << el.axioms << "\n";
    }
    os << print_indent << ")";

    return os;
}

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::Domain>, C>& el)
{
    os << "Domain(\n";
    {
        IndentScope scope(os);

        os << print_indent << "name = " << el.get_name() << "\n";

        os << print_indent << "static predicates = " << el.template get_predicates<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent predicates = " << el.template get_predicates<formalism::FluentTag>() << "\n";

        os << print_indent << "derived predicates = " << el.template get_predicates<formalism::DerivedTag>() << "\n";

        os << print_indent << "static functions = " << el.template get_functions<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent functions = " << el.template get_functions<formalism::FluentTag>() << "\n";

        os << print_indent << "auxiliary function = " << el.get_auxiliary_function() << "\n";

        os << print_indent << "constants = " << el.get_constants() << "\n";

        os << print_indent << "actions = " << el.get_actions() << "\n";

        os << print_indent << "axioms = " << el.get_axioms() << "\n";
    }
    os << print_indent << ")";

    return os;
}

template<formalism::FactKind T>
inline std::ostream& print(std::ostream& os, const Data<formalism::FDRVariable<T>>& el)
{
    os << "FDRVariable(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.index << "\n";

        os << print_indent << "domain size = " << el.domain_size << "\n";

        os << print_indent << "atoms = " << el.atoms << "\n";
    }
    os << print_indent << ")";

    return os;
}

template<formalism::FactKind T, formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::FDRVariable<T>>, C>& el)
{
    os << "FDRVariable(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.get_index() << "\n";

        os << print_indent << "domain size = " << el.get_domain_size() << "\n";

        os << print_indent << "atoms = " << el.get_atoms() << "\n";
    }
    os << print_indent << ")";

    return os;
}

inline std::ostream& print(std::ostream& os, const formalism::FDRValue& el)
{
    fmt::print(os, "{}", uint_t(el));
    return os;
}

template<formalism::FactKind T>
inline std::ostream& print(std::ostream& os, const Data<formalism::FDRFact<T>>& el)
{
    fmt::print(os, "<{},{}>", to_string(el.variable), to_string(el.value));
    return os;
}

template<formalism::FactKind T, formalism::Context C>
std::ostream& print(std::ostream& os, const View<Data<formalism::FDRFact<T>>, C>& el)
{
    fmt::print(os, "<{},{}>", to_string(el.get_variable().get_index()), to_string(el.get_value()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::GroundFDRConjunctiveCondition>& el)
{
    os << "GroundFDRConjunctiveCondition(\n";
    {
        IndentScope scope(os);

        os << print_indent << "static literals = " << el.static_literals << "\n";

        os << print_indent << "fluent facts = " << el.fluent_facts << "\n";

        os << print_indent << "derived literals = " << el.derived_literals << "\n";

        os << print_indent << "numeric constraints = " << el.numeric_constraints << "\n";
    }
    os << print_indent << ")";

    return os;
}

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::GroundFDRConjunctiveCondition>, C>& el)
{
    os << "FDRConjunctiveCondition(\n";
    {
        IndentScope scope(os);

        os << print_indent << "fluent facts = " << el.template get_facts<formalism::FluentTag>() << "\n";

        os << print_indent << "derived facts = " << el.template get_facts<formalism::DerivedTag>() << "\n";

        os << print_indent << "numeric constraints = " << el.get_numeric_constraints() << "\n";
    }
    os << print_indent << ")";

    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::FDRTask>& el)
{
    os << "FDRTask(\n";
    {
        IndentScope scope(os);

        os << print_indent << "name = " << el.name << "\n";

        os << print_indent << "derived predicates = " << el.derived_predicates << "\n";

        os << print_indent << "objects = " << el.objects << "\n";

        os << print_indent << "static atoms = " << el.static_atoms << "\n";

        os << print_indent << "fluent atoms = " << el.fluent_atoms << "\n";

        os << print_indent << "derived atoms = " << el.derived_atoms << "\n";

        os << print_indent << "static numeric variables = " << el.static_fterm_values << "\n";

        os << print_indent << "fluent numeric variables = " << el.fluent_fterm_values << "\n";

        os << print_indent << "auxiliary numeric variable = " << el.auxiliary_fterm_value << "\n";

        os << print_indent << "goal = " << el.goal << "\n";

        os << print_indent << "metric = " << el.metric << "\n";

        os << print_indent << "axioms = " << el.axioms << "\n";

        os << print_indent << "fluent variables = " << el.fluent_variables << "\n";

        os << print_indent << "derived variables = " << el.derived_variables << "\n";

        os << print_indent << "fluent facts = " << el.fluent_facts << "\n";

        os << print_indent << "goal = " << el.goal << "\n";

        os << print_indent << "ground actions = " << el.ground_actions << "\n";

        os << print_indent << "ground axioms = " << el.ground_axioms << "\n";
    }
    os << print_indent << ")";

    return os;
}

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::FDRTask>, C>& el)
{
    os << "FDRTask(\n";
    {
        IndentScope scope(os);

        os << print_indent << "name = " << el.get_name() << "\n";

        os << print_indent << "derived predicates = " << el.get_derived_predicates() << "\n";

        os << print_indent << "objects = " << el.get_objects() << "\n";

        os << print_indent << "static atoms = " << el.template get_atoms<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent atoms = " << el.template get_atoms<formalism::FluentTag>() << "\n";

        os << print_indent << "derived atoms = " << el.template get_atoms<formalism::DerivedTag>() << "\n";

        os << print_indent << "static numeric variables = " << el.template get_fterm_values<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent numeric variables = " << el.template get_fterm_values<formalism::FluentTag>() << "\n";

        os << print_indent << "auxiliary numeric variable = " << el.get_auxiliary_fterm_value() << "\n";

        os << print_indent << "goal = " << el.get_goal() << "\n";

        os << print_indent << "metric = " << el.get_metric() << "\n";

        os << print_indent << "axioms = " << el.get_axioms() << "\n";

        os << print_indent << "fluent variables = " << el.template get_variables<formalism::FluentTag>() << "\n";

        os << print_indent << "derived variables = " << el.template get_variables<formalism::DerivedTag>() << "\n";

        os << print_indent << "fluent facts = " << el.get_fluent_facts() << "\n";

        os << print_indent << "goal = " << el.get_goal() << "\n";

        os << print_indent << "ground actions = " << el.get_ground_actions() << "\n";

        os << print_indent << "ground axioms = " << el.get_ground_axioms() << "\n";
    }
    os << print_indent << ")";

    return os;
}

namespace formalism
{
inline std::ostream& operator<<(std::ostream& os, const ParameterIndex& el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, OpEq el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, OpNe el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, OpLe el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, OpLt el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, OpGe el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, OpGt el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, OpAdd el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, OpSub el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, OpMul el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, OpDiv el) { return tyr::print(os, el); }

template<OpKind Op, typename T>
inline std::ostream& operator<<(std::ostream& os, const Data<UnaryOperator<Op, T>>& el)
{
    return tyr::print(os, el);
}

template<OpKind Op, typename T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<UnaryOperator<Op, T>>, C>& el)
{
    return tyr::print(os, el);
}

template<OpKind Op, typename T>
inline std::ostream& operator<<(std::ostream& os, const Data<BinaryOperator<Op, T>>& el)
{
    return tyr::print(os, el);
}

template<OpKind Op, typename T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<BinaryOperator<Op, T>>, C>& el)
{
    return tyr::print(os, el);
}

template<OpKind Op, typename T>
inline std::ostream& operator<<(std::ostream& os, const Data<MultiOperator<Op, T>>& el)
{
    return tyr::print(os, el);
}

template<OpKind Op, typename T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<MultiOperator<Op, T>>, C>& el)
{
    return tyr::print(os, el);
}

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const Data<ArithmeticOperator<T>>& el)
{
    return tyr::print(os, el);
}

template<typename T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Data<ArithmeticOperator<T>>, C>& el)
{
    return tyr::print(os, el);
}

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const Data<BooleanOperator<T>>& el)
{
    return tyr::print(os, el);
}

template<typename T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Data<BooleanOperator<T>>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<Variable>& el) { return tyr::print(os, el); }

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Variable>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<Object>& el) { return tyr::print(os, el); }

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Object>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<Binding>& el) { return tyr::print(os, el); }

template<formalism::Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Binding>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<Term>& el) { return tyr::print(os, el); }

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Data<Term>, C>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<Predicate<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Predicate<T>>, C>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<Atom<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Atom<T>>, C>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<Literal<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Literal<T>>, C>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<GroundAtom<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<GroundAtom<T>>, C>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<GroundLiteral<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<GroundLiteral<T>>, C>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<Function<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Function<T>>, C>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<FunctionTerm<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<FunctionTerm<T>>, C>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<GroundFunctionTerm<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<GroundFunctionTerm<T>>, C>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<GroundFunctionTermValue<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T, Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<GroundFunctionTermValue<T>>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<FunctionExpression>& el) { return tyr::print(os, el); }

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Data<FunctionExpression>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<GroundFunctionExpression>& el) { return tyr::print(os, el); }

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Data<GroundFunctionExpression>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<ConjunctiveCondition>& el) { return tyr::print(os, el); }

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<ConjunctiveCondition>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<Rule>& el) { return tyr::print(os, el); }

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Rule>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<GroundConjunctiveCondition>& el) { return tyr::print(os, el); }

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<GroundConjunctiveCondition>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<GroundRule>& el) { return tyr::print(os, el); }

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<GroundRule>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<Program>& el) { return tyr::print(os, el); }

template<Context C>
inline std::ostream& operator<<(std::ostream& os, const View<Index<Program>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const OpAssign& el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, const OpIncrease& el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, const OpDecrease& el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, const OpScaleUp& el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, const OpScaleDown& el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, const Minimize& el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, const Maximize& el) { return tyr::print(os, el); }

template<NumericEffectOpKind Op, FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<NumericEffect<Op, T>>& el)
{
    return tyr::print(os, el);
}

template<NumericEffectOpKind Op, FactKind T, Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<NumericEffect<Op, T>>, C>& el)
{
    return tyr::print(os, el);
}

template<NumericEffectOpKind Op, FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundNumericEffect<Op, T>>& el)
{
    return tyr::print(os, el);
}

template<NumericEffectOpKind Op, FactKind T, Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<GroundNumericEffect<Op, T>>, C>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<NumericEffectOperator<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T, Context C>
std::ostream& operator<<(std::ostream& os, const View<Data<NumericEffectOperator<T>>, C>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundNumericEffectOperator<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T, Context C>
std::ostream& operator<<(std::ostream& os, const View<Data<GroundNumericEffectOperator<T>>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<ConditionalEffect>& el) { return tyr::print(os, el); }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<ConditionalEffect>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<GroundConditionalEffect>& el) { return tyr::print(os, el); }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<GroundConditionalEffect>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<ConjunctiveEffect>& el) { return tyr::print(os, el); }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<ConjunctiveEffect>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<GroundConjunctiveEffect>& el) { return tyr::print(os, el); }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<GroundConjunctiveEffect>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<Action>& el) { return tyr::print(os, el); }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<Action>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<GroundAction>& el) { return tyr::print(os, el); }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<GroundAction>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<Axiom>& el) { return tyr::print(os, el); }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<Axiom>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<GroundAxiom>& el) { return tyr::print(os, el); }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<GroundAxiom>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<Metric>& el) { return tyr::print(os, el); }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<Metric>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<Task>& el) { return tyr::print(os, el); }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<Task>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<Domain>& el) { return tyr::print(os, el); }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<Domain>, C>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<FDRVariable<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T, Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<FDRVariable<T>>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const FDRValue& el) { return tyr::print(os, el); }

template<FactKind T>
inline std::ostream& operator<<(std::ostream& os, const Data<FDRFact<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T, Context C>
std::ostream& operator<<(std::ostream& os, const View<Data<FDRFact<T>>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<GroundFDRConjunctiveCondition>& el) { return tyr::print(os, el); }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<GroundFDRConjunctiveCondition>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<FDRTask>& el) { return tyr::print(os, el); }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<FDRTask>, C>& el)
{
    return tyr::print(os, el);
}

}
}
#endif
