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
#include "tyr/formalism/datas.hpp"
#include "tyr/formalism/views.hpp"

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <ostream>

namespace tyr
{
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
    fmt::print(os, "({} {})", to_string(el.predicate), fmt::format("{}", fmt::join(to_strings(el.objects), " ")));
    return os;
}

template<formalism::FactKind T, formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::GroundAtom<T>>, C>& el)
{
    fmt::print(os, "({} {})", to_string(el.get_predicate().get_name()), fmt::format("{}", fmt::join(to_strings(el.get_objects()), " ")));
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
    fmt::print(os, "({} {})", to_string(el.function), fmt::format("{}", fmt::join(to_strings(el.objects), " ")));
    return os;
}

template<formalism::FactKind T, formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::GroundFunctionTerm<T>>, C>& el)
{
    fmt::print(os, "({} {})", to_string(el.get_function().get_name()), fmt::format("{}", fmt::join(to_strings(el.get_objects()), " ")));
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
    fmt::print(os,
               "{}{}{}",
               (el.static_literals.empty() ? "" : fmt::format("{}", fmt::join(to_strings(el.static_literals), ", "))),
               (el.fluent_literals.empty() ? "" : fmt::format(" {}", fmt::join(to_strings(el.fluent_literals), ", "))),
               (el.numeric_constraints.empty() ? "" : fmt::format(" {}", fmt::join(to_strings(el.numeric_constraints), ", "))));
    return os;
}

template<formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::ConjunctiveCondition>, C>& el)
{
    fmt::print(os,
               "{}{}{}",
               (el.template get_literals<formalism::StaticTag>().empty() ?
                    "" :
                    fmt::format("{}", fmt::join(to_strings(el.template get_literals<formalism::StaticTag>()), ", "))),
               (el.template get_literals<formalism::FluentTag>().empty() ?
                    "" :
                    fmt::format(" {}", fmt::join(to_strings(el.template get_literals<formalism::FluentTag>()), ", "))),
               (el.get_numeric_constraints().empty() ? "" : fmt::format(" {}", fmt::join(to_strings(el.get_numeric_constraints()), ", "))));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Rule>& el)
{
    fmt::print(os, "{} :- {}", to_string(el.head), to_string(el.body));
    return os;
}

template<formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::Rule>, C>& el)
{
    fmt::print(os, "{} :- {}", to_string(el.get_head()), to_string(el.get_body()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::GroundConjunctiveCondition>& el)
{
    fmt::print(os,
               "{}{}{}",
               (el.static_literals.empty() ? "" : fmt::format("{}", fmt::join(to_strings(el.static_literals), ", "))),
               (el.fluent_literals.empty() ? "" : fmt::format(" {}", fmt::join(to_strings(el.fluent_literals), ", "))),
               (el.numeric_constraints.empty() ? "" : fmt::format(" {}", fmt::join(to_strings(el.numeric_constraints), ", "))));
    return os;
}

template<formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::GroundConjunctiveCondition>, C>& el)
{
    fmt::print(os,
               "{}{}{}",
               (el.template get_literals<formalism::StaticTag>().empty() ?
                    "" :
                    fmt::format("{}", fmt::join(to_strings(el.template get_literals<formalism::StaticTag>()), ", "))),
               (el.template get_literals<formalism::FluentTag>().empty() ?
                    "" :
                    fmt::format(" {}", fmt::join(to_strings(el.template get_literals<formalism::FluentTag>()), ", "))),
               (el.get_numeric_constraints().empty() ? "" : fmt::format(" {}", fmt::join(to_strings(el.get_numeric_constraints()), ", "))));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::GroundRule>& el)
{
    fmt::print(os, "{} :- {}", to_string(el.head), to_string(el.body));
    return os;
}

template<formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::GroundRule>, C>& el)
{
    fmt::print(os, "{} :- {}", to_string(el.get_head()), to_string(el.get_body()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Program>& el)
{
    fmt::print(os,
               "% static predicates\n{}\n"
               "% fluent predicates\n{}\n"
               "% static functions\n{}\n"
               "% fluent functions\n{}\n"
               "% objects\n{}\n"
               "% static atoms\n{}\n"
               "% fluent atoms\n{}\n"
               "% static fterms\n{}\n"
               "% fluent fterms\n{}\n"
               "% rules\n{}",
               fmt::format("{}", fmt::join(to_strings(el.static_predicates), "\n")),
               fmt::format("{}", fmt::join(to_strings(el.fluent_predicates), "\n")),
               fmt::format("{}", fmt::join(to_strings(el.static_functions), "\n")),
               fmt::format("{}", fmt::join(to_strings(el.fluent_functions), "\n")),
               fmt::format("{}", fmt::join(to_strings(el.objects), "\n")),
               fmt::format("{}", fmt::join(to_strings(el.static_atoms), "\n")),
               fmt::format("{}", fmt::join(to_strings(el.fluent_atoms), "\n")),
               fmt::format("{}", fmt::join(to_strings(el.static_fterm_values), "\n")),
               fmt::format("{}", fmt::join(to_strings(el.fluent_fterm_values), "\n")),
               fmt::format("{}", fmt::join(to_strings(el.rules), "\n")));
    return os;
}

template<formalism::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::Program>, C>& el)
{
    fmt::print(os,
               "% static predicates\n{}\n"
               "% fluent predicates\n{}\n"
               "% static functions\n{}\n"
               "% fluent functions\n{}\n"
               "% objects\n{}\n"
               "% static atoms\n{}\n"
               "% fluent atoms\n{}\n"
               "% static fterms\n{}\n"
               "% fluent fterms\n{}\n"
               "% rules\n{}",
               fmt::format("{}", fmt::join(to_strings(el.template get_predicates<formalism::StaticTag>()), "\n")),
               fmt::format("{}", fmt::join(to_strings(el.template get_predicates<formalism::FluentTag>()), "\n")),
               fmt::format("{}", fmt::join(to_strings(el.template get_functions<formalism::StaticTag>()), "\n")),
               fmt::format("{}", fmt::join(to_strings(el.template get_functions<formalism::FluentTag>()), "\n")),
               fmt::format("{}", fmt::join(to_strings(el.get_objects()), "\n")),
               fmt::format("{}", fmt::join(to_strings(el.template get_atoms<formalism::StaticTag>()), "\n")),
               fmt::format("{}", fmt::join(to_strings(el.template get_atoms<formalism::FluentTag>()), "\n")),
               fmt::format("{}", fmt::join(to_strings(el.template get_fterm_values<formalism::StaticTag>()), "\n")),
               fmt::format("{}", fmt::join(to_strings(el.template get_fterm_values<formalism::FluentTag>()), "\n")),
               fmt::format("{}", fmt::join(to_strings(el.get_rules()), "\n")));
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
    return os;
}

template<formalism::FactKind T, formalism::Context C>
std::ostream& print(std::ostream& os, const View<Data<formalism::NumericEffectOperator<T>>, C>& el)
{
    return tyr::print(os, el.get_variant());
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::GroundNumericEffectOperator<T>>& el)
{
    return os;
}

template<formalism::FactKind T, formalism::Context C>
std::ostream& print(std::ostream& os, const View<Data<formalism::GroundNumericEffectOperator<T>>, C>& el)
{
    return tyr::print(os, el.get_variant());
}

inline std::ostream& print(std::ostream& os, const Data<formalism::ConditionalEffect>& el) { return os; }

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::ConditionalEffect>, C>& el)
{
    fmt::print(os,
               "\nCondition: {}\n"
               "Effect: {}",
               to_string(el.get_condition()),
               to_string(el.get_effect()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::GroundConditionalEffect>& el) { return os; }

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::GroundConditionalEffect>, C>& el)
{
    fmt::print(os,
               "\nCondition: {}\n"
               "Effect: {}",
               to_string(el.get_condition()),
               to_string(el.get_effect()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::ConjunctiveEffect>& el) { return os; }

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::ConjunctiveEffect>, C>& el)
{
    fmt::print(os,
               "\nFluent literals: {}\n"
               "Fluent numeric effects: {}\n"
               "Auxiliary numeric effect: {}",
               to_string(el.get_literals()),
               to_string(el.get_numeric_effects()),
               to_string(el.get_auxiliary_numeric_effect()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::GroundConjunctiveEffect>& el) { return os; }

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::GroundConjunctiveEffect>, C>& el)
{
    fmt::print(os,
               "\nFluent literals: {}\n"
               "Fluent numeric effects: {}\n"
               "Auxiliary numeric effect: {}",
               to_string(el.get_literals()),
               to_string(el.get_numeric_effects()),
               to_string(el.get_auxiliary_numeric_effect()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Action>& el) { return os; }

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::Action>, C>& el)
{
    fmt::print(os,
               "\nName: {}\n"
               "Condition: {}\n"
               "Effect: {}",
               to_string(el.get_name()),
               to_string(el.get_condition()),
               to_string(el.get_effects()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::GroundAction>& el) { return os; }

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::GroundAction>, C>& el)
{
    fmt::print(os,
               "\nName: {}\n"
               "Condition: {}\n"
               "Effect: {}",
               to_string(el.get_name()),
               to_string(el.get_condition()),
               to_string(el.get_effects()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Axiom>& el) { return os; }

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::Axiom>, C>& el)
{
    fmt::print(os,
               "\nBody: {}\n"
               "Head: {}",
               to_string(el.get_body()),
               to_string(el.get_head()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::GroundAxiom>& el) { return os; }

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::GroundAxiom>, C>& el)
{
    fmt::print(os,
               "\nBody: {}\n"
               "Head: {}",
               to_string(el.get_body()),
               to_string(el.get_head()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Metric>& el) { return os; }

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::Metric>, C>& el)
{
    fmt::print(os, "({} {})", to_string(el.get_objective()), to_string(el.get_fexpr()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Task>& el) { return os; }

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::Task>, C>& el)
{
    fmt::print(os,
               "\nName: {}\n"
               "Derived predicates: {}\n"
               "Objects: {}\n"
               "Static atoms: {}\n"
               "Fluent atoms: {}\n"
               "Static function term values: {}\n"
               "Fluent function term values: {}\n"
               "Auxiliary function term value: {}\n"
               "Goal: {}\n"
               "Metric: {}\n"
               "Axioms: {}",
               to_string(el.get_name()),
               to_string(el.template get_derived_predicates()),
               to_string(el.get_objects()),
               to_string(el.template get_atoms<formalism::StaticTag>()),
               to_string(el.template get_atoms<formalism::FluentTag>()),
               to_string(el.template get_fterm_values<formalism::StaticTag>()),
               to_string(el.template get_fterm_values<formalism::FluentTag>()),
               to_string(el.get_auxiliary_fterm_value()),
               to_string(el.get_goal()),
               to_string(el.get_metric()),
               to_string(el.get_axioms()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::Domain>& el) { return os; }

template<formalism::Context C>
std::ostream& print(std::ostream& os, const View<Index<formalism::Domain>, C>& el)
{
    fmt::print(os,
               "\nName: {}\n"
               "Static predicates: {}\n"
               "Fluent predicates: {}\n"
               "Derived predicates: {}\n"
               "Static functions: {}\n"
               "Fluent functions: {}\n"
               "Auxiliary function: {}\n"
               "Constants: {}\n"
               "Actions: {}\n"
               "Axioms: {}",
               to_string(el.get_name()),
               to_string(el.template get_predicates<formalism::StaticTag>()),
               to_string(el.template get_predicates<formalism::FluentTag>()),
               to_string(el.template get_predicates<formalism::DerivedTag>()),
               to_string(el.template get_functions<formalism::StaticTag>()),
               to_string(el.template get_functions<formalism::FluentTag>()),
               to_string(el.get_auxiliary_function()),
               to_string(el.get_constants()),
               to_string(el.get_actions()),
               to_string(el.get_axioms()));
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

inline std::ostream& operator<<(std::ostream& os, const Data<ConditionalEffect>& el) { return os; }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<ConditionalEffect>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<GroundConditionalEffect>& el) { return os; }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<GroundConditionalEffect>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<ConjunctiveEffect>& el) { return os; }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<ConjunctiveEffect>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<GroundConjunctiveEffect>& el) { return os; }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<GroundConjunctiveEffect>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<Action>& el) { return os; }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<Action>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<GroundAction>& el) { return os; }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<GroundAction>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<Axiom>& el) { return os; }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<Axiom>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<GroundAxiom>& el) { return os; }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<GroundAxiom>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<Metric>& el) { return os; }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<Metric>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<Task>& el) { return os; }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<Task>, C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const Data<Domain>& el) { return os; }

template<Context C>
std::ostream& operator<<(std::ostream& os, const View<Index<Domain>, C>& el)
{
    return tyr::print(os, el);
}

}
}
#endif
