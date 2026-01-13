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

#include "tyr/planning/programs/ground.hpp"

#include "common.hpp"
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/planning/formatter.hpp"
#include "tyr/formalism/planning/merge_datalog.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"

namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;
namespace fd = tyr::formalism::datalog;

namespace tyr::planning
{
namespace ground
{
/**
 * Assume each pre and eff is a set of literals
 *
 * Action = [pre, [<c1_pre,c1_eff>,...,cn_pre,cn_eff>]]
 * App_pre :- pre
 * App_c1_pre :- App_pre and ci_pre    forall i=1,...,n
 * e :- App_ci_pre                     forall i=1,...,n forall e in ci_eff
 */

static auto create_applicability_predicate(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
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

static auto create_applicability_atom(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
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

static auto create_applicability_predicate(View<Index<formalism::planning::Axiom>, formalism::OverlayRepository<formalism::planning::Repository>> axiom,
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

static auto create_applicability_atom(View<Index<formalism::planning::Axiom>, formalism::OverlayRepository<formalism::planning::Repository>> axiom,
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

static void append_from_condition(View<Index<formalism::planning::ConjunctiveCondition>, formalism::OverlayRepository<formalism::planning::Repository>> cond,
                                  formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context,
                                  Data<formalism::datalog::ConjunctiveCondition>& conj_cond)
{
    // Keep negated static atoms because they are monotonic
    for (const auto literal : cond.template get_literals<formalism::StaticTag>())
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

static auto create_applicability_literal(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
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

static auto create_applicability_rule(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
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

static auto create_applicability_literal(View<Index<formalism::planning::Axiom>, formalism::OverlayRepository<formalism::planning::Repository>> axiom,
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

static auto create_applicability_rule(View<Index<formalism::planning::Axiom>, formalism::OverlayRepository<formalism::planning::Repository>> axiom,
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

static auto create_cond_effect_rule(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
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
    for (const auto literal : action.get_condition().get_literals<formalism::StaticTag>())
        conj_cond.static_literals.push_back(merge_p2d(literal, context).first);
    conj_cond.fluent_literals.push_back(create_applicability_literal(action, context).first);

    for (const auto variable : cond_eff.get_variables())
        conj_cond.variables.push_back(merge_p2d(variable, context).first);
    append_from_condition(cond_eff.get_condition(), context, conj_cond);

    canonicalize(conj_cond);
    const auto new_conj_cond = make_view(context.destination.get_or_create(conj_cond, context.builder.get_buffer()).first, context.destination);

    rule.variables = new_conj_cond.get_variables().get_data();
    rule.body = new_conj_cond.get_index();
    rule.head = effect.get_index();

    canonicalize(rule);
    return context.destination.get_or_create(rule, context.builder.get_buffer());
}

static auto create_effect_rule(View<Index<formalism::planning::Axiom>, formalism::OverlayRepository<formalism::planning::Repository>> axiom,
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

static void translate_action_to_delete_free_rules(View<Index<fp::Action>, f::OverlayRepository<fp::Repository>> action,
                                                  Data<fd::Program>& program,
                                                  fp::MergeDatalogContext<fd::Repository>& context,
                                                  GroundTaskProgram::AppPredicateToActionsMapping& predicate_to_actions)
{
    const auto applicability_predicate = create_applicability_predicate(action, context).first;

    [[maybe_unused]] const auto [it, inserted] = predicate_to_actions.emplace(applicability_predicate, action.get_index());
    assert(inserted);

    program.fluent_predicates.push_back(applicability_predicate);

    const auto applicability_rule = create_applicability_rule(action, context).first;

    program.rules.push_back(applicability_rule);

    for (const auto cond_eff : action.get_effects())
    {
        for (const auto literal : cond_eff.get_effect().get_literals())
        {
            if (!literal.get_polarity())
                continue;  /// ignore delete effects

            program.rules.push_back(
                create_cond_effect_rule(action, cond_eff, make_view(fp::merge_p2d(literal.get_atom(), context).first, context.destination), context).first);
        }
    }
}

static void translate_axiom_to_delete_free_axiom_rules(View<Index<fp::Axiom>, f::OverlayRepository<fp::Repository>> axiom,
                                                       Data<fd::Program>& program,
                                                       fp::MergeDatalogContext<fd::Repository>& context,
                                                       GroundTaskProgram::AppPredicateToAxiomsMapping& predicate_to_axioms)
{
    const auto applicability_predicate = create_applicability_predicate(axiom, context).first;

    program.fluent_predicates.push_back(applicability_predicate);

    [[maybe_unused]] const auto [it, inserted] = predicate_to_axioms.emplace(applicability_predicate, axiom.get_index());
    assert(inserted);

    const auto applicability_rule = create_applicability_rule(axiom, context).first;

    program.rules.push_back(applicability_rule);

    program.rules.push_back(
        create_effect_rule(
            axiom,
            make_view(fp::merge_p2d<f::DerivedTag, f::OverlayRepository<fp::Repository>, fd::Repository, f::FluentTag>(axiom.get_head(), context).first,
                      context.destination),
            context)
            .first);
}

static Index<fd::Program> create_program(View<Index<fp::Task>, f::OverlayRepository<fp::Repository>> task,
                                         GroundTaskProgram::AppPredicateToActionsMapping& predicate_to_actions,
                                         GroundTaskProgram::AppPredicateToAxiomsMapping& predicate_to_axioms,
                                         fd::Repository& destination)
{
    auto builder = fd::Builder();
    auto context = fp::MergeDatalogContext<fd::Repository>(builder, destination);
    auto program_ptr = builder.get_builder<fd::Program>();
    auto& program = *program_ptr;
    program.clear();

    for (const auto predicate : task.get_domain().get_predicates<f::StaticTag>())
        program.static_predicates.push_back(fp::merge_p2d(predicate, context).first);

    for (const auto predicate : task.get_domain().get_predicates<f::FluentTag>())
        program.fluent_predicates.push_back(fp::merge_p2d(predicate, context).first);

    for (const auto predicate : task.get_domain().get_predicates<f::DerivedTag>())
        program.fluent_predicates.push_back(
            fp::merge_p2d<f::DerivedTag, f::OverlayRepository<fp::Repository>, fd::Repository, f::FluentTag>(predicate, context).first);

    for (const auto predicate : task.get_derived_predicates())
        program.fluent_predicates.push_back(
            fp::merge_p2d<f::DerivedTag, f::OverlayRepository<fp::Repository>, fd::Repository, f::FluentTag>(predicate, context).first);

    for (const auto function : task.get_domain().get_functions<f::StaticTag>())
        program.static_functions.push_back(fp::merge_p2d(function, context).first);

    for (const auto function : task.get_domain().get_functions<f::FluentTag>())
        program.fluent_functions.push_back(fp::merge_p2d(function, context).first);

    // We can ignore auxiliary function total-cost because it never occurs in a condition

    for (const auto object : task.get_domain().get_constants())
        program.objects.push_back(fp::merge_p2d(object, context).first);
    for (const auto object : task.get_objects())
        program.objects.push_back(fp::merge_p2d(object, context).first);

    for (const auto atom : task.get_atoms<f::StaticTag>())
        program.static_atoms.push_back(fp::merge_p2d(atom, context).first);

    for (const auto atom : task.get_atoms<f::FluentTag>())
        program.fluent_atoms.push_back(fp::merge_p2d(atom, context).first);

    for (const auto fterm_value : task.get_fterm_values<f::StaticTag>())
        program.static_fterm_values.push_back(fp::merge_p2d(fterm_value, context).first);

    for (const auto fterm_value : task.get_fterm_values<f::FluentTag>())
        program.fluent_fterm_values.push_back(fp::merge_p2d(fterm_value, context).first);

    for (const auto action : task.get_domain().get_actions())
        translate_action_to_delete_free_rules(action, program, context, predicate_to_actions);

    for (const auto axiom : task.get_domain().get_axioms())
        translate_axiom_to_delete_free_axiom_rules(axiom, program, context, predicate_to_axioms);

    for (const auto axiom : task.get_axioms())
        translate_axiom_to_delete_free_axiom_rules(axiom, program, context, predicate_to_axioms);

    canonicalize(program);
    return destination.get_or_create(program, builder.get_buffer()).first;
}

static auto create_program_context(View<Index<fp::Task>, f::OverlayRepository<fp::Repository>> task,
                                   GroundTaskProgram::AppPredicateToActionsMapping& predicate_to_actions,
                                   GroundTaskProgram::AppPredicateToAxiomsMapping& predicate_to_axioms)
{
    auto repository = std::make_shared<fd::Repository>();
    auto program = create_program(task, predicate_to_actions, predicate_to_axioms, *repository);
    auto domains = analysis::compute_variable_domains(make_view(program, *repository));
    auto strata = analysis::compute_rule_stratification(make_view(program, *repository));
    auto listeners = analysis::compute_listeners(strata, *repository);

    return datalog::ProgramContext(program, std::move(repository), std::move(domains), std::move(strata), std::move(listeners));
}

}

GroundTaskProgram::GroundTaskProgram(View<Index<fp::Task>, f::OverlayRepository<fp::Repository>> task) :
    m_predicate_to_actions(),
    m_predicate_to_axioms(),
    m_program_context(ground::create_program_context(task, m_predicate_to_actions, m_predicate_to_axioms)),
    m_program_workspace(m_program_context)
{
    // std::cout << m_program_context.get_program() << std::endl;
}

const GroundTaskProgram::AppPredicateToActionsMapping& GroundTaskProgram::get_predicate_to_actions_mapping() const noexcept { return m_predicate_to_actions; }

const GroundTaskProgram::AppPredicateToAxiomsMapping& GroundTaskProgram::get_predicate_to_axioms_mapping() const noexcept { return m_predicate_to_axioms; }

datalog::ProgramContext& GroundTaskProgram::get_program_context() noexcept { return m_program_context; }

const datalog::ProgramContext& GroundTaskProgram::get_program_context() const noexcept { return m_program_context; }

const datalog::ConstProgramWorkspace& GroundTaskProgram::get_const_program_workspace() const noexcept { return m_program_workspace; }

}
