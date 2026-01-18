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

#include "tyr/datalog/transformations/eliminate_dangling_existentials.hpp"

#include "tyr/formalism/datalog/arity.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/canonicalization.hpp"
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/datalog/merge.hpp"
#include "tyr/formalism/datalog/rewrite.hpp"

#include <boost/dynamic_bitset.hpp>

namespace f = tyr::formalism;
namespace fd = tyr::formalism::datalog;

namespace tyr::datalog
{

/**
 * add_edges
 */

template<f::FactKind T>
static void add_edges(View<Index<fd::Literal<T>>, fd::Repository> element, std::vector<boost::dynamic_bitset<>>& adj_matrix)
{
    auto parameters = fd::collect_parameters(element);

    for (const auto p1 : parameters)
    {
        for (const auto p2 : parameters)
        {
            adj_matrix[uint_t(p1)].set(uint_t(p2));
            adj_matrix[uint_t(p2)].set(uint_t(p1));
        }
    }

    // std::cout << element << std::endl;
    // std::cout << parameters << std::endl;
}

static void add_edges(View<Data<fd::BooleanOperator<Data<fd::FunctionExpression>>>, fd::Repository> element, std::vector<boost::dynamic_bitset<>>& adj_matrix)
{
    auto parameters = fd::collect_parameters(element);

    for (const auto p1 : parameters)
    {
        for (const auto p2 : parameters)
        {
            adj_matrix[uint_t(p1)].set(uint_t(p2));
            adj_matrix[uint_t(p2)].set(uint_t(p1));
        }
    }

    // std::cout << element << std::endl;
    // std::cout << parameters << std::endl;
}

inline ::cista::offset::string create_guard_name(View<Index<fd::Rule>, fd::Repository> rule)
{
    return ::cista::offset::string { std::string { "@guard_" } + std::to_string(rule.get_index().get_value()) };
}

inline auto create_guard_predicate(View<Index<fd::Rule>, fd::Repository> rule, fd::MergeContext<fd::Repository>& context)
{
    auto predicate_ptr = context.builder.get_builder<f::Predicate<f::FluentTag>>();
    auto& predicate = *predicate_ptr;
    predicate.clear();

    predicate.name = create_guard_name(rule);
    predicate.arity = 0;

    canonicalize(predicate);
    return context.destination.get_or_create(predicate, context.builder.get_buffer());
}

inline auto create_guard_atom(View<Index<fd::Rule>, fd::Repository> rule, fd::MergeContext<fd::Repository>& context)
{
    auto atom_ptr = context.builder.get_builder<fd::Atom<f::FluentTag>>();
    auto& atom = *atom_ptr;
    atom.clear();

    atom.predicate = create_guard_predicate(rule, context).first;

    canonicalize(atom);
    return context.destination.get_or_create(atom, context.builder.get_buffer());
}

inline auto create_guard_literal(View<Index<fd::Rule>, fd::Repository> rule, fd::MergeContext<fd::Repository>& context)
{
    auto literal_ptr = context.builder.get_builder<fd::Literal<f::FluentTag>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.atom = create_guard_atom(rule, context).first;
    literal.polarity = true;

    canonicalize(literal);
    return context.destination.get_or_create(literal, context.builder.get_buffer());
}

/**
 * eliminate_dangling_existentials
 */

static void append_guarded_rule(View<Index<fd::Rule>, fd::Repository> element,
                                fd::MergeContext<fd::Repository>& context,
                                IndexList<f::Predicate<f::FluentTag>>& out_predicates,
                                IndexList<fd::Rule>& out_rules)
{
    auto rule_ptr = context.builder.get_builder<fd::Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    auto body_ptr = context.builder.get_builder<fd::ConjunctiveCondition>();
    auto& body = *body_ptr;
    body.clear();

    auto adj_matrix = std::vector<boost::dynamic_bitset<>>(element.get_arity(), boost::dynamic_bitset<>(element.get_arity()));
    for (const auto literal : element.get_body().get_literals<f::StaticTag>())
        add_edges(literal, adj_matrix);
    for (const auto literal : element.get_body().get_literals<f::FluentTag>())
        add_edges(literal, adj_matrix);
    for (const auto numeric_constraint : element.get_body().get_numeric_constraints())
        add_edges(numeric_constraint, adj_matrix);

    auto reachable = boost::dynamic_bitset<>(element.get_arity(), false);
    auto stack = std::vector<uint_t> {};
    for (const auto p : fd::collect_parameters(element.get_head()))
    {
        stack.push_back(uint_t(p));
        reachable.set(uint_t(p));
    }
    for (uint_t p = 0; p < element.get_num_fixed_prefix_variables(); ++p)
    {
        stack.push_back(p);
        reachable.set(p);
    }

    while (!stack.empty())
    {
        const auto v = stack.back();
        stack.pop_back();

        const auto& row = adj_matrix[v];
        for (auto u = row.find_first(); u != boost::dynamic_bitset<>::npos; u = row.find_next(u))
        {
            if (!reachable.test(u))
            {
                reachable.set(u);
                stack.push_back(u);
            }
        }
    }

    auto mapping = UnorderedMap<f::ParameterIndex, f::ParameterIndex> {};
    auto next = uint_t(0);
    for (uint_t u = 0; u < element.get_arity(); ++u)
    {
        if (reachable.test(u))
        {
            mapping.emplace(f::ParameterIndex(u), f::ParameterIndex(next++));
            rule.variables.push_back(merge_d2d(element.get_variables()[u], context).first);
            body.variables.push_back(merge_d2d(element.get_body().get_variables()[u], context).first);
        }
    }
    rule.num_fixed_prefix_variables = element.get_num_fixed_prefix_variables();

    for (const auto literal : element.get_body().get_literals<f::StaticTag>())
        if (should_keep(literal, mapping))
            body.static_literals.push_back(merge(literal, mapping, context).first);
    for (const auto literal : element.get_body().get_literals<f::FluentTag>())
        if (should_keep(literal, mapping))
            body.fluent_literals.push_back(merge(literal, mapping, context).first);
    for (const auto numeric_constraint : element.get_body().get_numeric_constraints())
        if (should_keep(numeric_constraint, mapping))
            body.numeric_constraints.push_back(merge(numeric_constraint, mapping, context));

    out_predicates.push_back(create_guard_predicate(element, context).first);
    body.fluent_literals.push_back(create_guard_literal(element, context).first);
    canonicalize(body);
    rule.body = context.destination.get_or_create(body, context.builder.get_buffer()).first;
    rule.head = merge(element.get_head(), mapping, context).first;
    rule.cost = element.get_cost();

    canonicalize(rule);
    out_rules.push_back(context.destination.get_or_create(rule, context.builder.get_buffer()).first);
}

static void append_guard_rule(View<Index<fd::Rule>, fd::Repository> element, fd::MergeContext<fd::Repository>& context, IndexList<fd::Rule>& out_rules)
{
    auto guard_rule_ptr = context.builder.get_builder<fd::Rule>();
    auto& guard_rule = *guard_rule_ptr;
    guard_rule.clear();

    auto guard_body_ptr = context.builder.get_builder<fd::ConjunctiveCondition>();
    auto& guard_body = *guard_body_ptr;
    guard_body.clear();

    auto adj_matrix = std::vector<boost::dynamic_bitset<>>(element.get_arity(), boost::dynamic_bitset<>(element.get_arity()));
    for (const auto literal : element.get_body().get_literals<f::StaticTag>())
        add_edges(literal, adj_matrix);
    for (const auto literal : element.get_body().get_literals<f::FluentTag>())
        add_edges(literal, adj_matrix);
    for (const auto numeric_constraint : element.get_body().get_numeric_constraints())
        add_edges(numeric_constraint, adj_matrix);

    auto reachable = boost::dynamic_bitset<>(element.get_arity(), false);
    auto stack = std::vector<uint_t> {};
    for (const auto p : fd::collect_parameters(element.get_head()))
    {
        stack.push_back(uint_t(p));
        reachable.set(uint_t(p));
    }
    for (uint_t p = 0; p < element.get_num_fixed_prefix_variables(); ++p)
    {
        stack.push_back(p);
        reachable.set(p);
    }

    while (!stack.empty())
    {
        const auto v = stack.back();
        stack.pop_back();

        const auto& row = adj_matrix[v];
        for (auto u = row.find_first(); u != boost::dynamic_bitset<>::npos; u = row.find_next(u))
        {
            if (!reachable.test(u))
            {
                reachable.set(u);
                stack.push_back(u);
            }
        }
    }

    auto guard_mapping = UnorderedMap<f::ParameterIndex, f::ParameterIndex> {};
    auto guard_next = uint_t(0);
    for (uint_t u = 0; u < element.get_arity(); ++u)
    {
        if (!reachable.test(u))
        {
            guard_mapping.emplace(f::ParameterIndex(u), f::ParameterIndex(guard_next++));
            guard_rule.variables.push_back(merge_d2d(element.get_variables()[u], context).first);
            guard_body.variables.push_back(merge_d2d(element.get_body().get_variables()[u], context).first);
        }
    }

    for (const auto literal : element.get_body().get_literals<f::StaticTag>())
        if (should_keep(literal, guard_mapping))
            guard_body.static_literals.push_back(merge(literal, guard_mapping, context).first);
    for (const auto literal : element.get_body().get_literals<f::FluentTag>())
        if (should_keep(literal, guard_mapping))
            guard_body.fluent_literals.push_back(merge(literal, guard_mapping, context).first);
    for (const auto numeric_constraint : element.get_body().get_numeric_constraints())
        if (should_keep(numeric_constraint, guard_mapping))
            guard_body.numeric_constraints.push_back(merge(numeric_constraint, guard_mapping, context));

    canonicalize(guard_body);
    guard_rule.body = context.destination.get_or_create(guard_body, context.builder.get_buffer()).first;
    guard_rule.head = create_guard_atom(element, context).first;
    guard_rule.cost = 0;

    canonicalize(guard_rule);
    out_rules.push_back(context.destination.get_or_create(guard_rule, context.builder.get_buffer()).first);
}

Index<fd::Program> eliminate_dangling_existentials(View<Index<fd::Program>, fd::Repository> element, fd::Repository& destination)
{
    auto builder = fd::Builder();
    auto merge_context = fd::MergeContext { builder, destination };

    auto program_ptr = builder.get_builder<fd::Program>();
    auto& program = *program_ptr;
    program.clear();

    for (const auto predicate : element.get_predicates<f::StaticTag>())
        program.static_predicates.push_back(fd::merge_d2d(predicate, merge_context).first);

    for (const auto predicate : element.get_predicates<f::FluentTag>())
        program.fluent_predicates.push_back(fd::merge_d2d(predicate, merge_context).first);

    for (const auto function : element.get_functions<f::StaticTag>())
        program.static_functions.push_back(fd::merge_d2d(function, merge_context).first);

    for (const auto function : element.get_functions<f::FluentTag>())
        program.fluent_functions.push_back(fd::merge_d2d(function, merge_context).first);

    for (const auto object : element.get_objects())
        program.objects.push_back(fd::merge_d2d(object, merge_context).first);

    for (const auto atom : element.get_atoms<f::StaticTag>())
        program.static_atoms.push_back(fd::merge_d2d(atom, merge_context).first);

    for (const auto atom : element.get_atoms<f::FluentTag>())
        program.fluent_atoms.push_back(fd::merge_d2d(atom, merge_context).first);

    for (const auto fterm_value : element.get_fterm_values<f::StaticTag>())
        program.static_fterm_values.push_back(fd::merge_d2d(fterm_value, merge_context).first);

    for (const auto fterm_value : element.get_fterm_values<f::FluentTag>())
        program.fluent_fterm_values.push_back(fd::merge_d2d(fterm_value, merge_context).first);

    for (const auto rule : element.get_rules())
    {
        if (rule.get_arity() > 0)
            append_guarded_rule(rule, merge_context, program.fluent_predicates, program.rules);
        else
            program.rules.push_back(fd::merge_d2d(rule, merge_context).first);
    }
    // Append guard rules to prevent invalidating mappings between rules and actions
    for (const auto rule : element.get_rules())
    {
        if (rule.get_arity() > 0)
            append_guard_rule(rule, merge_context, program.rules);
    }

    canonicalize(program);
    return destination.get_or_create(program, builder.get_buffer()).first;
}

}
