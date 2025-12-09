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

static View<Index<formalism::Program>, formalism::Repository> create(const LiftedTask& task,
                                                                     ApplicableActionProgram::ObjectToObjectMapping& object_to_object_mapping,
                                                                     ApplicableActionProgram::RuleToActionsMapping& rule_to_actions_mapping,
                                                                     formalism::Repository& repository)
{
    auto merge_cache = formalism::MergeCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>();
    auto compile_cache = formalism::CompileCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>();
    auto builder = formalism::Builder();
    auto program_ptr = builder.get_builder<formalism::Program>();
    auto& program = *program_ptr;
    program.clear();

    for (const auto predicate : task.get_task().get_domain().get_predicates<formalism::StaticTag>())
    {
        program.static_predicates.push_back(formalism::merge(predicate, builder, repository, merge_cache).get_index());
    }

    for (const auto predicate : task.get_task().get_domain().get_predicates<formalism::FluentTag>())
    {
        program.fluent_predicates.push_back(formalism::merge(predicate, builder, repository, merge_cache).get_index());
    }

    for (const auto predicate : task.get_task().get_domain().get_predicates<formalism::DerivedTag>())
    {
        program.fluent_predicates.push_back(
            formalism::compile<formalism::DerivedTag, formalism::FluentTag>(predicate, builder, repository, compile_cache, merge_cache).get_index());
    }

    for (const auto predicate : task.get_task().get_derived_predicates())
    {
        program.fluent_predicates.push_back(
            formalism::compile<formalism::DerivedTag, formalism::FluentTag>(predicate, builder, repository, compile_cache, merge_cache).get_index());
    }

    for (const auto function : task.get_task().get_domain().get_functions<formalism::StaticTag>())
    {
        program.static_functions.push_back(formalism::merge(function, builder, repository, merge_cache).get_index());
    }

    for (const auto function : task.get_task().get_domain().get_functions<formalism::FluentTag>())
    {
        program.fluent_functions.push_back(formalism::merge(function, builder, repository, merge_cache).get_index());
    }

    // We can ignore auxiliary function total-cost because it never occurs in a condition

    for (const auto object : task.get_task().get_domain().get_constants())
    {
        const auto new_object = formalism::merge(object, builder, repository, merge_cache);

        object_to_object_mapping.emplace(new_object, object);

        program.objects.push_back(new_object.get_index());
    }
    for (const auto object : task.get_task().get_objects())
    {
        const auto new_object = formalism::merge(object, builder, repository, merge_cache);

        object_to_object_mapping.emplace(new_object, object);

        program.objects.push_back(new_object.get_index());
    }

    for (const auto atom : task.get_task().get_atoms<formalism::StaticTag>())
    {
        program.static_atoms.push_back(formalism::merge(atom, builder, repository, merge_cache).get_index());
    }

    for (const auto atom : task.get_task().get_atoms<formalism::FluentTag>())
    {
        program.fluent_atoms.push_back(formalism::merge(atom, builder, repository, merge_cache).get_index());
    }

    for (const auto fterm_value : task.get_task().get_fterm_values<formalism::StaticTag>())
    {
        program.static_fterm_values.push_back(formalism::merge(fterm_value, builder, repository, merge_cache).get_index());
    }

    for (const auto action : task.get_task().get_domain().get_actions())
    {
        auto predicate_ptr = builder.get_builder<formalism::Predicate<formalism::FluentTag>>();
        auto& predicate = *predicate_ptr;
        predicate.clear();

        predicate.name = action.get_name();
        predicate.arity = action.get_arity();

        formalism::canonicalize(predicate);
        const auto new_predicate = repository.get_or_create(predicate, builder.get_buffer()).first;

        if (std::find(program.fluent_predicates.begin(), program.fluent_predicates.end(), new_predicate.get_index()) == program.fluent_predicates.end())
            program.fluent_predicates.push_back(new_predicate.get_index());

        auto rule_ptr = builder.get_builder<formalism::Rule>();
        auto& rule = *rule_ptr;
        rule.clear();

        auto conj_cond_ptr = builder.get_builder<formalism::ConjunctiveCondition>();
        auto& conj_cond = *conj_cond_ptr;
        conj_cond.clear();

        for (const auto variable : action.get_variables())
            conj_cond.variables.push_back(formalism::merge(variable, builder, repository, merge_cache).get_index());

        for (const auto literal : action.get_condition().get_literals<formalism::StaticTag>())
        {
            conj_cond.static_literals.push_back(formalism::merge(literal, builder, repository, merge_cache).get_index());
        }

        for (const auto literal : action.get_condition().get_literals<formalism::FluentTag>())
        {
            conj_cond.fluent_literals.push_back(formalism::merge(literal, builder, repository, merge_cache).get_index());
        }

        for (const auto literal : action.get_condition().get_literals<formalism::DerivedTag>())
        {
            conj_cond.fluent_literals.push_back(
                formalism::compile<formalism::DerivedTag, formalism::FluentTag>(literal, builder, repository, compile_cache, merge_cache).get_index());
        }

        for (const auto numeric_constraint : action.get_condition().get_numeric_constraints())
        {
            conj_cond.numeric_constraints.push_back(formalism::merge(numeric_constraint, builder, repository, merge_cache).get_data());
        }

        for (const auto literal : action.get_condition().get_nullary_literals<formalism::StaticTag>())
        {
            conj_cond.static_nullary_literals.push_back(formalism::merge(literal, builder, repository, merge_cache).get_index());
        }

        for (const auto literal : action.get_condition().get_nullary_literals<formalism::FluentTag>())
        {
            conj_cond.fluent_nullary_literals.push_back(formalism::merge(literal, builder, repository, merge_cache).get_index());
        }

        for (const auto literal : action.get_condition().get_nullary_literals<formalism::DerivedTag>())
        {
            conj_cond.fluent_nullary_literals.push_back(
                formalism::compile<formalism::DerivedTag, formalism::FluentTag>(literal, builder, repository, compile_cache, merge_cache).get_index());
        }

        for (const auto numeric_constraint : action.get_condition().get_nullary_numeric_constraints())
        {
            conj_cond.nullary_numeric_constraints.push_back(formalism::merge(numeric_constraint, builder, repository, merge_cache).get_data());
        }

        formalism::canonicalize(conj_cond);
        const auto new_conj_cond = repository.get_or_create(conj_cond, builder.get_buffer()).first;

        auto atom_ptr = builder.get_builder<formalism::Atom<formalism::FluentTag>>();
        auto& atom = *atom_ptr;
        atom.clear();

        atom.predicate = new_predicate.get_index();
        for (uint_t i = 0; i < action.get_arity(); ++i)
            atom.terms.push_back(Data<formalism::Term>(formalism::ParameterIndex(i)));

        formalism::canonicalize(atom);
        const auto new_head = repository.get_or_create(atom, builder.get_buffer()).first;

        rule.body = new_conj_cond.get_index();
        rule.head = new_head.get_index();

        formalism::canonicalize(rule);
        const auto new_rule = repository.get_or_create(rule, builder.get_buffer()).first;

        rule_to_actions_mapping[new_rule].emplace_back(action);

        program.rules.push_back(new_rule.get_index());
    }

    formalism::canonicalize(program);
    return repository.get_or_create(program, builder.get_buffer()).first;
}

GroundTaskProgram::GroundTaskProgram(const LiftedTask& task) :
    m_rule_to_actions(),
    m_rule_to_axioms(),
    m_object_to_object(),
    m_repository(std::make_shared<formalism::Repository>()),
    m_program(create(task, m_object_to_object, m_rule_to_actions, *m_repository))
{
}

const GroundTaskProgram::RuleToActionsMapping& GroundTaskProgram::get_rule_to_actions_mapping() const noexcept { return m_rule_to_actions; }

const GroundTaskProgram::RuleToAxiomsMapping& GroundTaskProgram::get_rule_to_axioms_mapping() const noexcept { return m_rule_to_axioms; }

const GroundTaskProgram::ObjectToObjectMapping& GroundTaskProgram::get_object_to_object_mapping() const noexcept { return m_object_to_object; }

View<Index<formalism::Program>, formalism::Repository> GroundTaskProgram::get_program() const noexcept { return m_program; }

const formalism::RepositoryPtr& GroundTaskProgram::get_repository() const noexcept { return m_repository; }

}
