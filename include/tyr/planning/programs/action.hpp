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

#include "tyr/analysis/analysis.hpp"
#include "tyr/formalism/formalism.hpp"
#include "tyr/planning/declarations.hpp"

namespace tyr::planning
{

class ApplicableActionProgram
{
public:
    // Mapping from program predicate to task action; there may be multiple actions
    using PredicateToActionsMapping = UnorderedMap<View<Index<formalism::Predicate<formalism::FluentTag>>, formalism::Repository>,
                                                   std::vector<View<Index<formalism::Action>, formalism::OverlayRepository<formalism::Repository>>>>;

    explicit ApplicableActionProgram(const LiftedTask& task);

private:
    PredicateToActionsMapping m_predicate_to_actions;

    formalism::RepositoryPtr m_repository;
    View<Index<formalism::Program>, formalism::Repository> m_program;
    analysis::RuleStrata m_strata;
    analysis::Listeners m_listeners;
};

}

#endif