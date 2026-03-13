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

#include "common.hpp"
#include "tyr/formalism/datalog/datas.hpp"
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/planning/formatter.hpp"
#include "tyr/formalism/planning/merge_datalog.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"

namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;
namespace fd = tyr::formalism::datalog;

namespace tyr::planning
{
namespace axiom
{
static void process_axiom_body(fp::ConjunctiveConditionView axiom_body, fp::MergeDatalogContext& context, Data<fd::ConjunctiveCondition>& conj_cond)
{
    for (const auto literal : axiom_body.get_literals<f::StaticTag>())
        conj_cond.static_literals.push_back(fp::merge_p2d(literal, context).first.get_index());

    for (const auto literal : axiom_body.get_literals<f::FluentTag>())
        conj_cond.fluent_literals.push_back(fp::merge_p2d(literal, context).first.get_index());

    for (const auto literal : axiom_body.get_literals<f::DerivedTag>())
        conj_cond.fluent_literals.push_back(fp::merge_p2d<f::DerivedTag, f::FluentTag>(literal, context).first.get_index());

    for (const auto numeric_constraint : axiom_body.get_numeric_constraints())
        conj_cond.numeric_constraints.push_back(fp::merge_p2d(numeric_constraint, context));
}

static auto create_axiom_rule(fp::AxiomView axiom, fp::MergeDatalogContext& context)
{
    auto rule_ptr = context.builder.get_builder<fd::Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    auto conj_cond_ptr = context.builder.get_builder<fd::ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    for (const auto variable : axiom.get_variables())
        conj_cond.variables.push_back(fp::merge_p2d(variable, context).first.get_index());

    process_axiom_body(axiom.get_body(), context, conj_cond);

    canonicalize(conj_cond);
    const auto new_conj_cond = context.destination.get_or_create(conj_cond, context.builder.get_buffer()).first.get_index();

    rule.body = new_conj_cond;

    const auto new_head = fp::merge_p2d<f::DerivedTag, f::FluentTag>(axiom.get_head(), context).first.get_index();

    rule.head = new_head;

    canonicalize(rule);
    return context.destination.get_or_create(rule, context.builder.get_buffer());
}

static auto create_program(fp::TaskView task, AxiomEvaluatorProgram::PredicateToPredicateMapping& predicate_to_predicate, fd::Repository& repository)
{
    auto builder = fd::Builder();
    auto context = fp::MergeDatalogContext(builder, repository);
    auto program_ptr = builder.get_builder<fd::Program>();
    auto& program = *program_ptr;
    program.clear();

    for (const auto predicate : task.get_domain().get_predicates<f::StaticTag>())
        program.static_predicates.push_back(fp::merge_p2d(predicate, context).first.get_index());

    for (const auto predicate : task.get_domain().get_predicates<f::FluentTag>())
        program.fluent_predicates.push_back(fp::merge_p2d(predicate, context).first.get_index());

    for (const auto predicate : task.get_domain().get_predicates<f::DerivedTag>())
    {
        const auto new_predicate = fp::merge_p2d<f::DerivedTag, f::FluentTag>(predicate, context).first.get_index();

        [[maybe_unused]] const auto [it, inserted] = predicate_to_predicate.emplace(new_predicate, predicate.get_index());
        assert(inserted);

        program.fluent_predicates.push_back(new_predicate);
    }

    for (const auto predicate : task.get_derived_predicates())
    {
        const auto new_predicate = fp::merge_p2d<f::DerivedTag, f::FluentTag>(predicate, context).first.get_index();

        [[maybe_unused]] const auto [it, inserted] = predicate_to_predicate.emplace(new_predicate, predicate.get_index());
        assert(inserted);

        program.fluent_predicates.push_back(new_predicate);
    }

    for (const auto function : task.get_domain().get_functions<f::StaticTag>())
        program.static_functions.push_back(fp::merge_p2d(function, context).first.get_index());

    for (const auto function : task.get_domain().get_functions<f::FluentTag>())
        program.fluent_functions.push_back(fp::merge_p2d(function, context).first.get_index());

    // We can ignore auxiliary function total-cost because it never occurs in a condition

    for (const auto object : task.get_domain().get_constants())
        program.objects.push_back(fp::merge_p2d(object, context).first.get_index());
    for (const auto object : task.get_objects())
        program.objects.push_back(fp::merge_p2d(object, context).first.get_index());

    for (const auto atom : task.get_atoms<f::StaticTag>())
        program.static_atoms.push_back(fp::merge_p2d(atom, context).first.get_index());

    for (const auto atom : task.get_atoms<f::FluentTag>())
        program.fluent_atoms.push_back(fp::merge_p2d(atom, context).first.get_index());

    for (const auto fterm_value : task.get_fterm_values<f::StaticTag>())
        program.static_fterm_values.push_back(fp::merge_p2d(fterm_value, context).first.get_index());

    for (const auto axiom : task.get_domain().get_axioms())
        program.rules.push_back(create_axiom_rule(axiom, context).first.get_index());

    for (const auto axiom : task.get_axioms())
        program.rules.push_back(create_axiom_rule(axiom, context).first.get_index());

    canonicalize(program);
    return repository.get_or_create(program, builder.get_buffer()).first;
}

static auto create_program_context(fp::TaskView task, AxiomEvaluatorProgram::PredicateToPredicateMapping& mapping)
{
    auto repository = std::make_shared<fd::Repository>(task.get_domain().get_constants().size() + task.get_objects().size());
    auto program = create_program(task, mapping, *repository);
    auto domains = analysis::compute_variable_domains(program);
    auto strata = analysis::compute_rule_stratification(program);
    auto listeners = analysis::compute_listeners(strata, *repository);

    return datalog::ProgramContext(program, std::move(repository), std::move(domains), std::move(strata), std::move(listeners));
}
}

AxiomEvaluatorProgram::AxiomEvaluatorProgram(fp::TaskView task) :
    m_predicate_to_predicate(),
    m_program_context(axiom::create_program_context(task, m_predicate_to_predicate)),
    m_program_workspace(m_program_context)
{
    // std::cout << m_program_context.get_program() << std::endl;
}

const AxiomEvaluatorProgram::PredicateToPredicateMapping& AxiomEvaluatorProgram::get_predicate_to_predicate_mapping() const noexcept
{
    return m_predicate_to_predicate;
}

datalog::ProgramContext& AxiomEvaluatorProgram::get_program_context() noexcept { return m_program_context; }

const datalog::ProgramContext& AxiomEvaluatorProgram::get_program_context() const noexcept { return m_program_context; }

const datalog::ConstProgramWorkspace& AxiomEvaluatorProgram::get_const_program_workspace() const noexcept { return m_program_workspace; }
}
