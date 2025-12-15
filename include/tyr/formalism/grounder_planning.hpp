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
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/grounder_common.hpp"
#include "tyr/formalism/views.hpp"

namespace tyr::formalism
{
template<FactKind T, Context C_SRC, Context C_DST>
View<Index<GroundAtom<T>>, C_DST>
ground_planning(View<Index<Atom<T>>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    // Fetch and clear
    auto atom_ptr = builder.template get_builder<GroundAtom<T>>();
    auto& atom = *atom_ptr;
    atom.clear();

    // Fill data
    atom.predicate = element.get_predicate().get_index();
    atom.binding = ground_common(element.get_terms(), binding, builder, destination).get_index();

    // Canonicalize and Serialize
    canonicalize(atom);
    return destination.get_or_create(atom, builder.get_buffer()).first;
}

template<Context C>
inline auto create_fdr_variable(View<Index<GroundAtom<FluentTag>>, C> element, Builder& builder, C& destination)
{
    // Fetch and clear
    auto variable_ptr = builder.template get_builder<FDRVariable<FluentTag>>();
    auto& variable = *variable_ptr;
    variable.clear();

    // Fill data
    variable.domain_size = 2;
    variable.atoms.push_back(element.get_index());

    // Canonicalize and Serialize
    canonicalize(variable);
    return destination.get_or_create(variable, builder.get_buffer());
}

template<Context C_SRC, Context C_DST>
View<Data<FDRFact<FluentTag>>, C_DST>
ground_planning(View<Index<Atom<FluentTag>>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    // Fetch and clear
    auto atom_ptr = builder.template get_builder<GroundAtom<FluentTag>>();
    auto& atom = *atom_ptr;
    atom.clear();

    // Fill data
    atom.predicate = element.get_predicate().get_index();
    atom.binding = ground_common(element.get_terms(), binding, builder, destination).get_index();

    canonicalize(atom);
    const auto [new_atom, new_atom_inserted] = destination.get_or_create(atom, builder.get_buffer());

    if (new_atom_inserted)
    {
        const auto [new_variable, new_variable_inserted] = create_fdr_variable(new_atom, builder, destination);

        std::cout << new_atom.get_index() << " " << new_atom << " " << new_variable.get_index() << " " << new_variable << std::endl;

        assert(new_variable_inserted);
        assert(new_atom.get_index().get_value() == new_variable.get_index().get_value());
    }
    else
    {
        // Invariant check: ensure that a corresponding FDR variable exists
        assert(!create_fdr_variable(new_atom, builder, destination).second);
    }

    return make_view(Data<FDRFact<FluentTag>>(Index<FDRVariable<FluentTag>>(new_atom.get_index().get_value()), FDRValue { 1 }), destination);
}

template<FactKind T, Context C_SRC, Context C_DST>
View<Index<GroundLiteral<T>>, C_DST>
ground_planning(View<Index<Literal<T>>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    // Fetch and clear
    auto ground_literal_ptr = builder.template get_builder<GroundLiteral<T>>();
    auto& ground_literal = *ground_literal_ptr;
    ground_literal.clear();

    // Fill data
    ground_literal.polarity = element.get_polarity();
    ground_literal.atom = ground_planning(element.get_atom(), binding, builder, destination).get_index();

    // Canonicalize and Serialize
    canonicalize(ground_literal);
    return destination.get_or_create(ground_literal, builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST>
View<Data<FDRFact<FluentTag>>, C_DST>
ground_planning(View<Index<Literal<FluentTag>>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    auto fact = ground_planning(element.get_atom(), binding, builder, destination).get_data();
    if (!element.get_polarity())
        fact.value = FDRValue { 0 };

    return make_view(fact, destination);
}

template<Context C_SRC, Context C_DST>
View<Index<GroundFDRConjunctiveCondition>, C_DST>
ground_planning(View<Index<ConjunctiveCondition>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    // Fetch and clear
    auto conj_cond_ptr = builder.get_builder<GroundFDRConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    // Fill data
    for (const auto literal : element.template get_literals<StaticTag>())
        conj_cond.static_literals.push_back(ground_planning(literal, binding, builder, destination).get_index());
    for (const auto literal : element.template get_literals<FluentTag>())
        conj_cond.fluent_facts.push_back(ground_planning(literal, binding, builder, destination).get_data());
    for (const auto literal : element.template get_literals<DerivedTag>())
        conj_cond.derived_literals.push_back(ground_planning(literal, binding, builder, destination).get_index());
    for (const auto numeric_constraint : element.get_numeric_constraints())
        conj_cond.numeric_constraints.push_back(ground_common(numeric_constraint, binding, builder, destination).get_data());

    // Canonicalize and Serialize
    canonicalize(conj_cond);
    return destination.get_or_create(conj_cond, builder.get_buffer()).first;
}

template<NumericEffectOpKind Op, FactKind T, Context C_SRC, Context C_DST>
View<Index<GroundNumericEffect<Op, T>>, C_DST>
ground_planning(View<Index<NumericEffect<Op, T>>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    // Fetch and clear
    auto numeric_effect_ptr = builder.template get_builder<GroundNumericEffect<Op, T>>();
    auto& numeric_effect = *numeric_effect_ptr;
    numeric_effect.clear();

    // Fill data
    numeric_effect.fterm = ground_common(element.get_fterm(), binding, builder, destination).get_index();
    numeric_effect.fexpr = ground_common(element.get_fexpr(), binding, builder, destination).get_data();

    // Canonicalize and Serialize
    canonicalize(numeric_effect);
    return destination.get_or_create(numeric_effect, builder.get_buffer()).first;
}

template<FactKind T, Context C_SRC, Context C_DST>
View<Data<GroundNumericEffectOperator<T>>, C_DST>
ground_planning(View<Data<NumericEffectOperator<T>>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    return visit([&](auto&& arg)
                 { return make_view(Data<GroundNumericEffectOperator<T>>(ground_planning(arg, binding, builder, destination).get_index()), destination); },
                 element.get_variant());
}

template<Context C_SRC, Context C_DST>
View<Index<GroundConjunctiveEffect>, C_DST> ground_planning(View<Index<ConjunctiveEffect>, C_SRC> element,
                                                            View<IndexList<Object>, C_DST> binding,
                                                            UnorderedMap<Index<FDRVariable<FluentTag>>, FDRValue>& assign,
                                                            Builder& builder,
                                                            C_DST& destination)
{
    // Fetch and clear
    auto conj_effect_ptr = builder.get_builder<GroundConjunctiveEffect>();
    auto& conj_eff = *conj_effect_ptr;
    conj_eff.clear();

    // 0) create facts and variables
    for (const auto literal : element.get_literals())
        conj_eff.facts.push_back(ground_planning(literal, binding, builder, destination).get_data());

    // 1) deletes first
    assign.clear();
    for (const auto fact : conj_eff.facts)
        if (fact.value == FDRValue::none())
            assign[fact.variable] = fact.value;  // should be none()

    // 2) adds second (overwrite delete)
    for (const auto fact : conj_eff.facts)
        if (fact.value != FDRValue::none())
            assign[fact.variable] = fact.value;

    // 3) materialize
    conj_eff.facts.clear();
    for (const auto& [var, val] : assign)
        conj_eff.facts.push_back(Data<FDRFact<FluentTag>>(var, val));

    // Fill data
    for (const auto literal : element.get_literals())
        conj_eff.facts.push_back(ground_planning(literal, binding, builder, destination).get_data());
    for (const auto numeric_effect : element.get_numeric_effects())
        conj_eff.numeric_effects.push_back(ground_planning(numeric_effect, binding, builder, destination).get_data());
    if (element.get_auxiliary_numeric_effect().has_value())
        conj_eff.auxiliary_numeric_effect = ground_planning(element.get_auxiliary_numeric_effect().value(), binding, builder, destination).get_data();

    // Canonicalize and Serialize
    canonicalize(conj_eff);
    return destination.get_or_create(conj_eff, builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST>
View<Index<GroundConditionalEffect>, C_DST> ground_planning(View<Index<ConditionalEffect>, C_SRC> element,
                                                            View<IndexList<Object>, C_DST> binding,
                                                            UnorderedMap<Index<FDRVariable<FluentTag>>, FDRValue>& assign,
                                                            Builder& builder,
                                                            C_DST& destination)
{
    // Fetch and clear
    auto cond_effect_ptr = builder.get_builder<GroundConditionalEffect>();
    auto& cond_effect = *cond_effect_ptr;
    cond_effect.clear();

    // Fill data
    cond_effect.condition = ground_planning(element.get_condition(), binding, builder, destination).get_index();
    cond_effect.effect = ground_planning(element.get_effect(), binding, assign, builder, destination).get_index();

    // Canonicalize and Serialize
    canonicalize(cond_effect);
    return destination.get_or_create(cond_effect, builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST>
View<Index<GroundAction>, C_DST> ground_planning(View<Index<Action>, C_SRC> element,
                                                 View<IndexList<Object>, C_DST> binding,
                                                 IndexList<Object>& binding_full,
                                                 const analysis::DomainListListList& cond_effect_domains,
                                                 UnorderedMap<Index<FDRVariable<FluentTag>>, FDRValue>& assign,
                                                 Builder& builder,
                                                 C_DST& destination)
{
    // Fetch and clear
    auto action_ptr = builder.get_builder<GroundAction>();
    auto& action = *action_ptr;
    action.clear();

    // Fill data
    action.action = element.get_index();
    action.condition = ground_planning(element.get_condition(), binding, builder, destination).get_index();

    binding_full = binding.get_data();

    for (uint_t cond_effect_index = 0; cond_effect_index < element.get_effects().size(); ++cond_effect_index)
    {
        const auto cond_effect = element.get_effects()[cond_effect_index];
        const auto& parameter_domains = cond_effect_domains[cond_effect_index];

        // Ensure that we stripped off the action precondition parameter domains.
        assert(std::distance(parameter_domains.begin(), parameter_domains.end()) == static_cast<int>(cond_effect.get_condition().get_arity()));

        for_element_in_cartesian_set(
            parameter_domains.begin(),
            parameter_domains.end(),
            [&](auto&& binding_cond)
            {
                binding_full.resize(binding.size());  // strip from prev cond effect
                binding_full.insert(binding_full.end(), binding_cond.begin(), binding_cond.end());

                action.effects.push_back(
                    ground_planning(cond_effect, make_view(binding_full, binding.get_context()), assign, builder, destination).get_index());
            });
    }

    // Canonicalize and Serialize
    canonicalize(action);
    return destination.get_or_create(action, builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST>
View<Index<GroundAxiom>, C_DST> ground_planning(View<Index<Axiom>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    // Fetch and clear
    auto axiom_ptr = builder.get_builder<GroundAxiom>();
    auto& axiom = *axiom_ptr;
    axiom.clear();

    // Fill data
    axiom.axiom = element.get_index();
    axiom.body = ground_planning(element.get_body(), binding, builder, destination).get_index();
    axiom.head = ground_planning(element.get_head(), binding, builder, destination).get_index();

    // Canonicalize and Serialize
    canonicalize(axiom);
    return destination.get_or_create(axiom, builder.get_buffer()).first;
}

}

#endif