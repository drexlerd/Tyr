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

#ifndef TYR_GROUNDER_GENERATOR_HPP_
#define TYR_GROUNDER_GENERATOR_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_atom_index.hpp"
#include "tyr/grounder/applicability.hpp"
#include "tyr/grounder/declarations.hpp"
#include "tyr/grounder/kpkc.hpp"

namespace tyr::grounder
{

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
View<Index<formalism::GroundAtom<T>>, formalism::ScopedRepository<C>> ground(View<Index<formalism::Atom<T>>, C> element,
                                                                             const IndexList<formalism::Object>& binding,
                                                                             formalism::Builder& builder,
                                                                             formalism::ScopedRepository<C>& repository)
{
    auto& ground_atom = builder.get_ground_atom<T>();
    auto& objects = ground_atom.get_terms();
    objects.clear();

    ground_atom.index.group = element.get_index().get_group();

    for (const auto term : element.get_terms())
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, formalism::ParameterIndex>)
                {
                    objects.push_back(binding[uint_t(arg)]);
                }
                else if constexpr (std::is_same_v<Alternative, View<Index<formalism::Object>, C>>)
                {
                    objects.push_back(arg.get_index());
                }
                else
                {
                    static_assert(dependent_false<Alternative>::value, "Missing case");
                }
            },
            term.get());
    }
}

template<formalism::IsStaticOrFluentTag T, formalism::IsContext C>
View<Index<formalism::GroundLiteral<T>>, formalism::ScopedRepository<C>> ground(View<Index<formalism::Literal<T>>, C> element,
                                                                                const IndexList<formalism::Object>& binding,
                                                                                formalism::Builder& builder,
                                                                                formalism::ScopedRepository<C>& repository)
{
}

template<formalism::IsContext C>
View<Index<formalism::GroundConjunctiveCondition>, formalism::ScopedRepository<C>> ground(View<Index<formalism::ConjunctiveCondition>, C> element,
                                                                                          const IndexList<formalism::Object>& binding,
                                                                                          formalism::Builder& builder,
                                                                                          formalism::ScopedRepository<C>& repository)
{
}

template<formalism::IsContext C>
View<Index<formalism::GroundRule>, formalism::ScopedRepository<C>> ground(View<Index<formalism::Rule>, C> element,
                                                                          const IndexList<formalism::Object>& binding,
                                                                          formalism::Builder& builder,
                                                                          formalism::ScopedRepository<C>& repository)
{
}

template<formalism::IsContext C>
void ground_nullary_case(const ImmutableRuleWorkspace<C>& immutable_workspace, MutableRuleWorkspace<C>& mutable_workspace)
{
}

template<formalism::IsContext C>
void ground_unary_case(const ImmutableRuleWorkspace<C>& immutable_workspace, MutableRuleWorkspace<C>& mutable_workspace)
{
}

template<formalism::IsContext C>
void ground_general_case(const ImmutableRuleWorkspace<C>& immutable_workspace, MutableRuleWorkspace<C>& mutable_workspace)
{
    kpkc::for_each_k_clique(immutable_workspace.consistency_graph,
                            mutable_workspace.kpkc_workspace,
                            [](auto&& clique)
                            {
                                // TODO: ground the conjunctive condition and check if it applicable
                                std::cout << to_string(clique) << std::endl;

                                // TODO: ground the final rule
                            });
}

template<formalism::IsContext C>
void ground(const ImmutableRuleWorkspace<C>& immutable_workspace, MutableRuleWorkspace<C>& mutable_workspace)
{
    const auto rule = immutable_workspace.rule;
    const auto& fact_sets = immutable_workspace.fact_sets;

    if (!nullary_conditions_hold(rule.get_body(), fact_sets))
        return;

    const auto arity = rule.get_body().get_arity();

    if (arity == 0)
        ground_nullary_case(immutable_workspace, mutable_workspace);
    else if (arity == 1)
        ground_unary_case(immutable_workspace, mutable_workspace);
    else
        ground_general_case(immutable_workspace, mutable_workspace);
}

}

#endif
