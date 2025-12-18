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

#ifndef TYR_SRC_PLANNING_PROGRAMS_COMMON_HPP_
#define TYR_SRC_PLANNING_PROGRAMS_COMMON_HPP_

#include "tyr/common/declarations.hpp"
#include "tyr/common/types.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/merge_common.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/planning/declarations.hpp"

namespace tyr::planning
{
extern ::cista::offset::string create_applicability_name(View<Index<formalism::Action>, formalism::OverlayRepository<formalism::Repository>> action);

extern ::cista::offset::string create_triggered_name(View<Index<formalism::Action>, formalism::OverlayRepository<formalism::Repository>> action,
                                                     View<Index<formalism::ConditionalEffect>, formalism::OverlayRepository<formalism::Repository>> cond_eff);

extern ::cista::offset::string create_applicability_name(View<Index<formalism::Axiom>, formalism::OverlayRepository<formalism::Repository>> axiom);

extern View<Index<formalism::Predicate<formalism::FluentTag>>, formalism::Repository>
create_applicability_predicate(View<Index<formalism::Action>, formalism::OverlayRepository<formalism::Repository>> action,
                               formalism::MergeContext<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& context);

extern View<Index<formalism::Atom<formalism::FluentTag>>, formalism::Repository>
create_applicability_atom(View<Index<formalism::Action>, formalism::OverlayRepository<formalism::Repository>> action,
                          formalism::MergeContext<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& context);

extern View<Index<formalism::Literal<formalism::FluentTag>>, formalism::Repository>
create_applicability_literal(View<Index<formalism::Action>, formalism::OverlayRepository<formalism::Repository>> action,
                             formalism::MergeContext<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& context);

extern View<Index<formalism::Rule>, formalism::Repository>
create_applicability_rule(View<Index<formalism::Action>, formalism::OverlayRepository<formalism::Repository>> action,
                          formalism::MergeContext<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& context);

extern View<Index<formalism::Predicate<formalism::FluentTag>>, formalism::Repository>
create_triggered_predicate(View<Index<formalism::Action>, formalism::OverlayRepository<formalism::Repository>> action,
                           View<Index<formalism::ConditionalEffect>, formalism::OverlayRepository<formalism::Repository>> cond_eff,
                           formalism::MergeContext<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& context);

extern View<Index<formalism::Atom<formalism::FluentTag>>, formalism::Repository>
create_triggered_atom(View<Index<formalism::Action>, formalism::OverlayRepository<formalism::Repository>> action,
                      View<Index<formalism::ConditionalEffect>, formalism::OverlayRepository<formalism::Repository>> cond_eff,
                      formalism::MergeContext<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& context);

extern View<Index<formalism::Literal<formalism::FluentTag>>, formalism::Repository>
create_triggered_literal(View<Index<formalism::Action>, formalism::OverlayRepository<formalism::Repository>> action,
                         View<Index<formalism::ConditionalEffect>, formalism::OverlayRepository<formalism::Repository>> cond_eff,
                         formalism::MergeContext<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& context);

extern View<Index<formalism::Rule>, formalism::Repository>
create_triggered_rule(View<Index<formalism::Action>, formalism::OverlayRepository<formalism::Repository>> action,
                      View<Index<formalism::ConditionalEffect>, formalism::OverlayRepository<formalism::Repository>> cond_eff,
                      formalism::MergeContext<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& context);

extern View<Index<formalism::Rule>, formalism::Repository>
create_effect_rule(View<Index<formalism::Action>, formalism::OverlayRepository<formalism::Repository>> action,
                   View<Index<formalism::ConditionalEffect>, formalism::OverlayRepository<formalism::Repository>> cond_eff,
                   View<Index<formalism::Atom<formalism::FluentTag>>, formalism::Repository> effect,
                   formalism::MergeContext<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& context);

extern View<Index<formalism::Rule>, formalism::Repository>
create_cond_effect_rule(View<Index<formalism::Action>, formalism::OverlayRepository<formalism::Repository>> action,
                        View<Index<formalism::ConditionalEffect>, formalism::OverlayRepository<formalism::Repository>> cond_eff,
                        View<Index<formalism::Atom<formalism::FluentTag>>, formalism::Repository> effect,
                        formalism::MergeContext<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& context);

extern View<Index<formalism::Predicate<formalism::FluentTag>>, formalism::Repository>
create_applicability_predicate(View<Index<formalism::Axiom>, formalism::OverlayRepository<formalism::Repository>> axiom,
                               formalism::MergeContext<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& context);

extern View<Index<formalism::Atom<formalism::FluentTag>>, formalism::Repository>
create_applicability_atom(View<Index<formalism::Axiom>, formalism::OverlayRepository<formalism::Repository>> axiom,
                          formalism::MergeContext<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& context);

extern View<Index<formalism::Literal<formalism::FluentTag>>, formalism::Repository>
create_applicability_literal(View<Index<formalism::Axiom>, formalism::OverlayRepository<formalism::Repository>> axiom,
                             formalism::MergeContext<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& context);

extern View<Index<formalism::Rule>, formalism::Repository>
create_applicability_rule(View<Index<formalism::Axiom>, formalism::OverlayRepository<formalism::Repository>> axiom,
                          formalism::MergeContext<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& context);

extern View<Index<formalism::Rule>, formalism::Repository>
create_effect_rule(View<Index<formalism::Axiom>, formalism::OverlayRepository<formalism::Repository>> axiom,
                   View<Index<formalism::Atom<formalism::FluentTag>>, formalism::Repository> effect,
                   formalism::MergeContext<formalism::OverlayRepository<formalism::Repository>, formalism::Repository>& context);
}

#endif