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

#ifndef TYR_PLANNING_PROGRAMS_ACTION_HPP_
#define TYR_PLANNING_PROGRAMS_ACTION_HPP_

#include "tyr/formalism/formalism.hpp"
#include "tyr/planning/declarations.hpp"

namespace tyr::planning
{

struct ApplicableActionProgram
{
    std::shared_ptr<formalism::Repository> repository;
    View<Index<formalism::Program>, formalism::Repository> program;

    // Mapping from program predicate to task rule
    using PredicateToActionMapping = UnorderedMap<View<Index<formalism::Predicate<formalism::FluentTag>>, formalism::Repository>,
                                                  View<Index<formalism::Rule>, formalism::OverlayRepository<formalism::Repository>>>;

    PredicateToActionMapping prediate_to_action;

    ApplicableActionProgram(View<Index<formalism::Program>, formalism::Repository> program, std::shared_ptr<formalism::Repository> repository);
};

extern ApplicableActionProgram create_applicable_action_program(const LiftedTask& task);
}

#endif