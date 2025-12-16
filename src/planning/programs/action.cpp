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

#include "tyr/formalism/merge_datalog.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/planning/lifted_task.hpp"

using namespace tyr::formalism;

namespace tyr::planning
{
static View<Index<Program>, Repository>
create(const LiftedTask& task, ApplicableActionProgram::RuleToActionsMapping& rule_to_actions_mapping, Repository& repository)
{
    auto merge_cache = MergeCache<OverlayRepository<Repository>, Repository>();
    auto builder = Builder();
    auto context = MergeContext<OverlayRepository<Repository>, Repository>(builder, repository, merge_cache);
    auto program_ptr = builder.get_builder<Program>();
    auto& program = *program_ptr;
    program.clear();

    for (const auto predicate : task.get_task().get_domain().get_predicates<StaticTag>())
        program.static_predicates.push_back(merge(predicate, context).get_index());

    for (const auto predicate : task.get_task().get_domain().get_predicates<FluentTag>())
        program.fluent_predicates.push_back(merge(predicate, context).get_index());

    for (const auto predicate : task.get_task().get_domain().get_predicates<DerivedTag>())
        program.fluent_predicates.push_back(merge<DerivedTag, OverlayRepository<Repository>, Repository, FluentTag>(predicate, context).get_index());

    for (const auto predicate : task.get_task().get_derived_predicates())
        program.fluent_predicates.push_back(merge<DerivedTag, OverlayRepository<Repository>, Repository, FluentTag>(predicate, context).get_index());

    for (const auto function : task.get_task().get_domain().get_functions<StaticTag>())
        program.static_functions.push_back(merge(function, context).get_index());

    for (const auto function : task.get_task().get_domain().get_functions<FluentTag>())
        program.fluent_functions.push_back(merge(function, context).get_index());

    // We can ignore auxiliary function total-cost because it never occurs in a condition

    for (const auto object : task.get_task().get_domain().get_constants())
        program.objects.push_back(merge(object, context).get_index());
    for (const auto object : task.get_task().get_objects())
        program.objects.push_back(merge(object, context).get_index());

    for (const auto atom : task.get_task().get_atoms<StaticTag>())
        program.static_atoms.push_back(merge(atom, context).get_index());

    for (const auto atom : task.get_task().get_atoms<FluentTag>())
        program.fluent_atoms.push_back(merge(atom, context).get_index());

    for (const auto fterm_value : task.get_task().get_fterm_values<StaticTag>())
        program.static_fterm_values.push_back(merge(fterm_value, context).get_index());

    for (const auto action : task.get_task().get_domain().get_actions())
    {
        auto predicate_ptr = builder.get_builder<Predicate<FluentTag>>();
        auto& predicate = *predicate_ptr;
        predicate.clear();

        predicate.name = action.get_name();
        predicate.arity = action.get_arity();

        canonicalize(predicate);
        const auto new_predicate = repository.get_or_create(predicate, builder.get_buffer()).first;

        if (std::find(program.fluent_predicates.begin(), program.fluent_predicates.end(), new_predicate.get_index()) == program.fluent_predicates.end())
            program.fluent_predicates.push_back(new_predicate.get_index());

        auto rule_ptr = builder.get_builder<Rule>();
        auto& rule = *rule_ptr;
        rule.clear();

        auto conj_cond_ptr = builder.get_builder<ConjunctiveCondition>();
        auto& conj_cond = *conj_cond_ptr;
        conj_cond.clear();

        for (const auto variable : action.get_variables())
            conj_cond.variables.push_back(merge(variable, context).get_index());

        for (const auto literal : action.get_condition().get_literals<StaticTag>())
            conj_cond.static_literals.push_back(merge(literal, context).get_index());

        for (const auto literal : action.get_condition().get_literals<FluentTag>())
            conj_cond.fluent_literals.push_back(merge(literal, context).get_index());

        for (const auto literal : action.get_condition().get_literals<DerivedTag>())
            conj_cond.fluent_literals.push_back(merge<DerivedTag, OverlayRepository<Repository>, Repository, FluentTag>(literal, context).get_index());

        for (const auto numeric_constraint : action.get_condition().get_numeric_constraints())
            conj_cond.numeric_constraints.push_back(merge(numeric_constraint, context).get_data());

        canonicalize(conj_cond);
        const auto new_conj_cond = repository.get_or_create(conj_cond, builder.get_buffer()).first;

        auto atom_ptr = builder.get_builder<Atom<FluentTag>>();
        auto& atom = *atom_ptr;
        atom.clear();

        atom.predicate = new_predicate.get_index();
        for (uint_t i = 0; i < action.get_arity(); ++i)
            atom.terms.push_back(Data<Term>(ParameterIndex(i)));

        canonicalize(atom);
        const auto new_head = repository.get_or_create(atom, builder.get_buffer()).first;

        rule.body = new_conj_cond.get_index();
        rule.head = new_head.get_index();

        canonicalize(rule);
        const auto new_rule = repository.get_or_create(rule, builder.get_buffer()).first;

        rule_to_actions_mapping[new_rule].emplace_back(action);

        program.rules.push_back(new_rule.get_index());
    }

    canonicalize(program);
    return repository.get_or_create(program, builder.get_buffer()).first;
}

ApplicableActionProgram::ApplicableActionProgram(const LiftedTask& task) :
    m_rule_to_actions(),
    m_repository(std::make_shared<Repository>()),
    m_program(create(task, m_rule_to_actions, *m_repository)),
    m_domains(analysis::compute_variable_domains(m_program)),
    m_strata(analysis::compute_rule_stratification(m_program)),
    m_listeners(analysis::compute_listeners(m_strata))
{
}

const ApplicableActionProgram::RuleToActionsMapping& ApplicableActionProgram::get_rule_to_actions_mapping() const noexcept { return m_rule_to_actions; }

View<Index<Program>, Repository> ApplicableActionProgram::get_program() const noexcept { return m_program; }

const RepositoryPtr& ApplicableActionProgram::get_repository() const noexcept { return m_repository; }

const analysis::ProgramVariableDomains& ApplicableActionProgram::get_domains() const noexcept { return m_domains; }

const analysis::RuleStrata& ApplicableActionProgram::get_strata() const noexcept { return m_strata; }

const analysis::Listeners& ApplicableActionProgram::get_listeners() const noexcept { return m_listeners; }

}
