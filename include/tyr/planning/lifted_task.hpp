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

#ifndef TYR_PLANNING_LIFTED_TASK_HPP_
#define TYR_PLANNING_LIFTED_TASK_HPP_

#include "tyr/formalism/views.hpp"
#include "tyr/grounder/workspace.hpp"
#include "tyr/planning/domain.hpp"
#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/programs/action.hpp"
#include "tyr/planning/programs/axiom.hpp"
#include "tyr/planning/programs/ground.hpp"
#include "tyr/planning/task_mixin.hpp"

namespace tyr::planning
{

class LiftedTask : public TaskMixin<LiftedTask>
{
public:
    LiftedTask(DomainPtr domain,
               formalism::RepositoryPtr repository,
               formalism::OverlayRepositoryPtr<formalism::Repository> overlay_repository,
               View<Index<formalism::Task>, formalism::OverlayRepository<formalism::Repository>> task);

    std::vector<std::pair<View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>>, Node<LiftedTask>>>
    get_labeled_successor_nodes_impl(const Node<LiftedTask>& node);

    Node<LiftedTask> get_initial_node_impl();

    void get_labeled_successor_nodes_impl(
        const Node<LiftedTask>& node,
        std::vector<std::pair<View<Index<formalism::GroundAction>, formalism::OverlayRepository<formalism::Repository>>, Node<LiftedTask>>>& out_nodes);

    GroundTask get_ground_task();

    const ApplicableActionProgram& get_action_program() const;
    const AxiomEvaluatorProgram& get_axiom_program() const;
    const GroundTaskProgram& get_ground_program() const;

private:
    ApplicableActionProgram m_action_program;
    AxiomEvaluatorProgram m_axiom_program;
    GroundTaskProgram m_ground_program;

    grounder::ProgramExecutionContext m_action_context;
    grounder::ProgramExecutionContext m_axiom_context;

    using Domain = std::vector<Index<formalism::Object>>;
    using DomainList = std::vector<Domain>;
    using DomainListList = std::vector<DomainList>;
    using DomainListListList = std::vector<DomainListList>;

    std::vector<DomainListListList> parameter_domains_per_cond_effect_per_action;
};

}

#endif
