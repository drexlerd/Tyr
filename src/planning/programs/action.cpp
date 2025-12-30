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

#include "tyr/planning/programs/action.hpp"

#include "common.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/planning/formatter.hpp"
#include "tyr/formalism/planning/merge.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"

using namespace tyr::formalism;

namespace tyr::planning
{

static View<Index<formalism::datalog::Program>, formalism::datalog::Repository>
create(View<Index<formalism::planning::Task>, formalism::OverlayRepository<formalism::planning::Repository>> task,
       ApplicableActionProgram::AppPredicateToActionsMapping& predicate_to_actions_mapping,
       formalism::datalog::Repository& repository)
{
    auto merge_cache = formalism::planning::MergeDatalogCache();
    auto builder = formalism::datalog::Builder();
    auto context = formalism::planning::MergeDatalogContext(builder, repository, merge_cache);
    auto program_ptr = builder.get_builder<formalism::datalog::Program>();
    auto& program = *program_ptr;
    program.clear();

    for (const auto predicate : task.get_domain().get_predicates<StaticTag>())
        program.static_predicates.push_back(merge_p2d(predicate, context).first);

    for (const auto predicate : task.get_domain().get_predicates<FluentTag>())
        program.fluent_predicates.push_back(merge_p2d(predicate, context).first);

    for (const auto predicate : task.get_domain().get_predicates<DerivedTag>())
        program.fluent_predicates.push_back(
            merge_p2d<DerivedTag, OverlayRepository<formalism::planning::Repository>, formalism::datalog::Repository, FluentTag>(predicate, context).first);

    for (const auto predicate : task.get_derived_predicates())
        program.fluent_predicates.push_back(
            merge_p2d<DerivedTag, OverlayRepository<formalism::planning::Repository>, formalism::datalog::Repository, FluentTag>(predicate, context).first);

    for (const auto function : task.get_domain().get_functions<StaticTag>())
        program.static_functions.push_back(merge_p2d(function, context).first);

    for (const auto function : task.get_domain().get_functions<FluentTag>())
        program.fluent_functions.push_back(merge_p2d(function, context).first);

    // We can ignore auxiliary function total-cost because it never occurs in a condition

    for (const auto object : task.get_domain().get_constants())
        program.objects.push_back(merge_p2d(object, context).first);
    for (const auto object : task.get_objects())
        program.objects.push_back(merge_p2d(object, context).first);

    for (const auto atom : task.get_atoms<StaticTag>())
        program.static_atoms.push_back(merge_p2d(atom, context).first);

    for (const auto atom : task.get_atoms<FluentTag>())
        program.fluent_atoms.push_back(merge_p2d(atom, context).first);

    for (const auto fterm_value : task.get_fterm_values<StaticTag>())
        program.static_fterm_values.push_back(merge_p2d(fterm_value, context).first);

    for (const auto action : task.get_domain().get_actions())
    {
        const auto applicability_predicate = create_applicability_predicate(action, context).first;

        predicate_to_actions_mapping[applicability_predicate].emplace_back(action.get_index());

        program.fluent_predicates.push_back(applicability_predicate);

        auto rule_ptr = builder.get_builder<formalism::datalog::Rule>();
        auto& rule = *rule_ptr;
        rule.clear();

        auto conj_cond_ptr = builder.get_builder<formalism::datalog::ConjunctiveCondition>();
        auto& conj_cond = *conj_cond_ptr;
        conj_cond.clear();

        for (const auto variable : action.get_variables())
            rule.variables.push_back(merge_p2d(variable, context).first);

        for (const auto variable : action.get_condition().get_variables())
            conj_cond.variables.push_back(merge_p2d(variable, context).first);

        for (const auto literal : action.get_condition().get_literals<StaticTag>())
            conj_cond.static_literals.push_back(merge_p2d(literal, context).first);

        for (const auto literal : action.get_condition().get_literals<FluentTag>())
            conj_cond.fluent_literals.push_back(merge_p2d(literal, context).first);

        for (const auto literal : action.get_condition().get_literals<DerivedTag>())
            conj_cond.fluent_literals.push_back(
                merge_p2d<DerivedTag, OverlayRepository<formalism::planning::Repository>, formalism::datalog::Repository, FluentTag>(literal, context).first);

        for (const auto numeric_constraint : action.get_condition().get_numeric_constraints())
            conj_cond.numeric_constraints.push_back(merge_p2d(numeric_constraint, context));

        canonicalize(conj_cond);
        const auto new_conj_cond = repository.get_or_create(conj_cond, builder.get_buffer()).first;

        rule.body = new_conj_cond;

        const auto applicability_atom = create_applicability_atom(action, context).first;

        rule.head = applicability_atom;

        canonicalize(rule);
        const auto new_rule = repository.get_or_create(rule, builder.get_buffer()).first;

        program.rules.push_back(new_rule);
    }

    canonicalize(program);
    return make_view(repository.get_or_create(program, builder.get_buffer()).first, repository);
}

ApplicableActionProgram::ApplicableActionProgram(View<Index<formalism::planning::Task>, OverlayRepository<formalism::planning::Repository>> task) :
    m_predicate_to_actions(),
    m_repository(std::make_shared<formalism::datalog::Repository>()),
    m_program(create(task, m_predicate_to_actions, *m_repository)),
    m_domains(analysis::compute_variable_domains(m_program)),
    m_strata(analysis::compute_rule_stratification(m_program)),
    m_listeners(analysis::compute_listeners(m_strata, m_program.get_context()))
{
    // std::cout << m_program << std::endl;
}

const ApplicableActionProgram::AppPredicateToActionsMapping& ApplicableActionProgram::get_predicate_to_actions_mapping() const noexcept
{
    return m_predicate_to_actions;
}

View<Index<formalism::datalog::Program>, formalism::datalog::Repository> ApplicableActionProgram::get_program() const noexcept { return m_program; }

const formalism::datalog::RepositoryPtr& ApplicableActionProgram::get_repository() const noexcept { return m_repository; }

const analysis::ProgramVariableDomains& ApplicableActionProgram::get_domains() const noexcept { return m_domains; }

const analysis::RuleStrata& ApplicableActionProgram::get_strata() const noexcept { return m_strata; }

const analysis::ListenerStrata& ApplicableActionProgram::get_listeners() const noexcept { return m_listeners; }

}
