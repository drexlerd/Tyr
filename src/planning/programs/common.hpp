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

#ifndef TYR_SRC_PLANNING_PROGRAMS_COMMON_HPP_
#define TYR_SRC_PLANNING_PROGRAMS_COMMON_HPP_

#include "tyr/common/declarations.hpp"
#include "tyr/common/types.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/merge.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/merge_datalog.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"
#include "tyr/planning/declarations.hpp"

namespace tyr::planning
{
extern ::cista::offset::string
create_applicability_name(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action);

extern ::cista::offset::string
create_triggered_name(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
                      View<Index<formalism::planning::ConditionalEffect>, formalism::OverlayRepository<formalism::planning::Repository>> cond_eff);

extern ::cista::offset::string
create_applicability_name(View<Index<formalism::planning::Axiom>, formalism::OverlayRepository<formalism::planning::Repository>> axiom);

inline void append_from_condition(View<Index<formalism::planning::FDRConjunctiveCondition>, formalism::OverlayRepository<formalism::planning::Repository>> cond,
                                  formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context,
                                  Data<formalism::datalog::ConjunctiveCondition>& conj_cond)
{
    for (const auto literal : cond.template get_literals<formalism::StaticTag>())
        if (literal.get_polarity())
            conj_cond.static_literals.push_back(merge_p2d(literal, context).first);

    for (const auto literal : cond.template get_literals<formalism::FluentTag>())
        if (literal.get_polarity())
            conj_cond.fluent_literals.push_back(merge_p2d(literal, context).first);

    for (const auto literal : cond.template get_literals<formalism::DerivedTag>())
        if (literal.get_polarity())
            conj_cond.fluent_literals.push_back(merge_p2d<formalism::DerivedTag,
                                                          formalism::OverlayRepository<formalism::planning::Repository>,
                                                          formalism::datalog::Repository,
                                                          formalism::FluentTag>(literal, context)
                                                    .first);
};

inline auto create_applicability_predicate(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
                                           formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto predicate_ptr = context.builder.get_builder<formalism::Predicate<formalism::FluentTag>>();
    auto& predicate = *predicate_ptr;
    predicate.clear();

    predicate.name = create_applicability_name(action);
    predicate.arity = action.get_arity();

    canonicalize(predicate);
    return context.destination.get_or_create(predicate, context.builder.get_buffer());
}

inline auto create_applicability_atom(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
                                      formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto atom_ptr = context.builder.get_builder<formalism::datalog::Atom<formalism::FluentTag>>();
    auto& atom = *atom_ptr;
    atom.clear();

    const auto applicability_predicate = make_view(create_applicability_predicate(action, context).first, context.destination);

    atom.predicate = applicability_predicate.get_index();
    for (uint_t i = 0; i < applicability_predicate.get_arity(); ++i)
        atom.terms.push_back(Data<formalism::Term>(formalism::ParameterIndex(i)));

    canonicalize(atom);
    return context.destination.get_or_create(atom, context.builder.get_buffer());
}

inline auto create_applicability_literal(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
                                         formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto literal_ptr = context.builder.get_builder<formalism::datalog::Literal<formalism::FluentTag>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.polarity = true;
    literal.atom = create_applicability_atom(action, context).first;

    canonicalize(literal);
    return context.destination.get_or_create(literal, context.builder.get_buffer());
}

inline auto create_applicability_rule(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
                                      formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto rule_ptr = context.builder.get_builder<formalism::datalog::Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    auto conj_cond_ptr = context.builder.get_builder<formalism::datalog::ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto variable : action.get_variables())
        conj_cond.variables.push_back(merge_p2d(variable, context).first);
    append_from_condition(action.get_condition(), context, conj_cond);

    canonicalize(conj_cond);
    const auto new_conj_cond = make_view(context.destination.get_or_create(conj_cond, context.builder.get_buffer()).first, context.destination);

    rule.variables = new_conj_cond.get_variables().get_data();
    rule.body = new_conj_cond.get_index();
    rule.head = create_applicability_atom(action, context).first;

    canonicalize(rule);
    return context.destination.get_or_create(rule, context.builder.get_buffer());
}

inline auto
create_triggered_predicate(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
                           View<Index<formalism::planning::ConditionalEffect>, formalism::OverlayRepository<formalism::planning::Repository>> cond_eff,
                           formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto predicate_ptr = context.builder.get_builder<formalism::Predicate<formalism::FluentTag>>();
    auto& predicate = *predicate_ptr;
    predicate.clear();

    predicate.name = create_triggered_name(action, cond_eff);
    predicate.arity = action.get_arity() + cond_eff.get_arity();

    canonicalize(predicate);
    return context.destination.get_or_create(predicate, context.builder.get_buffer());
}

inline auto create_triggered_atom(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
                                  View<Index<formalism::planning::ConditionalEffect>, formalism::OverlayRepository<formalism::planning::Repository>> cond_eff,
                                  formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto atom_ptr = context.builder.get_builder<formalism::datalog::Atom<formalism::FluentTag>>();
    auto& atom = *atom_ptr;
    atom.clear();

    const auto triggered_predicate = make_view(create_triggered_predicate(action, cond_eff, context).first, context.destination);

    atom.predicate = triggered_predicate.get_index();
    for (uint_t i = 0; i < triggered_predicate.get_arity(); ++i)
        atom.terms.push_back(Data<formalism::Term>(formalism::ParameterIndex(i)));

    canonicalize(atom);
    return context.destination.get_or_create(atom, context.builder.get_buffer());
}

inline auto
create_triggered_literal(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
                         View<Index<formalism::planning::ConditionalEffect>, formalism::OverlayRepository<formalism::planning::Repository>> cond_eff,
                         formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto literal_ptr = context.builder.get_builder<formalism::datalog::Literal<formalism::FluentTag>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.polarity = true;
    literal.atom = create_triggered_atom(action, cond_eff, context).first;

    canonicalize(literal);
    return context.destination.get_or_create(literal, context.builder.get_buffer());
}

inline auto create_triggered_rule(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
                                  View<Index<formalism::planning::ConditionalEffect>, formalism::OverlayRepository<formalism::planning::Repository>> cond_eff,
                                  formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto rule_ptr = context.builder.get_builder<formalism::datalog::Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    auto conj_cond_ptr = context.builder.get_builder<formalism::datalog::ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto variable : action.get_variables())
        conj_cond.variables.push_back(merge_p2d(variable, context).first);
    for (const auto variable : cond_eff.get_variables())
        conj_cond.variables.push_back(merge_p2d(variable, context).first);
    for (const auto literal : action.get_condition().get_literals<formalism::StaticTag>())
        conj_cond.static_literals.push_back(merge_p2d(literal, context).first);
    for (const auto literal : action.get_condition().get_literals<formalism::StaticTag>())
        conj_cond.static_literals.push_back(merge_p2d(literal, context).first);
    append_from_condition(cond_eff.get_condition(), context, conj_cond);
    conj_cond.fluent_literals.push_back(create_applicability_literal(action, context).first);

    canonicalize(conj_cond);
    const auto new_conj_cond = make_view(context.destination.get_or_create(conj_cond, context.builder.get_buffer()).first, context.destination);

    rule.variables = new_conj_cond.get_variables().get_data();
    rule.body = new_conj_cond.get_index();
    rule.head = create_triggered_atom(action, cond_eff, context).first;

    canonicalize(rule);
    return context.destination.get_or_create(rule, context.builder.get_buffer());
}

inline auto create_effect_rule(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
                               View<Index<formalism::planning::ConditionalEffect>, formalism::OverlayRepository<formalism::planning::Repository>> cond_eff,
                               View<Index<formalism::datalog::Atom<formalism::FluentTag>>, formalism::datalog::Repository> effect,
                               formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto rule_ptr = context.builder.get_builder<formalism::datalog::Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    auto conj_cond_ptr = context.builder.get_builder<formalism::datalog::ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto variable : action.get_variables())
        conj_cond.variables.push_back(merge_p2d(variable, context).first);
    for (const auto variable : cond_eff.get_variables())
        conj_cond.variables.push_back(merge_p2d(variable, context).first);
    conj_cond.fluent_literals.push_back(create_triggered_literal(action, cond_eff, context).first);

    canonicalize(conj_cond);
    const auto new_conj_cond = make_view(context.destination.get_or_create(conj_cond, context.builder.get_buffer()).first, context.destination);

    rule.variables = new_conj_cond.get_variables().get_data();
    rule.body = new_conj_cond.get_index();
    rule.head = effect.get_index();

    canonicalize(rule);
    return context.destination.get_or_create(rule, context.builder.get_buffer());
}

inline auto create_cond_effect_rule(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
                                    View<Index<formalism::planning::ConditionalEffect>, formalism::OverlayRepository<formalism::planning::Repository>> cond_eff,
                                    View<Index<formalism::datalog::Atom<formalism::FluentTag>>, formalism::datalog::Repository> effect,
                                    formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto rule_ptr = context.builder.get_builder<formalism::datalog::Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    auto conj_cond_ptr = context.builder.get_builder<formalism::datalog::ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto variable : action.get_variables())
        conj_cond.variables.push_back(merge_p2d(variable, context).first);
    for (const auto variable : cond_eff.get_variables())
        conj_cond.variables.push_back(merge_p2d(variable, context).first);
    for (const auto literal : action.get_condition().get_literals<formalism::StaticTag>())
        conj_cond.static_literals.push_back(merge_p2d(literal, context).first);
    append_from_condition(cond_eff.get_condition(), context, conj_cond);
    conj_cond.fluent_literals.push_back(create_applicability_literal(action, context).first);

    canonicalize(conj_cond);
    const auto new_conj_cond = make_view(context.destination.get_or_create(conj_cond, context.builder.get_buffer()).first, context.destination);

    rule.variables = new_conj_cond.get_variables().get_data();
    rule.body = new_conj_cond.get_index();
    rule.head = effect.get_index();

    canonicalize(rule);
    return context.destination.get_or_create(rule, context.builder.get_buffer());
}

inline auto
create_full_cond_effect_rule(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
                             View<Index<formalism::planning::ConditionalEffect>, formalism::OverlayRepository<formalism::planning::Repository>> cond_eff,
                             View<Index<formalism::datalog::Atom<formalism::FluentTag>>, formalism::datalog::Repository> effect,
                             formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto rule_ptr = context.builder.get_builder<formalism::datalog::Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    auto conj_cond_ptr = context.builder.get_builder<formalism::datalog::ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto variable : action.get_variables())
        conj_cond.variables.push_back(merge_p2d(variable, context).first);
    for (const auto variable : cond_eff.get_variables())
        conj_cond.variables.push_back(merge_p2d(variable, context).first);
    append_from_condition(action.get_condition(), context, conj_cond);
    append_from_condition(cond_eff.get_condition(), context, conj_cond);

    canonicalize(conj_cond);
    const auto new_conj_cond = make_view(context.destination.get_or_create(conj_cond, context.builder.get_buffer()).first, context.destination);

    rule.variables = new_conj_cond.get_variables().get_data();
    rule.body = new_conj_cond.get_index();
    rule.head = effect.get_index();

    canonicalize(rule);
    return context.destination.get_or_create(rule, context.builder.get_buffer());
}

inline auto create_applicability_predicate(View<Index<formalism::planning::Axiom>, formalism::OverlayRepository<formalism::planning::Repository>> axiom,
                                           formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto predicate_ptr = context.builder.get_builder<formalism::Predicate<formalism::FluentTag>>();
    auto& predicate = *predicate_ptr;
    predicate.clear();

    predicate.name = create_applicability_name(axiom);
    predicate.arity = axiom.get_arity();

    canonicalize(predicate);
    return context.destination.get_or_create(predicate, context.builder.get_buffer());
}

inline auto create_applicability_atom(View<Index<formalism::planning::Axiom>, formalism::OverlayRepository<formalism::planning::Repository>> axiom,
                                      formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto atom_ptr = context.builder.get_builder<formalism::datalog::Atom<formalism::FluentTag>>();
    auto& atom = *atom_ptr;
    atom.clear();

    const auto applicability_predicate = make_view(create_applicability_predicate(axiom, context).first, context.destination);

    atom.predicate = applicability_predicate.get_index();
    for (uint_t i = 0; i < applicability_predicate.get_arity(); ++i)
        atom.terms.push_back(Data<formalism::Term>(formalism::ParameterIndex(i)));

    canonicalize(atom);
    return context.destination.get_or_create(atom, context.builder.get_buffer());
}

inline auto create_applicability_literal(View<Index<formalism::planning::Axiom>, formalism::OverlayRepository<formalism::planning::Repository>> axiom,
                                         formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto literal_ptr = context.builder.get_builder<formalism::datalog::Literal<formalism::FluentTag>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.polarity = true;
    literal.atom = create_applicability_atom(axiom, context).first;

    canonicalize(literal);
    return context.destination.get_or_create(literal, context.builder.get_buffer());
}

inline auto create_applicability_rule(View<Index<formalism::planning::Axiom>, formalism::OverlayRepository<formalism::planning::Repository>> axiom,
                                      formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto rule_ptr = context.builder.get_builder<formalism::datalog::Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    auto conj_cond_ptr = context.builder.get_builder<formalism::datalog::ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto variable : axiom.get_variables())
        conj_cond.variables.push_back(merge_p2d(variable, context).first);
    append_from_condition(axiom.get_body(), context, conj_cond);

    canonicalize(conj_cond);
    const auto new_conj_cond = make_view(context.destination.get_or_create(conj_cond, context.builder.get_buffer()).first, context.destination);

    rule.variables = new_conj_cond.get_variables().get_data();
    rule.body = new_conj_cond.get_index();
    rule.head = create_applicability_atom(axiom, context).first;

    canonicalize(rule);
    return context.destination.get_or_create(rule, context.builder.get_buffer());
}

inline auto create_effect_rule(View<Index<formalism::planning::Axiom>, formalism::OverlayRepository<formalism::planning::Repository>> axiom,
                               View<Index<formalism::datalog::Atom<formalism::FluentTag>>, formalism::datalog::Repository> effect,
                               formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto rule_ptr = context.builder.get_builder<formalism::datalog::Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    auto conj_cond_ptr = context.builder.get_builder<formalism::datalog::ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto variable : axiom.get_variables())
        conj_cond.variables.push_back(merge_p2d(variable, context).first);
    for (const auto literal : axiom.get_body().get_literals<formalism::StaticTag>())
        conj_cond.static_literals.push_back(merge_p2d(literal, context).first);
    conj_cond.fluent_literals.push_back(create_applicability_literal(axiom, context).first);

    canonicalize(conj_cond);
    const auto new_conj_cond = make_view(context.destination.get_or_create(conj_cond, context.builder.get_buffer()).first, context.destination);

    rule.variables = new_conj_cond.get_variables().get_data();
    rule.body = new_conj_cond.get_index();
    rule.head = effect.get_index();

    canonicalize(rule);
    return context.destination.get_or_create(rule, context.builder.get_buffer());
}
}

#endif