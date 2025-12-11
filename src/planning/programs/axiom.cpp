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

#include "tyr/planning/programs/axiom.hpp"

#include "tyr/formalism/compile.hpp"
#include "tyr/formalism/merge.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/planning/lifted_task.hpp"

namespace tyr::planning
{
static void process_axiom_body(View<Index<formalism::ConjunctiveCondition>, formalism::OverlayRepository<formalism::Repository>> axiom_body,
                               formalism::Builder& builder,
                               formalism::Repository& repository,
                               formalism::MergeCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& merge_cache,
                               formalism::CompileCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& compile_cache,
                               Data<formalism::ConjunctiveCondition>& conj_cond)
{
    for (const auto literal : axiom_body.get_literals<formalism::StaticTag>())
        conj_cond.static_literals.push_back(formalism::merge(literal, builder, repository, merge_cache).get_index());

    for (const auto literal : axiom_body.get_literals<formalism::FluentTag>())
        conj_cond.fluent_literals.push_back(formalism::merge(literal, builder, repository, merge_cache).get_index());

    for (const auto literal : axiom_body.get_literals<formalism::DerivedTag>())
        conj_cond.fluent_literals.push_back(
            formalism::compile<formalism::DerivedTag, formalism::FluentTag>(literal, builder, repository, compile_cache, merge_cache).get_index());

    for (const auto numeric_constraint : axiom_body.get_numeric_constraints())
        conj_cond.numeric_constraints.push_back(formalism::merge(numeric_constraint, builder, repository, merge_cache).get_data());

    for (const auto literal : axiom_body.get_nullary_literals<formalism::StaticTag>())
        conj_cond.static_nullary_literals.push_back(formalism::merge(literal, builder, repository, merge_cache).get_index());

    for (const auto literal : axiom_body.get_nullary_literals<formalism::FluentTag>())
        conj_cond.fluent_nullary_literals.push_back(formalism::merge(literal, builder, repository, merge_cache).get_index());

    for (const auto literal : axiom_body.get_nullary_literals<formalism::DerivedTag>())
        conj_cond.fluent_nullary_literals.push_back(
            formalism::compile<formalism::DerivedTag, formalism::FluentTag>(literal, builder, repository, compile_cache, merge_cache).get_index());

    for (const auto numeric_constraint : axiom_body.get_nullary_numeric_constraints())
        conj_cond.nullary_numeric_constraints.push_back(formalism::merge(numeric_constraint, builder, repository, merge_cache).get_data());
}

View<Index<formalism::Rule>, formalism::Repository> static create_axiom_rule(
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

    process_axiom_body(axiom.get_body(), builder, repository, merge_cache, compile_cache, conj_cond);

    formalism::canonicalize(conj_cond);
    const auto new_conj_cond = repository.get_or_create(conj_cond, builder.get_buffer()).first;

    rule.body = new_conj_cond.get_index();

    const auto new_head = formalism::compile<formalism::DerivedTag, formalism::FluentTag>(axiom.get_head(), builder, repository, compile_cache, merge_cache);

    rule.head = new_head.get_index();

    formalism::canonicalize(rule);
    return repository.get_or_create(rule, builder.get_buffer()).first;
}

static View<Index<formalism::Program>, formalism::Repository>
create(const LiftedTask& task, AxiomEvaluatorProgram::PredicateToPredicateMapping& predicate_to_predicate_mapping, formalism::Repository& repository)
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
    {
        const auto new_predicate = formalism::compile<formalism::DerivedTag, formalism::FluentTag>(predicate, builder, repository, compile_cache, merge_cache);

        predicate_to_predicate_mapping.emplace(new_predicate, predicate);

        program.fluent_predicates.push_back(new_predicate.get_index());
    }

    for (const auto predicate : task.get_task().get_derived_predicates())
    {
        const auto new_predicate = formalism::compile<formalism::DerivedTag, formalism::FluentTag>(predicate, builder, repository, compile_cache, merge_cache);

        predicate_to_predicate_mapping.emplace(new_predicate, predicate);

        program.fluent_predicates.push_back(new_predicate.get_index());
    }

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

    for (const auto axiom : task.get_task().get_domain().get_axioms())
        program.rules.push_back(create_axiom_rule(axiom, builder, repository, merge_cache, compile_cache).get_index());

    for (const auto axiom : task.get_task().get_axioms())
        program.rules.push_back(create_axiom_rule(axiom, builder, repository, merge_cache, compile_cache).get_index());

    formalism::canonicalize(program);
    return repository.get_or_create(program, builder.get_buffer()).first;
}

AxiomEvaluatorProgram::AxiomEvaluatorProgram(const LiftedTask& task) :
    m_prediate_to_predicate(),
    m_repository(std::make_shared<formalism::Repository>()),
    m_program(create(task, m_prediate_to_predicate, *m_repository))
{
}

const AxiomEvaluatorProgram::PredicateToPredicateMapping& AxiomEvaluatorProgram::get_predicate_to_predicate_mapping() const { return m_prediate_to_predicate; }

View<Index<formalism::Program>, formalism::Repository> AxiomEvaluatorProgram::get_program() const { return m_program; }

const formalism::RepositoryPtr& AxiomEvaluatorProgram::get_repository() const { return m_repository; }

}
