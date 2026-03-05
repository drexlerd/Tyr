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
     * Domain
     */

    nb::class_<Domain>(m, "Domain")  //
        .def("get_repository", &Domain::get_repository)
        .def("get_domain", &Domain::get_domain);

    /**
     * StateIndex
     */

    bind_index<StateIndex>(m, "StateIndex");

    /**
     * Statistics
     */

    nb::class_<Statistics>(m, "Statistics");

    /**
     * Parser
     */

    nb::class_<loki::ParserOptions>(m, "ParserOptions")
        .def(nb::init<>())
        .def_rw("strict", &loki::ParserOptions::strict, "Enable strict mode")
        .def_rw("verbose", &loki::ParserOptions::verbose, "Enable verbose output");

    nb::class_<Parser>(m, "Parser")
        .def(nb::init<const fs::path&, const loki::ParserOptions&>(), "domain_filepath"_a, "parser_options"_a)
        .def(nb::init<const std::string&, const fs::path&, const loki::ParserOptions&>(), "domain_description"_a, "domain_filepath"_a, "parser_options"_a)
        .def("parse_task", nb::overload_cast<const fs::path&, const loki::ParserOptions&>(&Parser::parse_task), "task_filepath"_a, "parser_options"_a)
        .def("parse_task",
             nb::overload_cast<const std::string&, const fs::path&, const loki::ParserOptions&>(&Parser::parse_task),
             "task_description"_a,
             "task_filepath"_a,
             "parser_options"_a)
        .def("get_domain", &Parser::get_domain);
}

void bind_ground_module_definitions(nb::module_& m)
{
    nb::class_<GroundTask>(m, "Task")  //
        .def("get_repository", &GroundTask::get_repository)
        .def("get_task", &GroundTask::get_task)
        .def("get_fdr_context", &GroundTask::get_fdr_context);

    bind_state<GroundTask>(m, "State");
    bind_node<GroundTask>(m, "Node");
    bind_labeled_node<GroundTask>(m, "LabeledNode");
    bind_plan<GroundTask>(m, "Plan");
    bind_axiom_evaluator<GroundTask>(m, "AxiomEvaluator");
    bind_state_repository<GroundTask>(m, "StateRepository");
    bind_successor_generator<GroundTask>(m, "SuccessorGenerator");
    bind_search_result<GroundTask>(m, "SearchResult");
    bind_heuristic<GroundTask>(m, "Heuristic");
    bind_blind_heuristic<GroundTask>(m, "BlindHeuristic");
}

void bind_lifted_module_definitions(nb::module_& m)
{
    nb::class_<LiftedTask>(m, "Task")  //
        .def("get_repository", &LiftedTask::get_repository)
        .def("get_task", &LiftedTask::get_task)
        .def("get_fdr_context", &LiftedTask::get_fdr_context)
        .def("instantiate_ground_task", &LiftedTask::instantiate_ground_task);

    bind_state<LiftedTask>(m, "State");
    bind_node<LiftedTask>(m, "Node");
    bind_labeled_node<LiftedTask>(m, "LabeledNode");
    bind_plan<LiftedTask>(m, "Plan");
    bind_axiom_evaluator<LiftedTask>(m, "AxiomEvaluator");
    bind_state_repository<LiftedTask>(m, "StateRepository");
    bind_successor_generator<LiftedTask>(m, "SuccessorGenerator");
    bind_search_result<LiftedTask>(m, "SearchResult");
    bind_heuristic<LiftedTask>(m, "Heuristic");
    bind_blind_heuristic<LiftedTask>(m, "BlindHeuristic");
    bind_max_heuristic<LiftedTask>(m, "MaxHeuristic");
    bind_add_heuristic<LiftedTask>(m, "AddHeuristic");
    bind_ff_heuristic<LiftedTask>(m, "FFHeuristic");
}

namespace astar_eager
{
void bind_ground_module_definitions(nb::module_& m)
{
    bind_options<GroundTask>(m, "Options");
    bind_find_solution<GroundTask>(m, "find_solution");
    bind_event_handler<GroundTask>(m, "EventHandler");
}

void bind_lifted_module_definitions(nb::module_& m)
{
    bind_options<LiftedTask>(m, "Options");
    bind_find_solution<LiftedTask>(m, "find_solution");
    bind_event_handler<LiftedTask>(m, "EventHandler");
}
}

}