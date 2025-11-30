/*
 * Copyright (C) 2023 Dominik Drexler and Simon Stahlberg
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

#include "tyr/formalism/builder.hpp"
#include "tyr/formalism/canonicalization.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"
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

using IndexAtomVariant =
    std::variant<Index<formalism::Atom<formalism::StaticTag>>, Index<formalism::Atom<formalism::FluentTag>>, Index<formalism::Atom<formalism::DerivedTag>>>;

using IndexLiteralVariant = std::
    variant<Index<formalism::Literal<formalism::StaticTag>>, Index<formalism::Literal<formalism::FluentTag>>, Index<formalism::Literal<formalism::DerivedTag>>>;

using IndexGroundAtomVariant = std::variant<Index<formalism::GroundAtom<formalism::StaticTag>>,
                                            Index<formalism::GroundAtom<formalism::FluentTag>>,
                                            Index<formalism::GroundAtom<formalism::DerivedTag>>>;

using IndexGroundLiteralVariant = std::variant<Index<formalism::GroundLiteral<formalism::StaticTag>>,
                                               Index<formalism::GroundLiteral<formalism::FluentTag>>,
                                               Index<formalism::GroundLiteral<formalism::DerivedTag>>>;

using IndexFunctionVariant = std::variant<Index<formalism::Function<formalism::StaticTag>>,
                                          Index<formalism::Function<formalism::FluentTag>>,
                                          Index<formalism::Function<formalism::AuxiliaryTag>>>;

using IndexFunctionTermVariant = std::variant<Index<formalism::FunctionTerm<formalism::StaticTag>>,
                                              Index<formalism::FunctionTerm<formalism::FluentTag>>,
                                              Index<formalism::FunctionTerm<formalism::AuxiliaryTag>>>;

using IndexGroundFunctionTermVariant = std::variant<Index<formalism::GroundFunctionTerm<formalism::StaticTag>>,
                                                    Index<formalism::GroundFunctionTerm<formalism::FluentTag>>,
                                                    Index<formalism::GroundFunctionTerm<formalism::AuxiliaryTag>>>;

using IndexGroundFunctionTermValueVariant = std::variant<Index<formalism::GroundFunctionTermValue<formalism::StaticTag>>,
                                                         Index<formalism::GroundFunctionTermValue<formalism::FluentTag>>,
                                                         Index<formalism::GroundFunctionTermValue<formalism::AuxiliaryTag>>>;

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

    template<formalism::Context C>
    IndexFunctionVariant translate_common(loki::FunctionSkeleton element, formalism::Builder& builder, C& context)
    {
        auto build_function = [&](auto tag_const) -> IndexFunctionVariant
        {
            using Tag = std::decay_t<decltype(tag_const)>;

            auto& function = builder.template get_function<Tag>();
            function.name = element->get_name();
            function.arity = element->get_parameters().size();
            formalism::canonicalize(function);
            return context.get_or_create(function, builder.get_buffer());
        };

        if (element->get_name() == "total-cost")
            return build_function(formalism::AuxiliaryTag {});
        else if (m_effect_function_skeletons.contains(element->get_name()))
            return build_function(formalism::FluentTag {});
        else
            return build_function(formalism::StaticTag {});
    }

    template<formalism::Context C>
    Index<formalism::Object> translate_common(loki::Object element, formalism::Builder& builder, C& context)
    {
        auto& object = builder.get_object();
        object.name = element->get_name();
        formalism::canonicalize(object);
        return context.get_or_create(object, builder.get_buffer());
    }

    template<formalism::Context C>
    Index<formalism::Variable> translate_common(loki::Parameter element, formalism::Builder& builder, C& context)
    {
        return translate_common(element->get_variable(), builder, context);
    }

    template<formalism::Context C>
    IndexPredicateVariant translate_common(loki::Predicate element, formalism::Builder& builder, C& context)
    {
        auto build_predicate = [&](auto tag_const) -> IndexPredicateVariant
        {
            using Tag = std::decay_t<decltype(tag_const)>;

            auto& predicate = builder.template get_predicate<Tag>();
            predicate.name = element->get_name();
            predicate.arity = element->get_parameters().size();
            formalism::canonicalize(predicate);
            return context.get_or_create(predicate, builder.get_buffer());
        };

        if (m_fluent_predicates.count(element->get_name()) && !m_derived_predicates.count(element->get_name()))
            return build_predicate(formalism::FluentTag {});
        else if (m_derived_predicates.count(element->get_name()))
            return build_predicate(formalism::DerivedTag {});
        else
            return build_predicate(formalism::StaticTag {});
    }

    template<formalism::Context C>
    Index<formalism::Variable> translate_common(loki::Variable element, formalism::Builder& builder, C& context)
    {
        auto& variable = builder.get_variable();
        variable.name = element->get_name();
        formalism::canonicalize(variable);
        return context.get_or_create(variable, builder.get_buffer());
    }

    /**
     * Lifted translation.
     */

    template<typename T, formalism::Context C>
    auto translate_lifted(const std::vector<const T*>& input, formalism::Builder& builder, C& context)
    {
        using ReturnType = decltype(this->translate_lifted(std::declval<const T*>(), builder, std::declval<C&>()));
        auto output = ::cista::offset::vector<ReturnType> {};
        output.reserve(input.size());
        std::transform(std::begin(input),
                       std::end(input),
                       std::back_inserter(output),
                       [&](auto&& arg) { return this->translate_lifted(arg, builder, context); });
        return output;
    }

    template<formalism::Context C>
    Data<formalism::Term> translate_lifted(loki::Term element, formalism::Builder& builder, C& context)
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

    template<formalism::Context C>
    IndexAtomVariant translate_lifted(loki::Atom element, formalism::Builder& builder, C& context)
    {
        auto index_predicate_variant = translate_lifted(element->get_predicate(), builder, context);

        auto build_atom = [&](auto tag_const, auto predicate_index) -> IndexAtomVariant
        {
            using Tag = std::decay_t<decltype(tag_const)>;

            auto& atom = builder.template get_atom<Tag>();
            atom.predicate = predicate_index;
            atom.terms = this->translate_lifted(element->get_terms(), builder, context);
            formalism::canonicalize(atom);
            return context.get_or_create(atom, builder.get_buffer());
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

    template<formalism::Context C>
    IndexLiteralVariant translate_lifted(loki::Literal element, formalism::Builder& builder, C& context)
    {
        auto index_atom_variant = translate_lifted(element->get_atom(), builder, context);

        auto build_literal = [&](auto tag_const, auto atom_index) -> IndexLiteralVariant
        {
            using Tag = std::decay_t<decltype(tag_const)>;

            auto& literal = builder.template get_literal<Tag>();
            literal.atom = atom_index;
            literal.polarity = element->get_polarity();
            formalism::canonicalize(literal);
            return context.get_or_create(literal, builder.get_buffer());
        };

        return std::visit(
            [&](auto&& arg) -> IndexLiteralVariant
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Index<formalism::Atom<formalism::StaticTag>>>)
                    return build_literal(formalism::StaticTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<formalism::Atom<formalism::FluentTag>>>)
                    return build_literal(formalism::FluentTag {}, arg);
                else if constexpr (std::is_same_v<T, Index<formalism::Atom<formalism::DerivedTag>>>)
                    return build_literal(formalism::DerivedTag {}, arg);
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            index_atom_variant);
    }

    template<formalism::Context C>
    Data<formalism::FunctionExpression> translate_lifted(loki::FunctionExpressionNumber element, formalism::Builder& builder, C& context)
    {
        return Data<formalism::FunctionExpression>(float_t(element->get_number()));
    }

    template<formalism::Context C>
    Data<formalism::FunctionExpression> translate_lifted(loki::FunctionExpressionBinaryOperator element, formalism::Builder& builder, C& context);

    template<formalism::Context C>
    Data<formalism::FunctionExpression> translate_lifted(loki::FunctionExpressionMultiOperator element, formalism::Builder& builder, C& context);

    template<formalism::Context C>
    Data<formalism::FunctionExpression> translate_lifted(loki::FunctionExpressionMinus element, formalism::Builder& builder, C& context);

    template<formalism::Context C>
    Data<formalism::FunctionExpression> translate_lifted(loki::FunctionExpressionFunction element, formalism::Builder& builder, C& context);

    template<formalism::Context C>
    Data<formalism::FunctionExpression> translate_lifted(loki::FunctionExpression element, formalism::Builder& builder, C& context);

    template<formalism::Context C>
    IndexFunctionTermVariant translate_lifted(loki::Function element, formalism::Builder& builder, C& context)
    {
        auto index_function_variant = translate_common(element->get_function_skeleton(), builder, context);

        auto build_function_term = [&](auto tag_const, auto function_index) -> IndexFunctionTermVariant
        {
            using Tag = std::decay_t<decltype(tag_const)>;

            auto& fterm = builder.template get_fterm<Tag>();
            fterm.function = function_index;
            fterm.terms = this->translate_lifted(element->get_terms(), builder, context);
            formalism::canonicalize(fterm);
            return context.get_or_create(fterm, builder.get_buffer());
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

    template<formalism::Context C>
    Data<formalism::BooleanOperator<Data<formalism::FunctionExpression>>>
    translate_lifted(loki::ConditionNumericConstraint element, formalism::Builder& builder, C& context);

    template<formalism::Context C>
    Index<formalism::ConjunctiveCondition>
    translate_lifted(loki::Condition element, const IndexList<formalism::Variable>& parameters, formalism::Builder& builder, C& context);

    template<formalism::Context C>
    IndexList<formalism::ConditionalEffect>
    translate_lifted(loki::Effect element, const IndexList<formalism::Variable>& parameters, formalism::Builder& builder, C& context);

    template<formalism::Context C>
    Index<formalism::Action> translate_lifted(loki::Action element, formalism::Builder& builder, C& context);

    template<formalism::Context C>
    Index<formalism::Axiom> translate_lifted(loki::Axiom element, formalism::Builder& builder, C& context);

    /**
     * Grounded translation
     */

    template<typename T, formalism::Context C>
    auto translate_grounded(const std::vector<const T*>& input, formalism::Builder& builder, C& context)
    {
        using ReturnType = decltype(this->translate_grounded(std::declval<const T*>(), builder, std::declval<C&>()));
        auto output = ::cista::offset::vector<ReturnType> {};
        output.reserve(input.size());
        std::transform(std::begin(input), std::end(input), std::back_inserter(output), [&](auto&& arg) { return this->translate_grounded(arg, context); });
        return output;
    }

    template<formalism::Context C>
    Index<formalism::Object> translate_grounded(loki::Term element, formalism::Builder& builder, C& context);

    template<formalism::Context C>
    IndexGroundAtomVariant translate_grounded(loki::Atom element, formalism::Builder& builder, C& context);

    template<formalism::Context C>
    IndexGroundLiteralVariant translate_grounded(loki::Literal element, formalism::Builder& builder, C& context);

    template<formalism::Context C>
    Data<formalism::GroundFunctionExpression> translate_grounded(loki::FunctionExpressionNumber element, formalism::Builder& builder, C& context)
    {
        return Data<formalism::GroundFunctionExpression>(float_t(element->get_number()));
    }

    template<formalism::Context C>
    Data<formalism::GroundFunctionExpression> translate_grounded(loki::FunctionExpressionBinaryOperator element, formalism::Builder& builder, C& context);

    template<formalism::Context C>
    Data<formalism::GroundFunctionExpression> translate_grounded(loki::FunctionExpressionMultiOperator element, formalism::Builder& builder, C& context);

    template<formalism::Context C>
    Data<formalism::GroundFunctionExpression> translate_grounded(loki::FunctionExpressionMinus element, formalism::Builder& builder, C& context);

    template<formalism::Context C>
    Data<formalism::GroundFunctionExpression> translate_grounded(loki::FunctionExpressionFunction element, formalism::Builder& builder, C& context);

    template<formalism::Context C>
    Data<formalism::GroundFunctionExpression> translate_grounded(loki::FunctionExpression element, formalism::Builder& builder, C& context);

    template<formalism::Context C>
    IndexGroundFunctionTermVariant translate_grounded(loki::Function element, formalism::Builder& builder, C& context);

    template<formalism::Context C>
    IndexGroundFunctionTermValueVariant translate_grounded(loki::FunctionValue element, formalism::Builder& builder, C& context);

    template<formalism::Context C>
    Data<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>>
    translate_grounded(loki::ConditionNumericConstraint element, formalism::Builder& builder, C& context);

    template<formalism::Context C>
    std::tuple<IndexList<formalism::GroundLiteral<formalism::StaticTag>>,
               IndexList<formalism::GroundLiteral<formalism::FluentTag>>,
               IndexList<formalism::GroundLiteral<formalism::DerivedTag>>,
               DataList<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>>>
    translate_grounded(loki::Condition element, formalism::Builder& builder, C& context);

    template<formalism::Context C>
    Index<formalism::Metric> translate_grounded(loki::OptimizationMetric element, formalism::Builder& builder, C& context);

public:
    LokiToTyrTranslator();

    DomainPtr translate(const loki::Domain& domain, formalism::Builder& builder, formalism::RepositoryPtr context);

    LiftedTaskPtr translate(const loki::Problem& problem, formalism::Builder& builder, formalism::OverlayRepositoryPtr<formalism::Repository> context);
};

}

#endif