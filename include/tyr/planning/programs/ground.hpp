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

#ifndef TYR_PLANNING_PROGRAMS_GROUND_TASK_HPP_
#define TYR_PLANNING_PROGRAMS_GROUND_TASK_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/formalism/overlay_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/views.hpp"
#include "tyr/planning/declarations.hpp"

namespace tyr::planning
{

class GroundTaskProgram
{
public:
    // Mapping from program rule to task action; there may be multiple actions
    using RuleToActionsMapping = UnorderedMap<View<Index<formalism::Rule>, formalism::Repository>,
                                              std::vector<View<Index<formalism::Action>, formalism::OverlayRepository<formalism::Repository>>>>;

    using RuleToAxiomsMapping = UnorderedMap<View<Index<formalism::Rule>, formalism::Repository>,
                                             std::vector<View<Index<formalism::Axiom>, formalism::OverlayRepository<formalism::Repository>>>>;

    explicit GroundTaskProgram(const LiftedTask& task);

    const RuleToActionsMapping& get_rule_to_actions_mapping() const noexcept;
    const RuleToAxiomsMapping& get_rule_to_axioms_mapping() const noexcept;

    View<Index<formalism::Program>, formalism::Repository> get_program() const noexcept;
    const formalism::RepositoryPtr& get_repository() const noexcept;

private:
    RuleToActionsMapping m_rule_to_actions;
    RuleToAxiomsMapping m_rule_to_axioms;

    formalism::RepositoryPtr m_repository;
    View<Index<formalism::Program>, formalism::Repository> m_program;
};

}

#endif