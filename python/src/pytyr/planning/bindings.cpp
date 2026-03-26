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

#include "bindings.hpp"

#include "../common/bindings.hpp"

namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;

namespace tyr::planning
{

void bind_module_definitions(nb::module_& m)
{
    /**
     * SearchStatus
     */

    nb::enum_<SearchStatus>(m, "SearchStatus")
        .value("IN_PROGRESS", SearchStatus::IN_PROGRESS)
        .value("OUT_OF_TIME", SearchStatus::OUT_OF_TIME)
        .value("OUT_OF_MEMORY", SearchStatus::OUT_OF_MEMORY)
        .value("OUT_OF_STATES", SearchStatus::OUT_OF_STATES)
        .value("FAILED", SearchStatus::FAILED)
        .value("EXHAUSTED", SearchStatus::EXHAUSTED)
        .value("SOLVED", SearchStatus::SOLVED)
        .value("UNSOLVABLE", SearchStatus::UNSOLVABLE)
        .export_values();

    /**
     * Statistics
     */

    nb::class_<Statistics>(m, "Statistics");
}

void bind_ground_module_definitions(nb::module_& m)
{
    nb::class_<GroundTask>(m, "Task")  //
        .def("get_formalism_task", &GroundTask::get_formalism_task)
        .def("get_repository", &GroundTask::get_repository)
        .def("get_task", &GroundTask::get_task)
        .def("get_fdr_context", &GroundTask::get_fdr_context);

    bind_index<Index<State<GroundTask>>>(m, "StateIndex");
    bind_state<GroundTask>(m, "State");
    bind_node<GroundTask>(m, "Node");
    bind_labeled_node<GroundTask>(m, "LabeledNode");
    bind_plan<GroundTask>(m, "Plan");
    bind_axiom_evaluator<GroundTask>(m, "AxiomEvaluator");
    bind_state_repository<GroundTask>(m, "StateRepository");
    bind_successor_generator<GroundTask>(m, "SuccessorGenerator");
    bind_search_result<GroundTask>(m, "SearchResult");
    bind_goal_strategy<GroundTask>(m, "GoalStrategy");
    bind_task_goal_strategy<GroundTask>(m, "TaskGoalStrategy");
    bind_pruning_strategy<GroundTask>(m, "PruningStrategy");
    bind_heuristic<GroundTask>(m, "Heuristic");
    bind_blind_heuristic<GroundTask>(m, "BlindHeuristic");
    bind_goal_count_heuristic<GroundTask>(m, "GoalCountHeuristic");
}

void bind_lifted_module_definitions(nb::module_& m)
{
    nb::class_<LiftedTask>(m, "Task")  //
        .def(nb::new_([](formalism::planning::PlanningTask&& task) { return LiftedTask::create(std::move(task)); }),
             "formalism_task"_a,
             R"doc(
Create a planning task from a formalism task.

Parameters
----------
formalism_task : formalism.planning.Task
    The formalism-level task used to construct the planning task.

Notes
-----
The `formalism_task` is **moved** into the planning task. After calling this
constructor, the original formalism task should be considered consumed and
should not be used further.
        )doc")
        .def("get_formalism_task", &LiftedTask::get_formalism_task)
        .def("get_repository", &LiftedTask::get_repository)
        .def("get_task", &LiftedTask::get_task)
        .def("get_fdr_context", nb::overload_cast<>(&LiftedTask::get_fdr_context, nb::const_))
        .def("instantiate_ground_task", &LiftedTask::instantiate_ground_task);

    bind_index<Index<State<LiftedTask>>>(m, "StateIndex");
    bind_state<LiftedTask>(m, "State");
    bind_node<LiftedTask>(m, "Node");
    bind_labeled_node<LiftedTask>(m, "LabeledNode");
    bind_plan<LiftedTask>(m, "Plan");
    bind_axiom_evaluator<LiftedTask>(m, "AxiomEvaluator");
    bind_state_repository<LiftedTask>(m, "StateRepository");
    bind_successor_generator<LiftedTask>(m, "SuccessorGenerator");
    bind_search_result<LiftedTask>(m, "SearchResult");
    bind_goal_strategy<LiftedTask>(m, "GoalStrategy");
    bind_task_goal_strategy<LiftedTask>(m, "TaskGoalStrategy");
    bind_pruning_strategy<LiftedTask>(m, "PruningStrategy");
    bind_heuristic<LiftedTask>(m, "Heuristic");
    bind_blind_heuristic<LiftedTask>(m, "BlindHeuristic");
    bind_rpg_max_heuristic<LiftedTask>(m, "MaxRPGHeuristic");
    bind_rpg_add_heuristic<LiftedTask>(m, "AddRPGHeuristic");
    bind_rpg_ff_heuristic<LiftedTask>(m, "FFRPGHeuristic");
    bind_goal_count_heuristic<LiftedTask>(m, "GoalCountHeuristic");
}

namespace astar_eager
{
void bind_ground_module_definitions(nb::module_& m)
{
    bind_options<GroundTask>(m, "Options");
    bind_find_solution<GroundTask>(m, "find_solution");
    bind_event_handler<GroundTask>(m, "EventHandler");
    bind_default_event_handler<GroundTask>(m, "DefaultEventHandler");
}

void bind_lifted_module_definitions(nb::module_& m)
{
    bind_options<LiftedTask>(m, "Options");
    bind_find_solution<LiftedTask>(m, "find_solution");
    bind_event_handler<LiftedTask>(m, "EventHandler");
    bind_default_event_handler<LiftedTask>(m, "DefaultEventHandler");
}
}

namespace gbfs_lazy
{
void bind_ground_module_definitions(nb::module_& m)
{
    bind_options<GroundTask>(m, "Options");
    bind_find_solution<GroundTask>(m, "find_solution");
    bind_event_handler<GroundTask>(m, "EventHandler");
    bind_default_event_handler<GroundTask>(m, "DefaultEventHandler");
}

void bind_lifted_module_definitions(nb::module_& m)
{
    bind_options<LiftedTask>(m, "Options");
    bind_find_solution<LiftedTask>(m, "find_solution");
    bind_event_handler<LiftedTask>(m, "EventHandler");
    bind_default_event_handler<LiftedTask>(m, "DefaultEventHandler");
}
}

}