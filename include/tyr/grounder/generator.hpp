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

#ifndef TYR_GROUNDER_GENERATOR_HPP_
#define TYR_GROUNDER_GENERATOR_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_atom_index.hpp"
#include "tyr/grounder/applicability.hpp"
#include "tyr/grounder/declarations.hpp"
#include "tyr/grounder/kpkc.hpp"

namespace tyr::grounder
{

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
View<Index<formalism::GroundAtom<T>>, formalism::ScopedRepository<C>> ground(View<Index<formalism::Atom<T>>, C> element, MutableRuleWorkspace<C>& workspace)
{
    // Fetch and clear
    auto& binding = workspace.binding;
    auto& builder = workspace.builder;
    auto& repository = workspace.repository;
    auto& buffer = workspace.buffer;
    auto& ground_atom = builder.template get_ground_atom<T>();
    auto& objects = ground_atom.objects;
    objects.clear();

    // Fill data
    ground_atom.index.group = element.get_index().get_group();
    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, formalism::ParameterIndex>)
                {
                    objects.push_back(binding[uint_t(arg)]);
                }
                else if constexpr (std::is_same_v<Alternative, View<Index<formalism::Object>, C>>)
                {
                    objects.push_back(arg.get_index());
                }
                else
                {
                    static_assert(dependent_false<Alternative>::value, "Missing case");
                }
            },
            term.get());
    }

    // Canonicalize and Serialize
    canonicalize(ground_atom);
    return repository.get_or_create(ground_atom, buffer).first;
}

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
View<Index<formalism::GroundLiteral<T>>, formalism::ScopedRepository<C>> ground(View<Index<formalism::Literal<T>>, C> element,
                                                                                MutableRuleWorkspace<C>& workspace)
{
    // Fetch and clear
    auto& builder = workspace.builder;
    auto& repository = workspace.repository;
    auto& buffer = workspace.buffer;
    auto& ground_literal = builder.template get_ground_literal<T>();

    // Fill data
    ground_literal.index.group = element.get_index().get_group();
    ground_literal.polarity = element.get_polarity();
    ground_literal.atom = ground(element.get_atom(), workspace).get_index();

    // Canonicalize and Serialize
    canonicalize(ground_literal);
    return repository.get_or_create(ground_literal, buffer).first;
}

template<formalism::IsContext C>
View<Index<formalism::GroundConjunctiveCondition>, formalism::ScopedRepository<C>> ground(View<Index<formalism::ConjunctiveCondition>, C> element,
                                                                                          MutableRuleWorkspace<C>& workspace)
{
    // Fetch and clear
    auto& binding = workspace.binding;
    auto& builder = workspace.builder;
    auto& repository = workspace.repository;
    auto& buffer = workspace.buffer;
    auto& conj_cond = builder.ground_conj_cond;
    auto& objects = conj_cond.objects;
    auto& static_literals = conj_cond.static_literals;
    auto& fluent_literals = conj_cond.fluent_literals;
    auto& numeric_constraints = conj_cond.numeric_constraints;
    objects.clear();
    static_literals.clear();
    fluent_literals.clear();
    numeric_constraints.clear();

    // Fill data
    objects = binding;
    for (const auto literal : element.template get_literals<formalism::StaticTag>())
        static_literals.push_back(ground(literal, workspace).get_index());
    for (const auto literal : element.template get_literals<formalism::FluentTag>())
        fluent_literals.push_back(ground(literal, workspace).get_index());
    // for (const auto numeric_constraint : element.get_numeric_constraints())
    //     numeric_constraints.push_back(ground(numeric_constraint, workspace).get_index());

    // Canonicalize and Serialize
    canonicalize(conj_cond);
    return repository.get_or_create(conj_cond, buffer).first;
}

template<formalism::IsContext C>
View<Index<formalism::GroundRule>, formalism::ScopedRepository<C>> ground(View<Index<formalism::Rule>, C> element, MutableRuleWorkspace<C>& workspace)
{
    // Fetch and clear
    auto& builder = workspace.builder;
    auto& repository = workspace.repository;
    auto& buffer = workspace.buffer;
    auto& rule = builder.ground_rule;
    auto& body = rule.body;
    auto& head = rule.head;

    // Fill data
    rule.index.group = element.get_index();
    body = ground(element.get_body(), workspace).get_index();
    head = ground(element.get_head(), workspace).get_index();

    // Canonicalize and Serialize
    canonicalize(rule);
    return repository.get_or_create(rule, buffer).first;
}

template<formalism::IsContext C>
void ground_nullary_case(const ImmutableRuleWorkspace<C>& immutable_workspace, MutableRuleWorkspace<C>& mutable_workspace)
{
    mutable_workspace.binding.clear();

    auto ground_rule = ground(immutable_workspace.rule, mutable_workspace);

    std::cout << ground_rule << std::endl;
}

template<formalism::IsContext C>
void ground_unary_case(const ImmutableRuleWorkspace<C>& immutable_workspace, MutableRuleWorkspace<C>& mutable_workspace)
{
    mutable_workspace.binding.clear();

    for (const auto vertex_index : mutable_workspace.kpkc_workspace.consistent_vertices_vec)
    {
        const auto& vertex = immutable_workspace.static_consistency_graph.get_vertex(vertex_index);
        mutable_workspace.binding.push_back(vertex.get_object_index());

        auto ground_rule = ground(immutable_workspace.rule, mutable_workspace);

        std::cout << ground_rule << std::endl;
    }
}

template<formalism::IsContext C>
void ground_general_case(const ImmutableRuleWorkspace<C>& immutable_workspace, MutableRuleWorkspace<C>& mutable_workspace)
{
    kpkc::for_each_k_clique(immutable_workspace.consistency_graph,
                            mutable_workspace.kpkc_workspace,
                            [&](auto&& clique)
                            {
                                mutable_workspace.binding.clear();

                                for (const auto integer : clique)
                                    mutable_workspace.binding.push_back(Index<formalism::Object>(integer));

                                auto ground_rule = ground(immutable_workspace.rule, mutable_workspace);

                                std::cout << ground_rule << std::endl;
                            });
}

template<formalism::IsContext C>
void ground(const ImmutableRuleWorkspace<C>& immutable_workspace, MutableRuleWorkspace<C>& mutable_workspace)
{
    const auto rule = immutable_workspace.rule;
    const auto& fact_sets = immutable_workspace.fact_sets;

    if (!nullary_conditions_hold(rule.get_body(), fact_sets))
        return;

    const auto arity = rule.get_body().get_arity();

    if (arity == 0)
        ground_nullary_case(immutable_workspace, mutable_workspace);
    else if (arity == 1)
        ground_unary_case(immutable_workspace, mutable_workspace);
    else
        ground_general_case(immutable_workspace, mutable_workspace);
}

}

#endif
