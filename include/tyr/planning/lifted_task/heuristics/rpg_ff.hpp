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

#ifndef TYR_PLANNING_LIFTED_TASK_HEURISTICS_FF_HPP_
#define TYR_PLANNING_LIFTED_TASK_HEURISTICS_FF_HPP_

#include "tyr/datalog/policies/annotation.hpp"
#include "tyr/datalog/policies/termination.hpp"
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/datalog/grounder.hpp"
#include "tyr/formalism/planning/formatter.hpp"
#include "tyr/formalism/planning/grounder.hpp"
#include "tyr/formalism/planning/merge_datalog.hpp"
#include "tyr/planning/applicability.hpp"
#include "tyr/planning/heuristics/rpg_ff.hpp"
#include "tyr/planning/lifted_task/heuristics/rpg.hpp"

#include <boost/dynamic_bitset.hpp>
#include <cassert>

namespace tyr::planning
{

template<>
class FFRPGHeuristic<LiftedTag> :
    public RPGBase<FFRPGHeuristic<LiftedTag>,
                   datalog::OrAnnotationPolicy,
                   datalog::AndAnnotationPolicy<datalog::SumAggregation>,
                   datalog::TerminationPolicy<datalog::SumAggregation>>
{
public:
    FFRPGHeuristic(std::shared_ptr<Task<LiftedTag>> task, ExecutionContextPtr execution_context);

    static std::shared_ptr<FFRPGHeuristic<LiftedTag>> create(std::shared_ptr<Task<LiftedTag>> task, ExecutionContextPtr execution_context);

    float_t extract_cost_and_set_preferred_actions_impl(const StateView<LiftedTag>& state);

    const UnorderedSet<Index<formalism::planning::GroundAction>>& get_preferred_actions() override;

    const UnorderedSet<formalism::planning::GroundActionView>& get_preferred_action_views() override;

    bool mark_atom(formalism::datalog::PredicateBindingView<formalism::FluentTag> atom);

private:
    void extract_relaxed_plan_and_preferred_actions(formalism::datalog::PredicateBindingView<formalism::FluentTag> atom,
                                                    const StateContext<LiftedTag>& state_context,
                                                    formalism::planning::GrounderContext& grounder_context);

private:
    std::vector<boost::dynamic_bitset<>> m_markings;

    /// For grounding actions
    IndexList<formalism::Object> m_binding;
    UnorderedMap<Index<formalism::planning::FDRVariable<formalism::FluentTag>>, formalism::planning::FDRValue> m_assign;
    itertools::cartesian_set::Workspace<Index<formalism::Object>> m_iter_workspace;
    formalism::planning::EffectFamilyList m_effect_families;

    UnorderedSet<Index<formalism::planning::GroundAction>> m_relaxed_plan;
    UnorderedSet<Index<formalism::planning::GroundAction>> m_preferred_actions;
    UnorderedSet<formalism::planning::GroundActionView> m_preferred_action_views;
    bool m_preferred_action_views_dirty;
};

}

#endif
