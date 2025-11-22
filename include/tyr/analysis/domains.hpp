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

#ifndef TYR_ANALYSIS_DOMAINS_HPP_
#define TYR_ANALYSIS_DOMAINS_HPP_

#include "tyr/common/formatter.hpp"
#include "tyr/common/unordered_set.hpp"
#include "tyr/formalism2/formalism.hpp"

#include <unordered_set>
#include <vector>

namespace tyr::analysis
{
using DomainSet = UnorderedSet<Index<formalism::Object>>;
using DomainSetList = std::vector<DomainSet>;
using DomainSetListList = std::vector<DomainSetList>;

using DomainList = std::vector<Index<formalism::Object>>;
using DomainListList = std::vector<DomainList>;
using DomainListListList = std::vector<DomainListList>;

struct VariableDomains
{
    DomainListListList static_predicate_domains;
    DomainListListList fluent_predicate_domains;
    DomainListListList static_function_domains;
    DomainListListList fluent_function_domains;
    DomainListListList rule_domains;
};

inline DomainListListList to_list(const DomainSetListList& set)
{
    auto vec = DomainListListList();
    vec.reserve(set.size());
    for (const auto& parameter_domains : set)
    {
        auto predicate_domains_vec = DomainListList();
        predicate_domains_vec.reserve(parameter_domains.size());
        for (const auto& parameter_domain : parameter_domains)
        {
            auto domain = DomainList(parameter_domain.begin(), parameter_domain.end());
            std::sort(domain.begin(), domain.end());
            predicate_domains_vec.push_back(std::move(domain));
        }
        vec.push_back(predicate_domains_vec);
    }
    return vec;
}

template<formalism::IsStaticOrFluentTag T>
inline DomainSetListList initialize_predicate_domain_sets(Proxy<formalism::Program, formalism::Repository> program)
{
    const auto num_predicates = program.get_predicates<T>().size();
    auto predicate_domain_sets = DomainSetListList(num_predicates);

    for (const auto predicate : program.get_predicates<T>())
        predicate_domain_sets[predicate.get_index().value].resize(predicate.get_arity());

    for (const auto atom : program.get_atoms<T>())
    {
        const auto predicate = atom.get_predicate();
        auto pos = size_t { 0 };
        for (const auto object : atom.get_terms())
            predicate_domain_sets[predicate.get_index().value][pos++].insert(object.get_index());
    }

    return predicate_domain_sets;
}

template<formalism::IsStaticOrFluentTag T>
inline DomainSetListList initialize_function_domain_sets(Proxy<formalism::Program, formalism::Repository> program)
{
    const auto num_functions = program.get_functions<T>().size();
    auto function_domain_sets = DomainSetListList(num_functions);

    for (const auto function : program.get_functions<T>())
        function_domain_sets[function.get_index().value].resize(function.get_arity());

    for (const auto term_value : program.get_function_values<T>())
    {
        const auto term = term_value.get_term();
        const auto function = term.get_function();
        auto pos = size_t { 0 };
        for (const auto object : term.get_terms())
            function_domain_sets[function.get_index().value][pos++].insert(object.get_index());
    }

    return function_domain_sets;
}

void restrict_parameter_domain_from_static_atom(Proxy<formalism::Atom<formalism::StaticTag>, formalism::Repository> atom,
                                                DomainSetList& parameter_domains,
                                                const DomainSetListList& static_predicate_domain_sets)
{
    const auto predicate = atom.get_predicate();

    auto pos = size_t { 0 };
    for (const auto term : atom.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using ProxyType = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<ProxyType, Proxy<formalism::Object, formalism::Repository>>) {}
                else if constexpr (std::is_same_v<ProxyType, formalism::ParameterIndex>)
                {
                    const auto parameter_index = to_uint_t(arg);
                    auto& parameter_domain = parameter_domains[parameter_index];
                    const auto& predicate_domain = static_predicate_domain_sets[predicate.get_index().value][pos];

                    intersect_inplace(parameter_domain, predicate_domain);
                }
                else
                {
                    static_assert(dependent_false<ProxyType>::value, "Missing case");
                }
            },
            term);
        ++pos;
    }
}

void restrict_parameter_domain_from_static_function_term(Proxy<formalism::FunctionTerm<formalism::StaticTag>, formalism::Repository> fterm,
                                                         DomainSetList& parameter_domains,
                                                         const DomainSetListList& static_function_domain_sets)
{
    const auto function = fterm.get_function();

    auto pos = size_t { 0 };
    for (const auto term : fterm.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using ProxyType = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<ProxyType, Proxy<formalism::Object, formalism::Repository>>) {}
                else if constexpr (std::is_same_v<ProxyType, formalism::ParameterIndex>)
                {
                    const auto parameter_index = to_uint_t(arg);
                    auto& parameter_domain = parameter_domains[parameter_index];
                    const auto& predicate_domain = static_function_domain_sets[function.get_index().value][pos];

                    intersect_inplace(parameter_domain, predicate_domain);
                }
                else
                {
                    static_assert(dependent_false<ProxyType>::value, "Missing case");
                }
            },
            term);
        ++pos;
    }
}

void restrict_parameter_domain_from_function_expression(Proxy<formalism::FunctionExpression, formalism::Repository> fexpr,
                                                        DomainSetList& parameter_domains,
                                                        const DomainSetListList& static_function_domain_sets)
{
    visit(
        [&](auto&& arg)
        {
            using ProxyType = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<ProxyType, formalism::Double>) {}
            else if constexpr (std::is_same_v<ProxyType,
                                              Proxy<formalism::UnaryOperator<formalism::OpSub, formalism::FunctionExpression>, formalism::Repository>>)
            {
                restrict_parameter_domain_from_function_expression(arg.get_arg(), parameter_domains, static_function_domain_sets);
            }
            else if constexpr (std::is_same_v<ProxyType,
                                              Proxy<formalism::BinaryOperator<formalism::OpSub, formalism::FunctionExpression>, formalism::Repository>>)
            {
                restrict_parameter_domain_from_function_expression(arg.get_lhs(), parameter_domains, static_function_domain_sets);
                restrict_parameter_domain_from_function_expression(arg.get_rhs(), parameter_domains, static_function_domain_sets);
            }
            else if constexpr (std::is_same_v<ProxyType,
                                              Proxy<formalism::BinaryOperator<formalism::OpAdd, formalism::FunctionExpression>, formalism::Repository>>)
            {
                restrict_parameter_domain_from_function_expression(arg.get_lhs(), parameter_domains, static_function_domain_sets);
                restrict_parameter_domain_from_function_expression(arg.get_rhs(), parameter_domains, static_function_domain_sets);
            }
            else if constexpr (std::is_same_v<ProxyType,
                                              Proxy<formalism::BinaryOperator<formalism::OpMul, formalism::FunctionExpression>, formalism::Repository>>)
            {
                restrict_parameter_domain_from_function_expression(arg.get_lhs(), parameter_domains, static_function_domain_sets);
                restrict_parameter_domain_from_function_expression(arg.get_rhs(), parameter_domains, static_function_domain_sets);
            }
            else if constexpr (std::is_same_v<ProxyType,
                                              Proxy<formalism::BinaryOperator<formalism::OpDiv, formalism::FunctionExpression>, formalism::Repository>>)
            {
                restrict_parameter_domain_from_function_expression(arg.get_lhs(), parameter_domains, static_function_domain_sets);
                restrict_parameter_domain_from_function_expression(arg.get_rhs(), parameter_domains, static_function_domain_sets);
            }
            else if constexpr (std::is_same_v<ProxyType,
                                              Proxy<formalism::MultiOperator<formalism::OpAdd, formalism::FunctionExpression>, formalism::Repository>>)
            {
                for (const auto part : arg.get_args())
                    restrict_parameter_domain_from_function_expression(part, parameter_domains, static_function_domain_sets);
            }
            else if constexpr (std::is_same_v<ProxyType,
                                              Proxy<formalism::MultiOperator<formalism::OpMul, formalism::FunctionExpression>, formalism::Repository>>)
            {
                for (const auto part : arg.get_args())
                    restrict_parameter_domain_from_function_expression(part, parameter_domains, static_function_domain_sets);
            }
            else if constexpr (std::is_same_v<ProxyType, Proxy<formalism::FunctionTerm<formalism::StaticTag>, formalism::Repository>>)
            {
                restrict_parameter_domain_from_static_function_term(arg, parameter_domains, static_function_domain_sets);
            }
            else if constexpr (std::is_same_v<ProxyType, Proxy<formalism::FunctionTerm<formalism::FluentTag>, formalism::Repository>>) {}
            else
            {
                static_assert(dependent_false<ProxyType>::value, "Missing case");
            }
        },
        fexpr);
}

void restrict_parameter_domain_from_boolean_operator(Proxy<formalism::BooleanOperator<formalism::FunctionExpression>, formalism::Repository> op,
                                                     DomainSetList& parameter_domains,
                                                     const DomainSetListList& static_function_domain_sets)
{
    visit(
        [&](auto&& arg)
        {
            restrict_parameter_domain_from_function_expression(arg.get_lhs(), parameter_domains, static_function_domain_sets);
            restrict_parameter_domain_from_function_expression(arg.get_rhs(), parameter_domains, static_function_domain_sets);
        },
        op);
}

void lift_parameter_domain_from_fluent_atom(Proxy<formalism::Atom<formalism::FluentTag>, formalism::Repository> atom,
                                            const DomainSetList& parameter_domains,
                                            DomainSetListList& fluent_predicate_domain_sets)
{
    const auto predicate = atom.get_predicate();

    auto pos = size_t { 0 };
    for (const auto term : atom.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using ProxyType = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<ProxyType, Proxy<formalism::Object, formalism::Repository>>) {}
                else if constexpr (std::is_same_v<ProxyType, formalism::ParameterIndex>)
                {
                    const auto parameter_index = to_uint_t(arg);
                    const auto& parameter_domain = parameter_domains[parameter_index];
                    auto& function_domain = fluent_predicate_domain_sets[predicate.get_index().value][pos];

                    union_inplace(function_domain, parameter_domain);
                }
                else
                {
                    static_assert(dependent_false<ProxyType>::value, "Missing case");
                }
            },
            term);
        ++pos;
    }
}

void lift_parameter_domain_from_fluent_function_term(Proxy<formalism::FunctionTerm<formalism::FluentTag>, formalism::Repository> fterm,
                                                     const DomainSetList& parameter_domains,
                                                     DomainSetListList& fluent_function_domain_sets)
{
    const auto function = fterm.get_function();

    auto pos = size_t { 0 };
    for (const auto term : fterm.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using ProxyType = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<ProxyType, Proxy<formalism::Object, formalism::Repository>>) {}
                else if constexpr (std::is_same_v<ProxyType, formalism::ParameterIndex>)
                {
                    const auto parameter_index = to_uint_t(arg);
                    const auto& parameter_domain = parameter_domains[parameter_index];
                    auto& function_domain = fluent_function_domain_sets[function.get_index().value][pos];

                    union_inplace(function_domain, parameter_domain);
                }
                else
                {
                    static_assert(dependent_false<ProxyType>::value, "Missing case");
                }
            },
            term);
        ++pos;
    }
}

void lift_parameter_domain_from_function_expression(Proxy<formalism::FunctionExpression, formalism::Repository> fexpr,
                                                    const DomainSetList& parameter_domains,
                                                    DomainSetListList& fluent_function_domain_sets)
{
    visit(
        [&](auto&& arg)
        {
            using ProxyType = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<ProxyType, formalism::Double>) {}
            else if constexpr (std::is_same_v<ProxyType,
                                              Proxy<formalism::UnaryOperator<formalism::OpSub, formalism::FunctionExpression>, formalism::Repository>>)
            {
                lift_parameter_domain_from_function_expression(arg.get_arg(), parameter_domains, fluent_function_domain_sets);
            }
            else if constexpr (std::is_same_v<ProxyType,
                                              Proxy<formalism::BinaryOperator<formalism::OpSub, formalism::FunctionExpression>, formalism::Repository>>)
            {
                lift_parameter_domain_from_function_expression(arg.get_lhs(), parameter_domains, fluent_function_domain_sets);
                lift_parameter_domain_from_function_expression(arg.get_rhs(), parameter_domains, fluent_function_domain_sets);
            }
            else if constexpr (std::is_same_v<ProxyType,
                                              Proxy<formalism::BinaryOperator<formalism::OpAdd, formalism::FunctionExpression>, formalism::Repository>>)
            {
                lift_parameter_domain_from_function_expression(arg.get_lhs(), parameter_domains, fluent_function_domain_sets);
                lift_parameter_domain_from_function_expression(arg.get_rhs(), parameter_domains, fluent_function_domain_sets);
            }
            else if constexpr (std::is_same_v<ProxyType,
                                              Proxy<formalism::BinaryOperator<formalism::OpMul, formalism::FunctionExpression>, formalism::Repository>>)
            {
                lift_parameter_domain_from_function_expression(arg.get_lhs(), parameter_domains, fluent_function_domain_sets);
                lift_parameter_domain_from_function_expression(arg.get_rhs(), parameter_domains, fluent_function_domain_sets);
            }
            else if constexpr (std::is_same_v<ProxyType,
                                              Proxy<formalism::BinaryOperator<formalism::OpDiv, formalism::FunctionExpression>, formalism::Repository>>)
            {
                lift_parameter_domain_from_function_expression(arg.get_lhs(), parameter_domains, fluent_function_domain_sets);
                lift_parameter_domain_from_function_expression(arg.get_rhs(), parameter_domains, fluent_function_domain_sets);
            }
            else if constexpr (std::is_same_v<ProxyType,
                                              Proxy<formalism::MultiOperator<formalism::OpAdd, formalism::FunctionExpression>, formalism::Repository>>)
            {
                for (const auto part : arg.get_args())
                    lift_parameter_domain_from_function_expression(part, parameter_domains, fluent_function_domain_sets);
            }
            else if constexpr (std::is_same_v<ProxyType,
                                              Proxy<formalism::MultiOperator<formalism::OpMul, formalism::FunctionExpression>, formalism::Repository>>)
            {
                for (const auto part : arg.get_args())
                    lift_parameter_domain_from_function_expression(part, parameter_domains, fluent_function_domain_sets);
            }
            else if constexpr (std::is_same_v<ProxyType, Proxy<formalism::FunctionTerm<formalism::StaticTag>, formalism::Repository>>) {}
            else if constexpr (std::is_same_v<ProxyType, Proxy<formalism::FunctionTerm<formalism::FluentTag>, formalism::Repository>>)
            {
                lift_parameter_domain_from_fluent_function_term(arg, parameter_domains, fluent_function_domain_sets);
            }
            else
            {
                static_assert(dependent_false<ProxyType>::value, "Missing case");
            }
        },
        fexpr);
}

void lift_parameter_domain_from_boolean_operator(Proxy<formalism::BooleanOperator<formalism::FunctionExpression>, formalism::Repository> op,
                                                 const DomainSetList& parameter_domains,
                                                 DomainSetListList& fluent_function_domain_sets)
{
    visit(
        [&](auto&& arg)
        {
            lift_parameter_domain_from_function_expression(arg.get_lhs(), parameter_domains, fluent_function_domain_sets);
            lift_parameter_domain_from_function_expression(arg.get_rhs(), parameter_domains, fluent_function_domain_sets);
        },
        op);
}

VariableDomains compute_variable_list_per_predicate(Proxy<formalism::Program, formalism::Repository> program)
{
    auto objects = std::vector<Index<formalism::Object>> {};
    for (const auto object : program.get_objects())
        objects.push_back(object.get_index());
    auto universe = DomainSet(objects.begin(), objects.end());

    ///--- Step 1: Initialize static and fluent predicate parameter domains

    auto static_predicate_domain_sets = initialize_predicate_domain_sets<formalism::StaticTag>(program);
    auto fluent_predicate_domain_sets = initialize_predicate_domain_sets<formalism::FluentTag>(program);

    ///--- Step 2: Initialize static and fluent function parameter domains

    auto static_function_domain_sets = initialize_function_domain_sets<formalism::StaticTag>(program);
    auto fluent_function_domain_sets = initialize_function_domain_sets<formalism::FluentTag>(program);

    ///--- Step 3: Compute rule parameter domains as tightest bound from the previously computed domains of the static predicates.

    auto rule_domain_sets = DomainSetListList();
    {
        for (const auto rule : program.get_rules())
        {
            auto variables = rule.get_variables();
            auto parameter_domains = DomainSetList(variables.size(), universe);

            for (const auto literal : rule.get_static_body())
                restrict_parameter_domain_from_static_atom(literal.get_atom(), parameter_domains, static_predicate_domain_sets);

            for (const auto op : rule.get_numeric_body())
                restrict_parameter_domain_from_boolean_operator(op, parameter_domains, static_function_domain_sets);

            rule_domain_sets.push_back(std::move(parameter_domains));
        }
    }

    ///--- Step 4: Lift the fluent predicate domains given the variable relationships in the rules.

    for (const auto rule : program.get_rules())
    {
        auto& parameter_domains = rule_domain_sets[rule.get_index().value];

        for (const auto literal : rule.get_fluent_body())
            lift_parameter_domain_from_fluent_atom(literal.get_atom(), parameter_domains, fluent_predicate_domain_sets);

        for (const auto op : rule.get_numeric_body())
            lift_parameter_domain_from_boolean_operator(op, parameter_domains, fluent_function_domain_sets);

        lift_parameter_domain_from_fluent_atom(rule.get_head(), parameter_domains, fluent_predicate_domain_sets);
    }

    ///--- Step 5: Compress sets to vectors.

    auto static_predicate_domains = to_list(static_predicate_domain_sets);
    auto fluent_predicate_domains = to_list(fluent_predicate_domain_sets);
    auto static_function_domains = to_list(static_function_domain_sets);
    auto fluent_function_domains = to_list(fluent_function_domain_sets);
    auto rule_domains = to_list(rule_domain_sets);

    // std::cout << "Static predicate domains: " << "\n" << static_predicate_domains << std::endl;
    // std::cout << "Fluent predicate domains: " << "\n" << fluent_predicate_domains << std::endl;
    // std::cout << "Static function domains: " << "\n" << static_function_domains << std::endl;
    // std::cout << "Fluent function domains: " << "\n" << fluent_function_domains << std::endl;
    // std::cout << "Rule domains: " << "\n" << rule_domains << std::endl;

    return VariableDomains { std::move(static_predicate_domains),
                             std::move(fluent_predicate_domains),
                             std::move(static_function_domains),
                             std::move(fluent_function_domains),
                             std::move(rule_domains) };
}
}

#endif