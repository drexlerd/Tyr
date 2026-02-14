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

#ifndef TYR_FORMALISM_PLANNING_GROUNDER_PLANNING_HPP_
#define TYR_FORMALISM_PLANNING_GROUNDER_PLANNING_HPP_

#include "tyr/analysis/domains.hpp"
#include "tyr/common/itertools.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/planning/builder.hpp"
#include "tyr/formalism/planning/canonicalization.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/fdr_context.hpp"
#include "tyr/formalism/planning/grounder.hpp"
#include "tyr/formalism/planning/merge.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"

namespace tyr::formalism::planning
{
struct GrounderContext
{
    Builder& builder;
    Repository& destination;
    IndexList<Object>& binding;
};

inline auto ground(View<DataList<Term>, Repository> element, GrounderContext& context)
{
    // Fetch and clear
    auto binding_ptr = context.builder.template get_builder<Binding>();
    auto& binding = *binding_ptr;
    binding.clear();

    // Fill data
    for (const auto term : element)
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                    binding.objects.push_back(context.binding[uint_t(arg)]);
                else if constexpr (std::is_same_v<Alternative, View<Index<Object>, Repository>>)
                    binding.objects.push_back(arg.get_index());
                else
                    static_assert(dependent_false<Alternative>::value, "Missing case");
            },
            term.get_variant());
    }

    // Canonicalize and Serialize
    canonicalize(binding);
    return context.destination.get_or_create(binding, context.builder.get_buffer());
}

inline auto ground(const IndexList<Object>& element, GrounderContext& context)
{
    // Fetch and clear
    auto binding_ptr = context.builder.template get_builder<Binding>();
    auto& binding = *binding_ptr;
    binding.clear();

    // Fill data
    binding.objects = element;

    // Canonicalize and Serialize
    canonicalize(binding);
    return context.destination.get_or_create(binding, context.builder.get_buffer());
}

template<FactKind T>
inline auto ground(View<Index<FunctionTerm<T>>, Repository> element, GrounderContext& context)
{
    // Fetch and clear
    auto fterm_ptr = context.builder.template get_builder<GroundFunctionTerm<T>>();
    auto& fterm = *fterm_ptr;
    fterm.clear();

    // Fill data
    fterm.function = element.get_function().get_index();
    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                    fterm.objects.push_back(context.binding[uint_t(arg)]);
                else if constexpr (std::is_same_v<Alternative, View<Index<Object>, Repository>>)
                    fterm.objects.push_back(arg.get_index());
                else
                    static_assert(dependent_false<Alternative>::value, "Missing case");
            },
            term.get_variant());
    }

    // Canonicalize and Serialize
    canonicalize(fterm);
    return context.destination.get_or_create(fterm, context.builder.get_buffer());
}

inline auto ground(View<Data<FunctionExpression>, Repository> element, GrounderContext& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return Data<GroundFunctionExpression>(arg);
            else if constexpr (std::is_same_v<Alternative, View<Data<ArithmeticOperator<Data<FunctionExpression>>>, Repository>>)
                return Data<GroundFunctionExpression>(ground(arg, context));
            else
                return Data<GroundFunctionExpression>(ground(arg, context).first);
        },
        element.get_variant());
}

template<OpKind O>
inline auto ground(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, Repository> element, GrounderContext& context)
{
    // Fetch and clear
    auto unary_ptr = context.builder.template get_builder<UnaryOperator<O, Data<GroundFunctionExpression>>>();
    auto& unary = *unary_ptr;
    unary.clear();

    // Fill data
    unary.arg = ground(element.get_arg(), context);

    // Canonicalize and Serialize
    canonicalize(unary);
    return context.destination.get_or_create(unary, context.builder.get_buffer());
}

template<OpKind O>
inline auto ground(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, Repository> element, GrounderContext& context)
{
    // Fetch and clear
    auto binary_ptr = context.builder.template get_builder<BinaryOperator<O, Data<GroundFunctionExpression>>>();
    auto& binary = *binary_ptr;
    binary.clear();

    // Fill data
    binary.lhs = ground(element.get_lhs(), context);
    binary.rhs = ground(element.get_rhs(), context);

    // Canonicalize and Serialize
    canonicalize(binary);
    return context.destination.get_or_create(binary, context.builder.get_buffer());
}

template<OpKind O>
inline auto ground(View<Index<MultiOperator<O, Data<FunctionExpression>>>, Repository> element, GrounderContext& context)
{
    // Fetch and clear
    auto multi_ptr = context.builder.template get_builder<MultiOperator<O, Data<GroundFunctionExpression>>>();
    auto& multi = *multi_ptr;
    multi.clear();

    // Fill data
    for (const auto arg : element.get_args())
        multi.args.push_back(ground(arg, context));

    // Canonicalize and Serialize
    canonicalize(multi);
    return context.destination.get_or_create(multi, context.builder.get_buffer());
}

inline auto ground(View<Data<BooleanOperator<Data<FunctionExpression>>>, Repository> element, GrounderContext& context)
{
    return visit([&](auto&& arg) { return Data<BooleanOperator<Data<GroundFunctionExpression>>>(ground(arg, context).first); }, element.get_variant());
}

inline auto ground(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, Repository> element, GrounderContext& context)
{
    return visit([&](auto&& arg) { return Data<ArithmeticOperator<Data<GroundFunctionExpression>>>(ground(arg, context).first); }, element.get_variant());
}

template<FactKind T_SRC, FactKind T_DST>
inline auto ground(View<Index<Atom<T_SRC>>, Repository> element, MergeContext& merge_context, GrounderContext& grounder_context)
{
    // Fetch and clear
    auto atom_ptr = grounder_context.builder.template get_builder<GroundAtom<T_DST>>();
    auto& atom = *atom_ptr;
    atom.clear();

    // Fill data
    atom.predicate = merge<T_SRC, T_DST>(element.get_predicate(), merge_context).first;
    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                    atom.objects.push_back(grounder_context.binding[uint_t(arg)]);
                else if constexpr (std::is_same_v<Alternative, View<Index<Object>, Repository>>)
                    atom.objects.push_back(arg.get_index());
                else
                    static_assert(dependent_false<Alternative>::value, "Missing case");
            },
            term.get_variant());
    }

    // Canonicalize and Serialize
    canonicalize(atom);
    return grounder_context.destination.get_or_create(atom, grounder_context.builder.get_buffer());
}

template<FactKind T_SRC, FactKind T_DST = T_SRC>
inline auto ground(View<Index<Atom<T_SRC>>, Repository> element, GrounderContext& context)
{
    // Fetch and clear
    auto atom_ptr = context.builder.template get_builder<GroundAtom<T_DST>>();
    auto& atom = *atom_ptr;
    atom.clear();

    // Fill data
    atom.predicate = element.get_predicate().get_index();
    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                    atom.objects.push_back(context.binding[uint_t(arg)]);
                else if constexpr (std::is_same_v<Alternative, View<Index<Object>, Repository>>)
                    atom.objects.push_back(arg.get_index());
                else
                    static_assert(dependent_false<Alternative>::value, "Missing case");
            },
            term.get_variant());
    }

    // Canonicalize and Serialize
    canonicalize(atom);
    return context.destination.get_or_create(atom, context.builder.get_buffer());
}

template<typename FDR>
    requires FDRContext<FDR>
inline auto ground(View<Index<Atom<FluentTag>>, Repository> element, GrounderContext& context, FDR& fdr)
{
    return fdr.get_fact(ground(element, context).first);
}

template<FactKind T>
inline auto ground(View<Index<Literal<T>>, Repository> element, GrounderContext& context)
{
    // Fetch and clear
    auto ground_literal_ptr = context.builder.template get_builder<GroundLiteral<T>>();
    auto& ground_literal = *ground_literal_ptr;
    ground_literal.clear();

    // Fill data
    ground_literal.polarity = element.get_polarity();
    ground_literal.atom = ground(element.get_atom(), context).first;

    // Canonicalize and Serialize
    canonicalize(ground_literal);
    return context.destination.get_or_create(ground_literal, context.builder.get_buffer());
}

template<typename FDR>
    requires FDRContext<FDR>
inline auto ground(View<Index<Literal<FluentTag>>, Repository> element, GrounderContext& context, FDR& fdr)
{
    auto fact = ground(element.get_atom(), context, fdr);
    if (!element.get_polarity())
        fact.value = FDRValue::none();

    return fact;
}

template<typename FDR>
    requires FDRContext<FDR>
inline auto ground(View<Index<ConjunctiveCondition>, Repository> element, GrounderContext& context, FDR& fdr)
{
    // Fetch and clear
    auto conj_cond_ptr = context.builder.template get_builder<GroundConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    // Fill data
    for (const auto literal : element.template get_literals<StaticTag>())
        conj_cond.static_literals.push_back(ground(literal, context).first);
    for (const auto literal : element.template get_literals<FluentTag>())
        conj_cond.fluent_facts.push_back(ground(literal, context, fdr));
    for (const auto literal : element.template get_literals<DerivedTag>())
        conj_cond.derived_literals.push_back(ground(literal, context).first);
    for (const auto numeric_constraint : element.get_numeric_constraints())
        conj_cond.numeric_constraints.push_back(ground(numeric_constraint, context));

    // Canonicalize and Serialize
    canonicalize(conj_cond);
    return context.destination.get_or_create(conj_cond, context.builder.get_buffer());
}

template<NumericEffectOpKind Op, FactKind T>
inline auto ground(View<Index<NumericEffect<Op, T>>, Repository> element, GrounderContext& context)
{
    // Fetch and clear
    auto numeric_effect_ptr = context.builder.template get_builder<GroundNumericEffect<Op, T>>();
    auto& numeric_effect = *numeric_effect_ptr;
    numeric_effect.clear();

    // Fill data
    numeric_effect.fterm = ground(element.get_fterm(), context).first;
    numeric_effect.fexpr = ground(element.get_fexpr(), context);

    // Canonicalize and Serialize
    canonicalize(numeric_effect);
    return context.destination.get_or_create(numeric_effect, context.builder.get_buffer());
}

template<FactKind T>
inline auto ground(View<Data<NumericEffectOperator<T>>, Repository> element, GrounderContext& context)
{
    return visit([&](auto&& arg) { return Data<GroundNumericEffectOperator<T>>(ground(arg, context).first); }, element.get_variant());
}

template<typename FDR>
    requires FDRContext<FDR>
inline auto
ground(View<Index<ConjunctiveEffect>, Repository> element, GrounderContext& context, UnorderedMap<Index<FDRVariable<FluentTag>>, FDRValue>& assign, FDR& fdr)
{
    // Fetch and clear
    auto conj_effect_ptr = context.builder.template get_builder<GroundConjunctiveEffect>();
    auto& conj_eff = *conj_effect_ptr;
    conj_eff.clear();

    // 1) create facts and variables
    for (const auto literal : element.get_literals())
        conj_eff.facts.push_back(ground(literal, context, fdr));

    // 2) deletes first
    assign.clear();
    for (const auto fact : conj_eff.facts)
        if (fact.value == FDRValue::none())
            assign[fact.variable] = fact.value;  // should be none()

    // 3) adds second (overwrite delete)
    for (const auto fact : conj_eff.facts)
        if (fact.value != FDRValue::none())
            assign[fact.variable] = fact.value;

    // 4) materialize
    conj_eff.facts.clear();
    for (const auto& [var, val] : assign)
        conj_eff.facts.push_back(Data<FDRFact<FluentTag>>(var, val));

    // Fill remaining data
    for (const auto numeric_effect : element.get_numeric_effects())
        conj_eff.numeric_effects.push_back(ground(numeric_effect, context));
    if (element.get_auxiliary_numeric_effect().has_value())
        conj_eff.auxiliary_numeric_effect = ground(element.get_auxiliary_numeric_effect().value(), context);

    // Canonicalize and Serialize
    canonicalize(conj_eff);
    return context.destination.get_or_create(conj_eff, context.builder.get_buffer());
}

template<typename FDR>
    requires FDRContext<FDR>
inline auto
ground(View<Index<ConditionalEffect>, Repository> element, GrounderContext& context, UnorderedMap<Index<FDRVariable<FluentTag>>, FDRValue>& assign, FDR& fdr)
{
    // Fetch and clear
    auto cond_effect_ptr = context.builder.template get_builder<GroundConditionalEffect>();
    auto& cond_effect = *cond_effect_ptr;
    cond_effect.clear();

    // Fill data
    cond_effect.condition = ground(element.get_condition(), context, fdr).first;
    cond_effect.effect = ground(element.get_effect(), context, assign, fdr).first;

    // Canonicalize and Serialize
    canonicalize(cond_effect);
    return context.destination.get_or_create(cond_effect, context.builder.get_buffer());
}

template<typename FDR>
    requires FDRContext<FDR>
inline auto ground(View<Index<Action>, Repository> element,
                   GrounderContext& context,
                   const analysis::DomainListListList& cond_effect_domains,
                   UnorderedMap<Index<FDRVariable<FluentTag>>, FDRValue>& assign,
                   itertools::cartesian_set::Workspace<Index<formalism::Object>>& iter_workspace,
                   FDR& fdr)
{
    // Fetch and clear
    auto action_ptr = context.builder.template get_builder<GroundAction>();
    auto& action = *action_ptr;
    action.clear();

    // Fill data
    action.action = element.get_index();
    action.binding = ground(context.binding, context).first;
    action.condition = ground(element.get_condition(), context, fdr).first;

    auto binding_size = context.binding.size();

    for (uint_t cond_effect_index = 0; cond_effect_index < element.get_effects().size(); ++cond_effect_index)
    {
        const auto cond_effect = element.get_effects()[cond_effect_index];
        const auto& parameter_domains = cond_effect_domains[cond_effect_index];

        // Ensure that we stripped off the action precondition parameter domains.
        assert(std::distance(parameter_domains.begin(), parameter_domains.end()) == static_cast<int>(cond_effect.get_arity()));

        itertools::cartesian_set::for_each_element(parameter_domains.begin(),
                                                   parameter_domains.end(),
                                                   iter_workspace,
                                                   [&](auto&& binding_cond)
                                                   {
                                                       // push the additional parameters to the end
                                                       context.binding.resize(binding_size);
                                                       context.binding.insert(context.binding.end(), binding_cond.begin(), binding_cond.end());

                                                       action.effects.push_back(ground(cond_effect, context, assign, fdr).first);
                                                   });
    }
    context.binding.resize(binding_size);  ///< important to restore the binding in case of grounding other actions

    // Canonicalize and Serialize
    canonicalize(action);
    return context.destination.get_or_create(action, context.builder.get_buffer());
}

template<typename FDR>
    requires FDRContext<FDR>
inline auto ground(View<Index<Axiom>, Repository> element, GrounderContext& context, FDR& fdr)
{
    // Fetch and clear
    auto axiom_ptr = context.builder.template get_builder<GroundAxiom>();
    auto& axiom = *axiom_ptr;
    axiom.clear();

    // Fill data
    axiom.axiom = element.get_index();
    axiom.binding = ground(context.binding, context).first;
    axiom.body = ground(element.get_body(), context, fdr).first;
    axiom.head = ground(element.get_head(), context).first;

    // Canonicalize and Serialize
    canonicalize(axiom);
    return context.destination.get_or_create(axiom, context.builder.get_buffer());
}

}

#endif