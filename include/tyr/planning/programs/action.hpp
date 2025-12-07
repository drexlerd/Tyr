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

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/planning/declarations.hpp"

namespace tyr::planning
{

class ApplicableActionProgram
{
public:
    // Mapping from program predicate to task action; there may be multiple actions
    using PredicateToActionsMapping = UnorderedMap<View<Index<formalism::Predicate<formalism::FluentTag>>, formalism::Repository>,
                                                   std::vector<View<Index<formalism::Action>, formalism::OverlayRepository<formalism::Repository>>>>;

    // Mapping from program object to task object
    using ObjectToObjectMapping = UnorderedMap<View<Index<formalism::Object>, formalism::Repository>,
                                               View<Index<formalism::Object>, formalism::OverlayRepository<formalism::Repository>>>;

    explicit ApplicableActionProgram(const LiftedTask& task);

    const PredicateToActionsMapping& get_predicate_to_actions_mapping() const noexcept;
    const ObjectToObjectMapping& get_object_to_object_mapping() const noexcept;

    View<Index<formalism::Program>, formalism::Repository> get_program() const noexcept;
    const formalism::RepositoryPtr& get_repository() const noexcept;

private:
    PredicateToActionsMapping m_predicate_to_actions;
    ObjectToObjectMapping m_object_to_object;

    formalism::RepositoryPtr m_repository;
    View<Index<formalism::Program>, formalism::Repository> m_program;
};

}

#endif