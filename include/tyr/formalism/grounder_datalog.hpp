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

#ifndef TYR_FORMALISM_GROUNDER_DATALOG_HPP_
#define TYR_FORMALISM_GROUNDER_DATALOG_HPP_

#include "tyr/analysis/domains.hpp"
#include "tyr/formalism/builder.hpp"
#include "tyr/formalism/canonicalization.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/grounder_common.hpp"
#include "tyr/formalism/merge.hpp"
#include "tyr/formalism/views.hpp"

namespace tyr::formalism
{
template<FactKind T, Context C_SRC, Context C_DST>
View<Index<GroundAtom<T>>, C_DST>
ground_datalog(View<Index<Atom<T>>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
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

template<FactKind T, Context C_SRC, Context C_DST>
View<Index<GroundLiteral<T>>, C_DST>
ground_datalog(View<Index<Literal<T>>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    // Fetch and clear
    auto ground_literal_ptr = builder.template get_builder<GroundLiteral<T>>();
    auto& ground_literal = *ground_literal_ptr;
    ground_literal.clear();

    // Fill data
    ground_literal.polarity = element.get_polarity();
    ground_literal.atom = ground_datalog(element.get_atom(), binding, builder, destination).get_index();

    // Canonicalize and Serialize
    canonicalize(ground_literal);
    return destination.get_or_create(ground_literal, builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST>
View<Index<GroundConjunctiveCondition>, C_DST>
ground_datalog(View<Index<ConjunctiveCondition>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    // Fetch and clear
    auto conj_cond_ptr = builder.get_builder<GroundConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    // Fill data
    for (const auto literal : element.template get_literals<StaticTag>())
        conj_cond.static_literals.push_back(ground_datalog(literal, binding, builder, destination).get_index());
    for (const auto literal : element.template get_literals<FluentTag>())
        conj_cond.fluent_literals.push_back(ground_datalog(literal, binding, builder, destination).get_index());
    for (const auto literal : element.template get_literals<DerivedTag>())
        conj_cond.derived_literals.push_back(ground_datalog(literal, binding, builder, destination).get_index());
    for (const auto numeric_constraint : element.get_numeric_constraints())
        conj_cond.numeric_constraints.push_back(ground_common(numeric_constraint, binding, builder, destination).get_data());

    // Canonicalize and Serialize
    canonicalize(conj_cond);
    return destination.get_or_create(conj_cond, builder.get_buffer()).first;
}

template<Context C_SRC, Context C_DST>
View<Index<GroundRule>, C_DST> ground_datalog(View<Index<Rule>, C_SRC> element, View<IndexList<Object>, C_DST> binding, Builder& builder, C_DST& destination)
{
    // Fetch and clear
    auto rule_ptr = builder.get_builder<GroundRule>();
    auto& rule = *rule_ptr;
    rule.clear();

    // Fill data
    rule.rule = element.get_index();
    rule.binding = merge(binding, builder, destination).get_index();
    rule.body = ground_datalog(element.get_body(), binding, builder, destination).get_index();
    rule.head = ground_datalog(element.get_head(), binding, builder, destination).get_index();

    // Canonicalize and Serialize
    canonicalize(rule);
    return destination.get_or_create(rule, builder.get_buffer()).first;
}

}

#endif