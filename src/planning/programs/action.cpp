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

namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;
namespace fd = tyr::formalism::datalog;

namespace tyr::planning
{

static Index<fd::Program> create_program(View<Index<fp::Task>, f::OverlayRepository<fp::Repository>> task,
                                         ApplicableActionProgram::AppPredicateToActionsMapping& predicate_to_actions,
                                         fd::Repository& repository)
{
    auto merge_cache = fp::MergeDatalogCache();
    auto builder = fd::Builder();
    auto context = fp::MergeDatalogContext(builder, repository, merge_cache);
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

    for (const auto action : task.get_domain().get_actions())
    {
        const auto applicability_predicate = create_applicability_predicate(action, context).first;

        [[maybe_unused]] const auto [it, inserted] = predicate_to_actions.emplace(applicability_predicate, action.get_index());
        assert(inserted);

        program.fluent_predicates.push_back(applicability_predicate);

        auto rule_ptr = builder.get_builder<fd::Rule>();
        auto& rule = *rule_ptr;
        rule.clear();

        auto conj_cond_ptr = builder.get_builder<fd::ConjunctiveCondition>();
        auto& conj_cond = *conj_cond_ptr;
        conj_cond.clear();

        for (const auto variable : action.get_variables())
            rule.variables.push_back(fp::merge_p2d(variable, context).first);

        for (const auto variable : action.get_condition().get_variables())
            conj_cond.variables.push_back(fp::merge_p2d(variable, context).first);

        for (const auto literal : action.get_condition().get_literals<f::StaticTag>())
            conj_cond.static_literals.push_back(fp::merge_p2d(literal, context).first);

        for (const auto literal : action.get_condition().get_literals<f::FluentTag>())
            conj_cond.fluent_literals.push_back(fp::merge_p2d(literal, context).first);

        for (const auto literal : action.get_condition().get_literals<f::DerivedTag>())
            conj_cond.fluent_literals.push_back(
                fp::merge_p2d<f::DerivedTag, f::OverlayRepository<fp::Repository>, fd::Repository, f::FluentTag>(literal, context).first);

        for (const auto numeric_constraint : action.get_condition().get_numeric_constraints())
            conj_cond.numeric_constraints.push_back(fp::merge_p2d(numeric_constraint, context));

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
    return repository.get_or_create(program, builder.get_buffer()).first;
}

static auto create_program_context(View<Index<fp::Task>, f::OverlayRepository<fp::Repository>> task,
                                   ApplicableActionProgram::AppPredicateToActionsMapping& mapping)
{
    auto repository = std::make_shared<fd::Repository>();
    auto program = create_program(task, mapping, *repository);
    auto domains = analysis::compute_variable_domains(make_view(program, *repository));
    auto strata = analysis::compute_rule_stratification(make_view(program, *repository));
    auto listeners = analysis::compute_listeners(strata, *repository);

    return datalog::ProgramContext(program, std::move(repository), std::move(domains), std::move(strata), std::move(listeners));
}

ApplicableActionProgram::ApplicableActionProgram(View<Index<fp::Task>, f::OverlayRepository<fp::Repository>> task) :
    m_predicate_to_actions(),
    m_program_context(create_program_context(task, m_predicate_to_actions)),
    m_program_workspace(m_program_context)
{
    // std::cout << m_program_context.get_program() << std::endl;
}

const ApplicableActionProgram::AppPredicateToActionsMapping& ApplicableActionProgram::get_predicate_to_actions_mapping() const noexcept
{
    return m_predicate_to_actions;
}

datalog::ProgramContext& ApplicableActionProgram::get_program_context() noexcept { return m_program_context; }

const datalog::ProgramContext& ApplicableActionProgram::get_program_context() const noexcept { return m_program_context; }

const datalog::ConstProgramWorkspace& ApplicableActionProgram::get_const_program_workspace() const noexcept { return m_program_workspace; }
}
