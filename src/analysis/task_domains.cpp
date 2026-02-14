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
#include "tyr/common/config.hpp"
#include "tyr/common/formatter.hpp"
#include "tyr/common/index_mixins.hpp"
#include "tyr/common/types.hpp"
#include "tyr/common/unordered_set.hpp"
#include "tyr/common/variant.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/planning/datas.hpp"
#include "tyr/formalism/planning/formatter.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"

#include <algorithm>
#include <assert.h>
#include <gtl/phmap.hpp>
#include <stddef.h>
#include <type_traits>

namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;

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

template<f::FactKind T, fp::Context C>
DomainSetListList initialize_predicate_domain_sets(View<IndexList<f::Predicate<T>>, C> predicates)
{
    auto predicate_domain_sets = DomainSetListList(predicates.size());

    for (const auto predicate : predicates)
        predicate_domain_sets[predicate.get_index().value].resize(predicate.get_arity());

    return predicate_domain_sets;
}

template<f::FactKind T, fp::Context C>
void insert_into_predicate_domain_sets(View<IndexList<fp::GroundAtom<T>>, C> atoms, DomainSetListList& predicate_domain_sets)
{
    for (const auto atom : atoms)
    {
        const auto predicate = atom.get_predicate();
        auto pos = size_t { 0 };
        for (const auto object : atom.get_objects())
            predicate_domain_sets[predicate.get_index().value][pos++].insert(object.get_index());
    }
}

template<f::FactKind T, fp::Context C>
DomainSetListList initialize_function_domain_sets(View<IndexList<f::Function<T>>, C> functions)
{
    auto function_domain_sets = DomainSetListList(functions.size());

    for (const auto function : functions)
        function_domain_sets[function.get_index().value].resize(function.get_arity());

    return function_domain_sets;
}

template<f::FactKind T, fp::Context C>
void insert_into_function_domain_sets(View<IndexList<fp::GroundFunctionTermValue<T>>, C> fterm_values, DomainSetListList& function_domain_sets)
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
 * Insert constants
 */

template<fp::Context C>
void insert_constants_into_parameter_domain(View<Data<fp::FunctionExpression>, C> element, DomainSetListList& function_domain_sets);

static void insert_constants_into_parameter_domain(float_t, DomainSetListList&) {}

template<f::OpKind O, fp::Context C>
void insert_constants_into_parameter_domain(View<Index<fp::UnaryOperator<O, Data<fp::FunctionExpression>>>, C> element, DomainSetListList& function_domain_sets)
{
    insert_constants_into_parameter_domain(element.get_arg(), function_domain_sets);
}

template<f::OpKind O, fp::Context C>
void insert_constants_into_parameter_domain(View<Index<fp::BinaryOperator<O, Data<fp::FunctionExpression>>>, C> element,
                                            DomainSetListList& function_domain_sets)
{
    insert_constants_into_parameter_domain(element.get_lhs(), function_domain_sets);
    insert_constants_into_parameter_domain(element.get_rhs(), function_domain_sets);
}

template<f::OpKind O, fp::Context C>
void insert_constants_into_parameter_domain(View<Index<fp::MultiOperator<O, Data<fp::FunctionExpression>>>, C> element, DomainSetListList& function_domain_sets)
{
    for (const auto arg : element.get_args())
        insert_constants_into_parameter_domain(arg, function_domain_sets);
}

template<f::FactKind T, fp::Context C>
void insert_constants_into_parameter_domain(View<Index<fp::Atom<T>>, C> element, DomainSetListList& predicate_domain_sets)
{
    const auto predicate = element.get_predicate();

    auto pos = size_t { 0 };
    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, View<Index<f::Object>, C>>)
                {
                    auto& predicate_domain = predicate_domain_sets[predicate.get_index().value][pos];
                    predicate_domain.insert(arg.get_index());
                }
                else if constexpr (std::is_same_v<Alternative, f::ParameterIndex>) {}
                else
                {
                    static_assert(dependent_false<Alternative>::value, "Missing case");
                }
            },
            term.get_variant());
        ++pos;
    }
}

template<f::FactKind T, fp::Context C>
void insert_constants_into_parameter_domain(View<Index<fp::FunctionTerm<T>>, C> element, DomainSetListList& function_domain_sets)
{
    const auto function = element.get_function();

    auto pos = size_t { 0 };
    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, View<Index<f::Object>, C>>)
                {
                    auto& function_domain = function_domain_sets[function.get_index().value][pos];
                    function_domain.insert(arg.get_index());
                }
                else if constexpr (std::is_same_v<Alternative, f::ParameterIndex>) {}
                else
                {
                    static_assert(dependent_false<Alternative>::value, "Missing case");
                }
            },
            term.get_variant());
        ++pos;
    }
}

template<fp::Context C>
void insert_constants_into_parameter_domain(View<Index<fp::FunctionTerm<f::FluentTag>>, C> element, DomainSetListList& function_domain_sets)
{
    // Dont restrict for fluent fterm
}

template<fp::Context C>
void insert_constants_into_parameter_domain(View<Data<fp::ArithmeticOperator<Data<fp::FunctionExpression>>>, C> element,
                                            DomainSetListList& function_domain_sets)
{
    visit([&](auto&& arg) { insert_constants_into_parameter_domain(arg, function_domain_sets); }, element.get_variant());
}

template<fp::Context C>
void insert_constants_into_parameter_domain(View<Data<fp::FunctionExpression>, C> element, DomainSetListList& function_domain_sets)
{
    visit([&](auto&& arg) { insert_constants_into_parameter_domain(arg, function_domain_sets); }, element.get_variant());
}

template<fp::Context C>
void insert_constants_into_parameter_domain(View<Data<fp::BooleanOperator<Data<fp::FunctionExpression>>>, C> element, DomainSetListList& function_domain_sets)
{
    visit([&](auto&& arg) { insert_constants_into_parameter_domain(arg, function_domain_sets); }, element.get_variant());
}

/**
 * Restrict
 */

static void restrict_parameter_domain(float_t, DomainSetList&, const DomainSetListList&);

template<f::OpKind O, fp::Context C>
void restrict_parameter_domain(View<Index<fp::UnaryOperator<O, Data<fp::FunctionExpression>>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& function_domain_sets);

template<f::OpKind O, fp::Context C>
void restrict_parameter_domain(View<Index<fp::BinaryOperator<O, Data<fp::FunctionExpression>>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& function_domain_sets);

template<f::OpKind O, fp::Context C>
void restrict_parameter_domain(View<Index<fp::MultiOperator<O, Data<fp::FunctionExpression>>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& function_domain_sets);

template<f::FactKind T, fp::Context C>
void restrict_parameter_domain(View<Index<fp::Atom<T>>, C> element, DomainSetList& parameter_domains, const DomainSetListList& predicate_domain_sets);

template<f::FactKind T, fp::Context C>
void restrict_parameter_domain(View<Index<fp::Literal<T>>, C> element, DomainSetList& parameter_domains, const DomainSetListList& predicate_domain_sets);

template<f::FactKind T, fp::Context C>
void restrict_parameter_domain(View<Index<fp::FunctionTerm<T>>, C> element, DomainSetList& parameter_domains, const DomainSetListList& function_domain_sets);

template<fp::Context C>
void restrict_parameter_domain(View<Index<fp::FunctionTerm<f::FluentTag>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& function_domain_sets);

template<fp::Context C>
void restrict_parameter_domain(View<Data<fp::ArithmeticOperator<Data<fp::FunctionExpression>>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& function_domain_sets);

template<fp::Context C>
void restrict_parameter_domain(View<Data<fp::FunctionExpression>, C> element, DomainSetList& parameter_domains, const DomainSetListList& function_domain_sets);

template<fp::Context C>
void restrict_parameter_domain(View<Data<fp::BooleanOperator<Data<fp::FunctionExpression>>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& function_domain_sets);

static void restrict_parameter_domain(float_t, DomainSetList&, const DomainSetListList&) {}

template<f::OpKind O, fp::Context C>
void restrict_parameter_domain(View<Index<fp::UnaryOperator<O, Data<fp::FunctionExpression>>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& function_domain_sets)
{
    restrict_parameter_domain(element.get_arg(), parameter_domains, function_domain_sets);
}

template<f::OpKind O, fp::Context C>
void restrict_parameter_domain(View<Index<fp::BinaryOperator<O, Data<fp::FunctionExpression>>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& function_domain_sets)
{
    restrict_parameter_domain(element.get_lhs(), parameter_domains, function_domain_sets);
    restrict_parameter_domain(element.get_rhs(), parameter_domains, function_domain_sets);
}

template<f::OpKind O, fp::Context C>
void restrict_parameter_domain(View<Index<fp::MultiOperator<O, Data<fp::FunctionExpression>>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& function_domain_sets)
{
    for (const auto arg : element.get_args())
        restrict_parameter_domain(arg, parameter_domains, function_domain_sets);
}

template<f::FactKind T, fp::Context C>
void restrict_parameter_domain(View<Index<fp::Atom<T>>, C> element, DomainSetList& parameter_domains, const DomainSetListList& predicate_domain_sets)
{
    const auto predicate = element.get_predicate();

    auto pos = size_t { 0 };
    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, View<Index<f::Object>, C>>)
                {
                    // Cannot know parameter index such that there is nothing to be done.
                }
                else if constexpr (std::is_same_v<Alternative, f::ParameterIndex>)
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

template<f::FactKind T, fp::Context C>
void restrict_parameter_domain(View<Index<fp::Literal<T>>, C> element, DomainSetList& parameter_domains, const DomainSetListList& predicate_domain_sets)
{
    if (!element.get_polarity())
        return;  // IMPORTANT: do NOT restrict from negated literals

    restrict_parameter_domain(element.get_atom(), parameter_domains, predicate_domain_sets);
}

template<f::FactKind T, fp::Context C>
void restrict_parameter_domain(View<Index<fp::FunctionTerm<T>>, C> element, DomainSetList& parameter_domains, const DomainSetListList& function_domain_sets)
{
    const auto function = element.get_function();

    auto pos = size_t { 0 };
    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, View<Index<f::Object>, C>>)
                {
                    // Cannot know parameter index such that there is nothing to be done.
                }
                else if constexpr (std::is_same_v<Alternative, f::ParameterIndex>)
                {
                    const auto parameter_index = uint_t(arg);
                    auto& parameter_domain = parameter_domains[parameter_index];
                    const auto& function_domain = function_domain_sets[function.get_index().value][pos];

                    intersect_inplace(parameter_domain, function_domain);
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

template<fp::Context C>
void restrict_parameter_domain(View<Index<fp::FunctionTerm<f::FluentTag>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& function_domain_sets)
{
    // Dont restrict for fluent fterm
}

template<fp::Context C>
void restrict_parameter_domain(View<Data<fp::ArithmeticOperator<Data<fp::FunctionExpression>>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& function_domain_sets)
{
    visit([&](auto&& arg) { restrict_parameter_domain(arg, parameter_domains, function_domain_sets); }, element.get_variant());
}

template<fp::Context C>
void restrict_parameter_domain(View<Data<fp::FunctionExpression>, C> element, DomainSetList& parameter_domains, const DomainSetListList& function_domain_sets)
{
    visit([&](auto&& arg) { restrict_parameter_domain(arg, parameter_domains, function_domain_sets); }, element.get_variant());
}

template<fp::Context C>
void restrict_parameter_domain(View<Data<fp::BooleanOperator<Data<fp::FunctionExpression>>>, C> element,
                               DomainSetList& parameter_domains,
                               const DomainSetListList& function_domain_sets)
{
    visit([&](auto&& arg) { restrict_parameter_domain(arg, parameter_domains, function_domain_sets); }, element.get_variant());
}

/**
 * Lift
 */

template<fp::Context C>
bool lift_parameter_domain(View<Data<fp::FunctionExpression>, C> element, const DomainSetList& parameter_domains, DomainSetListList& function_domain_sets);

static bool lift_parameter_domain(float_t, const DomainSetList&, DomainSetListList&);

template<f::OpKind O, fp::Context C>
bool lift_parameter_domain(View<Index<fp::UnaryOperator<O, Data<fp::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets);

template<f::OpKind O, fp::Context C>
bool lift_parameter_domain(View<Index<fp::BinaryOperator<O, Data<fp::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets);

template<f::OpKind O, fp::Context C>
bool lift_parameter_domain(View<Index<fp::MultiOperator<O, Data<fp::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets);

template<fp::NumericEffectOpKind Op, f::FactKind T, fp::Context C>
bool lift_parameter_domain(View<Index<fp::NumericEffect<Op, T>>, C> element, const DomainSetList& parameter_domains, DomainSetListList& function_domain_sets);

template<f::FactKind T, fp::Context C>
bool lift_parameter_domain(View<Index<fp::Atom<T>>, C> element, const DomainSetList& parameter_domains, DomainSetListList& predicate_domain_sets);

template<f::FactKind T, fp::Context C>
bool lift_parameter_domain(View<Index<fp::Literal<T>>, C> element, const DomainSetList& parameter_domains, DomainSetListList& predicate_domain_sets);

template<f::FactKind T, fp::Context C>
bool lift_parameter_domain(View<Index<fp::FunctionTerm<T>>, C> element, const DomainSetList& parameter_domains, DomainSetListList& function_domain_sets);

template<fp::Context C>
bool lift_parameter_domain(View<Index<fp::FunctionTerm<f::StaticTag>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets);

template<fp::Context C>
bool lift_parameter_domain(View<Data<fp::ArithmeticOperator<Data<fp::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets);

template<fp::Context C>
bool lift_parameter_domain(View<Data<fp::FunctionExpression>, C> element, const DomainSetList& parameter_domains, DomainSetListList& function_domain_sets);

template<fp::Context C>
bool lift_parameter_domain(View<Data<fp::BooleanOperator<Data<fp::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets);

template<f::FactKind T, fp::Context C>
bool lift_parameter_domain(View<Data<fp::NumericEffectOperator<T>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets);

template<fp::Context C>
bool lift_parameter_domain(View<Data<fp::FunctionExpression>, C> element, const DomainSetList& parameter_domains, DomainSetListList& function_domain_sets);

static bool lift_parameter_domain(float_t, const DomainSetList&, DomainSetListList&) { return false; }

template<f::OpKind O, fp::Context C>
bool lift_parameter_domain(View<Index<fp::UnaryOperator<O, Data<fp::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets)
{
    return lift_parameter_domain(element.get_arg(), parameter_domains, function_domain_sets);
}

template<f::OpKind O, fp::Context C>
bool lift_parameter_domain(View<Index<fp::BinaryOperator<O, Data<fp::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets)
{
    return lift_parameter_domain(element.get_lhs(), parameter_domains, function_domain_sets)
           || lift_parameter_domain(element.get_rhs(), parameter_domains, function_domain_sets);
}

template<f::OpKind O, fp::Context C>
bool lift_parameter_domain(View<Index<fp::MultiOperator<O, Data<fp::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets)
{
    return std::any_of(element.get_args().begin(),
                       element.get_args().end(),
                       [&](auto&& arg) { return lift_parameter_domain(arg, parameter_domains, function_domain_sets); });
}

template<fp::NumericEffectOpKind Op, f::FactKind T, fp::Context C>
bool lift_parameter_domain(View<Index<fp::NumericEffect<Op, T>>, C> element, const DomainSetList& parameter_domains, DomainSetListList& function_domain_sets)
{
    return lift_parameter_domain(element.get_fterm(), parameter_domains, function_domain_sets)
           || lift_parameter_domain(element.get_fexpr(), parameter_domains, function_domain_sets);
}

template<f::FactKind T, fp::Context C>
bool lift_parameter_domain(View<Index<fp::Atom<T>>, C> element, const DomainSetList& parameter_domains, DomainSetListList& predicate_domain_sets)
{
    const auto predicate = element.get_predicate();

    bool changed = false;

    auto pos = size_t { 0 };
    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, View<Index<f::Object>, C>>)
                {
                    auto& predicate_domain = predicate_domain_sets[predicate.get_index().value][pos];
                    size_t before = predicate_domain.size();
                    union_inplace(predicate_domain, DomainSet { arg.get_index() });
                    if (predicate_domain.size() != before)
                        changed = true;
                }
                else if constexpr (std::is_same_v<Alternative, f::ParameterIndex>)
                {
                    const auto parameter_index = uint_t(arg);
                    const auto& parameter_domain = parameter_domains[parameter_index];
                    auto& predicate_domain = predicate_domain_sets[predicate.get_index().value][pos];
                    size_t before = predicate_domain.size();
                    union_inplace(predicate_domain, parameter_domain);
                    if (predicate_domain.size() != before)
                        changed = true;
                }
                else
                {
                    static_assert(dependent_false<Alternative>::value, "Missing case");
                }
            },
            term.get_variant());
        ++pos;
    }
    return changed;
}

template<f::FactKind T, fp::Context C>
bool lift_parameter_domain(View<Index<fp::Literal<T>>, C> element, const DomainSetList& parameter_domains, DomainSetListList& predicate_domain_sets)
{
    return lift_parameter_domain(element.get_atom(), parameter_domains, predicate_domain_sets);
}

template<f::FactKind T, fp::Context C>
bool lift_parameter_domain(View<Index<fp::FunctionTerm<T>>, C> element, const DomainSetList& parameter_domains, DomainSetListList& function_domain_sets)
{
    const auto function = element.get_function();

    bool changed = false;

    auto pos = size_t { 0 };
    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, View<Index<f::Object>, C>>)
                {
                    auto& function_domain = function_domain_sets[function.get_index().value][pos];
                    size_t before = function_domain.size();
                    union_inplace(function_domain, DomainSet { arg.get_index() });
                    if (function_domain.size() != before)
                        changed = true;
                }
                else if constexpr (std::is_same_v<Alternative, f::ParameterIndex>)
                {
                    const auto parameter_index = uint_t(arg);
                    const auto& parameter_domain = parameter_domains[parameter_index];
                    auto& function_domain = function_domain_sets[function.get_index().value][pos];
                    size_t before = function_domain.size();
                    union_inplace(function_domain, parameter_domain);
                    if (function_domain.size() != before)
                        changed = true;
                }
                else
                {
                    static_assert(dependent_false<Alternative>::value, "Missing case");
                }
            },
            term.get_variant());
        ++pos;
    }
    return changed;
}

template<fp::Context C>
bool lift_parameter_domain(View<Index<fp::FunctionTerm<f::StaticTag>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets)
{
    return false;
}

template<fp::Context C>
bool lift_parameter_domain(View<Data<fp::ArithmeticOperator<Data<fp::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets)
{
    return visit([&](auto&& arg) { return lift_parameter_domain(arg, parameter_domains, function_domain_sets); }, element.get_variant());
}

template<fp::Context C>
bool lift_parameter_domain(View<Data<fp::FunctionExpression>, C> element, const DomainSetList& parameter_domains, DomainSetListList& function_domain_sets)
{
    return visit([&](auto&& arg) { return lift_parameter_domain(arg, parameter_domains, function_domain_sets); }, element.get_variant());
}

template<fp::Context C>
bool lift_parameter_domain(View<Data<fp::BooleanOperator<Data<fp::FunctionExpression>>>, C> element,
                           const DomainSetList& parameter_domains,
                           DomainSetListList& function_domain_sets)
{
    return visit([&](auto&& arg) { return lift_parameter_domain(arg, parameter_domains, function_domain_sets); }, element.get_variant());
}

template<f::FactKind T, fp::Context C>
bool lift_parameter_domain(View<Data<fp::NumericEffectOperator<T>>, C> element, const DomainSetList& parameter_domains, DomainSetListList& function_domain_sets)
{
    return visit([&](auto&& arg) { return lift_parameter_domain(arg, parameter_domains, function_domain_sets); }, element.get_variant());
}

TaskVariableDomains compute_variable_domains(View<Index<fp::Task>, fp::Repository> task)
{
    auto objects = std::vector<Index<f::Object>> {};
    for (const auto object : task.get_domain().get_constants())
        objects.push_back(object.get_index());
    for (const auto object : task.get_objects())
        objects.push_back(object.get_index());
    auto universe = DomainSet(objects.begin(), objects.end());

    ///--- Step 1: Initialize static and fluent predicate parameter domains

    auto static_predicate_domain_sets = initialize_predicate_domain_sets(task.get_domain().get_predicates<f::StaticTag>());
    auto fluent_predicate_domain_sets = initialize_predicate_domain_sets(task.get_domain().get_predicates<f::FluentTag>());
    auto derived_predicate_indices = IndexList<f::Predicate<f::DerivedTag>> {};
    for (const auto predicate : task.get_domain().get_predicates<f::DerivedTag>())
        derived_predicate_indices.push_back(predicate.get_index());
    for (const auto predicate : task.get_derived_predicates())
        derived_predicate_indices.push_back(predicate.get_index());
    auto derived_predicate_domain_sets = initialize_predicate_domain_sets(make_view(derived_predicate_indices, task.get_context()));
    insert_into_predicate_domain_sets(task.get_atoms<f::StaticTag>(), static_predicate_domain_sets);
    insert_into_predicate_domain_sets(task.get_atoms<f::FluentTag>(), fluent_predicate_domain_sets);

    ///--- Step 2: Initialize static and fluent function parameter domains

    auto static_function_domain_sets = initialize_function_domain_sets(task.get_domain().get_functions<f::StaticTag>());
    auto fluent_function_domain_sets = initialize_function_domain_sets(task.get_domain().get_functions<f::FluentTag>());
    insert_into_function_domain_sets(task.get_fterm_values<f::StaticTag>(), static_function_domain_sets);
    insert_into_function_domain_sets(task.get_fterm_values<f::FluentTag>(), fluent_function_domain_sets);

    // Important not to forget constants in schemas
    for (const auto action : task.get_domain().get_actions())
    {
        for (const auto literal : action.get_condition().get_literals<f::StaticTag>())
            insert_constants_into_parameter_domain(literal.get_atom(), static_predicate_domain_sets);

        for (const auto op : action.get_condition().get_numeric_constraints())
            insert_constants_into_parameter_domain(op, static_function_domain_sets);

        for (const auto c_effect : action.get_effects())
        {
            for (const auto literal : c_effect.get_condition().get_literals<f::StaticTag>())
                insert_constants_into_parameter_domain(literal.get_atom(), static_predicate_domain_sets);

            for (const auto literal : c_effect.get_condition().get_literals<f::FluentTag>())
                insert_constants_into_parameter_domain(literal.get_atom(), fluent_predicate_domain_sets);

            for (const auto op : c_effect.get_condition().get_numeric_constraints())
                insert_constants_into_parameter_domain(op, static_function_domain_sets);
        }
    }

    for (const auto axiom : task.get_domain().get_axioms())
    {
        for (const auto literal : axiom.get_body().get_literals<f::StaticTag>())
            insert_constants_into_parameter_domain(literal.get_atom(), static_predicate_domain_sets);

        for (const auto op : axiom.get_body().get_numeric_constraints())
            insert_constants_into_parameter_domain(op, static_function_domain_sets);
    }

    for (const auto axiom : task.get_axioms())
    {
        for (const auto literal : axiom.get_body().get_literals<f::StaticTag>())
            insert_constants_into_parameter_domain(literal.get_atom(), static_predicate_domain_sets);

        for (const auto op : axiom.get_body().get_numeric_constraints())
            insert_constants_into_parameter_domain(op, static_function_domain_sets);
    }

    ///--- Step 3: Compute rule parameter domains as tightest bound from the previously computed domains of the static predicates.

    auto action_domain_sets = std::vector<std::pair<DomainSetList, DomainSetListList>> {};
    {
        for (const auto action : task.get_domain().get_actions())
        {
            auto variables = action.get_variables();
            auto parameter_domains = DomainSetList(variables.size(), universe);

            for (const auto literal : action.get_condition().get_literals<f::StaticTag>())
                restrict_parameter_domain(literal, parameter_domains, static_predicate_domain_sets);

            for (const auto op : action.get_condition().get_numeric_constraints())
                restrict_parameter_domain(op, parameter_domains, static_function_domain_sets);

            auto parameter_domains_per_cond_effect = DomainSetListList();

            for (const auto c_effect : action.get_effects())
            {
                const auto c_variables = c_effect.get_variables();  ///< all quantified variables

                auto c_parameter_domains = parameter_domains;
                c_parameter_domains.resize(variables.size() + c_variables.size(), universe);

                for (const auto literal : c_effect.get_condition().get_literals<f::StaticTag>())
                    restrict_parameter_domain(literal, c_parameter_domains, static_predicate_domain_sets);

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

            for (const auto literal : axiom.get_body().get_literals<f::StaticTag>())
                restrict_parameter_domain(literal, parameter_domains, static_predicate_domain_sets);

            for (const auto op : axiom.get_body().get_numeric_constraints())
                restrict_parameter_domain(op, parameter_domains, static_function_domain_sets);

            assert(axiom.get_index().value == axiom_domain_sets.size());
            axiom_domain_sets.push_back(std::move(parameter_domains));
        }

        for (const auto axiom : task.get_axioms())
        {
            auto variables = axiom.get_body().get_variables();
            auto parameter_domains = DomainSetList(variables.size(), universe);

            for (const auto literal : axiom.get_body().get_literals<f::StaticTag>())
                restrict_parameter_domain(literal, parameter_domains, static_predicate_domain_sets);

            for (const auto op : axiom.get_body().get_numeric_constraints())
                restrict_parameter_domain(op, parameter_domains, static_function_domain_sets);

            assert(axiom.get_index().value == axiom_domain_sets.size());
            axiom_domain_sets.push_back(std::move(parameter_domains));
        }
    }

    ///--- Step 4: Lift the fluent predicate domains given the variable relationships in the rules.

    bool changed = false;

    do
    {
        changed = false;

        for (const auto action : task.get_domain().get_actions())
        {
            auto& [parameter_domains, parameter_domains_per_cond_effect] = action_domain_sets[action.get_index().value];

            for (const auto literal : action.get_condition().get_literals<f::StaticTag>())
                if (lift_parameter_domain(literal, parameter_domains, static_predicate_domain_sets))
                    changed = true;

            for (const auto literal : action.get_condition().get_literals<f::FluentTag>())
                if (lift_parameter_domain(literal, parameter_domains, fluent_predicate_domain_sets))
                    changed = true;

            for (const auto literal : action.get_condition().get_literals<f::DerivedTag>())
                if (lift_parameter_domain(literal, parameter_domains, derived_predicate_domain_sets))
                    changed = true;

            for (const auto op : action.get_condition().get_numeric_constraints())
                if (lift_parameter_domain(op, parameter_domains, fluent_function_domain_sets))
                    changed = true;

            for (uint_t i = 0; i < action.get_effects().size(); ++i)
            {
                const auto c_effect = action.get_effects()[i];
                auto& c_parameter_domains = parameter_domains_per_cond_effect[i];

                for (const auto literal : c_effect.get_condition().get_literals<f::StaticTag>())
                    if (lift_parameter_domain(literal, c_parameter_domains, static_predicate_domain_sets))
                        changed = true;

                for (const auto literal : c_effect.get_condition().get_literals<f::FluentTag>())
                    if (lift_parameter_domain(literal, c_parameter_domains, fluent_predicate_domain_sets))
                        changed = true;

                for (const auto op : c_effect.get_condition().get_numeric_constraints())
                    if (lift_parameter_domain(op, c_parameter_domains, fluent_function_domain_sets))
                        changed = true;

                for (const auto literal : c_effect.get_effect().get_literals())
                    if (lift_parameter_domain(literal, c_parameter_domains, fluent_predicate_domain_sets))
                        changed = true;

                for (const auto op : c_effect.get_effect().get_numeric_effects())
                    if (lift_parameter_domain(op, c_parameter_domains, fluent_function_domain_sets))
                        changed = true;
            }
        }

        for (const auto axiom : task.get_domain().get_axioms())
        {
            auto& parameter_domains = axiom_domain_sets[axiom.get_index().value];

            for (const auto literal : axiom.get_body().get_literals<f::StaticTag>())
                if (lift_parameter_domain(literal, parameter_domains, static_predicate_domain_sets))
                    changed = true;

            for (const auto literal : axiom.get_body().get_literals<f::FluentTag>())
                if (lift_parameter_domain(literal, parameter_domains, fluent_predicate_domain_sets))
                    changed = true;

            for (const auto literal : axiom.get_body().get_literals<f::DerivedTag>())
                if (lift_parameter_domain(literal, parameter_domains, derived_predicate_domain_sets))
                    changed = true;

            for (const auto op : axiom.get_body().get_numeric_constraints())
                if (lift_parameter_domain(op, parameter_domains, fluent_function_domain_sets))
                    changed = true;

            if (lift_parameter_domain(axiom.get_head(), parameter_domains, derived_predicate_domain_sets))
                changed = true;
        }

        for (const auto axiom : task.get_axioms())
        {
            auto& parameter_domains = axiom_domain_sets[axiom.get_index().value];

            for (const auto literal : axiom.get_body().get_literals<f::StaticTag>())
                if (lift_parameter_domain(literal, parameter_domains, static_predicate_domain_sets))
                    changed = true;

            for (const auto literal : axiom.get_body().get_literals<f::FluentTag>())
                if (lift_parameter_domain(literal, parameter_domains, fluent_predicate_domain_sets))
                    changed = true;

            for (const auto literal : axiom.get_body().get_literals<f::DerivedTag>())
                if (lift_parameter_domain(literal, parameter_domains, derived_predicate_domain_sets))
                    changed = true;

            for (const auto op : axiom.get_body().get_numeric_constraints())
                if (lift_parameter_domain(op, parameter_domains, fluent_function_domain_sets))
                    changed = true;

            if (lift_parameter_domain(axiom.get_head(), parameter_domains, derived_predicate_domain_sets))
                changed = true;
        }
    } while (changed);

    ///--- Step 5: Compress sets to vectors.

    auto static_predicate_domains = to_list(static_predicate_domain_sets);
    auto fluent_predicate_domains = to_list(fluent_predicate_domain_sets);
    auto derived_predicate_domains = to_list(derived_predicate_domain_sets);
    auto static_function_domains = to_list(static_function_domain_sets);
    auto fluent_function_domains = to_list(fluent_function_domain_sets);
    auto action_domains = to_list(action_domain_sets);
    auto axiom_domains = to_list(axiom_domain_sets);

    // std::cout << std::endl;
    // std::cout << "static_predicate_domains: " << static_predicate_domains << std::endl;
    // std::cout << "fluent_predicate_domains: " << fluent_predicate_domains << std::endl;
    // std::cout << "derived_predicate_domains: " << derived_predicate_domains << std::endl;
    // std::cout << "static_function_domains: " << static_function_domains << std::endl;
    // std::cout << "fluent_function_domains: " << fluent_function_domains << std::endl;
    // std::cout << "action_domains: " << action_domains << std::endl;
    // std::cout << "axiom_domains: " << axiom_domains << std::endl;
    // std::cout << std::endl;

    return TaskVariableDomains { std::move(static_predicate_domains),
                                 std::move(fluent_predicate_domains),
                                 std::move(derived_predicate_domains),
                                 std::move(static_function_domains),
                                 std::move(fluent_function_domains),
                                 std::move(action_domains),
                                 std::move(axiom_domains) };
}
}
