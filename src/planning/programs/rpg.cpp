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

#include "tyr/planning/programs/rpg.hpp"

#include "common.hpp"
#include "tyr/common/unordered_set.hpp"
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/planning/formatter.hpp"
#include "tyr/formalism/planning/merge_datalog.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"

namespace f = tyr::formalism;
namespace d = tyr::datalog;
namespace fp = tyr::formalism::planning;
namespace fd = tyr::formalism::datalog;

namespace tyr::planning
{
namespace rpg
{

static auto create_cond_effect_rule(View<Index<formalism::planning::Action>, formalism::planning::Repository> action,
                                    View<Index<formalism::planning::ConditionalEffect>, formalism::planning::Repository> cond_eff,
                                    View<Index<formalism::planning::Atom<formalism::FluentTag>>, formalism::planning::Repository> effect,
                                    formalism::planning::MergeDatalogContext& context)
{
    auto rule_ptr = context.builder.get_builder<formalism::datalog::Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    auto conj_cond_ptr = context.builder.get_builder<formalism::datalog::ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    // Action parameter may get deleted.
    for (const auto& variable : action.get_variables())
        conj_cond.variables.push_back(merge_p2d(variable, context).first.get_index());
    for (const auto literal : action.get_condition().get_literals<formalism::StaticTag>())
        conj_cond.static_literals.push_back(merge_p2d(literal, context).first.get_index());
    for (const auto literal : action.get_condition().get_literals<formalism::FluentTag>())
        if (literal.get_polarity())
            conj_cond.fluent_literals.push_back(merge_p2d(literal, context).first.get_index());

    for (const auto variable : cond_eff.get_variables())
        conj_cond.variables.push_back(merge_p2d(variable, context).first.get_index());
    for (const auto literal : cond_eff.get_condition().template get_literals<formalism::StaticTag>())
        conj_cond.static_literals.push_back(merge_p2d(literal, context).first.get_index());
    for (const auto literal : cond_eff.get_condition().template get_literals<formalism::FluentTag>())
        if (literal.get_polarity())
            conj_cond.fluent_literals.push_back(merge_p2d(literal, context).first.get_index());

    canonicalize(conj_cond);
    const auto new_conj_cond = context.destination.get_or_create(conj_cond, context.builder.get_buffer()).first;

    rule.variables = new_conj_cond.get_variables().get_data();
    rule.body = new_conj_cond.get_index();
    rule.head = merge_p2d(effect, context).first.get_index();
    rule.cost = 1;

    canonicalize(rule);
    return context.destination.get_or_create(rule, context.builder.get_buffer());
}

static void translate_action_to_delete_free_rules(View<Index<fp::Action>, fp::Repository> action,
                                                  Data<fd::Program>& program,
                                                  fp::MergeDatalogContext& context,
                                                  RPGProgram::RuleToActionMapping& rule_to_action)
{
    for (const auto cond_eff : action.get_effects())
    {
        for (const auto literal : cond_eff.get_effect().get_literals())
        {
            if (!literal.get_polarity())
                continue;  /// ignore delete effects

            const auto rule = create_cond_effect_rule(action, cond_eff, literal.get_atom(), context).first.get_index();

            program.rules.push_back(rule);
            rule_to_action.emplace(rule, action.get_index());
        }
    }
}

static auto create_program(View<Index<fp::Task>, fp::Repository> task, fd::Repository& destination, RPGProgram::RuleToActionMapping& rule_to_action)
{
    auto builder = fd::Builder();
    auto context = fp::MergeDatalogContext(builder, destination);
    auto program_ptr = builder.get_builder<fd::Program>();
    auto& program = *program_ptr;
    program.clear();

    for (const auto predicate : task.get_domain().get_predicates<f::StaticTag>())
        program.static_predicates.push_back(fp::merge_p2d(predicate, context).first.get_index());

    for (const auto predicate : task.get_domain().get_predicates<f::FluentTag>())
        program.fluent_predicates.push_back(fp::merge_p2d(predicate, context).first.get_index());

    // We can ignore auxiliary function total-cost because it never occurs in a condition

    for (const auto object : task.get_domain().get_constants())
        program.objects.push_back(fp::merge_p2d(object, context).first.get_index());
    for (const auto object : task.get_objects())
        program.objects.push_back(fp::merge_p2d(object, context).first.get_index());

    for (const auto atom : task.get_atoms<f::StaticTag>())
        program.static_atoms.push_back(fp::merge_p2d(atom, context).first.get_index());

    for (const auto atom : task.get_atoms<f::FluentTag>())
        program.fluent_atoms.push_back(fp::merge_p2d(atom, context).first.get_index());

    for (const auto action : task.get_domain().get_actions())
        translate_action_to_delete_free_rules(action, program, context, rule_to_action);

    canonicalize(program);
    return destination.get_or_create(program, builder.get_buffer()).first;
}

static auto create_program_context(View<Index<fp::Task>, fp::Repository> task, RPGProgram::RuleToActionMapping& rule_to_action)
{
    auto repository = std::make_shared<fd::Repository>();
    auto program = create_program(task, *repository, rule_to_action);
    auto domains = analysis::compute_variable_domains(program);
    auto strata = analysis::compute_rule_stratification(program);
    auto listeners = analysis::compute_listeners(strata, *repository);

    return datalog::ProgramContext(program, std::move(repository), std::move(domains), std::move(strata), std::move(listeners));
}

}

RPGProgram::RPGProgram(View<Index<fp::Task>, fp::Repository> task) :
    m_rule_to_action(),
    m_program_context(rpg::create_program_context(task, m_rule_to_action)),
    m_program_workspace(m_program_context)
{
    // std::cout << m_program_context.get_program() << std::endl;
}

const RPGProgram::RuleToActionMapping& RPGProgram::get_rule_to_action_mapping() const noexcept { return m_rule_to_action; }

datalog::ProgramContext& RPGProgram::get_program_context() noexcept { return m_program_context; }

const datalog::ProgramContext& RPGProgram::get_program_context() const noexcept { return m_program_context; }

const datalog::ConstProgramWorkspace& RPGProgram::get_const_program_workspace() const noexcept { return m_program_workspace; }

}