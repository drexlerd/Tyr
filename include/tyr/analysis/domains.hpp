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
#include "tyr/formalism/formalism.hpp"

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

template<formalism::IsFactTag T>
inline DomainSetListList initialize_predicate_domain_sets(View<Index<formalism::Program>, formalism::Repository> program)
{
    const auto num_predicates = program.get_predicates<T>().size();
    auto predicate_domain_sets = DomainSetListList(num_predicates);

    for (const auto predicate : program.get_predicates<T>())
        predicate_domain_sets[predicate.get_index().value].resize(predicate.get_arity());

    for (const auto atom : program.get_atoms<T>())
    {
        const auto predicate = atom.get_predicate();
        auto pos = size_t { 0 };
        for (const auto object : atom.get_objects())
            predicate_domain_sets[predicate.get_index().value][pos++].insert(object.get_index());
    }

    return predicate_domain_sets;
}

template<formalism::IsFactTag T>
inline DomainSetListList initialize_function_domain_sets(View<Index<formalism::Program>, formalism::Repository> program)
{
    const auto num_functions = program.get_functions<T>().size();
    auto function_domain_sets = DomainSetListList(num_functions);

    for (const auto function : program.get_functions<T>())
        function_domain_sets[function.get_index().value].resize(function.get_arity());

    for (const auto term_value : program.get_fterm_values<T>())
    {
        const auto fterm = term_value.get_fterm();
        const auto function = fterm.get_function();
        auto pos = size_t { 0 };
        for (const auto object : fterm.get_objects())
            function_domain_sets[function.get_index().value][pos++].insert(object.get_index());
    }

    return function_domain_sets;
}

void restrict_parameter_domain_from_static_atom(View<Index<formalism::Atom<formalism::StaticTag>>, formalism::Repository> atom,
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
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, View<Index<formalism::Object>, formalism::Repository>>) {}
                else if constexpr (std::is_same_v<Alternative, formalism::ParameterIndex>)
                {
                    const auto parameter_index = uint_t(arg);
                    auto& parameter_domain = parameter_domains[parameter_index];
                    const auto& predicate_domain = static_predicate_domain_sets[predicate.get_index().value][pos];

                    intersect_inplace(parameter_domain, predicate_domain);
                }
                else
                {
                    static_assert(dependent_false<Alternative>::value, "Missing case");
                }
            },
            term.get_variant());
        ++pos;
    }
}

/**
 * Restrict
 */

template<formalism::IsContext C>
void restrict_parameter_domain(View<Data<formalism::FunctionExpression>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& static_function_domain_sets);

void restrict_parameter_domain(float_t, DomainSetList&, const DomainSetListList&) {}

template<formalism::IsOp O, formalism::IsContext C>
void restrict_parameter_domain(View<Index<formalism::UnaryOperator<O, Data<formalism::FunctionExpression>>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& static_function_domain_sets)
{
    restrict_parameter_domain(element.get_arg(), parameter_domains, static_function_domain_sets);
}

template<formalism::IsOp O, formalism::IsContext C>
void restrict_parameter_domain(View<Index<formalism::BinaryOperator<O, Data<formalism::FunctionExpression>>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& static_function_domain_sets)
{
    restrict_parameter_domain(element.get_lhs(), parameter_domains, static_function_domain_sets);
    restrict_parameter_domain(element.get_rhs(), parameter_domains, static_function_domain_sets);
}

template<formalism::IsOp O, formalism::IsContext C>
void restrict_parameter_domain(View<Index<formalism::MultiOperator<O, Data<formalism::FunctionExpression>>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& static_function_domain_sets)
{
    for (const auto arg : element.get_args())
        restrict_parameter_domain(arg, parameter_domains, static_function_domain_sets);
}

template<formalism::IsContext C>
void restrict_parameter_domain(View<Index<formalism::Atom<formalism::StaticTag>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& static_predicate_domain_sets)
{
    const auto predicate = element.get_predicate();

    auto pos = size_t { 0 };
    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, View<Index<formalism::Object>, C>>) {}
                else if constexpr (std::is_same_v<Alternative, formalism::ParameterIndex>)
                {
                    const auto parameter_index = uint_t(arg);
                    auto& parameter_domain = parameter_domains[parameter_index];
                    const auto& predicate_domain = static_predicate_domain_sets[predicate.get_index().value][pos];

                    intersect_inplace(parameter_domain, predicate_domain);
                }
                else
                {
                    static_assert(dependent_false<Alternative>::value, "Missing case");
                }
            },
            term.get_variant());
        ++pos;
    }
}

template<formalism::IsContext C>
void restrict_parameter_domain(View<Index<formalism::FunctionTerm<formalism::StaticTag>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& static_function_domain_sets)
{
    const auto function = element.get_function();

    auto pos = size_t { 0 };
    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, View<Index<formalism::Object>, C>>) {}
                else if constexpr (std::is_same_v<Alternative, formalism::ParameterIndex>)
                {
                    const auto parameter_index = uint_t(arg);
                    auto& parameter_domain = parameter_domains[parameter_index];
                    const auto& predicate_domain = static_function_domain_sets[function.get_index().value][pos];

                    intersect_inplace(parameter_domain, predicate_domain);
                }
                else
                {
                    static_assert(dependent_false<Alternative>::value, "Missing case");
                }
            },
            term.get_variant());
        ++pos;
    }
}

template<formalism::IsContext C>
void restrict_parameter_domain(View<Index<formalism::FunctionTerm<formalism::FluentTag>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& static_function_domain_sets)
{
}

template<formalism::IsContext C>
void restrict_parameter_domain(View<Data<formalism::FunctionExpression>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& static_function_domain_sets)
{
    visit([&](auto&& arg) { restrict_parameter_domain(arg, parameter_domains, static_function_domain_sets); }, element.get_variant());
}

template<formalism::IsContext C>
void restrict_parameter_domain(View<Data<formalism::ArithmeticOperator<Data<formalism::FunctionExpression>>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& static_function_domain_sets)
{
    visit([&](auto&& arg) { restrict_parameter_domain(arg, parameter_domains, static_function_domain_sets); }, element.get_variant());
}

template<formalism::IsContext C>
void restrict_parameter_domain(View<Data<formalism::BooleanOperator<Data<formalism::FunctionExpression>>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& static_function_domain_sets)
{
    visit([&](auto&& arg) { restrict_parameter_domain(arg, parameter_domains, static_function_domain_sets); }, element.get_variant());
}

/**
 * Lift
 */

template<formalism::IsContext C>
void lift_parameter_domain(View<Data<formalism::FunctionExpression>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& fluent_function_domain_sets);

void lift_parameter_domain(float_t, const DomainSetList&, DomainSetListList&) {}

template<formalism::IsOp O, formalism::IsContext C>
void lift_parameter_domain(View<Index<formalism::UnaryOperator<O, Data<formalism::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& fluent_function_domain_sets)
{
    lift_parameter_domain(element.get_arg(), parameter_domains, fluent_function_domain_sets);
}

template<formalism::IsOp O, formalism::IsContext C>
void lift_parameter_domain(View<Index<formalism::BinaryOperator<O, Data<formalism::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& fluent_function_domain_sets)
{
    lift_parameter_domain(element.get_lhs(), parameter_domains, fluent_function_domain_sets);
    lift_parameter_domain(element.get_rhs(), parameter_domains, fluent_function_domain_sets);
}

template<formalism::IsOp O, formalism::IsContext C>
void lift_parameter_domain(View<Index<formalism::MultiOperator<O, Data<formalism::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& fluent_function_domain_sets)
{
    for (const auto arg : element.get_args())
        lift_parameter_domain(arg, parameter_domains, fluent_function_domain_sets);
}

template<formalism::IsContext C>
void lift_parameter_domain(View<Index<formalism::Atom<formalism::FluentTag>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& fluent_predicate_domain_sets)
{
    const auto predicate = element.get_predicate();

    auto pos = size_t { 0 };
    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, View<Index<formalism::Object>, C>>) {}
                else if constexpr (std::is_same_v<Alternative, formalism::ParameterIndex>)
                {
                    const auto parameter_index = uint_t(arg);
                    const auto& parameter_domain = parameter_domains[parameter_index];
                    auto& function_domain = fluent_predicate_domain_sets[predicate.get_index().value][pos];

                    union_inplace(function_domain, parameter_domain);
                }
                else
                {
                    static_assert(dependent_false<Alternative>::value, "Missing case");
                }
            },
            term.get_variant());
        ++pos;
    }
}

template<formalism::IsContext C>
void lift_parameter_domain(View<Index<formalism::FunctionTerm<formalism::FluentTag>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& fluent_function_domain_sets)
{
    const auto function = element.get_function();

    auto pos = size_t { 0 };
    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, View<Index<formalism::Object>, C>>) {}
                else if constexpr (std::is_same_v<Alternative, formalism::ParameterIndex>)
                {
                    const auto parameter_index = uint_t(arg);
                    const auto& parameter_domain = parameter_domains[parameter_index];
                    auto& function_domain = fluent_function_domain_sets[function.get_index().value][pos];

                    union_inplace(function_domain, parameter_domain);
                }
                else
                {
                    static_assert(dependent_false<Alternative>::value, "Missing case");
                }
            },
            term.get_variant());
        ++pos;
    }
}

template<formalism::IsContext C>
void lift_parameter_domain(View<Index<formalism::FunctionTerm<formalism::StaticTag>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& fluent_function_domain_sets)
{
}

template<formalism::IsContext C>
void lift_parameter_domain(View<Data<formalism::FunctionExpression>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& fluent_function_domain_sets)
{
    visit([&](auto&& arg) { lift_parameter_domain(arg, parameter_domains, fluent_function_domain_sets); }, element.get_variant());
}

template<formalism::IsContext C>
void lift_parameter_domain(View<Data<formalism::ArithmeticOperator<Data<formalism::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& fluent_function_domain_sets)
{
    visit([&](auto&& arg) { lift_parameter_domain(arg, parameter_domains, fluent_function_domain_sets); }, element.get_variant());
}

template<formalism::IsContext C>
void lift_parameter_domain(View<Data<formalism::BooleanOperator<Data<formalism::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& fluent_function_domain_sets)
{
    visit([&](auto&& arg) { lift_parameter_domain(arg, parameter_domains, fluent_function_domain_sets); }, element.get_variant());
}

VariableDomains compute_variable_domains(View<Index<formalism::Program>, formalism::Repository> program)
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
            auto variables = rule.get_body().get_variables();
            auto parameter_domains = DomainSetList(variables.size(), universe);

            for (const auto literal : rule.get_body().get_literals<formalism::StaticTag>())
                restrict_parameter_domain(literal.get_atom(), parameter_domains, static_predicate_domain_sets);

            for (const auto op : rule.get_body().get_numeric_constraints())
                restrict_parameter_domain(op, parameter_domains, static_function_domain_sets);

            rule_domain_sets.push_back(std::move(parameter_domains));
        }
    }

    ///--- Step 4: Lift the fluent predicate domains given the variable relationships in the rules.

    for (const auto rule : program.get_rules())
    {
        auto& parameter_domains = rule_domain_sets[rule.get_index().value];

        for (const auto literal : rule.get_body().get_literals<formalism::FluentTag>())
            lift_parameter_domain(literal.get_atom(), parameter_domains, fluent_predicate_domain_sets);

        for (const auto op : rule.get_body().get_numeric_constraints())
            lift_parameter_domain(op, parameter_domains, fluent_function_domain_sets);

        lift_parameter_domain(rule.get_head(), parameter_domains, fluent_predicate_domain_sets);
    }

    ///--- Step 5: Compress sets to vectors.

    auto static_predicate_domains = to_list(static_predicate_domain_sets);
    auto fluent_predicate_domains = to_list(fluent_predicate_domain_sets);
    auto static_function_domains = to_list(static_function_domain_sets);
    auto fluent_function_domains = to_list(fluent_function_domain_sets);
    auto rule_domains = to_list(rule_domain_sets);

    return VariableDomains { std::move(static_predicate_domains),
                             std::move(fluent_predicate_domains),
                             std::move(static_function_domains),
                             std::move(fluent_function_domains),
                             std::move(rule_domains) };
}
}

#endif