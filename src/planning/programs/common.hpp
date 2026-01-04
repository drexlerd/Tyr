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
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/merge.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/merge_datalog.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"
#include "tyr/planning/declarations.hpp"

namespace tyr::planning
{
extern ::cista::offset::string
create_applicability_name(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action);

extern ::cista::offset::string
create_triggered_name(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
                      View<Index<formalism::planning::ConditionalEffect>, formalism::OverlayRepository<formalism::planning::Repository>> cond_eff);

extern ::cista::offset::string
create_applicability_name(View<Index<formalism::planning::Axiom>, formalism::OverlayRepository<formalism::planning::Repository>> axiom);

inline auto create_applicability_predicate(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
                                           formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto predicate_ptr = context.builder.get_builder<formalism::Predicate<formalism::FluentTag>>();
    auto& predicate = *predicate_ptr;
    predicate.clear();

    predicate.name = create_applicability_name(action);
    predicate.arity = action.get_arity();

    canonicalize(predicate);
    return context.destination.get_or_create(predicate, context.builder.get_buffer());
}

inline auto create_applicability_atom(View<Index<formalism::planning::Action>, formalism::OverlayRepository<formalism::planning::Repository>> action,
                                      formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto atom_ptr = context.builder.get_builder<formalism::datalog::Atom<formalism::FluentTag>>();
    auto& atom = *atom_ptr;
    atom.clear();

    const auto applicability_predicate = make_view(create_applicability_predicate(action, context).first, context.destination);

    atom.predicate = applicability_predicate.get_index();
    for (uint_t i = 0; i < applicability_predicate.get_arity(); ++i)
        atom.terms.push_back(Data<formalism::Term>(formalism::ParameterIndex(i)));

    canonicalize(atom);
    return context.destination.get_or_create(atom, context.builder.get_buffer());
}

inline auto create_applicability_predicate(View<Index<formalism::planning::Axiom>, formalism::OverlayRepository<formalism::planning::Repository>> axiom,
                                           formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto predicate_ptr = context.builder.get_builder<formalism::Predicate<formalism::FluentTag>>();
    auto& predicate = *predicate_ptr;
    predicate.clear();

    predicate.name = create_applicability_name(axiom);
    predicate.arity = axiom.get_arity();

    canonicalize(predicate);
    return context.destination.get_or_create(predicate, context.builder.get_buffer());
}

inline auto create_applicability_atom(View<Index<formalism::planning::Axiom>, formalism::OverlayRepository<formalism::planning::Repository>> axiom,
                                      formalism::planning::MergeDatalogContext<formalism::datalog::Repository>& context)
{
    auto atom_ptr = context.builder.get_builder<formalism::datalog::Atom<formalism::FluentTag>>();
    auto& atom = *atom_ptr;
    atom.clear();

    const auto applicability_predicate = make_view(create_applicability_predicate(axiom, context).first, context.destination);

    atom.predicate = applicability_predicate.get_index();
    for (uint_t i = 0; i < applicability_predicate.get_arity(); ++i)
        atom.terms.push_back(Data<formalism::Term>(formalism::ParameterIndex(i)));

    canonicalize(atom);
    return context.destination.get_or_create(atom, context.builder.get_buffer());
}

}

#endif