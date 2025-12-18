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

#include "common.hpp"

using namespace tyr::formalism;

namespace tyr::planning
{

inline void append_from_condition(View<Index<FDRConjunctiveCondition>, OverlayRepository<Repository>> cond,
                                  MergeContext<OverlayRepository<Repository>, Repository>& context,
                                  Data<ConjunctiveCondition>& conj_cond)
{
    for (const auto literal : cond.template get_literals<StaticTag>())
        if (literal.get_polarity())
            conj_cond.static_literals.push_back(merge(literal, context).get_index());

    for (const auto literal : cond.template get_literals<FluentTag>())
        if (literal.get_polarity())
            conj_cond.fluent_literals.push_back(merge(literal, context).get_index());

    for (const auto literal : cond.template get_literals<DerivedTag>())
        if (literal.get_polarity())
            conj_cond.fluent_literals.push_back(merge<DerivedTag, OverlayRepository<Repository>, Repository, FluentTag>(literal, context).get_index());
};

::cista::offset::string create_applicability_name(View<Index<Action>, OverlayRepository<Repository>> action)
{
    return ::cista::offset::string { std::string { "@" } + action.get_name().str() };
}

::cista::offset::string create_triggered_name(View<Index<Action>, OverlayRepository<Repository>> action,
                                              View<Index<ConditionalEffect>, OverlayRepository<Repository>> cond_eff)
{
    return ::cista::offset::string { std::string { "@" } + action.get_name().str() + std::string("_") + std::to_string(cond_eff.get_index().get_value()) };
}

::cista::offset::string create_applicability_name(View<Index<Axiom>, OverlayRepository<Repository>> axiom)
{
    return ::cista::offset::string { std::string { "@" } + axiom.get_head().get_predicate().get_name().str() + std::string("/")
                                     + std::to_string(axiom.get_head().get_predicate().get_arity()) };
}

View<Index<Predicate<FluentTag>>, Repository> create_applicability_predicate(View<Index<Action>, OverlayRepository<Repository>> action,
                                                                             MergeContext<OverlayRepository<Repository>, Repository>& context)
{
    auto predicate_ptr = context.builder.get_builder<Predicate<FluentTag>>();
    auto& predicate = *predicate_ptr;
    predicate.clear();

    predicate.name = create_applicability_name(action);
    predicate.arity = action.get_arity();

    canonicalize(predicate);
    return context.destination.get_or_create(predicate, context.builder.get_buffer()).first;
}

View<Index<Atom<FluentTag>>, Repository> create_applicability_atom(View<Index<Action>, OverlayRepository<Repository>> action,
                                                                   MergeContext<OverlayRepository<Repository>, Repository>& context)
{
    auto atom_ptr = context.builder.get_builder<Atom<FluentTag>>();
    auto& atom = *atom_ptr;
    atom.clear();

    const auto applicability_predicate = create_applicability_predicate(action, context);

    atom.predicate = applicability_predicate.get_index();
    for (uint_t i = 0; i < applicability_predicate.get_arity(); ++i)
        atom.terms.push_back(Data<Term>(ParameterIndex(i)));

    canonicalize(atom);
    return context.destination.get_or_create(atom, context.builder.get_buffer()).first;
}

View<Index<Literal<FluentTag>>, Repository> create_applicability_literal(View<Index<Action>, OverlayRepository<Repository>> action,
                                                                         MergeContext<OverlayRepository<Repository>, Repository>& context)
{
    auto literal_ptr = context.builder.get_builder<Literal<FluentTag>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.polarity = true;
    literal.atom = create_applicability_atom(action, context).get_index();

    canonicalize(literal);
    return context.destination.get_or_create(literal, context.builder.get_buffer()).first;
}

View<Index<Rule>, Repository> create_applicability_rule(View<Index<Action>, OverlayRepository<Repository>> action,
                                                        MergeContext<OverlayRepository<Repository>, Repository>& context)
{
    auto rule_ptr = context.builder.get_builder<Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    auto conj_cond_ptr = context.builder.get_builder<ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto variable : action.get_variables())
        conj_cond.variables.push_back(merge(variable, context).get_index());
    append_from_condition(action.get_condition(), context, conj_cond);

    canonicalize(conj_cond);
    const auto new_conj_cond = context.destination.get_or_create(conj_cond, context.builder.get_buffer()).first;

    rule.variables = new_conj_cond.get_variables().get_data();
    rule.body = new_conj_cond.get_index();
    rule.head = create_applicability_atom(action, context).get_index();

    canonicalize(rule);
    return context.destination.get_or_create(rule, context.builder.get_buffer()).first;
}

View<Index<Predicate<FluentTag>>, Repository> create_triggered_predicate(View<Index<Action>, OverlayRepository<Repository>> action,
                                                                         View<Index<ConditionalEffect>, OverlayRepository<Repository>> cond_eff,
                                                                         MergeContext<OverlayRepository<Repository>, Repository>& context)
{
    auto predicate_ptr = context.builder.get_builder<Predicate<FluentTag>>();
    auto& predicate = *predicate_ptr;
    predicate.clear();

    predicate.name = create_triggered_name(action, cond_eff);
    predicate.arity = action.get_arity() + cond_eff.get_arity();

    canonicalize(predicate);
    return context.destination.get_or_create(predicate, context.builder.get_buffer()).first;
}

View<Index<Atom<FluentTag>>, Repository> create_triggered_atom(View<Index<Action>, OverlayRepository<Repository>> action,
                                                               View<Index<ConditionalEffect>, OverlayRepository<Repository>> cond_eff,
                                                               MergeContext<OverlayRepository<Repository>, Repository>& context)
{
    auto atom_ptr = context.builder.get_builder<Atom<FluentTag>>();
    auto& atom = *atom_ptr;
    atom.clear();

    const auto triggered_predicate = create_triggered_predicate(action, cond_eff, context);

    atom.predicate = triggered_predicate.get_index();
    for (uint_t i = 0; i < triggered_predicate.get_arity(); ++i)
        atom.terms.push_back(Data<Term>(ParameterIndex(i)));

    canonicalize(atom);
    return context.destination.get_or_create(atom, context.builder.get_buffer()).first;
}

View<Index<Literal<FluentTag>>, Repository> create_triggered_literal(View<Index<Action>, OverlayRepository<Repository>> action,
                                                                     View<Index<ConditionalEffect>, OverlayRepository<Repository>> cond_eff,
                                                                     MergeContext<OverlayRepository<Repository>, Repository>& context)
{
    auto literal_ptr = context.builder.get_builder<Literal<FluentTag>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.polarity = true;
    literal.atom = create_triggered_atom(action, cond_eff, context).get_index();

    canonicalize(literal);
    return context.destination.get_or_create(literal, context.builder.get_buffer()).first;
}

View<Index<Rule>, Repository> create_triggered_rule(View<Index<Action>, OverlayRepository<Repository>> action,
                                                    View<Index<ConditionalEffect>, OverlayRepository<Repository>> cond_eff,
                                                    MergeContext<OverlayRepository<Repository>, Repository>& context)
{
    auto rule_ptr = context.builder.get_builder<Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    auto conj_cond_ptr = context.builder.get_builder<ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto variable : action.get_variables())
        conj_cond.variables.push_back(merge(variable, context).get_index());
    for (const auto variable : cond_eff.get_variables())
        conj_cond.variables.push_back(merge(variable, context).get_index());
    append_from_condition(cond_eff.get_condition(), context, conj_cond);
    conj_cond.fluent_literals.push_back(create_applicability_literal(action, context).get_index());

    canonicalize(conj_cond);
    const auto new_conj_cond = context.destination.get_or_create(conj_cond, context.builder.get_buffer()).first;

    rule.variables = new_conj_cond.get_variables().get_data();
    rule.body = new_conj_cond.get_index();
    rule.head = create_triggered_atom(action, cond_eff, context).get_index();

    canonicalize(rule);
    return context.destination.get_or_create(rule, context.builder.get_buffer()).first;
}

View<Index<Rule>, Repository> create_effect_rule(View<Index<Action>, OverlayRepository<Repository>> action,
                                                 View<Index<ConditionalEffect>, OverlayRepository<Repository>> cond_eff,
                                                 View<Index<Atom<FluentTag>>, Repository> effect,
                                                 MergeContext<OverlayRepository<Repository>, Repository>& context)
{
    auto rule_ptr = context.builder.get_builder<Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    auto conj_cond_ptr = context.builder.get_builder<ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto variable : action.get_variables())
        conj_cond.variables.push_back(merge(variable, context).get_index());
    for (const auto variable : cond_eff.get_variables())
        conj_cond.variables.push_back(merge(variable, context).get_index());
    conj_cond.fluent_literals.push_back(create_triggered_literal(action, cond_eff, context).get_index());

    canonicalize(conj_cond);
    const auto new_conj_cond = context.destination.get_or_create(conj_cond, context.builder.get_buffer()).first;

    rule.variables = new_conj_cond.get_variables().get_data();
    rule.body = new_conj_cond.get_index();
    rule.head = effect.get_index();

    canonicalize(rule);
    return context.destination.get_or_create(rule, context.builder.get_buffer()).first;
}

extern View<Index<Rule>, Repository> create_cond_effect_rule(View<Index<Action>, OverlayRepository<Repository>> action,
                                                             View<Index<ConditionalEffect>, OverlayRepository<Repository>> cond_eff,
                                                             View<Index<Atom<FluentTag>>, Repository> effect,
                                                             MergeContext<OverlayRepository<Repository>, Repository>& context)
{
    auto rule_ptr = context.builder.get_builder<Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    auto conj_cond_ptr = context.builder.get_builder<ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto variable : action.get_variables())
        conj_cond.variables.push_back(merge(variable, context).get_index());
    for (const auto variable : cond_eff.get_variables())
        conj_cond.variables.push_back(merge(variable, context).get_index());
    append_from_condition(cond_eff.get_condition(), context, conj_cond);
    conj_cond.fluent_literals.push_back(create_applicability_literal(action, context).get_index());

    canonicalize(conj_cond);
    const auto new_conj_cond = context.destination.get_or_create(conj_cond, context.builder.get_buffer()).first;

    rule.variables = new_conj_cond.get_variables().get_data();
    rule.body = new_conj_cond.get_index();
    rule.head = effect.get_index();

    canonicalize(rule);
    return context.destination.get_or_create(rule, context.builder.get_buffer()).first;
}

View<Index<Predicate<FluentTag>>, Repository> create_applicability_predicate(View<Index<Axiom>, OverlayRepository<Repository>> axiom,
                                                                             MergeContext<OverlayRepository<Repository>, Repository>& context)
{
    auto predicate_ptr = context.builder.get_builder<Predicate<FluentTag>>();
    auto& predicate = *predicate_ptr;
    predicate.clear();

    predicate.name = create_applicability_name(axiom);
    predicate.arity = axiom.get_arity();

    canonicalize(predicate);
    return context.destination.get_or_create(predicate, context.builder.get_buffer()).first;
}

View<Index<Atom<FluentTag>>, Repository> create_applicability_atom(View<Index<Axiom>, OverlayRepository<Repository>> axiom,
                                                                   MergeContext<OverlayRepository<Repository>, Repository>& context)
{
    auto atom_ptr = context.builder.get_builder<Atom<FluentTag>>();
    auto& atom = *atom_ptr;
    atom.clear();

    const auto applicability_predicate = create_applicability_predicate(axiom, context);

    atom.predicate = applicability_predicate.get_index();
    for (uint_t i = 0; i < applicability_predicate.get_arity(); ++i)
        atom.terms.push_back(Data<Term>(ParameterIndex(i)));

    canonicalize(atom);
    return context.destination.get_or_create(atom, context.builder.get_buffer()).first;
}

View<Index<Literal<FluentTag>>, Repository> create_applicability_literal(View<Index<Axiom>, OverlayRepository<Repository>> axiom,
                                                                         MergeContext<OverlayRepository<Repository>, Repository>& context)
{
    auto literal_ptr = context.builder.get_builder<Literal<FluentTag>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.polarity = true;
    literal.atom = create_applicability_atom(axiom, context).get_index();

    canonicalize(literal);
    return context.destination.get_or_create(literal, context.builder.get_buffer()).first;
}

View<Index<Rule>, Repository> create_applicability_rule(View<Index<Axiom>, OverlayRepository<Repository>> axiom,
                                                        MergeContext<OverlayRepository<Repository>, Repository>& context)
{
    auto rule_ptr = context.builder.get_builder<Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    auto conj_cond_ptr = context.builder.get_builder<ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto variable : axiom.get_variables())
        conj_cond.variables.push_back(merge(variable, context).get_index());
    append_from_condition(axiom.get_body(), context, conj_cond);

    canonicalize(conj_cond);
    const auto new_conj_cond = context.destination.get_or_create(conj_cond, context.builder.get_buffer()).first;

    rule.variables = new_conj_cond.get_variables().get_data();
    rule.body = new_conj_cond.get_index();
    rule.head = create_applicability_atom(axiom, context).get_index();

    canonicalize(rule);
    return context.destination.get_or_create(rule, context.builder.get_buffer()).first;
}

View<Index<Rule>, Repository> create_effect_rule(View<Index<Axiom>, OverlayRepository<Repository>> axiom,
                                                 View<Index<Atom<FluentTag>>, Repository> effect,
                                                 MergeContext<OverlayRepository<Repository>, Repository>& context)
{
    auto rule_ptr = context.builder.get_builder<Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    auto conj_cond_ptr = context.builder.get_builder<ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto variable : axiom.get_variables())
        conj_cond.variables.push_back(merge(variable, context).get_index());
    conj_cond.fluent_literals.push_back(create_applicability_literal(axiom, context).get_index());

    canonicalize(conj_cond);
    const auto new_conj_cond = context.destination.get_or_create(conj_cond, context.builder.get_buffer()).first;

    rule.variables = new_conj_cond.get_variables().get_data();
    rule.body = new_conj_cond.get_index();
    rule.head = effect.get_index();

    canonicalize(rule);
    return context.destination.get_or_create(rule, context.builder.get_buffer()).first;
}
}
