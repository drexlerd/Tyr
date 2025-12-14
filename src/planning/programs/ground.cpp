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

#include "tyr/formalism/compile.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/merge.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/planning/lifted_task.hpp"

namespace tyr::planning
{

void append_from_condition(View<Index<formalism::ConjunctiveCondition>, formalism::OverlayRepository<formalism::Repository>> cond,
                           formalism::Builder& builder,
                           formalism::Repository& repository,
                           formalism::MergeCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& merge_cache,
                           formalism::CompileCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& compile_cache,
                           Data<formalism::ConjunctiveCondition>& conj_cond)
{
    for (const auto literal : cond.template get_literals<formalism::StaticTag>())
        if (literal.get_polarity())
            conj_cond.static_literals.push_back(formalism::merge(literal, builder, repository, merge_cache).get_index());

    for (const auto literal : cond.template get_literals<formalism::FluentTag>())
        if (literal.get_polarity())
            conj_cond.fluent_literals.push_back(formalism::merge(literal, builder, repository, merge_cache).get_index());

    for (const auto literal : cond.template get_literals<formalism::DerivedTag>())
        if (literal.get_polarity())
            conj_cond.fluent_literals.push_back(
                formalism::compile<formalism::DerivedTag, formalism::FluentTag>(literal, builder, repository, compile_cache, merge_cache).get_index());

    for (const auto literal : cond.template get_nullary_literals<formalism::StaticTag>())
        if (literal.get_polarity())
            conj_cond.static_nullary_literals.push_back(formalism::merge(literal, builder, repository, merge_cache).get_index());

    for (const auto literal : cond.template get_nullary_literals<formalism::FluentTag>())
        if (literal.get_polarity())
            conj_cond.fluent_nullary_literals.push_back(formalism::merge(literal, builder, repository, merge_cache).get_index());

    for (const auto literal : cond.template get_nullary_literals<formalism::DerivedTag>())
        if (literal.get_polarity())
            conj_cond.fluent_nullary_literals.push_back(
                formalism::compile<formalism::DerivedTag, formalism::FluentTag>(literal, builder, repository, compile_cache, merge_cache).get_index());
};

static View<Index<formalism::ConjunctiveCondition>, formalism::Repository>
make_delete_free_body(View<Index<formalism::Action>, formalism::OverlayRepository<formalism::Repository>> action,
                      formalism::Builder& builder,
                      formalism::Repository& repository,
                      formalism::MergeCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& merge_cache,
                      formalism::CompileCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& compile_cache)
{
    auto conj_cond_ptr = builder.get_builder<formalism::ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto variable : action.get_variables())
        conj_cond.variables.push_back(formalism::merge(variable, builder, repository, merge_cache).get_index());

    append_from_condition(action.get_condition(), builder, repository, merge_cache, compile_cache, conj_cond);

    formalism::canonicalize(conj_cond);
    return repository.get_or_create(conj_cond, builder.get_buffer()).first;
}

static View<Index<formalism::ConjunctiveCondition>, formalism::Repository>
make_delete_free_body(View<Index<formalism::Action>, formalism::OverlayRepository<formalism::Repository>> action,
                      View<Index<formalism::ConditionalEffect>, formalism::OverlayRepository<formalism::Repository>> cond_eff,
                      formalism::Builder& builder,
                      formalism::Repository& repository,
                      formalism::MergeCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& merge_cache,
                      formalism::CompileCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& compile_cache)
{
    auto conj_cond_ptr = builder.get_builder<formalism::ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto variable : action.get_variables())
        conj_cond.variables.push_back(formalism::merge(variable, builder, repository, merge_cache).get_index());
    for (const auto variable : cond_eff.get_condition().get_variables())
        conj_cond.variables.push_back(formalism::merge(variable, builder, repository, merge_cache).get_index());

    append_from_condition(action.get_condition(), builder, repository, merge_cache, compile_cache, conj_cond);
    append_from_condition(cond_eff.get_condition(), builder, repository, merge_cache, compile_cache, conj_cond);

    formalism::canonicalize(conj_cond);
    return repository.get_or_create(conj_cond, builder.get_buffer()).first;
}

static void
translate_action_to_delete_free_rules(View<Index<formalism::Action>, formalism::OverlayRepository<formalism::Repository>> action,
                                      Data<formalism::Program>& program,
                                      formalism::Repository& repository,
                                      formalism::Builder& builder,
                                      formalism::MergeCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& merge_cache,
                                      formalism::CompileCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& compile_cache,
                                      GroundTaskProgram::RuleToActionsMapping& rule_to_actions_mapping)
{
    // Check whether we need special instantiation
    auto instantiated_through_literals = std::any_of(action.get_effects().begin(),
                                                     action.get_effects().end(),
                                                     [&](auto&& cond_eff)
                                                     {
                                                         return std::any_of(cond_eff.get_effect().get_literals().begin(),
                                                                            cond_eff.get_effect().get_literals().end(),
                                                                            [](auto&& literal) { return literal.get_polarity(); });
                                                     });

    if (instantiated_through_literals)
        for (const auto cond_eff : action.get_effects())
        {
            // 1) Build shared body: Pre(a) ∧ Cond(ce)
            auto body = make_delete_free_body(action, cond_eff, builder, repository, merge_cache, compile_cache);

            // 2) For each positive fluent literal in ce.effect, create a rule:
            //    Body -> head_atom
            for (const auto literal : cond_eff.get_effect().get_literals())
            {
                if (!literal.get_polarity())
                    continue;  /// ignore delete effects

                auto rule_ptr = builder.get_builder<formalism::Rule>();
                auto& rule = *rule_ptr;
                rule.clear();

                rule.body = body.get_index();
                rule.head = formalism::merge(literal.get_atom(), builder, repository, merge_cache).get_index();

                formalism::canonicalize(rule);
                auto new_rule = repository.get_or_create(rule, builder.get_buffer()).first;

                rule_to_actions_mapping[new_rule].emplace_back(action);

                program.rules.push_back(new_rule.get_index());
            }
        }
    else
    {
        auto predicate_ptr = builder.get_builder<formalism::Predicate<formalism::FluentTag>>();
        auto& predicate = *predicate_ptr;
        predicate.clear();

        predicate.name = ::cista::offset::string { std::string { "@" } + action.get_name().str() };
        predicate.arity = action.get_arity();

        formalism::canonicalize(predicate);
        const auto new_predicate = repository.get_or_create(predicate, builder.get_buffer()).first;
        program.fluent_predicates.push_back(new_predicate.get_index());

        auto atom_ptr = builder.get_builder<formalism::Atom<formalism::FluentTag>>();
        auto& atom = *atom_ptr;
        atom.clear();

        atom.predicate = new_predicate.get_index();
        for (uint_t i = 0; i < action.get_arity(); ++i)
            atom.terms.push_back(Data<formalism::Term>(formalism::ParameterIndex(i)));

        formalism::canonicalize(atom);
        const auto new_atom = repository.get_or_create(atom, builder.get_buffer()).first;

        auto body = make_delete_free_body(action, builder, repository, merge_cache, compile_cache);

        auto rule_ptr = builder.get_builder<formalism::Rule>();
        auto& rule = *rule_ptr;
        rule.clear();

        rule.body = body.get_index();
        rule.head = new_atom.get_index();

        formalism::canonicalize(rule);
        auto new_rule = repository.get_or_create(rule, builder.get_buffer()).first;

        rule_to_actions_mapping[new_rule].emplace_back(action);

        program.rules.push_back(new_rule.get_index());
    }
}

static void process_delete_free_axiom_body(View<Index<formalism::ConjunctiveCondition>, formalism::OverlayRepository<formalism::Repository>> axiom_body,
                                           formalism::Builder& builder,
                                           formalism::Repository& repository,
                                           formalism::MergeCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& merge_cache,
                                           formalism::CompileCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& compile_cache,
                                           Data<formalism::ConjunctiveCondition>& conj_cond)
{
    for (const auto literal : axiom_body.get_literals<formalism::StaticTag>())
        if (literal.get_polarity())
            conj_cond.static_literals.push_back(formalism::merge(literal, builder, repository, merge_cache).get_index());

    for (const auto literal : axiom_body.get_literals<formalism::FluentTag>())
        if (literal.get_polarity())
            conj_cond.fluent_literals.push_back(formalism::merge(literal, builder, repository, merge_cache).get_index());

    for (const auto literal : axiom_body.get_literals<formalism::DerivedTag>())
        if (literal.get_polarity())
            conj_cond.fluent_literals.push_back(
                formalism::compile<formalism::DerivedTag, formalism::FluentTag>(literal, builder, repository, compile_cache, merge_cache).get_index());

    for (const auto literal : axiom_body.get_nullary_literals<formalism::StaticTag>())
        if (literal.get_polarity())
            conj_cond.static_nullary_literals.push_back(formalism::merge(literal, builder, repository, merge_cache).get_index());

    for (const auto literal : axiom_body.get_nullary_literals<formalism::FluentTag>())
        if (literal.get_polarity())
            conj_cond.fluent_nullary_literals.push_back(formalism::merge(literal, builder, repository, merge_cache).get_index());

    for (const auto literal : axiom_body.get_nullary_literals<formalism::DerivedTag>())
        if (literal.get_polarity())
            conj_cond.fluent_nullary_literals.push_back(
                formalism::compile<formalism::DerivedTag, formalism::FluentTag>(literal, builder, repository, compile_cache, merge_cache).get_index());
}

View<Index<formalism::Rule>, formalism::Repository> static create_delete_free_axiom_rule(
    View<Index<formalism::Axiom>, formalism::OverlayRepository<formalism::Repository>> axiom,
    formalism::Builder& builder,
    formalism::Repository& repository,
    formalism::MergeCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& merge_cache,
    formalism::CompileCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& compile_cache)
{
    auto rule_ptr = builder.get_builder<formalism::Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    auto conj_cond_ptr = builder.get_builder<formalism::ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto variable : axiom.get_variables())
        conj_cond.variables.push_back(formalism::merge(variable, builder, repository, merge_cache).get_index());

    process_delete_free_axiom_body(axiom.get_body(), builder, repository, merge_cache, compile_cache, conj_cond);

    formalism::canonicalize(conj_cond);
    const auto new_conj_cond = repository.get_or_create(conj_cond, builder.get_buffer()).first;

    rule.body = new_conj_cond.get_index();

    const auto new_head = formalism::compile<formalism::DerivedTag, formalism::FluentTag>(axiom.get_head(), builder, repository, compile_cache, merge_cache);

    rule.head = new_head.get_index();

    formalism::canonicalize(rule);
    return repository.get_or_create(rule, builder.get_buffer()).first;
}

static View<Index<formalism::Program>, formalism::Repository> create(const LiftedTask& task,
                                                                     GroundTaskProgram::RuleToActionsMapping& rule_to_actions_mapping,
                                                                     GroundTaskProgram::RuleToAxiomsMapping& rule_to_axioms_mapping,
                                                                     formalism::Repository& repository)
{
    auto merge_cache = formalism::MergeCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>();
    auto compile_cache = formalism::CompileCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>();
    auto builder = formalism::Builder();
    auto program_ptr = builder.get_builder<formalism::Program>();
    auto& program = *program_ptr;
    program.clear();

    for (const auto predicate : task.get_task().get_domain().get_predicates<formalism::StaticTag>())
        program.static_predicates.push_back(formalism::merge(predicate, builder, repository, merge_cache).get_index());

    for (const auto predicate : task.get_task().get_domain().get_predicates<formalism::FluentTag>())
        program.fluent_predicates.push_back(formalism::merge(predicate, builder, repository, merge_cache).get_index());

    for (const auto predicate : task.get_task().get_domain().get_predicates<formalism::DerivedTag>())
        program.fluent_predicates.push_back(
            formalism::compile<formalism::DerivedTag, formalism::FluentTag>(predicate, builder, repository, compile_cache, merge_cache).get_index());

    for (const auto predicate : task.get_task().get_derived_predicates())
        program.fluent_predicates.push_back(
            formalism::compile<formalism::DerivedTag, formalism::FluentTag>(predicate, builder, repository, compile_cache, merge_cache).get_index());

    for (const auto function : task.get_task().get_domain().get_functions<formalism::StaticTag>())
        program.static_functions.push_back(formalism::merge(function, builder, repository, merge_cache).get_index());

    for (const auto function : task.get_task().get_domain().get_functions<formalism::FluentTag>())
        program.fluent_functions.push_back(formalism::merge(function, builder, repository, merge_cache).get_index());

    // We can ignore auxiliary function total-cost because it never occurs in a condition

    for (const auto object : task.get_task().get_domain().get_constants())
        program.objects.push_back(formalism::merge(object, builder, repository, merge_cache).get_index());
    for (const auto object : task.get_task().get_objects())
        program.objects.push_back(formalism::merge(object, builder, repository, merge_cache).get_index());

    for (const auto atom : task.get_task().get_atoms<formalism::StaticTag>())
        program.static_atoms.push_back(formalism::merge(atom, builder, repository, merge_cache).get_index());

    for (const auto atom : task.get_task().get_atoms<formalism::FluentTag>())
        program.fluent_atoms.push_back(formalism::merge(atom, builder, repository, merge_cache).get_index());

    for (const auto fterm_value : task.get_task().get_fterm_values<formalism::StaticTag>())
        program.static_fterm_values.push_back(formalism::merge(fterm_value, builder, repository, merge_cache).get_index());

    for (const auto action : task.get_task().get_domain().get_actions())
        translate_action_to_delete_free_rules(action, program, repository, builder, merge_cache, compile_cache, rule_to_actions_mapping);

    for (const auto axiom : task.get_task().get_domain().get_axioms())
    {
        auto new_rule = create_delete_free_axiom_rule(axiom, builder, repository, merge_cache, compile_cache);
        rule_to_axioms_mapping[new_rule].emplace_back(axiom);
        program.rules.push_back(new_rule.get_index());
    }

    for (const auto axiom : task.get_task().get_axioms())
    {
        auto new_rule = create_delete_free_axiom_rule(axiom, builder, repository, merge_cache, compile_cache);
        rule_to_axioms_mapping[new_rule].emplace_back(axiom);
        program.rules.push_back(new_rule.get_index());
    }

    formalism::canonicalize(program);
    return repository.get_or_create(program, builder.get_buffer()).first;
}

GroundTaskProgram::GroundTaskProgram(const LiftedTask& task) :
    m_rule_to_actions(),
    m_rule_to_axioms(),
    m_repository(std::make_shared<formalism::Repository>()),
    m_program(create(task, m_rule_to_actions, m_rule_to_axioms, *m_repository)),
    m_domains(analysis::compute_variable_domains(m_program)),
    m_strata(analysis::compute_rule_stratification(m_program)),
    m_listeners(analysis::compute_listeners(m_strata))
{
}

const GroundTaskProgram::RuleToActionsMapping& GroundTaskProgram::get_rule_to_actions_mapping() const noexcept { return m_rule_to_actions; }

const GroundTaskProgram::RuleToAxiomsMapping& GroundTaskProgram::get_rule_to_axioms_mapping() const noexcept { return m_rule_to_axioms; }

View<Index<formalism::Program>, formalism::Repository> GroundTaskProgram::get_program() const noexcept { return m_program; }

const formalism::RepositoryPtr& GroundTaskProgram::get_repository() const noexcept { return m_repository; }

const analysis::ProgramVariableDomains& GroundTaskProgram::get_domains() const noexcept { return m_domains; }

const analysis::RuleStrata& GroundTaskProgram::get_strata() const noexcept { return m_strata; }

const analysis::Listeners& GroundTaskProgram::get_listeners() const noexcept { return m_listeners; }

}
