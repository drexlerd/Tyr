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

#ifndef TYR_FORMALISM_MERGE_HPP_
#define TYR_FORMALISM_MERGE_HPP_

#include "tyr/common/tuple.hpp"
#include "tyr/formalism/builder.hpp"
#include "tyr/formalism/canonicalization.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/merge_common.hpp"
#include "tyr/formalism/views.hpp"

namespace tyr::formalism
{

/**
 * Forward declarations
 */

template<Context C_SRC, Context C_DST>
auto merge(View<Index<GroundConjunctiveCondition>, C_SRC> element, MergeContext<C_SRC, C_DST>& context);

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Rule>, C_SRC> element, MergeContext<C_SRC, C_DST>& context);

template<Context C_SRC, Context C_DST>
auto merge(View<Index<GroundRule>, C_SRC> element, MergeContext<C_SRC, C_DST>& context);

/**
 * Implementations
 */

template<Context C_SRC, Context C_DST>
auto merge(View<Index<GroundConjunctiveCondition>, C_SRC> element, MergeContext<C_SRC, C_DST>& context)
{
    return with_cache<GroundConjunctiveCondition, GroundConjunctiveCondition>(
        element,
        context.cache,
        [&]()
        {
            auto conj_cond_ptr = context.builder.template get_builder<GroundConjunctiveCondition>();
            auto& conj_cond = *conj_cond_ptr;
            conj_cond.clear();

            for (const auto literal : element.template get_literals<StaticTag>())
                conj_cond.static_literals.push_back(merge(literal, context).get_index());
            for (const auto literal : element.template get_literals<FluentTag>())
                conj_cond.fluent_literals.push_back(merge(literal, context).get_index());
            for (const auto numeric_constraint : element.get_numeric_constraints())
                conj_cond.numeric_constraints.push_back(merge(numeric_constraint, context).get_data());

            canonicalize(conj_cond);
            return context.destination.get_or_create(conj_cond, context.builder.get_buffer()).first;
        });
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Rule>, C_SRC> element, MergeContext<C_SRC, C_DST>& context)
{
    return with_cache<Rule, Rule>(element,
                                  context.cache,
                                  [&]()
                                  {
                                      auto rule_ptr = context.builder.template get_builder<Rule>();
                                      auto& rule = *rule_ptr;
                                      rule.clear();

                                      rule.body = merge(element.get_body(), context).get_index();
                                      rule.head = merge(element.get_head(), context).get_index();

                                      canonicalize(rule);
                                      return context.destination.get_or_create(rule, context.builder.get_buffer()).first;
                                  });
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<GroundRule>, C_SRC> element, MergeContext<C_SRC, C_DST>& context)
{
    return with_cache<GroundRule, GroundRule>(element,
                                              context.cache,
                                              [&]()
                                              {
                                                  auto rule_ptr = context.builder.template get_builder<GroundRule>();
                                                  auto& rule = *rule_ptr;
                                                  rule.clear();

                                                  rule.rule = element.get_rule().get_index();
                                                  rule.body = merge(element.get_body(), context).get_index();
                                                  rule.head = merge(element.get_head(), context).get_index();

                                                  canonicalize(rule);
                                                  return context.destination.get_or_create(rule, context.builder.get_buffer()).first;
                                              });
}

}

#endif