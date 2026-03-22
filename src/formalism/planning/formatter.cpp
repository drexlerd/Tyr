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

#include "tyr/common/formatter.hpp"

#include "tyr/common/iostream.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/formatter.hpp"
#include "tyr/formalism/planning/planning_domain.hpp"
#include "tyr/formalism/planning/planning_fdr_task.hpp"
#include "tyr/formalism/planning/planning_task.hpp"
#include "tyr/formalism/planning/repository.hpp"

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <ostream>

namespace tyr
{

std::ostream& print(std::ostream& os, const Data<formalism::planning::FunctionExpression>& el)
{
    fmt::print(os, "{}", to_string(el.value));
    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::FunctionExpressionView& el)
{
    fmt::print(os, "{}", to_string(el.get_variant()));
    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::planning::GroundFunctionExpression>& el)
{
    fmt::print(os, "{}", to_string(el.value));
    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::GroundFunctionExpressionView& el)
{
    fmt::print(os, "{}", to_string(el.get_variant()));
    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::OpAssign& el)
{
    fmt::print(os, "assign");
    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::OpIncrease& el)
{
    fmt::print(os, "increase");
    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::OpDecrease& el)
{
    fmt::print(os, "decrease");
    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::OpScaleUp& el)
{
    fmt::print(os, "scale-up");
    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::OpScaleDown& el)
{
    fmt::print(os, "scale-down");
    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::Minimize& el)
{
    fmt::print(os, "minimize");
    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::Maximize& el)
{
    fmt::print(os, "maximize");
    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::planning::ConditionalEffect>& el)
{
    os << "ConditionalEffect(\n";
    {
        IndentScope scope(os);

        os << print_indent << "variables = " << el.variables << "\n";

        os << print_indent << "condition = " << el.condition << "\n";

        os << print_indent << "effect = " << el.effect << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::ConditionalEffectView& el)
{
    os << "ConditionalEffect(\n";
    {
        IndentScope scope(os);

        os << print_indent << "variables = " << el.get_variables() << "\n";

        os << print_indent << "condition = " << el.get_condition() << "\n";

        os << print_indent << "effect = " << el.get_effect() << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::planning::GroundConditionalEffect>& el)
{
    os << "GroundConditionalEffect(\n";
    {
        IndentScope scope(os);

        os << print_indent << "condition = " << el.condition << "\n";

        os << print_indent << "effect = " << el.effect << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::GroundConditionalEffectView& el)
{
    os << "GroundConditionalEffect(\n";
    {
        IndentScope scope(os);

        os << print_indent << "condition = " << el.get_condition() << "\n";

        os << print_indent << "effect = " << el.get_effect() << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::planning::ConjunctiveEffect>& el)
{
    os << "ConjunctiveEffect(\n";
    {
        IndentScope scope(os);

        os << print_indent << "fluent literals = " << el.literals << "\n";

        os << print_indent << "fluent numeric effects = " << el.numeric_effects << "\n";

        os << print_indent << "auxiliary numeric effect = " << el.auxiliary_numeric_effect << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::ConjunctiveEffectView& el)
{
    os << "ConjunctiveEffect(\n";
    {
        IndentScope scope(os);

        os << print_indent << "fluent literals = " << el.get_literals() << "\n";

        os << print_indent << "fluent numeric effects = " << el.get_numeric_effects() << "\n";

        os << print_indent << "auxiliary numeric effect = " << el.get_auxiliary_numeric_effect() << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::planning::GroundConjunctiveEffect>& el)
{
    os << "GroundConjunctiveEffect(\n";
    {
        IndentScope scope(os);

        os << print_indent << "fluent facts = " << el.facts << "\n";

        os << print_indent << "fluent numeric effects = " << el.numeric_effects << "\n";

        os << print_indent << "auxiliary numeric effect = " << el.auxiliary_numeric_effect << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::GroundConjunctiveEffectView& el)
{
    os << "GroundConjunctiveEffect(\n";
    {
        IndentScope scope(os);

        os << print_indent << "fluent facts = " << el.get_facts() << "\n";

        os << print_indent << "fluent numeric effects = " << el.get_numeric_effects() << "\n";

        os << print_indent << "auxiliary numeric effect = " << el.get_auxiliary_numeric_effect() << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::planning::Action>& el)
{
    os << "Action(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.index << "\n";

        os << print_indent << "name = " << el.name << "\n";

        os << print_indent << "variables = " << el.variables << "\n";

        os << print_indent << "condition = " << el.condition << "\n";

        os << print_indent << "effects = " << el.effects << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::ActionView& el)
{
    os << "Action(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.get_index() << "\n";

        os << print_indent << "name = " << el.get_name() << "\n";

        os << print_indent << "variables = " << el.get_variables() << "\n";

        os << print_indent << "condition = " << el.get_condition() << "\n";

        os << print_indent << "effects = " << el.get_effects() << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::planning::GroundAction>& el)
{
    os << "GroundAction(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.index << "\n";

        os << print_indent << "binding = " << el.binding << "\n";

        os << print_indent << "condition = " << el.condition << "\n";

        os << print_indent << "effects = " << el.effects << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::GroundActionView& el)
{
    os << "GroundAction(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.get_index() << "\n";

        os << print_indent << "action index = " << el.get_action().get_index() << "\n";

        os << print_indent << "condition = " << el.get_condition() << "\n";

        os << print_indent << "effects = " << el.get_effects() << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const std::pair<formalism::planning::GroundActionView, formalism::planning::PlanFormatting>& el)
{
    fmt::print(os, "({}", to_string(el.first.get_action().get_name()));
    for (size_t i = 0; i < el.first.get_action().get_original_arity(); ++i)
    {
        os << " " << el.first.get_row().get_objects()[i];
    }
    fmt::print(os, ")");

    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::planning::Axiom>& el)
{
    os << "Axiom(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.index << "\n";

        os << print_indent << "variables = " << el.variables << "\n";

        os << print_indent << "body = " << el.body << "\n";

        os << print_indent << "head = " << el.head << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::AxiomView& el)
{
    os << "Axiom(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.get_index() << "\n";

        os << print_indent << "variables = " << el.get_variables() << "\n";

        os << print_indent << "body = " << el.get_body() << "\n";

        os << print_indent << "head = " << el.get_head() << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::planning::GroundAxiom>& el)
{
    os << "GroundAxiom(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.index << "\n";

        os << print_indent << "binding = " << el.binding << "\n";

        os << print_indent << "body = " << el.body << "\n";

        os << print_indent << "head = " << el.head << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::GroundAxiomView& el)
{
    os << "GroundAxiom(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.get_index() << "\n";

        os << print_indent << "axiom index = " << el.get_axiom().get_index() << "\n";

        os << print_indent << "body = " << el.get_body() << "\n";

        os << print_indent << "head = " << el.get_head() << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::ActionBindingView& el)
{
    fmt::print(os, "{}", fmt::join(to_strings(el.get_objects()), " "));
    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::AxiomBindingView& el)
{
    fmt::print(os, "{}", fmt::join(to_strings(el.get_objects()), " "));
    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::planning::Metric>& el)
{
    fmt::print(os, "({} {})", to_string(el.objective), to_string(el.fexpr));
    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::MetricView& el)
{
    fmt::print(os, "({} {})", to_string(el.get_objective()), to_string(el.get_fexpr()));
    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::planning::Task>& el)
{
    os << "Task(\n";
    {
        IndentScope scope(os);

        os << print_indent << "name = " << el.name << "\n";

        os << print_indent << "derived predicates = " << el.derived_predicates << "\n";

        os << print_indent << "objects = " << el.objects << "\n";

        os << print_indent << "static atoms = " << el.static_atoms << "\n";

        os << print_indent << "fluent atoms = " << el.fluent_atoms << "\n";

        os << print_indent << "static numeric variables = " << el.static_fterm_values << "\n";

        os << print_indent << "fluent numeric variables = " << el.fluent_fterm_values << "\n";

        os << print_indent << "auxiliary numeric variable = " << el.auxiliary_fterm_value << "\n";

        os << print_indent << "goal = " << el.goal << "\n";

        os << print_indent << "metric = " << el.metric << "\n";

        os << print_indent << "axioms = " << el.axioms << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::TaskView& el)
{
    os << "Task(\n";
    {
        IndentScope scope(os);

        os << print_indent << "name = " << el.get_name() << "\n";

        os << print_indent << "derived predicates = " << el.get_derived_predicates() << "\n";

        os << print_indent << "objects = " << el.get_objects() << "\n";

        os << print_indent << "static atoms = " << el.template get_atoms<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent atoms = " << el.template get_atoms<formalism::FluentTag>() << "\n";

        os << print_indent << "static numeric variables = " << el.template get_fterm_values<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent numeric variables = " << el.template get_fterm_values<formalism::FluentTag>() << "\n";

        os << print_indent << "auxiliary numeric variable = " << el.get_auxiliary_fterm_value() << "\n";

        os << print_indent << "goal = " << el.get_goal() << "\n";

        os << print_indent << "metric = " << el.get_metric() << "\n";

        os << print_indent << "axioms = " << el.get_axioms() << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::planning::Domain>& el)
{
    os << "Domain(\n";
    {
        IndentScope scope(os);

        os << print_indent << "name = " << el.name << "\n";

        os << print_indent << "static predicates = " << el.static_predicates << "\n";

        os << print_indent << "fluent predicates = " << el.fluent_predicates << "\n";

        os << print_indent << "derived predicates = " << el.derived_predicates << "\n";

        os << print_indent << "static functions = " << el.static_functions << "\n";

        os << print_indent << "fluent functions = " << el.fluent_functions << "\n";

        os << print_indent << "auxiliary function = " << el.auxiliary_function << "\n";

        os << print_indent << "constants = " << el.constants << "\n";

        os << print_indent << "actions = " << el.actions << "\n";

        os << print_indent << "axioms = " << el.axioms << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::DomainView& el)
{
    os << "Domain(\n";
    {
        IndentScope scope(os);

        os << print_indent << "name = " << el.get_name() << "\n";

        os << print_indent << "static predicates = " << el.template get_predicates<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent predicates = " << el.template get_predicates<formalism::FluentTag>() << "\n";

        os << print_indent << "derived predicates = " << el.template get_predicates<formalism::DerivedTag>() << "\n";

        os << print_indent << "static functions = " << el.template get_functions<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent functions = " << el.template get_functions<formalism::FluentTag>() << "\n";

        os << print_indent << "auxiliary function = " << el.get_auxiliary_function() << "\n";

        os << print_indent << "constants = " << el.get_constants() << "\n";

        os << print_indent << "actions = " << el.get_actions() << "\n";

        os << print_indent << "axioms = " << el.get_axioms() << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::FDRValue& el)
{
    fmt::print(os, "{}", uint_t(el));
    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::planning::ConjunctiveCondition>& el)
{
    os << "ConjunctiveCondition(\n";
    {
        IndentScope scope(os);

        os << print_indent << "variables = " << el.variables << "\n";

        os << print_indent << "static literals = " << el.static_literals << "\n";

        os << print_indent << "fluent literals = " << el.fluent_literals << "\n";

        os << print_indent << "derived literals = " << el.derived_literals << "\n";

        os << print_indent << "numeric constraints = " << el.numeric_constraints << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::ConjunctiveConditionView& el)
{
    os << "ConjunctiveCondition(\n";
    {
        IndentScope scope(os);

        os << print_indent << "variables = " << el.get_variables() << "\n";

        os << print_indent << "static literals = " << el.template get_literals<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent literals = " << el.template get_literals<formalism::FluentTag>() << "\n";

        os << print_indent << "derived literals = " << el.template get_literals<formalism::DerivedTag>() << "\n";

        os << print_indent << "numeric constraints = " << el.get_numeric_constraints() << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::planning::GroundConjunctiveCondition>& el)
{
    os << "GroundConjunctiveCondition(\n";
    {
        IndentScope scope(os);

        os << print_indent << "static literals = " << el.static_literals << "\n";

        os << print_indent << "fluent facts = " << el.fluent_facts << "\n";

        os << print_indent << "derived literals = " << el.derived_literals << "\n";

        os << print_indent << "numeric constraints = " << el.numeric_constraints << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::GroundConjunctiveConditionView& el)
{
    os << "GroundConjunctiveCondition(\n";
    {
        IndentScope scope(os);

        os << print_indent << "static literals = " << el.template get_facts<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent facts = " << el.template get_facts<formalism::FluentTag>() << "\n";

        os << print_indent << "derived facts = " << el.template get_facts<formalism::DerivedTag>() << "\n";

        os << print_indent << "numeric constraints = " << el.get_numeric_constraints() << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::planning::FDRTask>& el)
{
    os << "FDRTask(\n";
    {
        IndentScope scope(os);

        os << print_indent << "name = " << el.name << "\n";

        os << print_indent << "derived predicates = " << el.derived_predicates << "\n";

        os << print_indent << "objects = " << el.objects << "\n";

        os << print_indent << "static atoms = " << el.static_atoms << "\n";

        os << print_indent << "fluent atoms = " << el.fluent_atoms << "\n";

        os << print_indent << "derived atoms = " << el.derived_atoms << "\n";

        os << print_indent << "static fterms = " << el.static_fterms << "\n";

        os << print_indent << "fluent fterms = " << el.fluent_fterms << "\n";

        os << print_indent << "auxiliary fterm = " << el.auxiliary_fterm << "\n";

        os << print_indent << "static numeric variables = " << el.static_fterm_values << "\n";

        os << print_indent << "fluent numeric variables = " << el.fluent_fterm_values << "\n";

        os << print_indent << "auxiliary numeric variable = " << el.auxiliary_fterm_value << "\n";

        os << print_indent << "goal = " << el.goal << "\n";

        os << print_indent << "metric = " << el.metric << "\n";

        os << print_indent << "axioms = " << el.axioms << "\n";

        os << print_indent << "fluent variables = " << el.fluent_variables << "\n";

        os << print_indent << "fluent facts = " << el.fluent_facts << "\n";

        os << print_indent << "goal = " << el.goal << "\n";

        os << print_indent << "ground actions = " << el.ground_actions << "\n";

        os << print_indent << "ground axioms = " << el.ground_axioms << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::FDRTaskView& el)
{
    os << "FDRTask(\n";
    {
        IndentScope scope(os);

        os << print_indent << "name = " << el.get_name() << "\n";

        os << print_indent << "derived predicates = " << el.get_derived_predicates() << "\n";

        os << print_indent << "objects = " << el.get_objects() << "\n";

        os << print_indent << "static atoms = " << el.template get_atoms<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent atoms = " << el.template get_atoms<formalism::FluentTag>() << "\n";

        os << print_indent << "derived atoms = " << el.template get_atoms<formalism::DerivedTag>() << "\n";

        os << print_indent << "static fterms = " << el.template get_fterms<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent fterms = " << el.template get_fterms<formalism::FluentTag>() << "\n";

        os << print_indent << "auxiliary fterm = " << el.get_auxiliary_fterm() << "\n";

        os << print_indent << "static numeric variables = " << el.template get_fterm_values<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent numeric variables = " << el.template get_fterm_values<formalism::FluentTag>() << "\n";

        os << print_indent << "auxiliary numeric variable = " << el.get_auxiliary_fterm_value() << "\n";

        os << print_indent << "goal = " << el.get_goal() << "\n";

        os << print_indent << "metric = " << el.get_metric() << "\n";

        os << print_indent << "axioms = " << el.get_axioms() << "\n";

        os << print_indent << "fluent variables = " << el.get_fluent_variables() << "\n";

        os << print_indent << "fluent facts = " << el.get_fluent_facts() << "\n";

        os << print_indent << "goal = " << el.get_goal() << "\n";

        os << print_indent << "ground actions = " << el.get_ground_actions() << "\n";

        os << print_indent << "ground axioms = " << el.get_ground_axioms() << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::PlanningDomain& el)
{
    fmt::print(os, "{}", to_string(el.get_domain()));
    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::PlanningTask& el)
{
    fmt::print(os, "{}", to_string(el.get_task()));
    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::PlanningFDRTask& el)
{
    fmt::print(os, "{}", to_string(el.get_task()));
    return os;
}

namespace formalism::planning
{

std::ostream& operator<<(std::ostream& os, const Data<FunctionExpression>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const FunctionExpressionView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<GroundFunctionExpression>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const GroundFunctionExpressionView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const OpAssign& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const OpIncrease& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const OpDecrease& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const OpScaleUp& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const OpScaleDown& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Minimize& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Maximize& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<ConditionalEffect>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const ConditionalEffectView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<GroundConditionalEffect>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const GroundConditionalEffectView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<ConjunctiveEffect>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const ConjunctiveEffectView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<GroundConjunctiveEffect>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const GroundConjunctiveEffectView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<Action>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const ActionView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<GroundAction>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const GroundActionView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const std::pair<GroundActionView, PlanFormatting>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<Axiom>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const AxiomView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<GroundAxiom>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const GroundAxiomView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const ActionBindingView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const AxiomBindingView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<Metric>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const MetricView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<Task>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const TaskView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<Domain>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const DomainView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<ConjunctiveCondition>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const ConjunctiveConditionView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<GroundConjunctiveCondition>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const GroundConjunctiveConditionView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<FDRTask>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const FDRTaskView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const PlanningDomain& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const PlanningTask& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const PlanningFDRTask& el) { return tyr::print(os, el); }

}
}
