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
 *<
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "loki_to_tyr.hpp"

#include "tyr/planning/domain.hpp"
#include "tyr/planning/lifted_task.hpp"

namespace tyr::planning
{

/**
 * Prepare common
 */
void LokiToTyrTranslator::prepare(loki::FunctionSkeleton function_skeleton)
{
    prepare(function_skeleton->get_parameters());
    prepare(function_skeleton->get_type());
}
void LokiToTyrTranslator::prepare(loki::Object object) { prepare(object->get_bases()); }
void LokiToTyrTranslator::prepare(loki::Parameter parameter) { prepare(parameter->get_variable()); }
void LokiToTyrTranslator::prepare(loki::Predicate predicate) { prepare(predicate->get_parameters()); }
void LokiToTyrTranslator::prepare(loki::Requirements requirements) {}
void LokiToTyrTranslator::prepare(loki::Type type) { prepare(type->get_bases()); }
void LokiToTyrTranslator::prepare(loki::Variable variabl) {}

/**
 * Prepare lifted
 */

void LokiToTyrTranslator::prepare(loki::Term term)
{
    std::visit([&](auto&& arg) { return this->prepare(arg); }, term->get_object_or_variable());
}

void LokiToTyrTranslator::prepare(loki::Atom atom)
{
    prepare(atom->get_predicate());
    prepare(atom->get_terms());
}
void LokiToTyrTranslator::prepare(loki::Literal literal) { prepare(literal->get_atom()); }
void LokiToTyrTranslator::prepare(loki::FunctionExpressionNumber function_expression) {}
void LokiToTyrTranslator::prepare(loki::FunctionExpressionBinaryOperator function_expression)
{
    prepare(function_expression->get_left_function_expression());
    prepare(function_expression->get_right_function_expression());
}
void LokiToTyrTranslator::prepare(loki::FunctionExpressionMultiOperator function_expression) { this->prepare(function_expression->get_function_expressions()); }
void LokiToTyrTranslator::prepare(loki::FunctionExpressionMinus function_expression) { this->prepare(function_expression->get_function_expression()); }
void LokiToTyrTranslator::prepare(loki::FunctionExpressionFunction function_expression)
{
    m_fexpr_functions.insert(function_expression->get_function()->get_function_skeleton()->get_name());

    this->prepare(function_expression->get_function());
}
void LokiToTyrTranslator::prepare(loki::FunctionExpression function_expression)
{
    std::visit([&](auto&& arg) { return this->prepare(arg); }, function_expression->get_function_expression());
}
void LokiToTyrTranslator::prepare(loki::Function function)
{
    prepare(function->get_function_skeleton());
    prepare(function->get_terms());
}
void LokiToTyrTranslator::prepare(loki::Condition condition)
{
    if (const auto condition_and = std::get_if<loki::ConditionAnd>(&condition->get_condition()))
    {
        for (const auto& part : (*condition_and)->get_conditions())
        {
            if (const auto condition_literal = std::get_if<loki::ConditionLiteral>(&part->get_condition()))
            {
                prepare((*condition_literal)->get_literal());
            }
            else if (const auto condition_numeric = std::get_if<loki::ConditionNumericConstraint>(&part->get_condition()))
            {
                prepare((*condition_numeric)->get_left_function_expression());
                prepare((*condition_numeric)->get_right_function_expression());
            }
            else
            {
                // std::visit([](auto&& arg) { std::cout << arg << std::endl; }, part->get_condition());

                throw std::logic_error("Expected literal in conjunctive condition.");
            }
        }
    }
    else if (const auto condition_literal = std::get_if<loki::ConditionLiteral>(&condition->get_condition()))
    {
        prepare((*condition_literal)->get_literal());
    }
    else
    {
        // std::cout << std::visit([](auto&& arg) { return arg.str(); }, *condition_ptr) << std::endl;

        throw std::logic_error("Expected conjunctive condition.");
    }
}

void LokiToTyrTranslator::prepare(loki::Effect effect)
{
    const auto prepare_effect_func = [&](loki::Effect arg_effect)
    {
        auto tmp_effect = arg_effect;

        // 2. Prepare universal part
        if (const auto& tmp_effect_forall = std::get_if<loki::EffectCompositeForall>(&tmp_effect->get_effect()))
        {
            prepare((*tmp_effect_forall)->get_parameters());

            tmp_effect = (*tmp_effect_forall)->get_effect();
        }

        // 3. Prepare conditional part
        if (const auto& tmp_effect_when = std::get_if<loki::EffectCompositeWhen>(&tmp_effect->get_effect()))
        {
            if (const auto condition_and = std::get_if<loki::ConditionAnd>(&(*tmp_effect_when)->get_condition()->get_condition()))
            {
                for (const auto& part : (*condition_and)->get_conditions())
                {
                    if (const auto condition_literal = std::get_if<loki::ConditionLiteral>(&part->get_condition()))
                    {
                        prepare((*condition_literal)->get_literal());
                    }
                    else
                    {
                        // std::cout << std::visit([](auto&& arg) { return arg.str(); }, *part) << std::endl;

                        throw std::logic_error("Expected literal in conjunctive condition.");
                    }
                }
            }
            else if (const auto condition_literal = std::get_if<loki::ConditionLiteral>(&(*tmp_effect_when)->get_condition()->get_condition()))
            {
                prepare((*condition_literal)->get_literal());
            }

            tmp_effect = (*tmp_effect_when)->get_effect();
        }

        // 4. Parse simple effect
        if (const auto& effect_literal = std::get_if<loki::EffectLiteral>(&tmp_effect->get_effect()))
        {
            prepare((*effect_literal)->get_literal());

            // Found predicate affected by an effect
            m_fluent_predicates.insert((*effect_literal)->get_literal()->get_atom()->get_predicate()->get_name());
        }
        else if (const auto& effect_numeric = std::get_if<loki::EffectNumeric>(&tmp_effect->get_effect()))
        {
            // Found function affected by an effect
            m_effect_function_skeletons.insert((*effect_numeric)->get_function()->get_function_skeleton()->get_name());

            prepare((*effect_numeric)->get_function_expression());
        }
        else
        {
            // std::cout << std::visit([](auto&& arg) { return arg.str(); }, *tmp_effect) << std::endl;

            throw std::logic_error("Expected simple effect.");
        }
    };

    // 1. Prepare conjunctive part
    if (const auto& effect_and = std::get_if<loki::EffectAnd>(&effect->get_effect()))
    {
        for (const auto& nested_effect : (*effect_and)->get_effects())
        {
            prepare_effect_func(nested_effect);
        }
    }
    else
    {
        prepare_effect_func(effect);
    }
}
void LokiToTyrTranslator::prepare(loki::Action action)
{
    prepare(action->get_parameters());
    prepare(action->get_condition());
    prepare(action->get_effect());
}
void LokiToTyrTranslator::prepare(loki::Axiom axiom)
{
    prepare(axiom->get_parameters());
    prepare(axiom->get_literal());
    prepare(axiom->get_condition());

    m_derived_predicates.insert(axiom->get_literal()->get_atom()->get_predicate()->get_name());
}
void LokiToTyrTranslator::prepare(loki::FunctionValue function_value) { prepare(function_value->get_function()); }
void LokiToTyrTranslator::prepare(loki::OptimizationMetric metric) { prepare(metric->get_function_expression()); }

void LokiToTyrTranslator::prepare(loki::Domain domain)
{
    prepare(domain->get_requirements());
    prepare(domain->get_types());
    prepare(domain->get_constants());
    prepare(domain->get_predicates());
    prepare(domain->get_function_skeletons());
    prepare(domain->get_actions());
    prepare(domain->get_axioms());
}

void LokiToTyrTranslator::prepare(loki::Problem problem)
{
    prepare(problem->get_domain());
    prepare(problem->get_requirements());
    prepare(problem->get_objects());
    prepare(problem->get_predicates());
    prepare(problem->get_initial_literals());
    prepare(problem->get_initial_function_values());
    prepare(problem->get_goal_condition());
    prepare(problem->get_optimization_metric());
    prepare(problem->get_axioms());

    for (const auto& derived_predicate : problem->get_predicates())
    {
        m_derived_predicates.insert(derived_predicate->get_name());
    }
}

DomainPtr LokiToTyrTranslator::translate(const loki::Domain& element, formalism::Builder& builder, formalism::RepositoryPtr context)
{
    /* Perform static type analysis */
    prepare(element);

    auto domain_ptr = builder.get_builder<formalism::Domain>();
    auto& domain = *domain_ptr;
    domain.clear();

    /* Name */
    domain.name = element->get_name();

    /* Requirements section */

    /* Constants section */
    domain.constants = translate_common(element->get_constants(), builder, *context);

    /* Predicates section */
    const auto func_insert_predicate = [](IndexPredicateVariant index_literal_variant,
                                          IndexList<formalism::Predicate<formalism::StaticTag>>& static_predicates,
                                          IndexList<formalism::Predicate<formalism::FluentTag>>& fluent_predicates,
                                          IndexList<formalism::Predicate<formalism::DerivedTag>>& derived_predicates)
    {
        std::visit(
            [&](auto&& arg)
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, Index<formalism::Predicate<formalism::StaticTag>>>)
                    static_predicates.push_back(arg);
                else if constexpr (std::is_same_v<T, Index<formalism::Predicate<formalism::FluentTag>>>)
                    fluent_predicates.push_back(arg);
                else if constexpr (std::is_same_v<T, Index<formalism::Predicate<formalism::DerivedTag>>>)
                    derived_predicates.push_back(arg);
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_literal_variant);
    };

    for (const auto& index_predicate_variant : translate_common(element->get_predicates(), builder, *context))
    {
        func_insert_predicate(index_predicate_variant, domain.static_predicates, domain.fluent_predicates, domain.derived_predicates);
    }

    /* Functions section */
    const auto func_insert_function = [](IndexFunctionVariant index_literal_variant,
                                         IndexList<formalism::Function<formalism::StaticTag>>& static_functions,
                                         IndexList<formalism::Function<formalism::FluentTag>>& fluent_functions,
                                         ::cista::optional<Index<formalism::Function<formalism::AuxiliaryTag>>>& auxiliary_function)
    {
        std::visit(
            [&](auto&& arg)
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, Index<formalism::Function<formalism::StaticTag>>>)
                    static_functions.push_back(arg);
                else if constexpr (std::is_same_v<T, Index<formalism::Function<formalism::FluentTag>>>)
                    fluent_functions.push_back(arg);
                else if constexpr (std::is_same_v<T, Index<formalism::Function<formalism::AuxiliaryTag>>>)
                {
                    assert(!auxiliary_function);
                    auxiliary_function = arg;
                }
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_literal_variant);
    };

    for (const auto& index_function_variant : translate_common(element->get_function_skeletons(), builder, *context))
    {
        func_insert_function(index_function_variant, domain.static_functions, domain.fluent_functions, domain.auxiliary_function);
    }

    /* Structures section */
    domain.actions = translate_lifted(element->get_actions(), builder, *context);
    domain.axioms = translate_lifted(element->get_axioms(), builder, *context);

    formalism::canonicalize(domain);
    return std::make_shared<Domain>(context, context->get_or_create(domain, builder.get_buffer()).first);
}

LiftedTaskPtr
LokiToTyrTranslator::translate(const loki::Problem& element, formalism::Builder& builder, DomainPtr domain, formalism::RepositoryPtr domain_context)
{
    /* Perform static type analysis */
    prepare(element);

    auto task_ptr = builder.get_builder<formalism::Task>();
    auto& task = *task_ptr;
    task.clear();

    auto task_context = std::make_shared<formalism::Repository>();
    auto overlay_task_context = std::make_shared<formalism::OverlayRepository<formalism::Repository>>(*domain_context, *task_context);

    /* Name */
    task.name = element->get_name();

    /* Domain */
    task.domain = domain->get_domain().get_index();

    /* Requirements section */

    /* Objects section */
    task.objects = translate_common(element->get_objects(), builder, *overlay_task_context);

    /* Predicates section */
    const auto func_insert_predicate =
        [](IndexPredicateVariant index_predicate_variant, IndexList<formalism::Predicate<formalism::DerivedTag>>& derived_predicates)
    {
        std::visit(
            [&](auto&& arg)
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, Index<formalism::Predicate<formalism::StaticTag>>>)
                    throw std::runtime_error("Static predicate definition in task is not supported");
                else if constexpr (std::is_same_v<T, Index<formalism::Predicate<formalism::FluentTag>>>)
                    throw std::runtime_error("Fluent predicate definition in task is not supported");
                else if constexpr (std::is_same_v<T, Index<formalism::Predicate<formalism::DerivedTag>>>)
                    derived_predicates.push_back(arg);
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_predicate_variant);
    };

    for (const auto& index_predicate_variant : translate_common(element->get_predicates(), builder, *overlay_task_context))
    {
        func_insert_predicate(index_predicate_variant, task.derived_predicates);
    }

    /* Initial section */
    const auto func_insert_ground_atom = [&](IndexGroundLiteralOrFactVariant index_atom_variant,
                                             IndexList<formalism::GroundAtom<formalism::StaticTag>>& static_atoms,
                                             IndexList<formalism::GroundAtom<formalism::FluentTag>>& fluent_atoms)
    {
        std::visit(
            [&](auto&& arg)
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, Index<formalism::GroundLiteral<formalism::StaticTag>>>)
                    static_atoms.push_back(make_view(arg, *overlay_task_context).get_atom().get_index());
                else if constexpr (std::is_same_v<T, Data<formalism::FDRFact<formalism::FluentTag>>>)
                    fluent_atoms.push_back(make_view(arg, *overlay_task_context).get_atom().get_index());  // we know it must have a value
                else if constexpr (std::is_same_v<T, Index<formalism::GroundLiteral<formalism::DerivedTag>>>)
                    throw std::runtime_error("Derived ground atoms are not allowed to be defined in the initial section.");
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_atom_variant);
    };

    for (const auto& index_atom_variant : translate_grounded(element->get_initial_literals(), builder, *overlay_task_context))
    {
        func_insert_ground_atom(index_atom_variant, task.static_atoms, task.fluent_atoms);
    }

    const auto func_insert_fterm_values = [](IndexGroundFunctionTermValueVariant index_fterm_value_variant,
                                             IndexList<formalism::GroundFunctionTermValue<formalism::StaticTag>>& static_fterm_values,
                                             IndexList<formalism::GroundFunctionTermValue<formalism::FluentTag>>& fluent_fterm_values,
                                             ::cista::optional<Index<formalism::GroundFunctionTermValue<formalism::AuxiliaryTag>>>& auxiliary_fterm_value)
    {
        std::visit(
            [&](auto&& arg)
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, Index<formalism::GroundFunctionTermValue<formalism::StaticTag>>>)
                    static_fterm_values.push_back(arg);
                else if constexpr (std::is_same_v<T, Index<formalism::GroundFunctionTermValue<formalism::FluentTag>>>)
                    fluent_fterm_values.push_back(arg);
                else if constexpr (std::is_same_v<T, Index<formalism::GroundFunctionTermValue<formalism::AuxiliaryTag>>>)
                {
                    assert(!auxiliary_fterm_value);
                    auxiliary_fterm_value = arg;
                }
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_fterm_value_variant);
    };

    for (const auto index_fterm_value_variant : translate_grounded(element->get_initial_function_values(), builder, *overlay_task_context))
    {
        func_insert_fterm_values(index_fterm_value_variant, task.static_fterm_values, task.fluent_fterm_values, task.auxiliary_fterm_value);
    }

    /* Goal section */

    if (element->get_goal_condition().has_value())
    {
        task.goal = translate_grounded(element->get_goal_condition().value(), builder, *overlay_task_context);
    }
    else
    {
        // Create empty conjunctive condition
        auto conj_cond_ptr = builder.get_builder<formalism::GroundFDRConjunctiveCondition>();
        auto& conj_cond = *conj_cond_ptr;
        conj_cond.clear();
        canonicalize(conj_cond);
        task.goal = overlay_task_context->get_or_create(conj_cond, builder.get_buffer()).first.get_index();
    }

    /* Metric section */
    if (element->get_optimization_metric().has_value())
    {
        task.metric = translate_grounded(element->get_optimization_metric().value(), builder, *overlay_task_context);
    }
    else
    {
        task.metric = std::nullopt;
    }

    /* Structures section */
    task.axioms = translate_lifted(element->get_axioms(), builder, *overlay_task_context);

    formalism::canonicalize(task);
    return std::make_shared<LiftedTask>(domain, task_context, overlay_task_context, overlay_task_context->get_or_create(task, builder.get_buffer()).first);
}

}
