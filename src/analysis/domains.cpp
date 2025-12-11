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

#include "tyr/analysis/domains.hpp"

#include "tyr/common/unordered_set.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/views.hpp"

namespace tyr::analysis
{

static DomainListListList to_list(const DomainSetListList& set)
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

static DomainListList to_list(const DomainSetList& set)
{
    auto vec = DomainListList();
    vec.reserve(set.size());
    for (const auto& parameter_domain : set)
    {
        auto domain = DomainList(parameter_domain.begin(), parameter_domain.end());
        std::sort(domain.begin(), domain.end());
        vec.push_back(std::move(domain));
    }
    return vec;
}

static std::vector<std::pair<DomainListList, DomainListListList>> to_list(const std::vector<std::pair<DomainSetList, DomainSetListList>>& set)
{
    auto vec = std::vector<std::pair<DomainListList, DomainListListList>>();
    vec.reserve(set.size());
    for (const auto& [parameter_domains, parameter_domains_per_cond_effect] : set)
    {
        vec.emplace_back(to_list(parameter_domains), to_list(parameter_domains_per_cond_effect));
    }
    return vec;
}

template<formalism::FactKind T, formalism::Context C>
DomainSetListList initialize_predicate_domain_sets(View<IndexList<formalism::Predicate<T>>, C> predicates)
{
    auto predicate_domain_sets = DomainSetListList(predicates.size());

    for (const auto predicate : predicates)
        predicate_domain_sets[predicate.get_index().value].resize(predicate.get_arity());

    return predicate_domain_sets;
}

template<formalism::FactKind T, formalism::Context C>
void insert_into_predicate_domain_sets(View<IndexList<formalism::GroundAtom<T>>, C> atoms, DomainSetListList& predicate_domain_sets)
{
    for (const auto atom : atoms)
    {
        const auto predicate = atom.get_predicate();
        auto pos = size_t { 0 };
        for (const auto object : atom.get_objects())
            predicate_domain_sets[predicate.get_index().value][pos++].insert(object.get_index());
    }
}

template<formalism::FactKind T, formalism::Context C>
DomainSetListList initialize_function_domain_sets(View<IndexList<formalism::Function<T>>, C> functions)
{
    auto function_domain_sets = DomainSetListList(functions.size());

    for (const auto function : functions)
        function_domain_sets[function.get_index().value].resize(function.get_arity());

    return function_domain_sets;
}

template<formalism::FactKind T, formalism::Context C>
void insert_into_function_domain_sets(View<IndexList<formalism::GroundFunctionTermValue<T>>, C> fterm_values, DomainSetListList& function_domain_sets)
{
    for (const auto term_value : fterm_values)
    {
        const auto fterm = term_value.get_fterm();
        const auto function = fterm.get_function();
        auto pos = size_t { 0 };
        for (const auto object : fterm.get_objects())
            function_domain_sets[function.get_index().value][pos++].insert(object.get_index());
    }
}

/**
 * Restrict
 */

template<formalism::Context C>
void restrict_parameter_domain(View<Data<formalism::FunctionExpression>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& function_domain_sets);

static void restrict_parameter_domain(float_t, DomainSetList&, const DomainSetListList&) {}

template<formalism::OpKind O, formalism::Context C>
void restrict_parameter_domain(View<Index<formalism::UnaryOperator<O, Data<formalism::FunctionExpression>>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& function_domain_sets)
{
    restrict_parameter_domain(element.get_arg(), parameter_domains, function_domain_sets);
}

template<formalism::OpKind O, formalism::Context C>
void restrict_parameter_domain(View<Index<formalism::BinaryOperator<O, Data<formalism::FunctionExpression>>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& function_domain_sets)
{
    restrict_parameter_domain(element.get_lhs(), parameter_domains, function_domain_sets);
    restrict_parameter_domain(element.get_rhs(), parameter_domains, function_domain_sets);
}

template<formalism::OpKind O, formalism::Context C>
void restrict_parameter_domain(View<Index<formalism::MultiOperator<O, Data<formalism::FunctionExpression>>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& function_domain_sets)
{
    for (const auto arg : element.get_args())
        restrict_parameter_domain(arg, parameter_domains, function_domain_sets);
}

template<formalism::FactKind T, formalism::Context C>
void restrict_parameter_domain(View<Index<formalism::Atom<T>>, C> element, DomainSetList& parameter_domains, const DomainSetListList& predicate_domain_sets)
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
                    const auto& predicate_domain = predicate_domain_sets[predicate.get_index().value][pos];

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

template<formalism::FactKind T, formalism::Context C>
void restrict_parameter_domain(View<Index<formalism::FunctionTerm<T>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& function_domain_sets)
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
                    const auto& predicate_domain = function_domain_sets[function.get_index().value][pos];

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

template<formalism::Context C>
void restrict_parameter_domain(View<Index<formalism::FunctionTerm<formalism::FluentTag>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& function_domain_sets)
{
}

template<formalism::Context C>
void restrict_parameter_domain(View<Data<formalism::ArithmeticOperator<Data<formalism::FunctionExpression>>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& function_domain_sets)
{
    visit([&](auto&& arg) { restrict_parameter_domain(arg, parameter_domains, function_domain_sets); }, element.get_variant());
}

template<formalism::Context C>
void restrict_parameter_domain(View<Data<formalism::FunctionExpression>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& function_domain_sets)
{
    visit([&](auto&& arg) { restrict_parameter_domain(arg, parameter_domains, function_domain_sets); }, element.get_variant());
}

template<formalism::Context C>
void restrict_parameter_domain(View<Data<formalism::BooleanOperator<Data<formalism::FunctionExpression>>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& function_domain_sets)
{
    visit([&](auto&& arg) { restrict_parameter_domain(arg, parameter_domains, function_domain_sets); }, element.get_variant());
}

/**
 * Lift
 */

template<formalism::Context C>
void lift_parameter_domain(View<Data<formalism::FunctionExpression>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets);

static void lift_parameter_domain(float_t, const DomainSetList&, DomainSetListList&);

template<formalism::OpKind O, formalism::Context C>
void lift_parameter_domain(View<Index<formalism::UnaryOperator<O, Data<formalism::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets);

template<formalism::OpKind O, formalism::Context C>
void lift_parameter_domain(View<Index<formalism::BinaryOperator<O, Data<formalism::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets);

template<formalism::OpKind O, formalism::Context C>
void lift_parameter_domain(View<Index<formalism::MultiOperator<O, Data<formalism::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets);

template<formalism::NumericEffectOpKind Op, formalism::FactKind T, formalism::Context C>
void lift_parameter_domain(View<Index<formalism::NumericEffect<Op, T>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets);

template<formalism::FactKind T, formalism::Context C>
void lift_parameter_domain(View<Index<formalism::Atom<T>>, C> element, const DomainSetList& parameter_domains, DomainSetListList& predicate_domain_sets);

template<formalism::FactKind T, formalism::Context C>
void lift_parameter_domain(View<Index<formalism::FunctionTerm<T>>, C> element, const DomainSetList& parameter_domains, DomainSetListList& function_domain_sets);

template<formalism::Context C>
void lift_parameter_domain(View<Index<formalism::FunctionTerm<formalism::StaticTag>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets);

template<formalism::Context C>
void lift_parameter_domain(View<Data<formalism::ArithmeticOperator<Data<formalism::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets);

template<formalism::Context C>
void lift_parameter_domain(View<Data<formalism::FunctionExpression>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets);

template<formalism::Context C>
void lift_parameter_domain(View<Data<formalism::BooleanOperator<Data<formalism::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets);

template<formalism::FactKind T, formalism::Context C>
void lift_parameter_domain(View<Data<formalism::NumericEffectOperator<T>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets);

template<formalism::Context C>
void lift_parameter_domain(View<Data<formalism::FunctionExpression>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets);

static void lift_parameter_domain(float_t, const DomainSetList&, DomainSetListList&) {}

template<formalism::OpKind O, formalism::Context C>
void lift_parameter_domain(View<Index<formalism::UnaryOperator<O, Data<formalism::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets)
{
    lift_parameter_domain(element.get_arg(), parameter_domains, function_domain_sets);
}

template<formalism::OpKind O, formalism::Context C>
void lift_parameter_domain(View<Index<formalism::BinaryOperator<O, Data<formalism::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets)
{
    lift_parameter_domain(element.get_lhs(), parameter_domains, function_domain_sets);
    lift_parameter_domain(element.get_rhs(), parameter_domains, function_domain_sets);
}

template<formalism::OpKind O, formalism::Context C>
void lift_parameter_domain(View<Index<formalism::MultiOperator<O, Data<formalism::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets)
{
    for (const auto arg : element.get_args())
        lift_parameter_domain(arg, parameter_domains, function_domain_sets);
}

template<formalism::NumericEffectOpKind Op, formalism::FactKind T, formalism::Context C>
void lift_parameter_domain(View<Index<formalism::NumericEffect<Op, T>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets)
{
    lift_parameter_domain(element.get_fterm(), parameter_domains, function_domain_sets);
    lift_parameter_domain(element.get_fexpr(), parameter_domains, function_domain_sets);
}

template<formalism::FactKind T, formalism::Context C>
void lift_parameter_domain(View<Index<formalism::Atom<T>>, C> element, const DomainSetList& parameter_domains, DomainSetListList& predicate_domain_sets)
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
                    auto& function_domain = predicate_domain_sets[predicate.get_index().value][pos];

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

template<formalism::FactKind T, formalism::Context C>
void lift_parameter_domain(View<Index<formalism::FunctionTerm<T>>, C> element, const DomainSetList& parameter_domains, DomainSetListList& function_domain_sets)
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
                    auto& function_domain = function_domain_sets[function.get_index().value][pos];

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

template<formalism::Context C>
void lift_parameter_domain(View<Index<formalism::FunctionTerm<formalism::StaticTag>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets)
{
}

template<formalism::Context C>
void lift_parameter_domain(View<Data<formalism::ArithmeticOperator<Data<formalism::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets)
{
    visit([&](auto&& arg) { lift_parameter_domain(arg, parameter_domains, function_domain_sets); }, element.get_variant());
}

template<formalism::Context C>
void lift_parameter_domain(View<Data<formalism::FunctionExpression>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets)
{
    visit([&](auto&& arg) { lift_parameter_domain(arg, parameter_domains, function_domain_sets); }, element.get_variant());
}

template<formalism::Context C>
void lift_parameter_domain(View<Data<formalism::BooleanOperator<Data<formalism::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets)
{
    visit([&](auto&& arg) { lift_parameter_domain(arg, parameter_domains, function_domain_sets); }, element.get_variant());
}

template<formalism::FactKind T, formalism::Context C>
void lift_parameter_domain(View<Data<formalism::NumericEffectOperator<T>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets)
{
    visit([&](auto&& arg) { lift_parameter_domain(arg, parameter_domains, function_domain_sets); }, element.get_variant());
}

ProgramVariableDomains compute_variable_domains(View<Index<formalism::Program>, formalism::Repository> program)
{
    auto objects = std::vector<Index<formalism::Object>> {};
    for (const auto object : program.get_objects())
        objects.push_back(object.get_index());
    auto universe = DomainSet(objects.begin(), objects.end());

    ///--- Step 1: Initialize static and fluent predicate parameter domains

    auto static_predicate_domain_sets = initialize_predicate_domain_sets(program.get_predicates<formalism::StaticTag>());
    auto fluent_predicate_domain_sets = initialize_predicate_domain_sets(program.get_predicates<formalism::FluentTag>());
    insert_into_predicate_domain_sets(program.get_atoms<formalism::StaticTag>(), static_predicate_domain_sets);
    insert_into_predicate_domain_sets(program.get_atoms<formalism::FluentTag>(), fluent_predicate_domain_sets);

    ///--- Step 2: Initialize static and fluent function parameter domains

    auto static_function_domain_sets = initialize_function_domain_sets(program.get_functions<formalism::StaticTag>());
    auto fluent_function_domain_sets = initialize_function_domain_sets(program.get_functions<formalism::FluentTag>());
    insert_into_function_domain_sets(program.get_fterm_values<formalism::StaticTag>(), static_function_domain_sets);
    insert_into_function_domain_sets(program.get_fterm_values<formalism::FluentTag>(), fluent_function_domain_sets);

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

            assert(rule.get_index().value == rule_domain_sets.size());
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

    // std::cout << static_predicate_domains << std::endl;
    // std::cout << fluent_predicate_domains << std::endl;
    // std::cout << static_function_domains << std::endl;
    // std::cout << fluent_function_domains << std::endl;
    // std::cout << rule_domains << std::endl;

    return ProgramVariableDomains { std::move(static_predicate_domains),
                                    std::move(fluent_predicate_domains),
                                    std::move(static_function_domains),
                                    std::move(fluent_function_domains),
                                    std::move(rule_domains) };
}

TaskVariableDomains compute_variable_domains(View<Index<formalism::Task>, formalism::OverlayRepository<formalism::Repository>> task)
{
    auto objects = std::vector<Index<formalism::Object>> {};
    for (const auto object : task.get_domain().get_constants())
        objects.push_back(object.get_index());
    for (const auto object : task.get_objects())
        objects.push_back(object.get_index());
    auto universe = DomainSet(objects.begin(), objects.end());

    ///--- Step 1: Initialize static and fluent predicate parameter domains

    auto static_predicate_domain_sets = initialize_predicate_domain_sets(task.get_domain().get_predicates<formalism::StaticTag>());
    auto fluent_predicate_domain_sets = initialize_predicate_domain_sets(task.get_domain().get_predicates<formalism::FluentTag>());
    auto derived_predicate_indices = IndexList<formalism::Predicate<formalism::DerivedTag>> {};
    for (const auto predicate : task.get_domain().get_predicates<formalism::DerivedTag>())
        derived_predicate_indices.push_back(predicate.get_index());
    for (const auto predicate : task.get_derived_predicates())
        derived_predicate_indices.push_back(predicate.get_index());
    auto derived_predicate_domain_sets = initialize_predicate_domain_sets(make_view(derived_predicate_indices, task.get_context()));
    insert_into_predicate_domain_sets(task.get_atoms<formalism::StaticTag>(), static_predicate_domain_sets);
    insert_into_predicate_domain_sets(task.get_atoms<formalism::FluentTag>(), fluent_predicate_domain_sets);

    ///--- Step 2: Initialize static and fluent function parameter domains

    auto static_function_domain_sets = initialize_function_domain_sets(task.get_domain().get_functions<formalism::StaticTag>());
    auto fluent_function_domain_sets = initialize_function_domain_sets(task.get_domain().get_functions<formalism::FluentTag>());
    insert_into_function_domain_sets(task.get_fterm_values<formalism::StaticTag>(), static_function_domain_sets);
    insert_into_function_domain_sets(task.get_fterm_values<formalism::FluentTag>(), fluent_function_domain_sets);

    ///--- Step 3: Compute rule parameter domains as tightest bound from the previously computed domains of the static predicates.

    auto action_domain_sets = std::vector<std::pair<DomainSetList, DomainSetListList>> {};
    {
        for (const auto action : task.get_domain().get_actions())
        {
            auto variables = action.get_variables();
            auto parameter_domains = DomainSetList(variables.size(), universe);

            for (const auto literal : action.get_condition().get_literals<formalism::StaticTag>())
                restrict_parameter_domain(literal.get_atom(), parameter_domains, static_predicate_domain_sets);

            for (const auto op : action.get_condition().get_numeric_constraints())
                restrict_parameter_domain(op, parameter_domains, static_function_domain_sets);

            auto parameter_domains_per_cond_effect = DomainSetListList();

            for (const auto c_effect : action.get_effects())
            {
                const auto c_variables = c_effect.get_condition().get_variables();

                auto c_parameter_domains = parameter_domains;
                c_parameter_domains.resize(variables.size() + c_variables.size(), universe);

                for (const auto literal : c_effect.get_condition().get_literals<formalism::StaticTag>())
                    restrict_parameter_domain(literal.get_atom(), c_parameter_domains, static_predicate_domain_sets);

                for (const auto op : c_effect.get_condition().get_numeric_constraints())
                    restrict_parameter_domain(op, c_parameter_domains, static_function_domain_sets);

                parameter_domains_per_cond_effect.push_back(std::move(c_parameter_domains));
            }

            assert(action.get_index().value == action_domain_sets.size());
            action_domain_sets.emplace_back(std::make_pair(std::move(parameter_domains), std::move(parameter_domains_per_cond_effect)));
        }
    }

    auto axiom_domain_sets = DomainSetListList();
    {
        for (const auto axiom : task.get_domain().get_axioms())
        {
            auto variables = axiom.get_body().get_variables();
            auto parameter_domains = DomainSetList(variables.size(), universe);

            for (const auto literal : axiom.get_body().get_literals<formalism::StaticTag>())
                restrict_parameter_domain(literal.get_atom(), parameter_domains, static_predicate_domain_sets);

            for (const auto op : axiom.get_body().get_numeric_constraints())
                restrict_parameter_domain(op, parameter_domains, static_function_domain_sets);

            assert(axiom.get_index().value == axiom_domain_sets.size());
            axiom_domain_sets.push_back(std::move(parameter_domains));
        }

        for (const auto axiom : task.get_axioms())
        {
            auto variables = axiom.get_body().get_variables();
            auto parameter_domains = DomainSetList(variables.size(), universe);

            for (const auto literal : axiom.get_body().get_literals<formalism::StaticTag>())
                restrict_parameter_domain(literal.get_atom(), parameter_domains, static_predicate_domain_sets);

            for (const auto op : axiom.get_body().get_numeric_constraints())
                restrict_parameter_domain(op, parameter_domains, static_function_domain_sets);

            assert(axiom.get_index().value == axiom_domain_sets.size());
            axiom_domain_sets.push_back(std::move(parameter_domains));
        }
    }

    ///--- Step 4: Lift the fluent predicate domains given the variable relationships in the rules.

    for (const auto action : task.get_domain().get_actions())
    {
        auto& [parameter_domains, parameter_domains_per_cond_effect] = action_domain_sets[action.get_index().value];

        for (const auto literal : action.get_condition().get_literals<formalism::FluentTag>())
            lift_parameter_domain(literal.get_atom(), parameter_domains, fluent_predicate_domain_sets);

        for (const auto literal : action.get_condition().get_literals<formalism::DerivedTag>())
            lift_parameter_domain(literal.get_atom(), parameter_domains, derived_predicate_domain_sets);

        for (const auto op : action.get_condition().get_numeric_constraints())
            lift_parameter_domain(op, parameter_domains, fluent_function_domain_sets);

        for (uint_t i = 0; i < action.get_effects().size(); ++i)
        {
            const auto c_effect = action.get_effects()[i];
            auto& c_parameter_domains = parameter_domains_per_cond_effect[i];

            for (const auto literal : c_effect.get_condition().get_literals<formalism::FluentTag>())
                lift_parameter_domain(literal.get_atom(), c_parameter_domains, fluent_predicate_domain_sets);

            for (const auto op : c_effect.get_condition().get_numeric_constraints())
                lift_parameter_domain(op, c_parameter_domains, fluent_function_domain_sets);

            for (const auto literal : c_effect.get_effect().get_literals())
                lift_parameter_domain(literal.get_atom(), c_parameter_domains, fluent_predicate_domain_sets);

            for (const auto op : c_effect.get_effect().get_numeric_effects())
                lift_parameter_domain(op, c_parameter_domains, fluent_function_domain_sets);
        }
    }

    for (const auto axiom : task.get_domain().get_axioms())
    {
        auto& parameter_domains = axiom_domain_sets[axiom.get_index().value];

        for (const auto literal : axiom.get_body().get_literals<formalism::FluentTag>())
            lift_parameter_domain(literal.get_atom(), parameter_domains, fluent_predicate_domain_sets);

        for (const auto literal : axiom.get_body().get_literals<formalism::DerivedTag>())
            lift_parameter_domain(literal.get_atom(), parameter_domains, derived_predicate_domain_sets);

        for (const auto op : axiom.get_body().get_numeric_constraints())
            lift_parameter_domain(op, parameter_domains, fluent_function_domain_sets);

        lift_parameter_domain(axiom.get_head(), parameter_domains, derived_predicate_domain_sets);
    }

    for (const auto axiom : task.get_axioms())
    {
        auto& parameter_domains = axiom_domain_sets[axiom.get_index().value];

        for (const auto literal : axiom.get_body().get_literals<formalism::FluentTag>())
            lift_parameter_domain(literal.get_atom(), parameter_domains, fluent_predicate_domain_sets);

        for (const auto literal : axiom.get_body().get_literals<formalism::DerivedTag>())
            lift_parameter_domain(literal.get_atom(), parameter_domains, derived_predicate_domain_sets);

        for (const auto op : axiom.get_body().get_numeric_constraints())
            lift_parameter_domain(op, parameter_domains, fluent_function_domain_sets);

        lift_parameter_domain(axiom.get_head(), parameter_domains, derived_predicate_domain_sets);
    }

    ///--- Step 5: Compress sets to vectors.

    auto static_predicate_domains = to_list(static_predicate_domain_sets);
    auto fluent_predicate_domains = to_list(fluent_predicate_domain_sets);
    auto derived_predicate_domains = to_list(derived_predicate_domain_sets);
    auto static_function_domains = to_list(static_function_domain_sets);
    auto fluent_function_domains = to_list(fluent_function_domain_sets);
    auto action_domains = to_list(action_domain_sets);
    auto axiom_domains = to_list(axiom_domain_sets);

    // std::cout << std::endl;
    // std::cout << static_predicate_domains << std::endl;
    // std::cout << fluent_predicate_domains << std::endl;
    // std::cout << derived_predicate_domains << std::endl;
    // std::cout << static_function_domains << std::endl;
    // std::cout << fluent_function_domains << std::endl;
    // std::cout << action_domains << std::endl;
    // std::cout << axiom_domains << std::endl;

    return TaskVariableDomains { std::move(static_predicate_domains),
                                 std::move(fluent_predicate_domains),
                                 std::move(derived_predicate_domains),
                                 std::move(static_function_domains),
                                 std::move(fluent_function_domains),
                                 std::move(action_domains),
                                 std::move(axiom_domains) };
}
}
