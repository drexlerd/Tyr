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
auto merge(View<Index<NumericEffect<O, T>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache);

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Data<NumericEffectOperator<T>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache);

template<NumericEffectOpKind O, FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<GroundNumericEffect<O, T>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache);

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Data<GroundNumericEffectOperator<T>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache);

template<Context C_SRC, Context C_DST>
auto merge(View<Index<FDRVariable<FluentTag>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache);

template<Context C_SRC, Context C_DST>
auto merge(View<Data<FDRFact<FluentTag>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache);

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Axiom>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache);

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Metric>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache);

template<Context C_SRC, Context C_DST, typename FDR>
    requires FDRContext<FDR, C_DST>
auto merge(View<Index<GroundFDRConjunctiveCondition>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache, FDR& fdr);

template<Context C_SRC, Context C_DST, typename FDR>
    requires FDRContext<FDR, C_DST>
auto merge(View<Index<GroundConjunctiveEffect>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache, FDR& fdr);

template<Context C_SRC, Context C_DST, typename FDR>
    requires FDRContext<FDR, C_DST>
auto merge(View<Index<GroundConditionalEffect>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache, FDR& fdr);

template<Context C_SRC, Context C_DST, typename FDR>
    requires FDRContext<FDR, C_DST>
auto merge(View<Index<GroundAction>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache, FDR& fdr);

template<Context C_SRC, Context C_DST, typename FDR>
    requires FDRContext<FDR, C_DST>
auto merge(View<Index<GroundAxiom>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache, FDR& fdr);

/**
 * Implementations
 */

template<NumericEffectOpKind O, FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<NumericEffect<O, T>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache)
{
    return with_cache<NumericEffect<O, T>, NumericEffect<O, T>>(element,
                                                                cache,
                                                                [&]()
                                                                {
                                                                    auto numeric_effect_ptr = builder.template get_builder<NumericEffect<O, T>>();
                                                                    auto& numeric_effect = *numeric_effect_ptr;
                                                                    numeric_effect.clear();

                                                                    numeric_effect.fterm = merge(element.get_fterm(), builder, destination, cache).get_index();
                                                                    numeric_effect.fexpr = merge(element.get_fexpr(), builder, destination, cache).get_data();

                                                                    canonicalize(numeric_effect);
                                                                    return destination.get_or_create(numeric_effect, builder.get_buffer()).first;
                                                                });
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Data<NumericEffectOperator<T>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache)
{
    return visit(
        [&](auto&& arg) {
            return View<Data<NumericEffectOperator<T>>, C_DST>(Data<NumericEffectOperator<T>>(merge(arg, builder, destination, cache).get_index()),
                                                               destination);
        },
        element.get_variant());
}

template<NumericEffectOpKind O, FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Index<GroundNumericEffect<O, T>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache)
{
    return with_cache<GroundNumericEffect<O, T>, GroundNumericEffect<O, T>>(
        element,
        cache,
        [&]()
        {
            auto numeric_effect_ptr = builder.template get_builder<GroundNumericEffect<O, T>>();
            auto& numeric_effect = *numeric_effect_ptr;
            numeric_effect.clear();

            numeric_effect.fterm = merge(element.get_fterm(), builder, destination, cache).get_index();
            numeric_effect.fexpr = merge(element.get_fexpr(), builder, destination, cache).get_data();

            canonicalize(numeric_effect);
            return destination.get_or_create(numeric_effect, builder.get_buffer()).first;
        });
}

template<FactKind T, Context C_SRC, Context C_DST>
auto merge(View<Data<GroundNumericEffectOperator<T>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache)
{
    return visit(
        [&](auto&& arg)
        {
            return View<Data<GroundNumericEffectOperator<T>>, C_DST>(Data<GroundNumericEffectOperator<T>>(merge(arg, builder, destination, cache).get_index()),
                                                                     destination);
        },
        element.get_variant());
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<FDRVariable<FluentTag>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache)
{
    return with_cache<FDRVariable<FluentTag>, FDRVariable<FluentTag>>(element,
                                                                      cache,
                                                                      [&]()
                                                                      {
                                                                          auto variable_ptr = builder.get_builder<FDRVariable<FluentTag>>();
                                                                          auto& variable = *variable_ptr;
                                                                          variable.clear();

                                                                          variable.domain_size = element.get_domain_size();
                                                                          for (const auto atom : element.get_atoms())
                                                                              variable.atoms.push_back(merge(atom, builder, destination).get_index());

                                                                          canonicalize(variable);
                                                                          return destination.get_or_create(variable, builder.get_buffer()).first;
                                                                      });
}

template<Context C_SRC, Context C_DST>
auto merge(View<Data<FDRFact<FluentTag>>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache)
{
    return make_view(Data<FDRFact<FluentTag>>(merge(element.get_variable(), builder, destination, cache).get_index(), element.get_value()), destination);
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Axiom>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache)
{
    return with_cache<Axiom, Axiom>(element,
                                    cache,
                                    [&]()
                                    {
                                        auto axiom_ptr = builder.template get_builder<Axiom>();
                                        auto& axiom = *axiom_ptr;
                                        axiom.clear();

                                        axiom.body = merge(element.get_body(), builder, destination, cache).get_index();
                                        axiom.head = merge(element.get_head(), builder, destination, cache).get_index();

                                        canonicalize(axiom);
                                        return destination.get_or_create(axiom, builder.get_buffer()).first;
                                    });
}

template<Context C_SRC, Context C_DST>
auto merge(View<Index<Metric>, C_SRC> element, Builder& builder, C_DST& destination, MergeCache<C_SRC, C_DST>& cache)
{
    return with_cache<Metric, Metric>(element,
                                      cache,
                                      [&]()
                                      {
                                          auto metric_ptr = builder.template get_builder<Metric>();
                                          auto& metric = *metric_ptr;
                                          metric.clear();

                                          metric.objective = element.get_objective();
                                          metric.fexpr = merge(element.get_fexpr(), builder, destination, cache).get_data();

                                          canonicalize(metric);
                                          return destination.get_or_create(metric, builder.get_buffer()).first;
                                      });
}

}

#endif