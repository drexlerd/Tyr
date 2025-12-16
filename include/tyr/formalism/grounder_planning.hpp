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

#ifndef TYR_FORMALISM_GROUNDER_PLANNING_HPP_
#define TYR_FORMALISM_GROUNDER_PLANNING_HPP_

#include "tyr/analysis/domains.hpp"
#include "tyr/common/itertools.hpp"
#include "tyr/formalism/builder.hpp"
#include "tyr/formalism/canonicalization.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/grounder_common.hpp"
#include "tyr/formalism/planning/fdr_context.hpp"
#include "tyr/formalism/views.hpp"

namespace tyr::formalism
{
template<FactKind T_SRC, Context C_SRC, Context C_DST, FactKind T_DST>
View<Index<GroundAtom<T_DST>>, C_DST>
ground_planning(View<Index<Atom<T_SRC>>, C_SRC> element, MergeContext<C_SRC, C_DST>& merge_context, GrounderContext<C_SRC, C_DST>& grounder_context)
{
    // Fetch and clear
    auto atom_ptr = grounder_context.builder.template get_builder<GroundAtom<T_DST>>();
    auto& atom = *atom_ptr;
    atom.clear();

    // Fill data
    atom.predicate = merge<T_SRC, C_SRC, C_DST, T_DST>(element.get_predicate(), merge_context).get_index();
    atom.binding = ground_common(element.get_terms(), grounder_context).get_index();

    // Canonicalize and Serialize
    canonicalize(atom);
    return grounder_context.destination.get_or_create(atom, grounder_context.builder.get_buffer()).first;
}

template<FactKind T_SRC, Context C_SRC, Context C_DST, FactKind T_DST = T_SRC>
View<Index<GroundAtom<T_DST>>, C_DST> ground_planning(View<Index<Atom<T_SRC>>, C_SRC> element, GrounderContext<C_SRC, C_DST>& context)
{
    // Fetch and clear
    auto atom_ptr = context.builder.template get_builder<GroundAtom<T_DST>>();
    auto& atom = *atom_ptr;
    atom.clear();

    // Fill data
    atom.predicate = element.get_predicate().get_index();
    atom.binding = ground_common(element.get_terms(), context).get_index();

    // Canonicalize and Serialize
    canonicalize(atom);
    return context.destination.get_or_create(atom, context.builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST, typename FDR>
    requires FDRContext<FDR, C_DST>
View<Data<FDRFact<FluentTag>>, C_DST> ground_planning(View<Index<Atom<FluentTag>>, C_SRC> element, GrounderContext<C_SRC, C_DST>& context, FDR& fdr)
{
    return fdr.get_fact(ground_planning(element, context));
}

template<FactKind T, Context C_SRC, Context C_DST>
View<Index<GroundLiteral<T>>, C_DST> ground_planning(View<Index<Literal<T>>, C_SRC> element, GrounderContext<C_SRC, C_DST>& context)
{
    // Fetch and clear
    auto ground_literal_ptr = context.builder.template get_builder<GroundLiteral<T>>();
    auto& ground_literal = *ground_literal_ptr;
    ground_literal.clear();

    // Fill data
    ground_literal.polarity = element.get_polarity();
    ground_literal.atom = ground_planning(element.get_atom(), context).get_index();

    // Canonicalize and Serialize
    canonicalize(ground_literal);
    return context.destination.get_or_create(ground_literal, context.builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST, typename FDR>
    requires FDRContext<FDR, C_DST>
View<Data<FDRFact<FluentTag>>, C_DST> ground_planning(View<Index<Literal<FluentTag>>, C_SRC> element, GrounderContext<C_SRC, C_DST>& context, FDR& fdr)
{
    auto fact = ground_planning(element.get_atom(), context, fdr).get_data();
    if (!element.get_polarity())
        fact.value = FDRValue::none();

    return make_view(fact, context.destination);
}

template<Context C_SRC, Context C_DST, typename FDR>
    requires FDRContext<FDR, C_DST>
View<Index<GroundFDRConjunctiveCondition>, C_DST>
ground_planning(View<Index<FDRConjunctiveCondition>, C_SRC> element, GrounderContext<C_SRC, C_DST>& context, FDR& fdr)
{
    // Fetch and clear
    auto conj_cond_ptr = context.builder.template get_builder<GroundFDRConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    // Fill data
    for (const auto literal : element.template get_literals<StaticTag>())
        conj_cond.static_literals.push_back(ground_planning(literal, context).get_index());
    for (const auto literal : element.template get_literals<FluentTag>())
        conj_cond.fluent_facts.push_back(ground_planning(literal, context, fdr).get_data());
    for (const auto literal : element.template get_literals<DerivedTag>())
        conj_cond.derived_literals.push_back(ground_planning(literal, context).get_index());
    for (const auto numeric_constraint : element.get_numeric_constraints())
        conj_cond.numeric_constraints.push_back(ground_common(numeric_constraint, context).get_data());

    // Canonicalize and Serialize
    canonicalize(conj_cond);
    return context.destination.get_or_create(conj_cond, context.builder.get_buffer()).first;
}

template<NumericEffectOpKind Op, FactKind T, Context C_SRC, Context C_DST>
View<Index<GroundNumericEffect<Op, T>>, C_DST> ground_planning(View<Index<NumericEffect<Op, T>>, C_SRC> element, GrounderContext<C_SRC, C_DST>& context)
{
    // Fetch and clear
    auto numeric_effect_ptr = context.builder.template get_builder<GroundNumericEffect<Op, T>>();
    auto& numeric_effect = *numeric_effect_ptr;
    numeric_effect.clear();

    // Fill data
    numeric_effect.fterm = ground_common(element.get_fterm(), context).get_index();
    numeric_effect.fexpr = ground_common(element.get_fexpr(), context).get_data();

    // Canonicalize and Serialize
    canonicalize(numeric_effect);
    return context.destination.get_or_create(numeric_effect, context.builder.get_buffer()).first;
}

template<FactKind T, Context C_SRC, Context C_DST>
View<Data<GroundNumericEffectOperator<T>>, C_DST> ground_planning(View<Data<NumericEffectOperator<T>>, C_SRC> element, GrounderContext<C_SRC, C_DST>& context)
{
    return visit([&](auto&& arg) { return make_view(Data<GroundNumericEffectOperator<T>>(ground_planning(arg, context).get_index()), context.destination); },
                 element.get_variant());
}

template<Context C_SRC, Context C_DST, typename FDR>
    requires FDRContext<FDR, C_DST>
View<Index<GroundConjunctiveEffect>, C_DST> ground_planning(View<Index<ConjunctiveEffect>, C_SRC> element,
                                                            GrounderContext<C_SRC, C_DST>& context,
                                                            UnorderedMap<Index<FDRVariable<FluentTag>>, FDRValue>& assign,
                                                            FDR& fdr)
{
    // Fetch and clear
    auto conj_effect_ptr = context.builder.template get_builder<GroundConjunctiveEffect>();
    auto& conj_eff = *conj_effect_ptr;
    conj_eff.clear();

    // 1) create facts and variables
    for (const auto literal : element.get_literals())
        conj_eff.facts.push_back(ground_planning(literal, context, fdr).get_data());

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
        conj_eff.numeric_effects.push_back(ground_planning(numeric_effect, context).get_data());
    if (element.get_auxiliary_numeric_effect().has_value())
        conj_eff.auxiliary_numeric_effect = ground_planning(element.get_auxiliary_numeric_effect().value(), context).get_data();

    // Canonicalize and Serialize
    canonicalize(conj_eff);
    return context.destination.get_or_create(conj_eff, context.builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST, typename FDR>
    requires FDRContext<FDR, C_DST>
View<Index<GroundConditionalEffect>, C_DST> ground_planning(View<Index<ConditionalEffect>, C_SRC> element,
                                                            GrounderContext<C_SRC, C_DST>& context,
                                                            UnorderedMap<Index<FDRVariable<FluentTag>>, FDRValue>& assign,
                                                            FDR& fdr)
{
    // Fetch and clear
    auto cond_effect_ptr = context.builder.template get_builder<GroundConditionalEffect>();
    auto& cond_effect = *cond_effect_ptr;
    cond_effect.clear();

    // Fill data
    cond_effect.condition = ground_planning(element.get_condition(), context, fdr).get_index();
    cond_effect.effect = ground_planning(element.get_effect(), context, assign, fdr).get_index();

    // Canonicalize and Serialize
    canonicalize(cond_effect);
    return context.destination.get_or_create(cond_effect, context.builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST, typename FDR>
    requires FDRContext<FDR, C_DST>
View<Index<GroundAction>, C_DST> ground_planning(View<Index<Action>, C_SRC> element,
                                                 GrounderContext<C_SRC, C_DST>& context,
                                                 const analysis::DomainListListList& cond_effect_domains,
                                                 UnorderedMap<Index<FDRVariable<FluentTag>>, FDRValue>& assign,
                                                 FDR& fdr)
{
    // Fetch and clear
    auto action_ptr = context.builder.template get_builder<GroundAction>();
    auto& action = *action_ptr;
    action.clear();

    // Fill data
    action.action = element.get_index();
    action.binding = context.binding.get_index();
    action.condition = ground_planning(element.get_condition(), context, fdr).get_index();

    auto binding_full_ptr = context.builder.template get_builder<Binding>();
    auto& binding_full = *binding_full_ptr;

    for (uint_t cond_effect_index = 0; cond_effect_index < element.get_effects().size(); ++cond_effect_index)
    {
        const auto cond_effect = element.get_effects()[cond_effect_index];
        const auto& parameter_domains = cond_effect_domains[cond_effect_index];

        // Ensure that we stripped off the action precondition parameter domains.
        assert(std::distance(parameter_domains.begin(), parameter_domains.end()) == static_cast<int>(cond_effect.get_condition().get_arity()));

        for_element_in_cartesian_set(parameter_domains.begin(),
                                     parameter_domains.end(),
                                     [&](auto&& binding_cond)
                                     {
                                         binding_full.clear();
                                         binding_full.objects = context.binding.get_data().objects;
                                         binding_full.objects.insert(binding_full.objects.end(), binding_cond.begin(), binding_cond.end());

                                         canonicalize(binding_full);
                                         const auto new_binding_full = context.destination.get_or_create(binding_full, context.builder.get_buffer()).first;

                                         auto new_context = GrounderContext { context.builder, context.destination, new_binding_full, context.cache };

                                         action.effects.push_back(ground_planning(cond_effect, new_context, assign, fdr).get_index());
                                     });
    }

    // Canonicalize and Serialize
    canonicalize(action);
    return context.destination.get_or_create(action, context.builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST, typename FDR>
    requires FDRContext<FDR, C_DST>
View<Index<GroundAxiom>, C_DST> ground_planning(View<Index<Axiom>, C_SRC> element, GrounderContext<C_SRC, C_DST>& context, FDR& fdr)
{
    // Fetch and clear
    auto axiom_ptr = context.builder.template get_builder<GroundAxiom>();
    auto& axiom = *axiom_ptr;
    axiom.clear();

    // Fill data
    axiom.axiom = element.get_index();
    axiom.binding = context.binding.get_index();
    axiom.body = ground_planning(element.get_body(), context, fdr).get_index();
    axiom.head = ground_planning(element.get_head(), context).get_index();

    // Canonicalize and Serialize
    canonicalize(axiom);
    return context.destination.get_or_create(axiom, context.builder.get_buffer()).first;
}

}

#endif