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

#ifndef TYR_SRC_FORMALISM_PLANNING_LOKI_TO_TYR_HPP_
#define TYR_SRC_FORMALISM_PLANNING_LOKI_TO_TYR_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/formalism/planning/builder.hpp"
#include "tyr/formalism/planning/canonicalization.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/fdr_context.hpp"
#include "tyr/formalism/planning/grounder.hpp"
#include "tyr/formalism/planning/planning_domain.hpp"
#include "tyr/formalism/planning/planning_task.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"

#include <loki/loki.hpp>
#include <tuple>
#include <unordered_map>
#include <variant>

namespace tyr::formalism::planning
{

using IndexPredicateVariant = std::variant<Index<Predicate<StaticTag>>, Index<Predicate<FluentTag>>, Index<Predicate<DerivedTag>>>;

using IndexAtomVariant = std::variant<Index<Atom<StaticTag>>, Index<Atom<FluentTag>>, Index<Atom<DerivedTag>>>;

using IndexLiteralVariant = std::variant<Index<Literal<StaticTag>>, Index<Literal<FluentTag>>, Index<Literal<DerivedTag>>>;

using IndexGroundAtomVariant = std::variant<Index<GroundAtom<StaticTag>>, Index<GroundAtom<FluentTag>>, Index<GroundAtom<DerivedTag>>>;

using IndexGroundLiteralVariant = std::variant<Index<GroundLiteral<StaticTag>>, Index<GroundLiteral<FluentTag>>, Index<GroundLiteral<DerivedTag>>>;

using IndexGroundAtomOrFactVariant = std::variant<Index<GroundAtom<StaticTag>>, Data<FDRFact<FluentTag>>, Index<GroundAtom<DerivedTag>>>;

using IndexGroundLiteralOrFactVariant = std::variant<Index<GroundLiteral<StaticTag>>, Data<FDRFact<FluentTag>>, Index<GroundLiteral<DerivedTag>>>;

using IndexFunctionVariant = std::variant<Index<Function<StaticTag>>, Index<Function<FluentTag>>, Index<Function<AuxiliaryTag>>>;

using IndexFunctionTermVariant = std::variant<Index<FunctionTerm<StaticTag>>, Index<FunctionTerm<FluentTag>>, Index<FunctionTerm<AuxiliaryTag>>>;

using IndexGroundFunctionTermVariant =
    std::variant<Index<GroundFunctionTerm<StaticTag>>, Index<GroundFunctionTerm<FluentTag>>, Index<GroundFunctionTerm<AuxiliaryTag>>>;

using IndexGroundFunctionTermValueVariant =
    std::variant<Index<GroundFunctionTermValue<StaticTag>>, Index<GroundFunctionTermValue<FluentTag>>, Index<GroundFunctionTermValue<AuxiliaryTag>>>;

using IndexNumericEffectVariant = std::variant<Index<NumericEffect<OpAssign, FluentTag>>,
                                               Index<NumericEffect<OpIncrease, FluentTag>>,
                                               Index<NumericEffect<OpDecrease, FluentTag>>,
                                               Index<NumericEffect<OpScaleUp, FluentTag>>,
                                               Index<NumericEffect<OpScaleDown, FluentTag>>,
                                               Index<NumericEffect<OpIncrease, AuxiliaryTag>>>;

using IndexGroundNumericConstraintVariant = std::variant<Index<BinaryOperator<OpEq, Data<GroundFunctionExpression>>>,
                                                         Index<BinaryOperator<OpLe, Data<GroundFunctionExpression>>>,
                                                         Index<BinaryOperator<OpLt, Data<GroundFunctionExpression>>>,
                                                         Index<BinaryOperator<OpGe, Data<GroundFunctionExpression>>>,
                                                         Index<BinaryOperator<OpGt, Data<GroundFunctionExpression>>>>;

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

        UnorderedMap<Index<Variable>, ParameterIndex> map;

        void push_parameters(const IndexList<Variable>& parameters)
        {
            for (const auto parameter : parameters)
                map.emplace(parameter, map.size());
        }

        void pop_parameters(const IndexList<Variable>& parameters)
        {
            for (const auto parameter : parameters)
                map.erase(parameter);
        }

        ParameterIndex lookup_parameter_index(Index<Variable> variable) { return map.at(variable); }
    };

    ParameterIndexMapping m_param_map;

    template<typename T>
    auto translate_common(const std::vector<const T*>& input, Builder& builder, Repository& context)
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

    IndexFunctionVariant translate_common(loki::FunctionSkeleton element, Builder& builder, Repository& context)
    {
        auto build_function = [&](auto fact_tag) -> IndexFunctionVariant
        {
            using Tag = std::decay_t<decltype(fact_tag)>;

            auto function_ptr = builder.template get_builder<Function<Tag>>();
            auto& function = *function_ptr;
            function.clear();
            function.name = element->get_name();
            function.arity = element->get_parameters().size();
            canonicalize(function);
            return context.get_or_create(function, builder.get_buffer()).first.get_index();
        };

        if (element->get_name() == "total-cost")
            return build_function(AuxiliaryTag {});
        else if (m_effect_function_skeletons.contains(element->get_name()))
            return build_function(FluentTag {});
        else
            return build_function(StaticTag {});
    }

    Index<Object> translate_common(loki::Object element, Builder& builder, Repository& context)
    {
        auto object_ptr = builder.template get_builder<Object>();
        auto& object = *object_ptr;
        object.clear();
        object.name = element->get_name();
        canonicalize(object);
        return context.get_or_create(object, builder.get_buffer()).first.get_index();
    }

    Index<Variable> translate_common(loki::Parameter element, Builder& builder, Repository& context)
    {
        return translate_common(element->get_variable(), builder, context);
    }

    IndexPredicateVariant translate_common(loki::Predicate element, Builder& builder, Repository& context)
    {
        auto build_predicate = [&](auto fact_tag) -> IndexPredicateVariant
        {
            using Tag = std::decay_t<decltype(fact_tag)>;

            auto predicate_ptr = builder.template get_builder<Predicate<Tag>>();
            auto& predicate = *predicate_ptr;
            predicate.clear();
            predicate.name = element->get_name();
            predicate.arity = element->get_parameters().size();
            canonicalize(predicate);
            return context.get_or_create(predicate, builder.get_buffer()).first.get_index();
        };

        if (m_fluent_predicates.count(element->get_name()) && !m_derived_predicates.count(element->get_name()))
            return build_predicate(FluentTag {});
        else if (m_derived_predicates.count(element->get_name()))
            return build_predicate(DerivedTag {});
        else
            return build_predicate(StaticTag {});
    }

    Index<Variable> translate_common(loki::Variable element, Builder& builder, Repository& context)
    {
        auto variable_ptr = builder.template get_builder<Variable>();
        auto& variable = *variable_ptr;
        variable.clear();
        variable.name = element->get_name();
        canonicalize(variable);
        return context.get_or_create(variable, builder.get_buffer()).first.get_index();
    }

    /**
     * Lifted translation.
     */

    template<typename T>
    auto translate_lifted(const std::vector<const T*>& input, Builder& builder, Repository& context)
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

    Data<Term> translate_lifted(loki::Term element, Builder& builder, Repository& context)
    {
        return std::visit(
            [&](auto&& arg) -> Data<Term>
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, loki::Object>)
                    return Data<Term>(translate_common(arg, builder, context));
                else if constexpr (std::is_same_v<T, loki::Variable>)
                    return Data<Term>(m_param_map.lookup_parameter_index(translate_common(arg, builder, context)));
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            element->get_object_or_variable());
    }

    IndexAtomVariant translate_lifted(loki::Atom element, Builder& builder, Repository& context)
    {
        auto index_predicate_variant = translate_common(element->get_predicate(), builder, context);

        auto build_atom = [&](auto fact_tag, auto predicate_index) -> IndexAtomVariant
        {
            using Tag = std::decay_t<decltype(fact_tag)>;

            auto atom_ptr = builder.template get_builder<Atom<Tag>>();
            auto& atom = *atom_ptr;
            atom.clear();
            atom.predicate = predicate_index;
            atom.terms = this->translate_lifted(element->get_terms(), builder, context);
            canonicalize(atom);
            return context.get_or_create(atom, builder.get_buffer()).first.get_index();
        };

        return std::visit(
            [&](auto&& arg) -> IndexAtomVariant
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Index<Predicate<StaticTag>>>)
                    return build_atom(StaticTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<Predicate<FluentTag>>>)
                    return build_atom(FluentTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<Predicate<DerivedTag>>>)
                    return build_atom(DerivedTag {}, arg);
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_predicate_variant);
    }

    IndexLiteralVariant translate_lifted(loki::Literal element, Builder& builder, Repository& context)
    {
        auto index_atom_variant = translate_lifted(element->get_atom(), builder, context);

        auto build_literal = [&](auto fact_tag, auto atom_index) -> IndexLiteralVariant
        {
            using Tag = std::decay_t<decltype(fact_tag)>;

            auto literal_ptr = builder.template get_builder<Literal<Tag>>();
            auto& literal = *literal_ptr;
            literal.clear();
            literal.atom = atom_index;
            literal.polarity = element->get_polarity();
            canonicalize(literal);
            return context.get_or_create(literal, builder.get_buffer()).first.get_index();
        };

        return std::visit(
            [&](auto&& arg) -> IndexLiteralVariant
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Index<Atom<StaticTag>>>)
                    return build_literal(StaticTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<Atom<FluentTag>>>)
                    return build_literal(FluentTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<Atom<DerivedTag>>>)
                    return build_literal(DerivedTag {}, arg);
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_atom_variant);
    }

    Data<FunctionExpression> translate_lifted(loki::FunctionExpressionNumber element, Builder& builder, Repository& context)
    {
        return Data<FunctionExpression>(float_t(element->get_number()));
    }

    Data<FunctionExpression> translate_lifted(loki::FunctionExpressionBinaryOperator element, Builder& builder, Repository& context)
    {
        auto build_binary_op = [&](auto op_tag) -> Data<FunctionExpression>
        {
            using Tag = std::decay_t<decltype(op_tag)>;

            auto binary_ptr = builder.template get_builder<BinaryOperator<Tag, Data<FunctionExpression>>>();
            auto& binary = *binary_ptr;
            binary.clear();
            binary.lhs = translate_lifted(element->get_left_function_expression(), builder, context);
            binary.rhs = translate_lifted(element->get_right_function_expression(), builder, context);
            canonicalize(binary);
            return Data<FunctionExpression>(
                Data<ArithmeticOperator<Data<FunctionExpression>>>(context.get_or_create(binary, builder.get_buffer()).first.get_index()));
        };

        switch (element->get_binary_operator())
        {
            case loki::BinaryOperatorEnum::PLUS:
                return build_binary_op(OpAdd {});
            case loki::BinaryOperatorEnum::MINUS:
                return build_binary_op(OpSub {});
            case loki::BinaryOperatorEnum::MUL:
                return build_binary_op(OpMul {});
            case loki::BinaryOperatorEnum::DIV:
                return build_binary_op(OpDiv {});
            default:
                throw std::runtime_error("Unexpected case");
        }
    }

    Data<FunctionExpression> translate_lifted(loki::FunctionExpressionMultiOperator element, Builder& builder, Repository& context)
    {
        auto build_multi_op = [&](auto op_tag) -> Data<FunctionExpression>
        {
            using Tag = std::decay_t<decltype(op_tag)>;

            auto multi_ptr = builder.template get_builder<MultiOperator<Tag, Data<FunctionExpression>>>();
            auto& multi = *multi_ptr;
            multi.clear();
            multi.args = translate_lifted(element->get_function_expressions(), builder, context);
            canonicalize(multi);
            return Data<FunctionExpression>(
                Data<ArithmeticOperator<Data<FunctionExpression>>>(context.get_or_create(multi, builder.get_buffer()).first.get_index()));
        };

        switch (element->get_multi_operator())
        {
            case loki::MultiOperatorEnum::PLUS:
                return build_multi_op(OpAdd {});
            case loki::MultiOperatorEnum::MUL:
                return build_multi_op(OpMul {});
            default:
                throw std::runtime_error("Unexpected case");
        }
    }

    Data<FunctionExpression> translate_lifted(loki::FunctionExpressionMinus element, Builder& builder, Repository& context)
    {
        auto minus_ptr = builder.template get_builder<UnaryOperator<OpSub, Data<FunctionExpression>>>();
        auto& minus = *minus_ptr;
        minus.clear();
        minus.arg = translate_lifted(element->get_function_expression(), builder, context);
        canonicalize(minus);
        return Data<FunctionExpression>(
            Data<ArithmeticOperator<Data<FunctionExpression>>>(context.get_or_create(minus, builder.get_buffer()).first.get_index()));
    }

    Data<FunctionExpression> translate_lifted(loki::FunctionExpressionFunction element, Builder& builder, Repository& context)
    {
        const auto index_fterm_variant = translate_lifted(element->get_function(), builder, context);

        return std::visit(
            [&](auto&& arg) -> Data<FunctionExpression>
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, Index<FunctionTerm<StaticTag>>>)
                    return Data<FunctionExpression>(arg);
                else if constexpr (std::is_same_v<T, Index<FunctionTerm<FluentTag>>>)
                    return Data<FunctionExpression>(arg);
                else if constexpr (std::is_same_v<T, Index<FunctionTerm<AuxiliaryTag>>>)
                    throw std::runtime_error("Cannot create FunctionExpression over auxiliary function term.");
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_fterm_variant);
    }

    Data<FunctionExpression> translate_lifted(loki::FunctionExpression element, Builder& builder, Repository& context)
    {
        return std::visit([&](auto&& arg) { return translate_lifted(arg, builder, context); }, element->get_function_expression());
    }

    IndexFunctionTermVariant translate_lifted(loki::Function element, Builder& builder, Repository& context)
    {
        auto index_function_variant = translate_common(element->get_function_skeleton(), builder, context);

        auto build_function_term = [&](auto fact_tag, auto function_index) -> IndexFunctionTermVariant
        {
            using Tag = std::decay_t<decltype(fact_tag)>;

            auto fterm_ptr = builder.template get_builder<FunctionTerm<Tag>>();
            auto& fterm = *fterm_ptr;
            fterm.clear();
            fterm.function = function_index;
            fterm.terms = this->translate_lifted(element->get_terms(), builder, context);
            canonicalize(fterm);
            return context.get_or_create(fterm, builder.get_buffer()).first.get_index();
        };

        return std::visit(
            [&](auto&& arg) -> IndexFunctionTermVariant
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, Index<Function<StaticTag>>>)
                    return build_function_term(StaticTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<Function<FluentTag>>>)
                    return build_function_term(FluentTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<Function<AuxiliaryTag>>>)
                    return build_function_term(AuxiliaryTag {}, arg);
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_function_variant);
    }

    Data<BooleanOperator<Data<FunctionExpression>>> translate_lifted(loki::ConditionNumericConstraint element, Builder& builder, Repository& context)
    {
        auto build_binary_op = [&](auto op_tag) -> Data<BooleanOperator<Data<FunctionExpression>>>
        {
            using Tag = std::decay_t<decltype(op_tag)>;

            auto binary_ptr = builder.template get_builder<BinaryOperator<Tag, Data<FunctionExpression>>>();
            auto& binary = *binary_ptr;
            binary.clear();
            binary.lhs = translate_lifted(element->get_left_function_expression(), builder, context);
            binary.rhs = translate_lifted(element->get_right_function_expression(), builder, context);
            canonicalize(binary);
            return Data<BooleanOperator<Data<FunctionExpression>>>(context.get_or_create(binary, builder.get_buffer()).first.get_index());
        };

        switch (element->get_binary_comparator())
        {
            case loki::BinaryComparatorEnum::EQUAL:
                return build_binary_op(OpEq {});
            case loki::BinaryComparatorEnum::UNEQUAL:
                return build_binary_op(OpNe {});
            case loki::BinaryComparatorEnum::LESS_EQUAL:
                return build_binary_op(OpLe {});
            case loki::BinaryComparatorEnum::LESS:
                return build_binary_op(OpLt {});
            case loki::BinaryComparatorEnum::GREATER_EQUAL:
                return build_binary_op(OpGe {});
            case loki::BinaryComparatorEnum::GREATER:
                return build_binary_op(OpGt {});
            default:
                throw std::runtime_error("Unexpected case");
        }
    }

    Index<ConjunctiveCondition> translate_lifted(loki::Condition element, const IndexList<Variable>& parameters, Builder& builder, Repository& context)
    {
        auto conj_condition_ptr = builder.template get_builder<ConjunctiveCondition>();
        auto& conj_condition = *conj_condition_ptr;
        conj_condition.clear();

        conj_condition.variables = parameters;

        const auto func_insert_literal = [](IndexLiteralVariant index_literal_variant,
                                            IndexList<Literal<StaticTag>>& static_literals,
                                            IndexList<Literal<FluentTag>>& fluent_literals,
                                            IndexList<Literal<DerivedTag>>& derived_literals)
        {
            std::visit(
                [&](auto&& arg)
                {
                    using T = std::decay_t<decltype(arg)>;

                    if constexpr (std::is_same_v<T, Index<Literal<StaticTag>>>)
                        static_literals.push_back(arg);
                    else if constexpr (std::is_same_v<T, Index<Literal<FluentTag>>>)
                        fluent_literals.push_back(arg);
                    else if constexpr (std::is_same_v<T, Index<Literal<DerivedTag>>>)
                        derived_literals.push_back(arg);
                    else
                        static_assert(dependent_false<T>::value, "Missing case for type");
                },
                index_literal_variant);
        };

        return std::visit(
            [&](auto&& condition) -> Index<ConjunctiveCondition>
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

                    canonicalize(conj_condition);
                    return context.get_or_create(conj_condition, builder.get_buffer()).first.get_index();
                }
                else if constexpr (std::is_same_v<ConditionT, loki::ConditionLiteral>)
                {
                    const auto index_literal_variant = translate_lifted(condition->get_literal(), builder, context);

                    func_insert_literal(index_literal_variant, conj_condition.static_literals, conj_condition.fluent_literals, conj_condition.derived_literals);

                    canonicalize(conj_condition);
                    return context.get_or_create(conj_condition, builder.get_buffer()).first.get_index();
                }
                else if constexpr (std::is_same_v<ConditionT, loki::ConditionNumericConstraint>)
                {
                    const auto numeric_constraint = translate_lifted(condition, builder, context);

                    conj_condition.numeric_constraints.push_back(numeric_constraint);

                    canonicalize(conj_condition);
                    return context.get_or_create(conj_condition, builder.get_buffer()).first.get_index();
                }
                else
                {
                    // std::cout << std::visit([](auto&& arg) { return arg.str(); }, *part) << std::endl;
                    throw std::logic_error("Unexpected condition.");
                }
            },
            element->get_condition());
    }

    IndexNumericEffectVariant translate_lifted(loki::EffectNumeric element, Builder& builder, Repository& context)
    {
        auto index_fterm_variant = translate_lifted(element->get_function(), builder, context);

        auto build_numeric_effect_term_helper = [&](auto fact_tag, auto op_tag, auto fterm_index) -> IndexNumericEffectVariant
        {
            using Tag = std::decay_t<decltype(fact_tag)>;
            using Op = std::decay_t<decltype(op_tag)>;

            auto numeric_effect_ptr = builder.template get_builder<NumericEffect<Op, Tag>>();
            auto& numeric_effect = *numeric_effect_ptr;
            numeric_effect.clear();

            numeric_effect.fterm = fterm_index;
            numeric_effect.fexpr = this->translate_lifted(element->get_function_expression(), builder, context);
            canonicalize(numeric_effect);
            return context.get_or_create(numeric_effect, builder.get_buffer()).first.get_index();
        };

        auto build_numeric_effect_term = [&](auto fact_tag, auto fterm_index) -> IndexNumericEffectVariant
        {
            using Tag = std::decay_t<decltype(fact_tag)>;

            if constexpr (std::is_same_v<Tag, AuxiliaryTag>)
            {
                if (element->get_assign_operator() != loki::AssignOperatorEnum::INCREASE)
                    throw std::runtime_error("Auxiliary numeric effect must use INCREASE operator.");

                return build_numeric_effect_term_helper(Tag {}, OpIncrease {}, fterm_index);
            }
            else
            {
                switch (element->get_assign_operator())
                {
                    case loki::AssignOperatorEnum::ASSIGN:
                        return build_numeric_effect_term_helper(Tag {}, OpAssign {}, fterm_index);
                    case loki::AssignOperatorEnum::INCREASE:
                        return build_numeric_effect_term_helper(Tag {}, OpIncrease {}, fterm_index);
                    case loki::AssignOperatorEnum::DECREASE:
                        return build_numeric_effect_term_helper(Tag {}, OpDecrease {}, fterm_index);
                    case loki::AssignOperatorEnum::SCALE_UP:
                        return build_numeric_effect_term_helper(Tag {}, OpScaleUp {}, fterm_index);
                    case loki::AssignOperatorEnum::SCALE_DOWN:
                        return build_numeric_effect_term_helper(Tag {}, OpScaleDown {}, fterm_index);
                    default:
                        throw std::runtime_error("Unexpected case.");
                }
            }
        };

        return std::visit(
            [&](auto&& arg) -> IndexNumericEffectVariant
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, Index<FunctionTerm<StaticTag>>>)
                    throw std::runtime_error("Cannot create NumericEffect over static function term.");
                else if constexpr (std::is_same_v<T, Index<FunctionTerm<FluentTag>>>)
                    return build_numeric_effect_term(FluentTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<FunctionTerm<AuxiliaryTag>>>)
                    return build_numeric_effect_term(AuxiliaryTag {}, arg);
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_fterm_variant);
    }

    IndexList<ConditionalEffect> translate_lifted(loki::Effect element, const IndexList<Variable>& parameters, Builder& builder, Repository& context)
    {
        using ConditionalEffectData = UnorderedMap<Index<ConjunctiveCondition>,
                                                   std::tuple<IndexList<Variable>,
                                                              IndexList<Literal<FluentTag>>,
                                                              DataList<NumericEffectOperator<FluentTag>>,
                                                              ::cista::optional<Data<NumericEffectOperator<AuxiliaryTag>>>>>;

        const auto translate_effect_func = [&](loki::Effect effect, ConditionalEffectData& ref_conditional_effect_data)
        {
            auto tmp_effect = effect;

            auto universal_parameters = IndexList<Variable> {};

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
                            auto conj_cond_ptr = builder.template get_builder<ConjunctiveCondition>();
                            auto& conj_cond = *conj_cond_ptr;
                            conj_cond.clear();
                            canonicalize(conj_cond);
                            return context.get_or_create(conj_cond, builder.get_buffer()).first.get_index();
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

                                    if constexpr (std::is_same_v<SubSubEffectT, Index<Literal<StaticTag>>>)
                                        throw std::logic_error("Effect lieral cannot be Static!");
                                    else if constexpr (std::is_same_v<SubSubEffectT, Index<Literal<FluentTag>>>)
                                        data_fluent_literals.push_back(subsubeffect);
                                    else if constexpr (std::is_same_v<SubSubEffectT, Index<Literal<DerivedTag>>>)
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

                                    if constexpr (std::is_same_v<SubSubEffectT, Index<NumericEffect<OpAssign, FluentTag>>>)
                                        data_fluent_numeric_effects.push_back(Data<NumericEffectOperator<FluentTag>>(subsubeffect));
                                    else if constexpr (std::is_same_v<SubSubEffectT, Index<NumericEffect<OpIncrease, FluentTag>>>)
                                        data_fluent_numeric_effects.push_back(Data<NumericEffectOperator<FluentTag>>(subsubeffect));
                                    else if constexpr (std::is_same_v<SubSubEffectT, Index<NumericEffect<OpDecrease, FluentTag>>>)
                                        data_fluent_numeric_effects.push_back(Data<NumericEffectOperator<FluentTag>>(subsubeffect));
                                    else if constexpr (std::is_same_v<SubSubEffectT, Index<NumericEffect<OpScaleUp, FluentTag>>>)
                                        data_fluent_numeric_effects.push_back(Data<NumericEffectOperator<FluentTag>>(subsubeffect));
                                    else if constexpr (std::is_same_v<SubSubEffectT, Index<NumericEffect<OpScaleDown, FluentTag>>>)
                                        data_fluent_numeric_effects.push_back(Data<NumericEffectOperator<FluentTag>>(subsubeffect));
                                    else if constexpr (std::is_same_v<SubSubEffectT, Index<NumericEffect<OpIncrease, AuxiliaryTag>>>)
                                    {
                                        assert(!data_auxiliary_numeric_effect);
                                        data_auxiliary_numeric_effect = Data<NumericEffectOperator<AuxiliaryTag>>(subsubeffect);
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
        auto conditional_effects = IndexList<ConditionalEffect> {};

        for (const auto& [cond_conjunctive_condition, value] : conditional_effect_data)
        {
            const auto& [cond_effect_universal_parameters,
                         cond_effect_fluent_literals,
                         cond_effect_fluent_numeric_effects,
                         cond_effect_auxiliary_numeric_effects] = value;

            auto conj_effect_ptr = builder.template get_builder<ConjunctiveEffect>();
            auto& conj_effect = *conj_effect_ptr;
            conj_effect.clear();
            conj_effect.literals = cond_effect_fluent_literals;
            conj_effect.numeric_effects = cond_effect_fluent_numeric_effects;
            conj_effect.auxiliary_numeric_effect = cond_effect_auxiliary_numeric_effects;
            canonicalize(conj_effect);
            const auto conj_effect_index = context.get_or_create(conj_effect, builder.get_buffer()).first.get_index();

            auto cond_effect_ptr = builder.template get_builder<ConditionalEffect>();
            auto& cond_effect = *cond_effect_ptr;
            cond_effect.clear();
            cond_effect.variables = cond_effect_universal_parameters;
            cond_effect.condition = cond_conjunctive_condition;
            cond_effect.effect = conj_effect_index;
            canonicalize(cond_effect);
            const auto cond_effect_index = context.get_or_create(cond_effect, builder.get_buffer()).first.get_index();

            conditional_effects.push_back(cond_effect_index);
        }

        return conditional_effects;
    }

    Index<Action> translate_lifted(loki::Action element, Builder& builder, Repository& context)
    {
        auto action_ptr = builder.template get_builder<Action>();
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
            auto conjunctive_condition = Index<ConjunctiveCondition>::max();
            if (element->get_condition().has_value())
            {
                conjunctive_condition = translate_lifted(element->get_condition().value(), parameters, builder, context);
            }
            else
            {
                // Create empty one
                auto conj_cond_ptr = builder.template get_builder<ConjunctiveCondition>();
                auto& conj_cond = *conj_cond_ptr;
                conj_cond.clear();
                canonicalize(conj_cond);
                conjunctive_condition = context.get_or_create(conj_cond, builder.get_buffer()).first.get_index();
            }
            action.condition = conjunctive_condition;

            // 2. Translate effects
            auto conditional_effects = IndexList<ConditionalEffect> {};
            if (element->get_effect().has_value())
            {
                const auto conditional_effects_ = translate_lifted(element->get_effect().value(), parameters, builder, context);
                conditional_effects = conditional_effects_;
            }
            action.effects = conditional_effects;
        }
        ///---------- Pop parameters -------------
        m_param_map.pop_parameters(parameters);

        canonicalize(action);
        return context.get_or_create(action, builder.get_buffer()).first.get_index();
    }

    Index<Axiom> translate_lifted(loki::Axiom element, Builder& builder, Repository& context)
    {
        auto axiom_ptr = builder.template get_builder<Axiom>();
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
                    if constexpr (std::is_same_v<T, Index<Literal<DerivedTag>>>)
                        axiom.head = make_view(arg, context).get_atom().get_index();
                    else
                        throw std::runtime_error("ToMimirStructures::translate_lifted: Expected Literal<DerivedTag> in axiom head.");
                },
                index_literal_variant);
        }
        ///---------- Pop parameters -------------
        m_param_map.pop_parameters(parameters);

        canonicalize(axiom);
        return context.get_or_create(axiom, builder.get_buffer()).first.get_index();
    }

    /**
     * Grounded translation
     */

    template<typename T>
    auto translate_grounded(const std::vector<const T*>& input, Builder& builder, Repository& context)
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

    Index<Object> translate_grounded(loki::Term element, Builder& builder, Repository& context)
    {
        return std::visit(
            [&](auto&& arg) -> Index<Object>
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

    Index<Binding> to_binding(const IndexList<Object>& element, Builder& builder, Repository& context)
    {
        auto binding_ptr = builder.get_builder<Binding>();
        auto& binding = *binding_ptr;
        binding.clear();
        binding.objects = element;
        canonicalize(binding);
        return context.get_or_create(binding, builder.get_buffer()).first.get_index();
    }

    template<typename T>
    auto to_binding(View<T, Repository> element, const IndexList<Object>& objects, Repository& context)
    {
        return context.get_or_create(element, objects);
    }

    IndexGroundAtomVariant translate_grounded(loki::Atom element, Builder& builder, Repository& context)
    {
        auto index_predicate_variant = translate_common(element->get_predicate(), builder, context);

        auto build_atom = [&](auto fact_tag, auto predicate_index)
        {
            using Tag = std::decay_t<decltype(fact_tag)>;

            auto atom_ptr = builder.template get_builder<GroundAtom<Tag>>();
            auto& atom = *atom_ptr;
            atom.clear();
            atom.predicate = predicate_index;
            atom.row = to_binding(make_view(predicate_index, context), this->translate_grounded(element->get_terms(), builder, context), context)
                           .first.get_index()
                           .second;
            canonicalize(atom);
            return context.get_or_create(atom, builder.get_buffer()).first.get_index();
        };

        return std::visit(
            [&](auto&& arg) -> IndexGroundAtomVariant
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Index<Predicate<StaticTag>>>)
                    return build_atom(StaticTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<Predicate<FluentTag>>>)
                    return build_atom(FluentTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<Predicate<DerivedTag>>>)
                    return build_atom(DerivedTag {}, arg);
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_predicate_variant);
    }

    IndexGroundAtomOrFactVariant translate_grounded(loki::Atom element, Builder& builder, Repository& context, BinaryFDRContext& fdr_context)
    {
        auto atom_variant = translate_grounded(element, builder, context);

        return std::visit(
            [&](auto&& arg) -> IndexGroundAtomOrFactVariant
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Index<GroundAtom<StaticTag>>>)
                    return arg;
                else if constexpr (std::is_same_v<T, Index<GroundAtom<FluentTag>>>)
                    return fdr_context.get_fact(arg);
                else if constexpr (std::is_same_v<T, Index<GroundAtom<DerivedTag>>>)
                    return arg;
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            atom_variant);
    }

    IndexGroundLiteralVariant translate_grounded(loki::Literal element, Builder& builder, Repository& context)
    {
        auto atom_variant = translate_grounded(element->get_atom(), builder, context);

        auto build_literal = [&](auto fact_tag, auto atom)
        {
            using Tag = std::decay_t<decltype(fact_tag)>;

            auto literal_ptr = builder.template get_builder<GroundLiteral<Tag>>();
            auto& literal = *literal_ptr;
            literal.clear();
            literal.atom = atom;
            literal.polarity = element->get_polarity();
            canonicalize(literal);
            return context.get_or_create(literal, builder.get_buffer()).first.get_index();
        };

        return std::visit(
            [&](auto&& arg) -> IndexGroundLiteralVariant
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Index<GroundAtom<StaticTag>>>)
                    return build_literal(StaticTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<GroundAtom<FluentTag>>>)
                    return build_literal(FluentTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<GroundAtom<DerivedTag>>>)
                    return build_literal(DerivedTag {}, arg);
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            atom_variant);
    }

    IndexGroundLiteralOrFactVariant translate_grounded(loki::Literal element, Builder& builder, Repository& context, BinaryFDRContext& fdr_context)
    {
        auto literal_variant = translate_grounded(element, builder, context);

        return std::visit(
            [&](auto&& arg) -> IndexGroundLiteralOrFactVariant
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Index<GroundLiteral<StaticTag>>>)
                    return arg;
                else if constexpr (std::is_same_v<T, Index<GroundLiteral<FluentTag>>>)
                    return fdr_context.get_fact(arg);
                else if constexpr (std::is_same_v<T, Index<GroundLiteral<DerivedTag>>>)
                    return arg;
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            literal_variant);
    }

    Data<GroundFunctionExpression> translate_grounded(loki::FunctionExpressionNumber element, Builder& builder, Repository& context)
    {
        return Data<GroundFunctionExpression>(float_t(element->get_number()));
    }

    Data<GroundFunctionExpression> translate_grounded(loki::FunctionExpressionBinaryOperator element, Builder& builder, Repository& context)
    {
        auto build_binary_op = [&](auto op_tag) -> Data<GroundFunctionExpression>
        {
            using Tag = std::decay_t<decltype(op_tag)>;

            auto binary_ptr = builder.template get_builder<BinaryOperator<Tag, Data<GroundFunctionExpression>>>();
            auto& binary = *binary_ptr;
            binary.clear();
            binary.lhs = translate_grounded(element->get_left_function_expression(), builder, context);
            binary.rhs = translate_grounded(element->get_right_function_expression(), builder, context);
            canonicalize(binary);
            return Data<GroundFunctionExpression>(
                Data<ArithmeticOperator<Data<GroundFunctionExpression>>>(context.get_or_create(binary, builder.get_buffer()).first.get_index()));
        };

        switch (element->get_binary_operator())
        {
            case loki::BinaryOperatorEnum::PLUS:
                return build_binary_op(OpAdd {});
            case loki::BinaryOperatorEnum::MINUS:
                return build_binary_op(OpSub {});
            case loki::BinaryOperatorEnum::MUL:
                return build_binary_op(OpMul {});
            case loki::BinaryOperatorEnum::DIV:
                return build_binary_op(OpDiv {});
            default:
                throw std::runtime_error("Unexpected case");
        }
    }

    Data<GroundFunctionExpression> translate_grounded(loki::FunctionExpressionMultiOperator element, Builder& builder, Repository& context)
    {
        auto build_multi_op = [&](auto op_tag) -> Data<GroundFunctionExpression>
        {
            using Tag = std::decay_t<decltype(op_tag)>;

            auto multi_ptr = builder.template get_builder<MultiOperator<Tag, Data<GroundFunctionExpression>>>();
            auto& multi = *multi_ptr;
            multi.clear();
            multi.args = translate_grounded(element->get_function_expressions(), builder, context);
            canonicalize(multi);
            return Data<GroundFunctionExpression>(
                Data<ArithmeticOperator<Data<GroundFunctionExpression>>>(context.get_or_create(multi, builder.get_buffer()).first.get_index()));
        };

        switch (element->get_multi_operator())
        {
            case loki::MultiOperatorEnum::PLUS:
                return build_multi_op(OpAdd {});
            case loki::MultiOperatorEnum::MUL:
                return build_multi_op(OpMul {});
            default:
                throw std::runtime_error("Unexpected case");
        }
    }

    Data<GroundFunctionExpression> translate_grounded(loki::FunctionExpressionMinus element, Builder& builder, Repository& context)
    {
        auto minus_ptr = builder.template get_builder<UnaryOperator<OpSub, Data<GroundFunctionExpression>>>();
        auto& minus = *minus_ptr;
        minus.clear();
        minus.arg = translate_grounded(element->get_function_expression(), builder, context);
        canonicalize(minus);
        return Data<GroundFunctionExpression>(
            Data<ArithmeticOperator<Data<GroundFunctionExpression>>>(context.get_or_create(minus, builder.get_buffer()).first.get_index()));
    }

    Data<GroundFunctionExpression> translate_grounded(loki::FunctionExpressionFunction element, Builder& builder, Repository& context)
    {
        const auto index_fterm_variant = translate_grounded(element->get_function(), builder, context);

        return std::visit(
            [&](auto&& arg) -> Data<GroundFunctionExpression>
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, Index<GroundFunctionTerm<StaticTag>>>)
                    return Data<GroundFunctionExpression>(arg);
                else if constexpr (std::is_same_v<T, Index<GroundFunctionTerm<FluentTag>>>)
                    return Data<GroundFunctionExpression>(arg);
                else if constexpr (std::is_same_v<T, Index<GroundFunctionTerm<AuxiliaryTag>>>)
                    return Data<GroundFunctionExpression>(arg);
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_fterm_variant);
    }

    Data<GroundFunctionExpression> translate_grounded(loki::FunctionExpression element, Builder& builder, Repository& context)
    {
        return std::visit([&](auto&& arg) { return translate_grounded(arg, builder, context); }, element->get_function_expression());
    }

    IndexGroundFunctionTermVariant translate_grounded(loki::Function element, Builder& builder, Repository& context)
    {
        auto index_function_variant = translate_common(element->get_function_skeleton(), builder, context);

        auto build_function_term = [&](auto fact_tag, auto function_index) -> IndexGroundFunctionTermVariant
        {
            using Tag = std::decay_t<decltype(fact_tag)>;

            auto fterm_ptr = builder.template get_builder<GroundFunctionTerm<Tag>>();
            auto& fterm = *fterm_ptr;
            fterm.clear();
            fterm.function = function_index;
            fterm.row = to_binding(make_view(function_index, context), this->translate_grounded(element->get_terms(), builder, context), context)
                            .first.get_index()
                            .second;
            canonicalize(fterm);
            return context.get_or_create(fterm, builder.get_buffer()).first.get_index();
        };

        return std::visit(
            [&](auto&& arg) -> IndexGroundFunctionTermVariant
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Index<Function<StaticTag>>>)
                    return build_function_term(StaticTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<Function<FluentTag>>>)
                    return build_function_term(FluentTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<Function<AuxiliaryTag>>>)
                    return build_function_term(AuxiliaryTag {}, arg);
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_function_variant);
    }

    IndexGroundFunctionTermValueVariant translate_grounded(loki::FunctionValue element, Builder& builder, Repository& context)
    {
        auto index_fterm_variant = translate_grounded(element->get_function(), builder, context);

        auto build_fterm_value = [&](auto fact_tag, auto fterm_index) -> IndexGroundFunctionTermValueVariant
        {
            using Tag = std::decay_t<decltype(fact_tag)>;

            auto fterm_value_ptr = builder.template get_builder<GroundFunctionTermValue<Tag>>();
            auto& fterm_value = *fterm_value_ptr;
            fterm_value.clear();
            fterm_value.fterm = fterm_index;
            fterm_value.value = element->get_number();
            canonicalize(fterm_value);
            return context.get_or_create(fterm_value, builder.get_buffer()).first.get_index();
        };

        return std::visit(
            [&](auto&& arg) -> IndexGroundFunctionTermValueVariant
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Index<GroundFunctionTerm<StaticTag>>>)
                    return build_fterm_value(StaticTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<GroundFunctionTerm<FluentTag>>>)
                    return build_fterm_value(FluentTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<GroundFunctionTerm<AuxiliaryTag>>>)
                    return build_fterm_value(AuxiliaryTag {}, arg);
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_fterm_variant);
    }

    Data<BooleanOperator<Data<GroundFunctionExpression>>> translate_grounded(loki::ConditionNumericConstraint element, Builder& builder, Repository& context)
    {
        auto build_binary_op = [&](auto op_tag) -> Data<BooleanOperator<Data<GroundFunctionExpression>>>
        {
            using Tag = std::decay_t<decltype(op_tag)>;

            auto binary_ptr = builder.template get_builder<BinaryOperator<Tag, Data<GroundFunctionExpression>>>();
            auto& binary = *binary_ptr;
            binary.clear();
            binary.lhs = translate_grounded(element->get_left_function_expression(), builder, context);
            binary.rhs = translate_grounded(element->get_right_function_expression(), builder, context);
            canonicalize(binary);
            return Data<BooleanOperator<Data<GroundFunctionExpression>>>(context.get_or_create(binary, builder.get_buffer()).first.get_index());
        };

        switch (element->get_binary_comparator())
        {
            case loki::BinaryComparatorEnum::EQUAL:
                return build_binary_op(OpEq {});
            case loki::BinaryComparatorEnum::LESS_EQUAL:
                return build_binary_op(OpLe {});
            case loki::BinaryComparatorEnum::LESS:
                return build_binary_op(OpLt {});
            case loki::BinaryComparatorEnum::GREATER_EQUAL:
                return build_binary_op(OpGe {});
            case loki::BinaryComparatorEnum::GREATER:
                return build_binary_op(OpGt {});
            default:
                throw std::runtime_error("Unexpected case");
        }
    }

    Index<GroundConjunctiveCondition> translate_grounded(loki::Condition element, Builder& builder, Repository& context, BinaryFDRContext& fdr_context)
    {
        auto conj_condition_ptr = builder.template get_builder<GroundConjunctiveCondition>();
        auto& conj_condition = *conj_condition_ptr;
        conj_condition.clear();

        const auto func_insert_literal = [](IndexGroundLiteralOrFactVariant index_literal_variant,
                                            IndexList<GroundLiteral<StaticTag>>& static_literals,
                                            DataList<FDRFact<FluentTag>>& fluent_facts,
                                            IndexList<GroundLiteral<DerivedTag>>& derived_literals)
        {
            std::visit(
                [&](auto&& literal_index)
                {
                    using T = std::decay_t<decltype(literal_index)>;

                    if constexpr (std::is_same_v<T, Index<GroundLiteral<StaticTag>>>)
                        static_literals.push_back(literal_index);
                    else if constexpr (std::is_same_v<T, Data<FDRFact<FluentTag>>>)
                        fluent_facts.push_back(literal_index);
                    else if constexpr (std::is_same_v<T, Index<GroundLiteral<DerivedTag>>>)
                        derived_literals.push_back(literal_index);
                    else
                        static_assert(dependent_false<T>::value, "Missing case for type");
                },
                index_literal_variant);
        };

        return std::visit(
            [&](auto&& condition) -> Index<GroundConjunctiveCondition>
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

                    canonicalize(conj_condition);
                    return context.get_or_create(conj_condition, builder.get_buffer()).first.get_index();
                }
                else if constexpr (std::is_same_v<ConditionT, loki::ConditionLiteral>)
                {
                    const auto index_literal_variant = translate_grounded(condition->get_literal(), builder, context, fdr_context);

                    func_insert_literal(index_literal_variant, conj_condition.static_literals, conj_condition.fluent_facts, conj_condition.derived_literals);

                    canonicalize(conj_condition);
                    return context.get_or_create(conj_condition, builder.get_buffer()).first.get_index();
                }
                else if constexpr (std::is_same_v<ConditionT, loki::ConditionNumericConstraint>)
                {
                    const auto numeric_constraint = translate_grounded(condition, builder, context);

                    conj_condition.numeric_constraints.push_back(numeric_constraint);

                    canonicalize(conj_condition);
                    return context.get_or_create(conj_condition, builder.get_buffer()).first.get_index();
                }
                else
                {
                    // std::cout << std::visit([](auto&& arg) { return arg.str(); }, *condition_ptr) << std::endl;
                    throw std::logic_error("Unexpected condition.");
                }
            },
            element->get_condition());
    }

    Index<Metric> translate_grounded(loki::OptimizationMetric element, Builder& builder, Repository& context)
    {
        auto metric_ptr = builder.template get_builder<Metric>();
        auto& metric = *metric_ptr;
        metric.clear();

        metric.fexpr = translate_grounded(element->get_function_expression(), builder, context);
        switch (element->get_optimization_metric())
        {
            case loki::OptimizationMetricEnum::MINIMIZE:
            {
                metric.objective = Minimize {};
                break;
            }
            case loki::OptimizationMetricEnum::MAXIMIZE:
            {
                metric.objective = Maximize {};
                break;
            }
            default:
                throw std::runtime_error("Unexpected case.");
        }

        canonicalize(metric);
        return context.get_or_create(metric, builder.get_buffer()).first.get_index();
    }

public:
    LokiToTyrTranslator() = default;

    PlanningDomain translate(const loki::Domain& domain);

    PlanningTask translate(const loki::Problem& problem, PlanningDomain domain);
};

}

#endif