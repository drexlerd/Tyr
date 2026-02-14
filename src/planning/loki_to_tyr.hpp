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

#ifndef TYR_SRC_PLANNING_LOKI_TO_TYR_HPP_
#define TYR_SRC_PLANNING_LOKI_TO_TYR_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/formalism/planning/builder.hpp"
#include "tyr/formalism/planning/canonicalization.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/fdr_context.hpp"
#include "tyr/formalism/planning/grounder.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"
#include "tyr/planning/declarations.hpp"

#include <loki/loki.hpp>
#include <tuple>
#include <unordered_map>
#include <variant>

namespace tyr::planning
{

using IndexPredicateVariant = std::variant<Index<formalism::Predicate<formalism::StaticTag>>,
                                           Index<formalism::Predicate<formalism::FluentTag>>,
                                           Index<formalism::Predicate<formalism::DerivedTag>>>;

using IndexAtomVariant = std::variant<Index<formalism::planning::Atom<formalism::StaticTag>>,
                                      Index<formalism::planning::Atom<formalism::FluentTag>>,
                                      Index<formalism::planning::Atom<formalism::DerivedTag>>>;

using IndexLiteralVariant = std::variant<Index<formalism::planning::Literal<formalism::StaticTag>>,
                                         Index<formalism::planning::Literal<formalism::FluentTag>>,
                                         Index<formalism::planning::Literal<formalism::DerivedTag>>>;

using IndexGroundAtomVariant = std::variant<Index<formalism::planning::GroundAtom<formalism::StaticTag>>,
                                            Index<formalism::planning::GroundAtom<formalism::FluentTag>>,
                                            Index<formalism::planning::GroundAtom<formalism::DerivedTag>>>;

using IndexGroundLiteralVariant = std::variant<Index<formalism::planning::GroundLiteral<formalism::StaticTag>>,
                                               Index<formalism::planning::GroundLiteral<formalism::FluentTag>>,
                                               Index<formalism::planning::GroundLiteral<formalism::DerivedTag>>>;

using IndexGroundAtomOrFactVariant = std::variant<Index<formalism::planning::GroundAtom<formalism::StaticTag>>,
                                                  Data<formalism::planning::FDRFact<formalism::FluentTag>>,
                                                  Index<formalism::planning::GroundAtom<formalism::DerivedTag>>>;

using IndexGroundLiteralOrFactVariant = std::variant<Index<formalism::planning::GroundLiteral<formalism::StaticTag>>,
                                                     Data<formalism::planning::FDRFact<formalism::FluentTag>>,
                                                     Index<formalism::planning::GroundLiteral<formalism::DerivedTag>>>;

using IndexFunctionVariant = std::variant<Index<formalism::Function<formalism::StaticTag>>,
                                          Index<formalism::Function<formalism::FluentTag>>,
                                          Index<formalism::Function<formalism::AuxiliaryTag>>>;

using IndexFunctionTermVariant = std::variant<Index<formalism::planning::FunctionTerm<formalism::StaticTag>>,
                                              Index<formalism::planning::FunctionTerm<formalism::FluentTag>>,
                                              Index<formalism::planning::FunctionTerm<formalism::AuxiliaryTag>>>;

using IndexGroundFunctionTermVariant = std::variant<Index<formalism::planning::GroundFunctionTerm<formalism::StaticTag>>,
                                                    Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>>,
                                                    Index<formalism::planning::GroundFunctionTerm<formalism::AuxiliaryTag>>>;

using IndexGroundFunctionTermValueVariant = std::variant<Index<formalism::planning::GroundFunctionTermValue<formalism::StaticTag>>,
                                                         Index<formalism::planning::GroundFunctionTermValue<formalism::FluentTag>>,
                                                         Index<formalism::planning::GroundFunctionTermValue<formalism::AuxiliaryTag>>>;

using IndexNumericEffectVariant = std::variant<Index<formalism::planning::NumericEffect<formalism::planning::OpAssign, formalism::FluentTag>>,
                                               Index<formalism::planning::NumericEffect<formalism::planning::OpIncrease, formalism::FluentTag>>,
                                               Index<formalism::planning::NumericEffect<formalism::planning::OpDecrease, formalism::FluentTag>>,
                                               Index<formalism::planning::NumericEffect<formalism::planning::OpScaleUp, formalism::FluentTag>>,
                                               Index<formalism::planning::NumericEffect<formalism::planning::OpScaleDown, formalism::FluentTag>>,
                                               Index<formalism::planning::NumericEffect<formalism::planning::OpIncrease, formalism::AuxiliaryTag>>>;

using IndexGroundNumericConstraintVariant =
    std::variant<Index<formalism::planning::BinaryOperator<formalism::OpEq, Data<formalism::planning::GroundFunctionExpression>>>,
                 Index<formalism::planning::BinaryOperator<formalism::OpLe, Data<formalism::planning::GroundFunctionExpression>>>,
                 Index<formalism::planning::BinaryOperator<formalism::OpLt, Data<formalism::planning::GroundFunctionExpression>>>,
                 Index<formalism::planning::BinaryOperator<formalism::OpGe, Data<formalism::planning::GroundFunctionExpression>>>,
                 Index<formalism::planning::BinaryOperator<formalism::OpGt, Data<formalism::planning::GroundFunctionExpression>>>>;

struct ArityVisitor
{
    loki::VariableSet variables;

    void collect_variables(loki::Term term)
    {
        std::visit(
            [&](auto&& arg)
            {
                using Variant = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Variant, loki::Object>) {}
                else if constexpr (std::is_same_v<Variant, loki::Variable>)
                    this->variables.insert(arg);
                else
                    static_assert(dependent_false<Variant>::value, "Missing case for type");
            },
            term->get_object_or_variable());
    }

    void collect_variables(loki::Function element)
    {
        for (const auto& term : element->get_terms())
            collect_variables(term);
    }
    void collect_variables(loki::FunctionExpressionNumber element) {}
    void collect_variables(loki::FunctionExpressionBinaryOperator element)
    {
        collect_variables(element->get_left_function_expression());
        collect_variables(element->get_right_function_expression());
    }
    void collect_variables(loki::FunctionExpressionMultiOperator element)
    {
        for (const auto& fexpr : element->get_function_expressions())
            collect_variables(fexpr);
    }
    void collect_variables(loki::FunctionExpressionMinus element) { collect_variables(element->get_function_expression()); }
    void collect_variables(loki::FunctionExpressionFunction element) { collect_variables(element->get_function()); }
    void collect_variables(loki::FunctionExpression element)
    {
        std::visit([this](auto&& arg) { this->collect_variables(arg); }, element->get_function_expression());
    }

    void collect_variables(loki::ConditionLiteral element)
    {
        for (const auto& term : element->get_literal()->get_atom()->get_terms())
            collect_variables(term);
    }
    void collect_variables(loki::ConditionAnd element)
    {
        for (const auto& condition : element->get_conditions())
            collect_variables(condition);
    }
    void collect_variables(loki::ConditionOr element)
    {
        for (const auto& condition : element->get_conditions())
            collect_variables(condition);
    }
    void collect_variables(loki::ConditionNot element) { collect_variables(element->get_condition()); }
    void collect_variables(loki::ConditionImply element)
    {
        collect_variables(element->get_left_condition());
        collect_variables(element->get_right_condition());
    }
    void collect_variables(loki::ConditionExists element)
    {
        for (const auto& parameter : element->get_parameters())
            variables.insert(parameter->get_variable());
        collect_variables(element->get_condition());
    }
    void collect_variables(loki::ConditionForall element)
    {
        for (const auto& parameter : element->get_parameters())
            variables.insert(parameter->get_variable());
        collect_variables(element->get_condition());
    }
    void collect_variables(loki::ConditionNumericConstraint element)
    {
        collect_variables(element->get_left_function_expression());
        collect_variables(element->get_right_function_expression());
    }
    void collect_variables(loki::Condition element)
    {
        std::visit([this](auto&& arg) { this->collect_variables(arg); }, element->get_condition());
    }

    size_t get(loki::Condition element)
    {
        collect_variables(element);
        return variables.size();
    }

    size_t get(loki::ConditionNumericConstraint element)
    {
        collect_variables(element);
        return variables.size();
    }
};

class LokiToTyrTranslator
{
private:
    /* Computed in prepare step */

    // For type analysis of predicates.
    std::unordered_set<std::string> m_fluent_predicates;   ///< Fluent predicates that appear in an effect
    std::unordered_set<std::string> m_derived_predicates;  ///< Derived predicates

    // For type analysis of functions.
    std::unordered_set<std::string> m_fexpr_functions;            ///< Functions that appear in a lifted function expression, i.e., numeric effect or constraint
    std::unordered_set<std::string> m_effect_function_skeletons;  ///< Functions that appear in an effect

    template<std::ranges::input_range Range>
    void prepare(const Range& range)
    {
        std::for_each(std::begin(range), std::end(range), [&](auto&& arg) { this->prepare(arg); });
    }
    template<typename T>
    void prepare(const std::optional<T>& element)
    {
        if (element.has_value())
        {
            this->prepare(element.value());
        }
    }
    void prepare(loki::FunctionSkeleton element);
    void prepare(loki::Object element);
    void prepare(loki::Parameter element);
    void prepare(loki::Predicate element);
    void prepare(loki::Requirements element);
    void prepare(loki::Type element);
    void prepare(loki::Variable element);
    void prepare(loki::Term element);
    void prepare(loki::Atom element);
    void prepare(loki::Literal element);
    void prepare(loki::FunctionExpressionNumber element);
    void prepare(loki::FunctionExpressionBinaryOperator element);
    void prepare(loki::FunctionExpressionMultiOperator element);
    void prepare(loki::FunctionExpressionMinus element);
    void prepare(loki::FunctionExpressionFunction element);
    void prepare(loki::FunctionExpression element);
    void prepare(loki::Function element);
    void prepare(loki::Condition element);
    void prepare(loki::Effect element);
    void prepare(loki::Action element);
    void prepare(loki::Axiom element);
    void prepare(loki::Domain element);
    void prepare(loki::FunctionValue element);
    void prepare(loki::OptimizationMetric element);
    void prepare(loki::Problem element);

    /**
     * Common translations.
     */

    struct ParameterIndexMapping
    {
        ParameterIndexMapping() = default;

        UnorderedMap<Index<formalism::Variable>, formalism::ParameterIndex> map;

        void push_parameters(const IndexList<formalism::Variable>& parameters)
        {
            for (const auto parameter : parameters)
                map.emplace(parameter, map.size());
        }

        void pop_parameters(const IndexList<formalism::Variable>& parameters)
        {
            for (const auto parameter : parameters)
                map.erase(parameter);
        }

        formalism::ParameterIndex lookup_parameter_index(Index<formalism::Variable> variable) { return map.at(variable); }
    };

    ParameterIndexMapping m_param_map;

    template<typename T>
    auto translate_common(const std::vector<const T*>& input, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        using ReturnType = decltype(this->translate_common(std::declval<const T*>(), builder, context));
        auto output = ::cista::offset::vector<ReturnType> {};
        output.reserve(input.size());
        std::transform(std::begin(input),
                       std::end(input),
                       std::back_inserter(output),
                       [&](auto&& arg) { return this->translate_common(arg, builder, context); });
        return output;
    }

    IndexFunctionVariant translate_common(loki::FunctionSkeleton element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto build_function = [&](auto fact_tag) -> IndexFunctionVariant
        {
            using Tag = std::decay_t<decltype(fact_tag)>;

            auto function_ptr = builder.template get_builder<formalism::Function<Tag>>();
            auto& function = *function_ptr;
            function.clear();
            function.name = element->get_name();
            function.arity = element->get_parameters().size();
            formalism::canonicalize(function);
            return context.get_or_create(function, builder.get_buffer()).first;
        };

        if (element->get_name() == "total-cost")
            return build_function(formalism::AuxiliaryTag {});
        else if (m_effect_function_skeletons.contains(element->get_name()))
            return build_function(formalism::FluentTag {});
        else
            return build_function(formalism::StaticTag {});
    }

    Index<formalism::Object> translate_common(loki::Object element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto object_ptr = builder.template get_builder<formalism::Object>();
        auto& object = *object_ptr;
        object.clear();
        object.name = element->get_name();
        formalism::canonicalize(object);
        return context.get_or_create(object, builder.get_buffer()).first;
    }

    Index<formalism::Variable> translate_common(loki::Parameter element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        return translate_common(element->get_variable(), builder, context);
    }

    IndexPredicateVariant translate_common(loki::Predicate element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto build_predicate = [&](auto fact_tag) -> IndexPredicateVariant
        {
            using Tag = std::decay_t<decltype(fact_tag)>;

            auto predicate_ptr = builder.template get_builder<formalism::Predicate<Tag>>();
            auto& predicate = *predicate_ptr;
            predicate.clear();
            predicate.name = element->get_name();
            predicate.arity = element->get_parameters().size();
            formalism::canonicalize(predicate);
            return context.get_or_create(predicate, builder.get_buffer()).first;
        };

        if (m_fluent_predicates.count(element->get_name()) && !m_derived_predicates.count(element->get_name()))
            return build_predicate(formalism::FluentTag {});
        else if (m_derived_predicates.count(element->get_name()))
            return build_predicate(formalism::DerivedTag {});
        else
            return build_predicate(formalism::StaticTag {});
    }

    Index<formalism::Variable> translate_common(loki::Variable element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto variable_ptr = builder.template get_builder<formalism::Variable>();
        auto& variable = *variable_ptr;
        variable.clear();
        variable.name = element->get_name();
        formalism::canonicalize(variable);
        return context.get_or_create(variable, builder.get_buffer()).first;
    }

    /**
     * Lifted translation.
     */

    template<typename T>
    auto translate_lifted(const std::vector<const T*>& input, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        using ReturnType = decltype(this->translate_lifted(std::declval<const T*>(), builder, context));
        auto output = ::cista::offset::vector<ReturnType> {};
        output.reserve(input.size());
        std::transform(std::begin(input),
                       std::end(input),
                       std::back_inserter(output),
                       [&](auto&& arg) { return this->translate_lifted(arg, builder, context); });
        return output;
    }

    Data<formalism::Term> translate_lifted(loki::Term element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        return std::visit(
            [&](auto&& arg) -> Data<formalism::Term>
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, loki::Object>)
                    return Data<formalism::Term>(translate_common(arg, builder, context));
                else if constexpr (std::is_same_v<T, loki::Variable>)
                    return Data<formalism::Term>(m_param_map.lookup_parameter_index(translate_common(arg, builder, context)));
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            element->get_object_or_variable());
    }

    IndexAtomVariant translate_lifted(loki::Atom element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto index_predicate_variant = translate_common(element->get_predicate(), builder, context);

        auto build_atom = [&](auto fact_tag, auto predicate_index) -> IndexAtomVariant
        {
            using Tag = std::decay_t<decltype(fact_tag)>;

            auto atom_ptr = builder.template get_builder<formalism::planning::Atom<Tag>>();
            auto& atom = *atom_ptr;
            atom.clear();
            atom.predicate = predicate_index;
            atom.terms = this->translate_lifted(element->get_terms(), builder, context);
            formalism::planning::canonicalize(atom);
            return context.get_or_create(atom, builder.get_buffer()).first;
        };

        return std::visit(
            [&](auto&& arg) -> IndexAtomVariant
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Index<formalism::Predicate<formalism::StaticTag>>>)
                    return build_atom(formalism::StaticTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<formalism::Predicate<formalism::FluentTag>>>)
                    return build_atom(formalism::FluentTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<formalism::Predicate<formalism::DerivedTag>>>)
                    return build_atom(formalism::DerivedTag {}, arg);
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_predicate_variant);
    }

    IndexLiteralVariant translate_lifted(loki::Literal element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto index_atom_variant = translate_lifted(element->get_atom(), builder, context);

        auto build_literal = [&](auto fact_tag, auto atom_index) -> IndexLiteralVariant
        {
            using Tag = std::decay_t<decltype(fact_tag)>;

            auto literal_ptr = builder.template get_builder<formalism::planning::Literal<Tag>>();
            auto& literal = *literal_ptr;
            literal.clear();
            literal.atom = atom_index;
            literal.polarity = element->get_polarity();
            formalism::planning::canonicalize(literal);
            return context.get_or_create(literal, builder.get_buffer()).first;
        };

        return std::visit(
            [&](auto&& arg) -> IndexLiteralVariant
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Index<formalism::planning::Atom<formalism::StaticTag>>>)
                    return build_literal(formalism::StaticTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<formalism::planning::Atom<formalism::FluentTag>>>)
                    return build_literal(formalism::FluentTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<formalism::planning::Atom<formalism::DerivedTag>>>)
                    return build_literal(formalism::DerivedTag {}, arg);
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_atom_variant);
    }

    Data<formalism::planning::FunctionExpression>
    translate_lifted(loki::FunctionExpressionNumber element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        return Data<formalism::planning::FunctionExpression>(float_t(element->get_number()));
    }

    Data<formalism::planning::FunctionExpression>
    translate_lifted(loki::FunctionExpressionBinaryOperator element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto build_binary_op = [&](auto op_tag) -> Data<formalism::planning::FunctionExpression>
        {
            using Tag = std::decay_t<decltype(op_tag)>;

            auto binary_ptr = builder.template get_builder<formalism::planning::BinaryOperator<Tag, Data<formalism::planning::FunctionExpression>>>();
            auto& binary = *binary_ptr;
            binary.clear();
            binary.lhs = translate_lifted(element->get_left_function_expression(), builder, context);
            binary.rhs = translate_lifted(element->get_right_function_expression(), builder, context);
            formalism::planning::canonicalize(binary);
            return Data<formalism::planning::FunctionExpression>(Data<formalism::planning::ArithmeticOperator<Data<formalism::planning::FunctionExpression>>>(
                context.get_or_create(binary, builder.get_buffer()).first));
        };

        switch (element->get_binary_operator())
        {
            case loki::BinaryOperatorEnum::PLUS:
                return build_binary_op(formalism::OpAdd {});
            case loki::BinaryOperatorEnum::MINUS:
                return build_binary_op(formalism::OpSub {});
            case loki::BinaryOperatorEnum::MUL:
                return build_binary_op(formalism::OpMul {});
            case loki::BinaryOperatorEnum::DIV:
                return build_binary_op(formalism::OpDiv {});
            default:
                throw std::runtime_error("Unexpected case");
        }
    }

    Data<formalism::planning::FunctionExpression>
    translate_lifted(loki::FunctionExpressionMultiOperator element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto build_multi_op = [&](auto op_tag) -> Data<formalism::planning::FunctionExpression>
        {
            using Tag = std::decay_t<decltype(op_tag)>;

            auto multi_ptr = builder.template get_builder<formalism::planning::MultiOperator<Tag, Data<formalism::planning::FunctionExpression>>>();
            auto& multi = *multi_ptr;
            multi.clear();
            multi.args = translate_lifted(element->get_function_expressions(), builder, context);
            formalism::planning::canonicalize(multi);
            return Data<formalism::planning::FunctionExpression>(Data<formalism::planning::ArithmeticOperator<Data<formalism::planning::FunctionExpression>>>(
                context.get_or_create(multi, builder.get_buffer()).first));
        };

        switch (element->get_multi_operator())
        {
            case loki::MultiOperatorEnum::PLUS:
                return build_multi_op(formalism::OpAdd {});
            case loki::MultiOperatorEnum::MUL:
                return build_multi_op(formalism::OpMul {});
            default:
                throw std::runtime_error("Unexpected case");
        }
    }

    Data<formalism::planning::FunctionExpression>
    translate_lifted(loki::FunctionExpressionMinus element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto minus_ptr = builder.template get_builder<formalism::planning::UnaryOperator<formalism::OpSub, Data<formalism::planning::FunctionExpression>>>();
        auto& minus = *minus_ptr;
        minus.clear();
        minus.arg = translate_lifted(element->get_function_expression(), builder, context);
        formalism::planning::canonicalize(minus);
        return Data<formalism::planning::FunctionExpression>(Data<formalism::planning::ArithmeticOperator<Data<formalism::planning::FunctionExpression>>>(
            context.get_or_create(minus, builder.get_buffer()).first));
    }

    Data<formalism::planning::FunctionExpression>
    translate_lifted(loki::FunctionExpressionFunction element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        const auto index_fterm_variant = translate_lifted(element->get_function(), builder, context);

        return std::visit(
            [&](auto&& arg) -> Data<formalism::planning::FunctionExpression>
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, Index<formalism::planning::FunctionTerm<formalism::StaticTag>>>)
                    return Data<formalism::planning::FunctionExpression>(arg);
                else if constexpr (std::is_same_v<T, Index<formalism::planning::FunctionTerm<formalism::FluentTag>>>)
                    return Data<formalism::planning::FunctionExpression>(arg);
                else if constexpr (std::is_same_v<T, Index<formalism::planning::FunctionTerm<formalism::AuxiliaryTag>>>)
                    throw std::runtime_error("Cannot create FunctionExpression over auxiliary function term.");
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_fterm_variant);
    }

    Data<formalism::planning::FunctionExpression>
    translate_lifted(loki::FunctionExpression element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        return std::visit([&](auto&& arg) { return translate_lifted(arg, builder, context); }, element->get_function_expression());
    }

    IndexFunctionTermVariant translate_lifted(loki::Function element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto index_function_variant = translate_common(element->get_function_skeleton(), builder, context);

        auto build_function_term = [&](auto fact_tag, auto function_index) -> IndexFunctionTermVariant
        {
            using Tag = std::decay_t<decltype(fact_tag)>;

            auto fterm_ptr = builder.template get_builder<formalism::planning::FunctionTerm<Tag>>();
            auto& fterm = *fterm_ptr;
            fterm.clear();
            fterm.function = function_index;
            fterm.terms = this->translate_lifted(element->get_terms(), builder, context);
            formalism::planning::canonicalize(fterm);
            return context.get_or_create(fterm, builder.get_buffer()).first;
        };

        return std::visit(
            [&](auto&& arg) -> IndexFunctionTermVariant
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, Index<formalism::Function<formalism::StaticTag>>>)
                    return build_function_term(formalism::StaticTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<formalism::Function<formalism::FluentTag>>>)
                    return build_function_term(formalism::FluentTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<formalism::Function<formalism::AuxiliaryTag>>>)
                    return build_function_term(formalism::AuxiliaryTag {}, arg);
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_function_variant);
    }

    Data<formalism::planning::BooleanOperator<Data<formalism::planning::FunctionExpression>>>
    translate_lifted(loki::ConditionNumericConstraint element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto build_binary_op = [&](auto op_tag) -> Data<formalism::planning::BooleanOperator<Data<formalism::planning::FunctionExpression>>>
        {
            using Tag = std::decay_t<decltype(op_tag)>;

            auto binary_ptr = builder.template get_builder<formalism::planning::BinaryOperator<Tag, Data<formalism::planning::FunctionExpression>>>();
            auto& binary = *binary_ptr;
            binary.clear();
            binary.lhs = translate_lifted(element->get_left_function_expression(), builder, context);
            binary.rhs = translate_lifted(element->get_right_function_expression(), builder, context);
            formalism::planning::canonicalize(binary);
            return Data<formalism::planning::BooleanOperator<Data<formalism::planning::FunctionExpression>>>(
                context.get_or_create(binary, builder.get_buffer()).first);
        };

        switch (element->get_binary_comparator())
        {
            case loki::BinaryComparatorEnum::EQUAL:
                return build_binary_op(formalism::OpEq {});
            case loki::BinaryComparatorEnum::UNEQUAL:
                return build_binary_op(formalism::OpNe {});
            case loki::BinaryComparatorEnum::LESS_EQUAL:
                return build_binary_op(formalism::OpLe {});
            case loki::BinaryComparatorEnum::LESS:
                return build_binary_op(formalism::OpLt {});
            case loki::BinaryComparatorEnum::GREATER_EQUAL:
                return build_binary_op(formalism::OpGe {});
            case loki::BinaryComparatorEnum::GREATER:
                return build_binary_op(formalism::OpGt {});
            default:
                throw std::runtime_error("Unexpected case");
        }
    }

    Index<formalism::planning::ConjunctiveCondition> translate_lifted(loki::Condition element,
                                                                      const IndexList<formalism::Variable>& parameters,
                                                                      formalism::planning::Builder& builder,
                                                                      formalism::planning::Repository& context)
    {
        auto conj_condition_ptr = builder.template get_builder<formalism::planning::ConjunctiveCondition>();
        auto& conj_condition = *conj_condition_ptr;
        conj_condition.clear();

        conj_condition.variables = parameters;

        const auto func_insert_literal = [](IndexLiteralVariant index_literal_variant,
                                            IndexList<formalism::planning::Literal<formalism::StaticTag>>& static_literals,
                                            IndexList<formalism::planning::Literal<formalism::FluentTag>>& fluent_literals,
                                            IndexList<formalism::planning::Literal<formalism::DerivedTag>>& derived_literals)
        {
            std::visit(
                [&](auto&& arg)
                {
                    using T = std::decay_t<decltype(arg)>;

                    if constexpr (std::is_same_v<T, Index<formalism::planning::Literal<formalism::StaticTag>>>)
                        static_literals.push_back(arg);
                    else if constexpr (std::is_same_v<T, Index<formalism::planning::Literal<formalism::FluentTag>>>)
                        fluent_literals.push_back(arg);
                    else if constexpr (std::is_same_v<T, Index<formalism::planning::Literal<formalism::DerivedTag>>>)
                        derived_literals.push_back(arg);
                    else
                        static_assert(dependent_false<T>::value, "Missing case for type");
                },
                index_literal_variant);
        };

        return std::visit(
            [&](auto&& condition) -> Index<formalism::planning::ConjunctiveCondition>
            {
                using ConditionT = std::decay_t<decltype(condition)>;

                if constexpr (std::is_same_v<ConditionT, loki::ConditionAnd>)
                {
                    for (const auto& part : condition->get_conditions())
                    {
                        std::visit(
                            [&](auto&& subcondition)
                            {
                                using SubConditionT = std::decay_t<decltype(subcondition)>;

                                if constexpr (std::is_same_v<SubConditionT, loki::ConditionLiteral>)
                                {
                                    const auto index_literal_variant = translate_lifted(subcondition->get_literal(), builder, context);

                                    func_insert_literal(index_literal_variant,
                                                        conj_condition.static_literals,
                                                        conj_condition.fluent_literals,
                                                        conj_condition.derived_literals);
                                }
                                else if constexpr (std::is_same_v<SubConditionT, loki::ConditionNumericConstraint>)
                                {
                                    const auto numeric_constraint = translate_lifted(subcondition, builder, context);

                                    conj_condition.numeric_constraints.push_back(numeric_constraint);
                                }
                                else
                                {
                                    // std::cout << std::visit([](auto&& arg) { return arg.str(); }, *part) << std::endl;
                                    throw std::logic_error("Unexpected condition.");
                                }
                            },
                            part->get_condition());
                    }

                    formalism::planning::canonicalize(conj_condition);
                    return context.get_or_create(conj_condition, builder.get_buffer()).first;
                }
                else if constexpr (std::is_same_v<ConditionT, loki::ConditionLiteral>)
                {
                    const auto index_literal_variant = translate_lifted(condition->get_literal(), builder, context);

                    func_insert_literal(index_literal_variant, conj_condition.static_literals, conj_condition.fluent_literals, conj_condition.derived_literals);

                    formalism::planning::canonicalize(conj_condition);
                    return context.get_or_create(conj_condition, builder.get_buffer()).first;
                }
                else if constexpr (std::is_same_v<ConditionT, loki::ConditionNumericConstraint>)
                {
                    const auto numeric_constraint = translate_lifted(condition, builder, context);

                    conj_condition.numeric_constraints.push_back(numeric_constraint);

                    formalism::planning::canonicalize(conj_condition);
                    return context.get_or_create(conj_condition, builder.get_buffer()).first;
                }
                else
                {
                    // std::cout << std::visit([](auto&& arg) { return arg.str(); }, *part) << std::endl;
                    throw std::logic_error("Unexpected condition.");
                }
            },
            element->get_condition());
    }

    IndexNumericEffectVariant translate_lifted(loki::EffectNumeric element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto index_fterm_variant = translate_lifted(element->get_function(), builder, context);

        auto build_numeric_effect_term_helper = [&](auto fact_tag, auto op_tag, auto fterm_index) -> IndexNumericEffectVariant
        {
            using Tag = std::decay_t<decltype(fact_tag)>;
            using Op = std::decay_t<decltype(op_tag)>;

            auto numeric_effect_ptr = builder.template get_builder<formalism::planning::NumericEffect<Op, Tag>>();
            auto& numeric_effect = *numeric_effect_ptr;
            numeric_effect.clear();

            numeric_effect.fterm = fterm_index;
            numeric_effect.fexpr = this->translate_lifted(element->get_function_expression(), builder, context);
            formalism::planning::canonicalize(numeric_effect);
            return context.get_or_create(numeric_effect, builder.get_buffer()).first;
        };

        auto build_numeric_effect_term = [&](auto fact_tag, auto fterm_index) -> IndexNumericEffectVariant
        {
            using Tag = std::decay_t<decltype(fact_tag)>;

            if constexpr (std::is_same_v<Tag, formalism::AuxiliaryTag>)
            {
                if (element->get_assign_operator() != loki::AssignOperatorEnum::INCREASE)
                    throw std::runtime_error("Auxiliary numeric effect must use INCREASE operator.");

                return build_numeric_effect_term_helper(Tag {}, formalism::planning::OpIncrease {}, fterm_index);
            }
            else
            {
                switch (element->get_assign_operator())
                {
                    case loki::AssignOperatorEnum::ASSIGN:
                        return build_numeric_effect_term_helper(Tag {}, formalism::planning::OpAssign {}, fterm_index);
                    case loki::AssignOperatorEnum::INCREASE:
                        return build_numeric_effect_term_helper(Tag {}, formalism::planning::OpIncrease {}, fterm_index);
                    case loki::AssignOperatorEnum::DECREASE:
                        return build_numeric_effect_term_helper(Tag {}, formalism::planning::OpDecrease {}, fterm_index);
                    case loki::AssignOperatorEnum::SCALE_UP:
                        return build_numeric_effect_term_helper(Tag {}, formalism::planning::OpScaleUp {}, fterm_index);
                    case loki::AssignOperatorEnum::SCALE_DOWN:
                        return build_numeric_effect_term_helper(Tag {}, formalism::planning::OpScaleDown {}, fterm_index);
                    default:
                        throw std::runtime_error("Unexpected case.");
                }
            }
        };

        return std::visit(
            [&](auto&& arg) -> IndexNumericEffectVariant
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, Index<formalism::planning::FunctionTerm<formalism::StaticTag>>>)
                    throw std::runtime_error("Cannot create NumericEffect over static function term.");
                else if constexpr (std::is_same_v<T, Index<formalism::planning::FunctionTerm<formalism::FluentTag>>>)
                    return build_numeric_effect_term(formalism::FluentTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<formalism::planning::FunctionTerm<formalism::AuxiliaryTag>>>)
                    return build_numeric_effect_term(formalism::AuxiliaryTag {}, arg);
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_fterm_variant);
    }

    IndexList<formalism::planning::ConditionalEffect> translate_lifted(loki::Effect element,
                                                                       const IndexList<formalism::Variable>& parameters,
                                                                       formalism::planning::Builder& builder,
                                                                       formalism::planning::Repository& context)
    {
        using ConditionalEffectData = UnorderedMap<Index<formalism::planning::ConjunctiveCondition>,
                                                   std::tuple<IndexList<formalism::Variable>,
                                                              IndexList<formalism::planning::Literal<formalism::FluentTag>>,
                                                              DataList<formalism::planning::NumericEffectOperator<formalism::FluentTag>>,
                                                              ::cista::optional<Data<formalism::planning::NumericEffectOperator<formalism::AuxiliaryTag>>>>>;

        const auto translate_effect_func = [&](loki::Effect effect, ConditionalEffectData& ref_conditional_effect_data)
        {
            auto tmp_effect = effect;

            auto universal_parameters = IndexList<formalism::Variable> {};

            /* 1. Parse universal part. */

            std::visit(
                [&](auto&& subeffect)
                {
                    using SubEffectT = std::decay_t<decltype(subeffect)>;

                    if constexpr (std::is_same_v<SubEffectT, loki::EffectCompositeForall>)
                    {
                        universal_parameters = translate_common(subeffect->get_parameters(), builder, context);

                        tmp_effect = subeffect->get_effect();
                    }
                },
                tmp_effect->get_effect());

            ///---------- Push parameters and parse scope -------------
            m_param_map.push_parameters(universal_parameters);
            {
                /* 2. Parse conditional part */
                auto conjunctive_condition = std::visit(
                    [&](auto&& subeffect)
                    {
                        using SubEffectT = std::decay_t<decltype(subeffect)>;

                        auto all_parameters = parameters;
                        all_parameters.insert(all_parameters.end(), universal_parameters.begin(), universal_parameters.end());

                        if constexpr (std::is_same_v<SubEffectT, loki::EffectCompositeWhen>)
                        {
                            auto conjunctive_condition = translate_lifted(subeffect->get_condition(), all_parameters, builder, context);

                            tmp_effect = subeffect->get_effect();

                            return conjunctive_condition;
                        }
                        else
                        {
                            // Create empty conjunctive condition for unconditional effects
                            auto conj_cond_ptr = builder.template get_builder<formalism::planning::ConjunctiveCondition>();
                            auto& conj_cond = *conj_cond_ptr;
                            conj_cond.clear();
                            formalism::planning::canonicalize(conj_cond);
                            return context.get_or_create(conj_cond, builder.get_buffer()).first;
                        }
                    },
                    tmp_effect->get_effect());

                // Fetch container to store the effects
                auto& effect_data = ref_conditional_effect_data[conjunctive_condition];
                auto& stored_universal = std::get<0>(effect_data);
                if (stored_universal.empty())
                    stored_universal = universal_parameters;
                else
                    assert(stored_universal.size() == universal_parameters.size() && "Same guard but different forall-scope.");
                auto& data_fluent_literals = std::get<1>(effect_data);
                auto& data_fluent_numeric_effects = std::get<2>(effect_data);
                auto& data_auxiliary_numeric_effect = std::get<3>(effect_data);

                /* 3. Parse effect part */
                std::visit(
                    [&](auto&& subeffect)
                    {
                        using SubEffectT = std::decay_t<decltype(subeffect)>;

                        if constexpr (std::is_same_v<SubEffectT, loki::EffectLiteral>)
                        {
                            const auto index_literal_variant = translate_lifted(subeffect->get_literal(), builder, context);

                            std::visit(
                                [&](auto&& subsubeffect)
                                {
                                    using SubSubEffectT = std::decay_t<decltype(subsubeffect)>;

                                    if constexpr (std::is_same_v<SubSubEffectT, Index<formalism::planning::Literal<formalism::StaticTag>>>)
                                        throw std::logic_error("Effect lieral cannot be Static!");
                                    else if constexpr (std::is_same_v<SubSubEffectT, Index<formalism::planning::Literal<formalism::FluentTag>>>)
                                        data_fluent_literals.push_back(subsubeffect);
                                    else if constexpr (std::is_same_v<SubSubEffectT, Index<formalism::planning::Literal<formalism::DerivedTag>>>)
                                        throw std::runtime_error("Effect literal cannot be Derived!");
                                    else
                                        static_assert(dependent_false<SubSubEffectT>::value, "Unexpected case.");
                                },
                                index_literal_variant);
                        }
                        else if constexpr (std::is_same_v<SubEffectT, loki::EffectNumeric>)
                        {
                            const auto index_numeric_effect_variant = translate_lifted(subeffect, builder, context);

                            std::visit(
                                [&](auto&& subsubeffect)
                                {
                                    using SubSubEffectT = std::decay_t<decltype(subsubeffect)>;

                                    if constexpr (std::is_same_v<
                                                      SubSubEffectT,
                                                      Index<formalism::planning::NumericEffect<formalism::planning::OpAssign, formalism::FluentTag>>>)
                                        data_fluent_numeric_effects.push_back(
                                            Data<formalism::planning::NumericEffectOperator<formalism::FluentTag>>(subsubeffect));
                                    else if constexpr (std::is_same_v<
                                                           SubSubEffectT,
                                                           Index<formalism::planning::NumericEffect<formalism::planning::OpIncrease, formalism::FluentTag>>>)
                                        data_fluent_numeric_effects.push_back(
                                            Data<formalism::planning::NumericEffectOperator<formalism::FluentTag>>(subsubeffect));
                                    else if constexpr (std::is_same_v<
                                                           SubSubEffectT,
                                                           Index<formalism::planning::NumericEffect<formalism::planning::OpDecrease, formalism::FluentTag>>>)
                                        data_fluent_numeric_effects.push_back(
                                            Data<formalism::planning::NumericEffectOperator<formalism::FluentTag>>(subsubeffect));
                                    else if constexpr (std::is_same_v<
                                                           SubSubEffectT,
                                                           Index<formalism::planning::NumericEffect<formalism::planning::OpScaleUp, formalism::FluentTag>>>)
                                        data_fluent_numeric_effects.push_back(
                                            Data<formalism::planning::NumericEffectOperator<formalism::FluentTag>>(subsubeffect));
                                    else if constexpr (std::is_same_v<
                                                           SubSubEffectT,
                                                           Index<formalism::planning::NumericEffect<formalism::planning::OpScaleDown, formalism::FluentTag>>>)
                                        data_fluent_numeric_effects.push_back(
                                            Data<formalism::planning::NumericEffectOperator<formalism::FluentTag>>(subsubeffect));
                                    else if constexpr (std::is_same_v<
                                                           SubSubEffectT,
                                                           Index<formalism::planning::NumericEffect<formalism::planning::OpIncrease, formalism::AuxiliaryTag>>>)
                                    {
                                        assert(!data_auxiliary_numeric_effect);
                                        data_auxiliary_numeric_effect = Data<formalism::planning::NumericEffectOperator<formalism::AuxiliaryTag>>(subsubeffect);
                                    }
                                    else
                                        static_assert(dependent_false<SubSubEffectT>::value, "Unexpected case.");
                                },
                                index_numeric_effect_variant);
                        }
                        else
                        {
                            throw std::runtime_error("Unexpected effect");
                        }
                    },
                    tmp_effect->get_effect());
            }
            ///---------- Pop parameters -------------
            m_param_map.pop_parameters(universal_parameters);
        };

        /* Parse the effect */
        auto conditional_effect_data = ConditionalEffectData {};
        // Parse conjunctive part
        std::visit(
            [&](auto&& effect)
            {
                using EffectT = std::decay_t<decltype(effect)>;

                if constexpr (std::is_same_v<EffectT, loki::EffectAnd>)
                {
                    for (const auto& nested_effect : effect->get_effects())
                    {
                        translate_effect_func(nested_effect, conditional_effect_data);
                    }
                }
                else
                {
                    translate_effect_func(element, conditional_effect_data);
                }
            },
            element->get_effect());

        /* Instantiate conditional effects. */
        auto conditional_effects = IndexList<formalism::planning::ConditionalEffect> {};

        for (const auto& [cond_conjunctive_condition, value] : conditional_effect_data)
        {
            const auto& [cond_effect_universal_parameters,
                         cond_effect_fluent_literals,
                         cond_effect_fluent_numeric_effects,
                         cond_effect_auxiliary_numeric_effects] = value;

            auto conj_effect_ptr = builder.template get_builder<formalism::planning::ConjunctiveEffect>();
            auto& conj_effect = *conj_effect_ptr;
            conj_effect.clear();
            conj_effect.literals = cond_effect_fluent_literals;
            conj_effect.numeric_effects = cond_effect_fluent_numeric_effects;
            conj_effect.auxiliary_numeric_effect = cond_effect_auxiliary_numeric_effects;
            formalism::planning::canonicalize(conj_effect);
            const auto conj_effect_index = context.get_or_create(conj_effect, builder.get_buffer()).first;

            auto cond_effect_ptr = builder.template get_builder<formalism::planning::ConditionalEffect>();
            auto& cond_effect = *cond_effect_ptr;
            cond_effect.clear();
            cond_effect.variables = cond_effect_universal_parameters;
            cond_effect.condition = cond_conjunctive_condition;
            cond_effect.effect = conj_effect_index;
            formalism::planning::canonicalize(cond_effect);
            const auto cond_effect_index = context.get_or_create(cond_effect, builder.get_buffer()).first;

            conditional_effects.push_back(cond_effect_index);
        }

        return conditional_effects;
    }

    Index<formalism::planning::Action> translate_lifted(loki::Action element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto action_ptr = builder.template get_builder<formalism::planning::Action>();
        auto& action = *action_ptr;
        action.clear();
        action.original_arity = element->get_original_arity();
        action.name = element->get_name();

        // 1. Translate conditions
        auto parameters = translate_common(element->get_parameters(), builder, context);
        action.variables = parameters;

        ///---------- Push parameters and parse scope -------------
        m_param_map.push_parameters(parameters);
        {
            auto conjunctive_condition = Index<formalism::planning::ConjunctiveCondition>::max();
            if (element->get_condition().has_value())
            {
                conjunctive_condition = translate_lifted(element->get_condition().value(), parameters, builder, context);
            }
            else
            {
                // Create empty one
                auto conj_cond_ptr = builder.template get_builder<formalism::planning::ConjunctiveCondition>();
                auto& conj_cond = *conj_cond_ptr;
                conj_cond.clear();
                formalism::planning::canonicalize(conj_cond);
                conjunctive_condition = context.get_or_create(conj_cond, builder.get_buffer()).first;
            }
            action.condition = conjunctive_condition;

            // 2. Translate effects
            auto conditional_effects = IndexList<formalism::planning::ConditionalEffect> {};
            if (element->get_effect().has_value())
            {
                const auto conditional_effects_ = translate_lifted(element->get_effect().value(), parameters, builder, context);
                conditional_effects = conditional_effects_;
            }
            action.effects = conditional_effects;
        }
        ///---------- Pop parameters -------------
        m_param_map.pop_parameters(parameters);

        formalism::planning::canonicalize(action);
        return context.get_or_create(action, builder.get_buffer()).first;
    }

    Index<formalism::planning::Axiom> translate_lifted(loki::Axiom element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto axiom_ptr = builder.template get_builder<formalism::planning::Axiom>();
        auto& axiom = *axiom_ptr;
        axiom.clear();

        auto parameters = translate_common(element->get_parameters(), builder, context);
        axiom.variables = parameters;

        ///---------- Push parameters and parse scope -------------
        m_param_map.push_parameters(parameters);
        {
            axiom.body = translate_lifted(element->get_condition(), parameters, builder, context);
            const auto index_literal_variant = translate_lifted(element->get_literal(), builder, context);

            std::visit(
                [&](auto&& arg)
                {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, Index<formalism::planning::Literal<formalism::DerivedTag>>>)
                        axiom.head = make_view(arg, context).get_atom().get_index();
                    else
                        throw std::runtime_error("ToMimirStructures::translate_lifted: Expected Literal<DerivedTag> in axiom head.");
                },
                index_literal_variant);
        }
        ///---------- Pop parameters -------------
        m_param_map.pop_parameters(parameters);

        formalism::planning::canonicalize(axiom);
        return context.get_or_create(axiom, builder.get_buffer()).first;
    }

    /**
     * Grounded translation
     */

    template<typename T>
    auto translate_grounded(const std::vector<const T*>& input, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        using ReturnType = decltype(this->translate_grounded(std::declval<const T*>(), builder, context));
        auto output = ::cista::offset::vector<ReturnType> {};
        output.reserve(input.size());
        std::transform(std::begin(input),
                       std::end(input),
                       std::back_inserter(output),
                       [&](auto&& arg) { return this->translate_grounded(arg, builder, context); });
        return output;
    }

    Index<formalism::Object> translate_grounded(loki::Term element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        return std::visit(
            [&](auto&& arg) -> Index<formalism::Object>
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, loki::Object>)
                    return translate_common(arg, builder, context);
                else if constexpr (std::is_same_v<T, loki::Variable>)
                    throw std::runtime_error("Expected ground term.");
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            element->get_object_or_variable());
    }

    Index<formalism::Binding>
    to_binding(const IndexList<formalism::Object>& element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto binding_ptr = builder.get_builder<formalism::Binding>();
        auto& binding = *binding_ptr;
        binding.clear();
        binding.objects = element;
        formalism::canonicalize(binding);
        return context.get_or_create(binding, builder.get_buffer()).first;
    }

    IndexGroundAtomVariant translate_grounded(loki::Atom element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto index_predicate_variant = translate_common(element->get_predicate(), builder, context);

        auto build_atom = [&](auto fact_tag, auto predicate_index)
        {
            using Tag = std::decay_t<decltype(fact_tag)>;

            auto atom_ptr = builder.template get_builder<formalism::planning::GroundAtom<Tag>>();
            auto& atom = *atom_ptr;
            atom.clear();
            atom.predicate = predicate_index;
            atom.objects = this->translate_grounded(element->get_terms(), builder, context);
            formalism::planning::canonicalize(atom);
            return context.get_or_create(atom, builder.get_buffer()).first;
        };

        return std::visit(
            [&](auto&& arg) -> IndexGroundAtomVariant
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Index<formalism::Predicate<formalism::StaticTag>>>)
                    return build_atom(formalism::StaticTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<formalism::Predicate<formalism::FluentTag>>>)
                    return build_atom(formalism::FluentTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<formalism::Predicate<formalism::DerivedTag>>>)
                    return build_atom(formalism::DerivedTag {}, arg);
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_predicate_variant);
    }

    IndexGroundAtomOrFactVariant translate_grounded(loki::Atom element,
                                                    formalism::planning::Builder& builder,
                                                    formalism::planning::Repository& context,
                                                    formalism::planning::BinaryFDRContext& fdr_context)
    {
        auto atom_variant = translate_grounded(element, builder, context);

        return std::visit(
            [&](auto&& arg) -> IndexGroundAtomOrFactVariant
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Index<formalism::planning::GroundAtom<formalism::StaticTag>>>)
                    return arg;
                else if constexpr (std::is_same_v<T, Index<formalism::planning::GroundAtom<formalism::FluentTag>>>)
                    return fdr_context.get_fact(arg);
                else if constexpr (std::is_same_v<T, Index<formalism::planning::GroundAtom<formalism::DerivedTag>>>)
                    return arg;
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            atom_variant);
    }

    IndexGroundLiteralVariant translate_grounded(loki::Literal element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto atom_variant = translate_grounded(element->get_atom(), builder, context);

        auto build_literal = [&](auto fact_tag, auto atom)
        {
            using Tag = std::decay_t<decltype(fact_tag)>;

            auto literal_ptr = builder.template get_builder<formalism::planning::GroundLiteral<Tag>>();
            auto& literal = *literal_ptr;
            literal.clear();
            literal.atom = atom;
            literal.polarity = element->get_polarity();
            formalism::planning::canonicalize(literal);
            return context.get_or_create(literal, builder.get_buffer()).first;
        };

        return std::visit(
            [&](auto&& arg) -> IndexGroundLiteralVariant
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Index<formalism::planning::GroundAtom<formalism::StaticTag>>>)
                    return build_literal(formalism::StaticTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<formalism::planning::GroundAtom<formalism::FluentTag>>>)
                    return build_literal(formalism::FluentTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<formalism::planning::GroundAtom<formalism::DerivedTag>>>)
                    return build_literal(formalism::DerivedTag {}, arg);
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            atom_variant);
    }

    IndexGroundLiteralOrFactVariant translate_grounded(loki::Literal element,
                                                       formalism::planning::Builder& builder,
                                                       formalism::planning::Repository& context,
                                                       formalism::planning::BinaryFDRContext& fdr_context)
    {
        auto literal_variant = translate_grounded(element, builder, context);

        return std::visit(
            [&](auto&& arg) -> IndexGroundLiteralOrFactVariant
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Index<formalism::planning::GroundLiteral<formalism::StaticTag>>>)
                    return arg;
                else if constexpr (std::is_same_v<T, Index<formalism::planning::GroundLiteral<formalism::FluentTag>>>)
                    return fdr_context.get_fact(arg);
                else if constexpr (std::is_same_v<T, Index<formalism::planning::GroundLiteral<formalism::DerivedTag>>>)
                    return arg;
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            literal_variant);
    }

    Data<formalism::planning::GroundFunctionExpression>
    translate_grounded(loki::FunctionExpressionNumber element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        return Data<formalism::planning::GroundFunctionExpression>(float_t(element->get_number()));
    }

    Data<formalism::planning::GroundFunctionExpression>
    translate_grounded(loki::FunctionExpressionBinaryOperator element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto build_binary_op = [&](auto op_tag) -> Data<formalism::planning::GroundFunctionExpression>
        {
            using Tag = std::decay_t<decltype(op_tag)>;

            auto binary_ptr = builder.template get_builder<formalism::planning::BinaryOperator<Tag, Data<formalism::planning::GroundFunctionExpression>>>();
            auto& binary = *binary_ptr;
            binary.clear();
            binary.lhs = translate_grounded(element->get_left_function_expression(), builder, context);
            binary.rhs = translate_grounded(element->get_right_function_expression(), builder, context);
            formalism::planning::canonicalize(binary);
            return Data<formalism::planning::GroundFunctionExpression>(
                Data<formalism::planning::ArithmeticOperator<Data<formalism::planning::GroundFunctionExpression>>>(
                    context.get_or_create(binary, builder.get_buffer()).first));
        };

        switch (element->get_binary_operator())
        {
            case loki::BinaryOperatorEnum::PLUS:
                return build_binary_op(formalism::OpAdd {});
            case loki::BinaryOperatorEnum::MINUS:
                return build_binary_op(formalism::OpSub {});
            case loki::BinaryOperatorEnum::MUL:
                return build_binary_op(formalism::OpMul {});
            case loki::BinaryOperatorEnum::DIV:
                return build_binary_op(formalism::OpDiv {});
            default:
                throw std::runtime_error("Unexpected case");
        }
    }

    Data<formalism::planning::GroundFunctionExpression>
    translate_grounded(loki::FunctionExpressionMultiOperator element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto build_multi_op = [&](auto op_tag) -> Data<formalism::planning::GroundFunctionExpression>
        {
            using Tag = std::decay_t<decltype(op_tag)>;

            auto multi_ptr = builder.template get_builder<formalism::planning::MultiOperator<Tag, Data<formalism::planning::GroundFunctionExpression>>>();
            auto& multi = *multi_ptr;
            multi.clear();
            multi.args = translate_grounded(element->get_function_expressions(), builder, context);
            formalism::planning::canonicalize(multi);
            return Data<formalism::planning::GroundFunctionExpression>(
                Data<formalism::planning::ArithmeticOperator<Data<formalism::planning::GroundFunctionExpression>>>(
                    context.get_or_create(multi, builder.get_buffer()).first));
        };

        switch (element->get_multi_operator())
        {
            case loki::MultiOperatorEnum::PLUS:
                return build_multi_op(formalism::OpAdd {});
            case loki::MultiOperatorEnum::MUL:
                return build_multi_op(formalism::OpMul {});
            default:
                throw std::runtime_error("Unexpected case");
        }
    }

    Data<formalism::planning::GroundFunctionExpression>
    translate_grounded(loki::FunctionExpressionMinus element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto minus_ptr =
            builder.template get_builder<formalism::planning::UnaryOperator<formalism::OpSub, Data<formalism::planning::GroundFunctionExpression>>>();
        auto& minus = *minus_ptr;
        minus.clear();
        minus.arg = translate_grounded(element->get_function_expression(), builder, context);
        formalism::planning::canonicalize(minus);
        return Data<formalism::planning::GroundFunctionExpression>(
            Data<formalism::planning::ArithmeticOperator<Data<formalism::planning::GroundFunctionExpression>>>(
                context.get_or_create(minus, builder.get_buffer()).first));
    }

    Data<formalism::planning::GroundFunctionExpression>
    translate_grounded(loki::FunctionExpressionFunction element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        const auto index_fterm_variant = translate_grounded(element->get_function(), builder, context);

        return std::visit(
            [&](auto&& arg) -> Data<formalism::planning::GroundFunctionExpression>
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, Index<formalism::planning::GroundFunctionTerm<formalism::StaticTag>>>)
                    return Data<formalism::planning::GroundFunctionExpression>(arg);
                else if constexpr (std::is_same_v<T, Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>>>)
                    return Data<formalism::planning::GroundFunctionExpression>(arg);
                else if constexpr (std::is_same_v<T, Index<formalism::planning::GroundFunctionTerm<formalism::AuxiliaryTag>>>)
                    return Data<formalism::planning::GroundFunctionExpression>(arg);
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_fterm_variant);
    }

    Data<formalism::planning::GroundFunctionExpression>
    translate_grounded(loki::FunctionExpression element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        return std::visit([&](auto&& arg) { return translate_grounded(arg, builder, context); }, element->get_function_expression());
    }

    IndexGroundFunctionTermVariant translate_grounded(loki::Function element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto index_function_variant = translate_common(element->get_function_skeleton(), builder, context);

        auto build_function_term = [&](auto fact_tag, auto function_index) -> IndexGroundFunctionTermVariant
        {
            using Tag = std::decay_t<decltype(fact_tag)>;

            auto fterm_ptr = builder.template get_builder<formalism::planning::GroundFunctionTerm<Tag>>();
            auto& fterm = *fterm_ptr;
            fterm.clear();
            fterm.function = function_index;
            fterm.objects = this->translate_grounded(element->get_terms(), builder, context);
            formalism::planning::canonicalize(fterm);
            return context.get_or_create(fterm, builder.get_buffer()).first;
        };

        return std::visit(
            [&](auto&& arg) -> IndexGroundFunctionTermVariant
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Index<formalism::Function<formalism::StaticTag>>>)
                    return build_function_term(formalism::StaticTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<formalism::Function<formalism::FluentTag>>>)
                    return build_function_term(formalism::FluentTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<formalism::Function<formalism::AuxiliaryTag>>>)
                    return build_function_term(formalism::AuxiliaryTag {}, arg);
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_function_variant);
    }

    IndexGroundFunctionTermValueVariant
    translate_grounded(loki::FunctionValue element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto index_fterm_variant = translate_grounded(element->get_function(), builder, context);

        auto build_fterm_value = [&](auto fact_tag, auto fterm_index) -> IndexGroundFunctionTermValueVariant
        {
            using Tag = std::decay_t<decltype(fact_tag)>;

            auto fterm_value_ptr = builder.template get_builder<formalism::planning::GroundFunctionTermValue<Tag>>();
            auto& fterm_value = *fterm_value_ptr;
            fterm_value.clear();
            fterm_value.fterm = fterm_index;
            fterm_value.value = element->get_number();
            formalism::planning::canonicalize(fterm_value);
            return context.get_or_create(fterm_value, builder.get_buffer()).first;
        };

        return std::visit(
            [&](auto&& arg) -> IndexGroundFunctionTermValueVariant
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Index<formalism::planning::GroundFunctionTerm<formalism::StaticTag>>>)
                    return build_fterm_value(formalism::StaticTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>>>)
                    return build_fterm_value(formalism::FluentTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<formalism::planning::GroundFunctionTerm<formalism::AuxiliaryTag>>>)
                    return build_fterm_value(formalism::AuxiliaryTag {}, arg);
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_fterm_variant);
    }

    Data<formalism::planning::BooleanOperator<Data<formalism::planning::GroundFunctionExpression>>>
    translate_grounded(loki::ConditionNumericConstraint element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto build_binary_op = [&](auto op_tag) -> Data<formalism::planning::BooleanOperator<Data<formalism::planning::GroundFunctionExpression>>>
        {
            using Tag = std::decay_t<decltype(op_tag)>;

            auto binary_ptr = builder.template get_builder<formalism::planning::BinaryOperator<Tag, Data<formalism::planning::GroundFunctionExpression>>>();
            auto& binary = *binary_ptr;
            binary.clear();
            binary.lhs = translate_grounded(element->get_left_function_expression(), builder, context);
            binary.rhs = translate_grounded(element->get_right_function_expression(), builder, context);
            formalism::planning::canonicalize(binary);
            return Data<formalism::planning::BooleanOperator<Data<formalism::planning::GroundFunctionExpression>>>(
                context.get_or_create(binary, builder.get_buffer()).first);
        };

        switch (element->get_binary_comparator())
        {
            case loki::BinaryComparatorEnum::EQUAL:
                return build_binary_op(formalism::OpEq {});
            case loki::BinaryComparatorEnum::LESS_EQUAL:
                return build_binary_op(formalism::OpLe {});
            case loki::BinaryComparatorEnum::LESS:
                return build_binary_op(formalism::OpLt {});
            case loki::BinaryComparatorEnum::GREATER_EQUAL:
                return build_binary_op(formalism::OpGe {});
            case loki::BinaryComparatorEnum::GREATER:
                return build_binary_op(formalism::OpGt {});
            default:
                throw std::runtime_error("Unexpected case");
        }
    }

    Index<formalism::planning::GroundConjunctiveCondition> translate_grounded(loki::Condition element,
                                                                              formalism::planning::Builder& builder,
                                                                              formalism::planning::Repository& context,
                                                                              formalism::planning::BinaryFDRContext& fdr_context)
    {
        auto conj_condition_ptr = builder.template get_builder<formalism::planning::GroundConjunctiveCondition>();
        auto& conj_condition = *conj_condition_ptr;
        conj_condition.clear();

        const auto func_insert_literal = [](IndexGroundLiteralOrFactVariant index_literal_variant,
                                            IndexList<formalism::planning::GroundLiteral<formalism::StaticTag>>& static_literals,
                                            DataList<formalism::planning::FDRFact<formalism::FluentTag>>& fluent_facts,
                                            IndexList<formalism::planning::GroundLiteral<formalism::DerivedTag>>& derived_literals)
        {
            std::visit(
                [&](auto&& literal_index)
                {
                    using T = std::decay_t<decltype(literal_index)>;

                    if constexpr (std::is_same_v<T, Index<formalism::planning::GroundLiteral<formalism::StaticTag>>>)
                        static_literals.push_back(literal_index);
                    else if constexpr (std::is_same_v<T, Data<formalism::planning::FDRFact<formalism::FluentTag>>>)
                        fluent_facts.push_back(literal_index);
                    else if constexpr (std::is_same_v<T, Index<formalism::planning::GroundLiteral<formalism::DerivedTag>>>)
                        derived_literals.push_back(literal_index);
                    else
                        static_assert(dependent_false<T>::value, "Missing case for type");
                },
                index_literal_variant);
        };

        return std::visit(
            [&](auto&& condition) -> Index<formalism::planning::GroundConjunctiveCondition>
            {
                using ConditionT = std::decay_t<decltype(condition)>;

                if constexpr (std::is_same_v<ConditionT, loki::ConditionAnd>)
                {
                    for (const auto& part : condition->get_conditions())
                    {
                        std::visit(
                            [&](auto&& subcondition)
                            {
                                using SubConditionT = std::decay_t<decltype(subcondition)>;

                                if constexpr (std::is_same_v<SubConditionT, loki::ConditionLiteral>)
                                {
                                    const auto index_literal_variant = translate_grounded(subcondition->get_literal(), builder, context, fdr_context);

                                    func_insert_literal(index_literal_variant,
                                                        conj_condition.static_literals,
                                                        conj_condition.fluent_facts,
                                                        conj_condition.derived_literals);
                                }
                                else if constexpr (std::is_same_v<SubConditionT, loki::ConditionNumericConstraint>)
                                {
                                    const auto numeric_constraint = translate_grounded(subcondition, builder, context);

                                    conj_condition.numeric_constraints.push_back(numeric_constraint);
                                }
                                else
                                {
                                    // std::cout << std::visit([](auto&& arg) { return arg.str(); }, *part) << std::endl;
                                    throw std::logic_error("Unexpected condition.");
                                }
                            },
                            part->get_condition());
                    }

                    formalism::planning::canonicalize(conj_condition);
                    return context.get_or_create(conj_condition, builder.get_buffer()).first;
                }
                else if constexpr (std::is_same_v<ConditionT, loki::ConditionLiteral>)
                {
                    const auto index_literal_variant = translate_grounded(condition->get_literal(), builder, context, fdr_context);

                    func_insert_literal(index_literal_variant, conj_condition.static_literals, conj_condition.fluent_facts, conj_condition.derived_literals);

                    formalism::planning::canonicalize(conj_condition);
                    return context.get_or_create(conj_condition, builder.get_buffer()).first;
                }
                else if constexpr (std::is_same_v<ConditionT, loki::ConditionNumericConstraint>)
                {
                    const auto numeric_constraint = translate_grounded(condition, builder, context);

                    conj_condition.numeric_constraints.push_back(numeric_constraint);

                    formalism::planning::canonicalize(conj_condition);
                    return context.get_or_create(conj_condition, builder.get_buffer()).first;
                }
                else
                {
                    // std::cout << std::visit([](auto&& arg) { return arg.str(); }, *condition_ptr) << std::endl;
                    throw std::logic_error("Unexpected condition.");
                }
            },
            element->get_condition());
    }

    Index<formalism::planning::Metric>
    translate_grounded(loki::OptimizationMetric element, formalism::planning::Builder& builder, formalism::planning::Repository& context)
    {
        auto metric_ptr = builder.template get_builder<formalism::planning::Metric>();
        auto& metric = *metric_ptr;
        metric.clear();

        metric.fexpr = translate_grounded(element->get_function_expression(), builder, context);
        switch (element->get_optimization_metric())
        {
            case loki::OptimizationMetricEnum::MINIMIZE:
            {
                metric.objective = formalism::planning::Minimize {};
                break;
            }
            case loki::OptimizationMetricEnum::MAXIMIZE:
            {
                metric.objective = formalism::planning::Maximize {};
                break;
            }
            default:
                throw std::runtime_error("Unexpected case.");
        }

        formalism::planning::canonicalize(metric);
        return context.get_or_create(metric, builder.get_buffer()).first;
    }

public:
    LokiToTyrTranslator() = default;

    DomainPtr translate(const loki::Domain& domain, formalism::planning::Builder& builder, formalism::planning::RepositoryPtr context);

    LiftedTaskPtr
    translate(const loki::Problem& problem, formalism::planning::Builder& builder, DomainPtr domain, formalism::planning::RepositoryPtr domain_context);
};

}

#endif