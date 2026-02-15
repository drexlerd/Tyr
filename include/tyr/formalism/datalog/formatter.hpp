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

#ifndef TYR_FORMALISM_DATALOG_FORMATTER_HPP_
#define TYR_FORMALISM_DATALOG_FORMATTER_HPP_

#include "tyr/common/formatter.hpp"
#include "tyr/common/iostream.hpp"
#include "tyr/formalism/datalog/datas.hpp"
#include "tyr/formalism/datalog/variable_dependency_graph.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/formatter.hpp"

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <ostream>

namespace tyr
{
/**
 * Forward declarations
 */
namespace formalism::datalog
{
inline std::ostream& operator<<(std::ostream& os, const VariableDependencyGraph& el);

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

}  // end namespace formalism

/**
 * Definitions
 */

inline std::ostream& print(std::ostream& os, const formalism::datalog::VariableDependencyGraph& el)
{
    os << "graph {\n";

    const auto& adj_matrix = el.get_adj_matrix();
    const auto k = adj_matrix.k();

    for (uint_t i = 0; i < k; ++i)
    {
        fmt::print(os, "n{} [label=\"V{}\"];\n", i, i);
    }

    for (uint_t i = 0; i < k; ++i)
    {
        for (uint_t j = i + 1; j < k; ++j)
        {
            const auto& cell = adj_matrix.get_cell(formalism::ParameterIndex(i), formalism::ParameterIndex(j));

            auto labels = std::vector<std::string> {};

            for (const auto& label : cell.get_literal_labels<formalism::StaticTag>())
                labels.push_back(to_string(label));
            for (const auto& label : cell.get_literal_labels<formalism::FluentTag>())
                labels.push_back(to_string(label));
            for (const auto& label : cell.get_numeric_constraint_labels())
                labels.push_back(to_string(label));

            if (!labels.empty())
            {
                fmt::print(os, "n{} -- n{} [label=\"{}\\l\"];\n", i, j, fmt::join(labels, "\\l"));
            }
        }
    }

    os << "}\n";

    return os;
}

template<formalism::OpKind Op, typename T>
inline std::ostream& print(std::ostream& os, const Data<formalism::datalog::UnaryOperator<Op, T>>& el)
{
    fmt::print(os, "({} {})", to_string(Op {}), to_string(el.arg));
    return os;
}

template<formalism::OpKind Op, typename T, formalism::datalog::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::datalog::UnaryOperator<Op, T>>, C>& el)
{
    fmt::print(os, "({} {})", to_string(Op {}), to_string(el.get_arg()));
    return os;
}

template<formalism::OpKind Op, typename T>
inline std::ostream& print(std::ostream& os, const Data<formalism::datalog::BinaryOperator<Op, T>>& el)
{
    fmt::print(os, "({} {} {})", to_string(Op {}), to_string(el.lhs), to_string(el.rhs));
    return os;
}

template<formalism::OpKind Op, typename T, formalism::datalog::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::datalog::BinaryOperator<Op, T>>, C>& el)
{
    fmt::print(os, "({} {} {})", to_string(Op {}), to_string(el.get_lhs()), to_string(el.get_rhs()));
    return os;
}

template<formalism::OpKind Op, typename T>
inline std::ostream& print(std::ostream& os, const Data<formalism::datalog::MultiOperator<Op, T>>& el)
{
    fmt::print(os, "({} {})", to_string(Op {}), fmt::format("{}", fmt::join(to_strings(el.args), " ")));
    return os;
}

template<formalism::OpKind Op, typename T, formalism::datalog::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::datalog::MultiOperator<Op, T>>, C>& el)
{
    fmt::print(os, "({} {})", to_string(Op {}), fmt::format("{}", fmt::join(to_strings(el.get_args()), " ")));
    return os;
}

template<typename T>
inline std::ostream& print(std::ostream& os, const Data<formalism::datalog::ArithmeticOperator<T>>& el)
{
    fmt::print(os, "{}", to_string(el.value));
    return os;
}

template<typename T, formalism::datalog::Context C>
inline std::ostream& print(std::ostream& os, const View<Data<formalism::datalog::ArithmeticOperator<T>>, C>& el)
{
    fmt::print(os, "{}", to_string(el.get_variant()));
    return os;
}

template<typename T>
inline std::ostream& print(std::ostream& os, const Data<formalism::datalog::BooleanOperator<T>>& el)
{
    fmt::print(os, "{}", to_string(el.value));
    return os;
}

template<typename T, formalism::datalog::Context C>
inline std::ostream& print(std::ostream& os, const View<Data<formalism::datalog::BooleanOperator<T>>, C>& el)
{
    fmt::print(os, "{}", to_string(el.get_variant()));
    return os;
}

template<formalism::FactKind T>
inline std::ostream& print(std::ostream& os, const Data<formalism::datalog::Atom<T>>& el)
{
    fmt::print(os, "({} {})", to_string(el.predicate), fmt::format("{}", fmt::join(to_strings(el.terms), " ")));
    return os;
}

template<formalism::FactKind T, formalism::datalog::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::datalog::Atom<T>>, C>& el)
{
    fmt::print(os, "({} {})", to_string(el.get_predicate().get_name()), fmt::format("{}", fmt::join(to_strings(el.get_terms()), " ")));
    return os;
}

template<formalism::FactKind T>
inline std::ostream& print(std::ostream& os, const Data<formalism::datalog::Literal<T>>& el)
{
    if (el.polarity)
        print(os, to_string(el.atom));
    else
        fmt::print(os, "(not {})", to_string(el.atom));
    return os;
}

template<formalism::FactKind T, formalism::datalog::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::datalog::Literal<T>>, C>& el)
{
    if (el.get_polarity())
        print(os, to_string(el.get_atom()));
    else
        fmt::print(os, "(not {})", to_string(el.get_atom()));
    return os;
}

template<formalism::FactKind T>
inline std::ostream& print(std::ostream& os, const Data<formalism::datalog::GroundAtom<T>>& el)
{
    fmt::print(os, "({} {})", to_string(el.predicate), fmt::format("{}", fmt::join(to_strings(el.objects), " ")));
    return os;
}

template<formalism::FactKind T, formalism::datalog::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::datalog::GroundAtom<T>>, C>& el)
{
    fmt::print(os, "({} {})", to_string(el.get_predicate().get_name()), fmt::format("{}", fmt::join(to_strings(el.get_objects()), " ")));
    return os;
}

template<formalism::FactKind T>
inline std::ostream& print(std::ostream& os, const Data<formalism::datalog::GroundLiteral<T>>& el)
{
    if (el.polarity)
        print(os, to_string(el.atom));
    else
        fmt::print(os, "(not {})", to_string(el.atom));
    return os;
}

template<formalism::FactKind T, formalism::datalog::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::datalog::GroundLiteral<T>>, C>& el)
{
    if (el.get_polarity())
        print(os, to_string(el.get_atom()));
    else
        fmt::print(os, "(not {})", to_string(el.get_atom()));
    return os;
}

template<formalism::FactKind T>
inline std::ostream& print(std::ostream& os, const Data<formalism::datalog::FunctionTerm<T>>& el)
{
    fmt::print(os, "({} {})", to_string(el.function), fmt::format("{}", fmt::join(to_strings(el.terms), " ")));
    return os;
}

template<formalism::FactKind T, formalism::datalog::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::datalog::FunctionTerm<T>>, C>& el)
{
    fmt::print(os, "({} {})", to_string(el.get_function().get_name()), fmt::format("{}", fmt::join(to_strings(el.get_terms()), " ")));
    return os;
}

template<formalism::FactKind T>
inline std::ostream& print(std::ostream& os, const Data<formalism::datalog::GroundFunctionTerm<T>>& el)
{
    fmt::print(os, "({} {})", to_string(el.function), fmt::format("{}", fmt::join(to_strings(el.objects), " ")));
    return os;
}

template<formalism::FactKind T, formalism::datalog::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::datalog::GroundFunctionTerm<T>>, C>& el)
{
    fmt::print(os, "({} {})", to_string(el.get_function().get_name()), fmt::format("{}", fmt::join(to_strings(el.get_objects()), " ")));
    return os;
}

template<formalism::FactKind T>
inline std::ostream& print(std::ostream& os, const Data<formalism::datalog::GroundFunctionTermValue<T>>& el)
{
    fmt::print(os, "(= {} {})", to_string(el.fterm), to_string(el.value));
    return os;
}

template<formalism::FactKind T, formalism::datalog::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::datalog::GroundFunctionTermValue<T>>, C>& el)
{
    fmt::print(os, "(= {} {})", to_string(el.get_fterm()), to_string(el.get_value()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::datalog::FunctionExpression>& el)
{
    fmt::print(os, "{}", to_string(el.value));
    return os;
}

template<formalism::datalog::Context C>
inline std::ostream& print(std::ostream& os, const View<Data<formalism::datalog::FunctionExpression>, C>& el)
{
    fmt::print(os, "{}", to_string(el.get_variant()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::datalog::GroundFunctionExpression>& el)
{
    fmt::print(os, "{}", to_string(el.value));
    return os;
}

template<formalism::datalog::Context C>
inline std::ostream& print(std::ostream& os, const View<Data<formalism::datalog::GroundFunctionExpression>, C>& el)
{
    fmt::print(os, "{}", to_string(el.get_variant()));
    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::datalog::ConjunctiveCondition>& el)
{
    os << "ConjunctiveCondition(\n";
    {
        IndentScope scope(os);

        os << print_indent << "variables = " << el.variables << "\n";

        os << print_indent << "static literals = " << el.static_literals << "\n";

        os << print_indent << "fluent literals = " << el.fluent_literals << "\n";

        os << print_indent << "numeric constraints = " << el.numeric_constraints << "\n";
    }
    os << print_indent << ")";

    return os;
}

template<formalism::datalog::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::datalog::ConjunctiveCondition>, C>& el)
{
    os << "ConjunctiveCondition(\n";
    {
        IndentScope scope(os);

        os << print_indent << "variables = " << el.get_variables() << "\n";

        os << print_indent << "static literals = " << el.template get_literals<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent literals = " << el.template get_literals<formalism::FluentTag>() << "\n";

        os << print_indent << "numeric constraints = " << el.get_numeric_constraints() << "\n";
    }
    os << print_indent << ")";

    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::datalog::Rule>& el)
{
    os << "Rule(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.index << "\n";

        os << print_indent << "variables = " << el.variables << "\n";

        os << print_indent << "head = " << el.head << "\n";

        os << print_indent << "body = " << el.body << "\n";

        os << print_indent << "cost = " << el.cost << "\n";
    }
    os << print_indent << ")";

    return os;
}

template<formalism::datalog::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::datalog::Rule>, C>& el)
{
    os << "Rule(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.get_index() << "\n";

        os << print_indent << "variables = " << el.get_variables() << "\n";

        os << print_indent << "head = " << el.get_head() << "\n";

        os << print_indent << "body = " << el.get_body() << "\n";

        os << print_indent << "cost = " << el.get_cost() << "\n";
    }
    os << print_indent << ")";

    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::datalog::GroundConjunctiveCondition>& el)
{
    os << "GroundConjunctiveCondition(\n";
    {
        IndentScope scope(os);

        os << print_indent << "static literals = " << el.static_literals << "\n";

        os << print_indent << "fluent literals = " << el.fluent_literals << "\n";

        os << print_indent << "numeric constraints = " << el.numeric_constraints << "\n";
    }
    os << print_indent << ")";

    return os;
}

template<formalism::datalog::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::datalog::GroundConjunctiveCondition>, C>& el)
{
    os << "GroundConjunctiveCondition(\n";
    {
        IndentScope scope(os);

        os << print_indent << "static literals = " << el.template get_literals<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent literals = " << el.template get_literals<formalism::FluentTag>() << "\n";

        os << print_indent << "numeric constraints = " << el.get_numeric_constraints() << "\n";
    }
    os << print_indent << ")";

    return os;
}

inline std::ostream& print(std::ostream& os, const Data<formalism::datalog::GroundRule>& el)
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

template<formalism::datalog::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::datalog::GroundRule>, C>& el)
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

inline std::ostream& print(std::ostream& os, const Data<formalism::datalog::Program>& el)
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

template<formalism::datalog::Context C>
inline std::ostream& print(std::ostream& os, const View<Index<formalism::datalog::Program>, C>& el)
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

namespace formalism::datalog
{

inline std::ostream& operator<<(std::ostream& os, const VariableDependencyGraph& el) { return tyr::print(os, el); }

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

}
}
#endif
