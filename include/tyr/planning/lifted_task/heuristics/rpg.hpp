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

#ifndef TYR_PLANNING_LIFTED_TASK_HEURISTICS_RPG_HPP_
#define TYR_PLANNING_LIFTED_TASK_HEURISTICS_RPG_HPP_

#include "tyr/datalog/bottom_up.hpp"
#include "tyr/datalog/contexts/program.hpp"
#include "tyr/datalog/workspaces/program.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/heuristic.hpp"
#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/lifted_task/state.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"
#include "tyr/planning/task_utils.hpp"

namespace tyr::planning
{

template<typename Derived>
class RPGBase : public Heuristic<LiftedTask>
{
private:
    /// @brief Helper to cast to Derived.
    constexpr const auto& self() const { return static_cast<const Derived&>(*this); }
    constexpr auto& self() { return static_cast<Derived&>(*this); }

public:
    explicit RPGBase(std::shared_ptr<LiftedTask> task) :
        m_task(std::move(task)),
        m_workspace(m_task->get_rpg_program().get_program_context(), m_task->get_rpg_program().get_const_program_workspace())
    {
    }

    void set_goal(View<Index<formalism::planning::GroundConjunctiveCondition>, formalism::OverlayRepository<formalism::planning::Repository>> goal) override
    {
        m_workspace.facts.goal_fact_sets.reset();

        auto merge_context = formalism::planning::MergeDatalogContext { m_workspace.datalog_builder, m_workspace.repository, m_workspace.p2d.merge_cache };

        for (const auto fact : goal.get_facts<formalism::FluentTag>())
        {
            if (fact.get_value() != formalism::planning::FDRValue::none())
                m_workspace.facts.goal_fact_sets.insert(formalism::planning::merge_p2d(fact.get_atom(), merge_context).first);
        }
    }

    float_t evaluate(const State<LiftedTask>& state) override
    {
        m_workspace.facts.reset();
        m_workspace.p2d.clear();

        insert_fluent_atoms_to_fact_set(state.get_unpacked_state().get_atoms<formalism::FluentTag>(), *m_task->get_repository(), m_workspace);

        auto ctx = datalog::ProgramExecutionContext(m_workspace,
                                                    m_task->get_rpg_program().get_const_program_workspace(),
                                                    self().get_annotation_policies_impl(),
                                                    self().get_termination_policy_impl());

        datalog::solve_bottom_up(ctx);

        const auto value = (self().get_termination_policy_impl().check()) ? self().extract_cost_and_set_preferred_actions_impl(state) :
                                                                            std::numeric_limits<float_t>::infinity();

        std::cout << "Heuristic value: " << value << std::endl;

        return value;
    }

    const datalog::ProgramWorkspace& get_workspace() const noexcept { return m_workspace; }

protected:
    std::shared_ptr<LiftedTask> m_task;

    datalog::ProgramWorkspace m_workspace;
};

}

#endif
