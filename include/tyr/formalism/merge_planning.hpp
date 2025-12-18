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

#ifndef TYR_FORMALISM_MERGE_PLANNING_HPP_
#define TYR_FORMALISM_MERGE_PLANNING_HPP_

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

template<NumericEffectOpKind O, FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<NumericEffect<O, T>>, C_SRC> element, MergeContext<C_DST>& context);

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Data<NumericEffectOperator<T>>, C_SRC> element, MergeContext<C_DST>& context);

template<NumericEffectOpKind O, FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<GroundNumericEffect<O, T>>, C_SRC> element, MergeContext<C_DST>& context);

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Data<GroundNumericEffectOperator<T>>, C_SRC> element, MergeContext<C_DST>& context);

template<Context C_SRC, Context C_DST>
auto merge(View<Index<FDRVariable<FluentTag>>, C_SRC> element, MergeContext<C_DST>& context);

template<Context C_SRC, Context C_DST>
auto merge(View<Data<FDRFact<FluentTag>>, C_SRC> element, MergeContext<C_DST>& context);

template<Context C_SRC, Context C_DST>
auto merge(View<Index<FDRConjunctiveCondition>, C_SRC> element, MergeContext<C_DST>& context);

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Axiom>, C_SRC> element, MergeContext<C_DST>& context);

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Metric>, C_SRC> element, MergeContext<C_DST>& context);

template<Context C_SRC, Context C_DST, typename FDR>
    requires FDRContext<FDR>
auto merge(View<Index<GroundFDRConjunctiveCondition>, C_SRC> element, MergeContext<C_DST>& context, FDR& fdr);

template<Context C_SRC, Context C_DST, typename FDR>
    requires FDRContext<FDR>
auto merge(View<Index<GroundConjunctiveEffect>, C_SRC> element, MergeContext<C_DST>& context, FDR& fdr);

template<Context C_SRC, Context C_DST, typename FDR>
    requires FDRContext<FDR>
auto merge(View<Index<GroundConditionalEffect>, C_SRC> element, MergeContext<C_DST>& context, FDR& fdr);

template<Context C_SRC, Context C_DST, typename FDR>
    requires FDRContext<FDR>
auto merge(View<Index<GroundAction>, C_SRC> element, MergeContext<C_DST>& context, FDR& fdr);

template<Context C_SRC, Context C_DST, typename FDR>
    requires FDRContext<FDR>
auto merge(View<Index<GroundAxiom>, C_SRC> element, MergeContext<C_DST>& context, FDR& fdr);

/**
 * Implementations
 */

template<NumericEffectOpKind O, FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<NumericEffect<O, T>>, C_SRC> element, MergeContext<C_DST>& context)
{
    return with_cache<NumericEffect<O, T>, NumericEffect<O, T>>(element,
                                                                context.cache,
                                                                [&]()
                                                                {
                                                                    auto numeric_effect_ptr = context.builder.template get_builder<NumericEffect<O, T>>();
                                                                    auto& numeric_effect = *numeric_effect_ptr;
                                                                    numeric_effect.clear();

                                                                    numeric_effect.fterm = merge(element.get_fterm(), context).first;
                                                                    numeric_effect.fexpr = merge(element.get_fexpr(), context).first;

                                                                    canonicalize(numeric_effect);
                                                                    return context.destination.get_or_create(numeric_effect, context.builder.get_buffer());
                                                                });
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Data<NumericEffectOperator<T>>, C_SRC> element, MergeContext<C_DST>& context)
{
    return visit([&](auto&& arg) { return Data<NumericEffectOperator<T>>(merge(arg, context).get_index()); }, element.get_variant());
}

template<NumericEffectOpKind O, FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<GroundNumericEffect<O, T>>, C_SRC> element, MergeContext<C_DST>& context)
{
    return with_cache<GroundNumericEffect<O, T>, GroundNumericEffect<O, T>>(
        element,
        context.cache,
        [&]()
        {
            auto numeric_effect_ptr = context.builder.template get_builder<GroundNumericEffect<O, T>>();
            auto& numeric_effect = *numeric_effect_ptr;
            numeric_effect.clear();

            numeric_effect.fterm = merge(element.get_fterm(), context).first;
            numeric_effect.fexpr = merge(element.get_fexpr(), context);

            canonicalize(numeric_effect);
            return context.destination.get_or_create(numeric_effect, context.builder.get_buffer());
        });
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Data<GroundNumericEffectOperator<T>>, C_SRC> element, MergeContext<C_DST>& context)
{
    return visit([&](auto&& arg) { return Data<GroundNumericEffectOperator<T>>(merge(arg, context).first); }, element.get_variant());
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<FDRVariable<FluentTag>>, C_SRC> element, MergeContext<C_DST>& context)
{
    return with_cache<FDRVariable<FluentTag>, FDRVariable<FluentTag>>(element,
                                                                      context.cache,
                                                                      [&]()
                                                                      {
                                                                          auto variable_ptr = context.builder.template get_builder<FDRVariable<FluentTag>>();
                                                                          auto& variable = *variable_ptr;
                                                                          variable.clear();

                                                                          variable.domain_size = element.get_domain_size();
                                                                          for (const auto atom : element.get_atoms())
                                                                              variable.atoms.push_back(merge(atom, context).first);

                                                                          canonicalize(variable);
                                                                          return context.destination.get_or_create(variable, context.builder.get_buffer());
                                                                      });
}

template<Context C_SRC, Context C_DST>
auto merge(View<Data<FDRFact<FluentTag>>, C_SRC> element, MergeContext<C_DST>& context)
{
    return Data<FDRFact<FluentTag>>(merge(element.get_variable(), context).first, element.get_value());
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<FDRConjunctiveCondition>, C_SRC> element, MergeContext<C_DST>& context)
{
    return with_cache<FDRConjunctiveCondition, FDRConjunctiveCondition>(element,
                                                                        context.cache,
                                                                        [&]()
                                                                        {
                                                                            auto conj_cond_ptr =
                                                                                context.builder.template get_builder<FDRConjunctiveCondition>();
                                                                            auto& conj_cond = *conj_cond_ptr;
                                                                            conj_cond.clear();

                                                                            for (const auto variable : element.get_variables())
                                                                                conj_cond.variables.push_back(merge(variable, context).first);
                                                                            for (const auto literal : element.template get_literals<StaticTag>())
                                                                                conj_cond.static_literals.push_back(merge(literal, context).first);
                                                                            for (const auto literal : element.template get_literals<FluentTag>())
                                                                                conj_cond.fluent_literals.push_back(merge(literal, context).first);
                                                                            for (const auto literal : element.template get_literals<DerivedTag>())
                                                                                conj_cond.derived_literals.push_back(merge(literal, context).first);
                                                                            for (const auto numeric_constraint : element.get_numeric_constraints())
                                                                                conj_cond.numeric_constraints.push_back(merge(numeric_constraint, context));

                                                                            canonicalize(conj_cond);
                                                                            return context.destination.get_or_create(conj_cond, context.builder.get_buffer());
                                                                        });
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Axiom>, C_SRC> element, MergeContext<C_DST>& context)
{
    return with_cache<Axiom, Axiom>(element,
                                    context.cache,
                                    [&]()
                                    {
                                        auto axiom_ptr = context.builder.template get_builder<Axiom>();
                                        auto& axiom = *axiom_ptr;
                                        axiom.clear();

                                        for (const auto variable : element.get_variables())
                                            axiom.variables.push_back(merge(variable, context).first);
                                        axiom.body = merge(element.get_body(), context).first;
                                        axiom.head = merge(element.get_head(), context).first;

                                        canonicalize(axiom);
                                        return context.destination.get_or_create(axiom, context.builder.get_buffer());
                                    });
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Metric>, C_SRC> element, MergeContext<C_DST>& context)
{
    return with_cache<Metric, Metric>(element,
                                      context.cache,
                                      [&]()
                                      {
                                          auto metric_ptr = context.builder.template get_builder<Metric>();
                                          auto& metric = *metric_ptr;
                                          metric.clear();

                                          metric.objective = element.get_objective();
                                          metric.fexpr = merge(element.get_fexpr(), context);

                                          canonicalize(metric);
                                          return context.destination.get_or_create(metric, context.builder.get_buffer());
                                      });
}

}

#endif