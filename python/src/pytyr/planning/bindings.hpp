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

#ifndef TYR_PYTHON_PLANNING_BINDINGS_HPP
#define TYR_PYTHON_PLANNING_BINDINGS_HPP

#include "../init_declarations.hpp"

namespace tyr::planning
{
template<typename Task>
void bind_state(nb::module_& m, const std::string& name)
{
    nb::class_<State<Task>>(m, name.c_str())  //
        .def("__str__", [](const State<Task>& self) { return to_string(self); })
        .def("get_index", &State<Task>::get_index, nb::rv_policy::copy)
        .def("get_repository", &State<Task>::get_repository, nb::rv_policy::copy)
        .def("get_state_repository", &State<Task>::get_state_repository, nb::rv_policy::copy)
        // AccessibleStateConcept
        .def("test",
             nb::overload_cast<View<Index<formalism::planning::GroundAtom<formalism::StaticTag>>, formalism::planning::Repository>>(&State<Task>::test,
                                                                                                                                    nb::const_),
             nb::rv_policy::copy,
             "static_atom"_a)
        .def("test",
             nb::overload_cast<View<Index<formalism::planning::GroundAtom<formalism::DerivedTag>>, formalism::planning::Repository>>(&State<Task>::test,
                                                                                                                                     nb::const_),
             nb::rv_policy::copy,
             "derived_atom"_a)
        .def("get",
             nb::overload_cast<View<Index<formalism::planning::GroundFunctionTerm<formalism::StaticTag>>, formalism::planning::Repository>>(&State<Task>::get,
                                                                                                                                            nb::const_),
             nb::rv_policy::copy,
             "static_fterm"_a)
        .def("get",
             nb::overload_cast<View<Index<formalism::planning::FDRVariable<formalism::FluentTag>>, formalism::planning::Repository>>(&State<Task>::get,
                                                                                                                                     nb::const_),
             nb::rv_policy::copy,
             "fluent_fact"_a)
        .def("get",
             nb::overload_cast<View<Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>>, formalism::planning::Repository>>(&State<Task>::get,
                                                                                                                                            nb::const_),
             nb::rv_policy::copy,
             "fluent_fterm"_a)
        // IterableStateConcept
        .def(
            "static_atoms",
            [](const State<Task>& s)
            {
                auto range = s.get_static_atoms_view();
                return nb::make_iterator(nb::type<State<Task>>(), "static atom iterator", range);
            },
            nb::keep_alive<0, 1>())
        .def(
            "fluent_facts",
            [](const State<Task>& s)
            {
                auto range = s.get_fluent_facts_view();
                return nb::make_iterator(nb::type<State<Task>>(), "fluent facts iterator", range);
            },
            nb::keep_alive<0, 1>())
        .def(
            "derived_atoms",
            [](const State<Task>& s)
            {
                auto range = s.get_derived_atoms_view();
                return nb::make_iterator(nb::type<State<Task>>(), "derived atom iterator", range);
            },
            nb::keep_alive<0, 1>())
        .def(
            "static_fterm_values",
            [](const State<Task>& s)
            {
                auto range = s.get_static_fterm_values_view();
                return nb::make_iterator(nb::type<State<Task>>(), "static function term value iterator", range);
            },
            nb::keep_alive<0, 1>())
        .def(
            "fluent_fterm_values",
            [](const State<Task>& s)
            {
                auto range = s.get_fluent_fterm_values_view();
                return nb::make_iterator(nb::type<State<Task>>(), "fluent function term value iterator", range);
            },
            nb::keep_alive<0, 1>());
}

template<typename Task>
void bind_node(nb::module_& m, const std::string& name)
{
    using T = Node<Task>;

    nb::class_<T>(m, name.c_str())
        .def("__str__", [](const T& self) { return to_string(self); })
        .def("get_state", &T::get_state, nb::rv_policy::reference_internal)
        .def("get_metric", &T::get_metric, nb::rv_policy::copy);
}

template<typename Task>
void bind_labeled_node(nb::module_& m, const std::string& name)
{
    using T = LabeledNode<Task>;

    nb::class_<T>(m, name.c_str())  //
        .def_ro("label", &T::label, nb::rv_policy::copy)
        .def_ro("node", &T::node, nb::rv_policy::copy);
}

template<typename Task>
void bind_plan(nb::module_& m, const std::string& name)
{
    using T = Plan<Task>;

    nb::class_<T>(m, name.c_str())  //
        .def(nb::init<Node<Task>, LabeledNodeList<Task>>(), "start_node", "labeled_succ_nodes")
        .def("get_start_node", &T::get_start_node, nb::rv_policy::copy)
        .def("get_labeled_succ_nodes", &T::get_labeled_succ_nodes, nb::rv_policy::copy)
        .def("get_cost", &T::get_cost)
        .def("get_length", &T::get_length);
}

template<typename Task>
void bind_axiom_evaluator(nb::module_& m, const std::string& name)
{
    using T = AxiomEvaluator<Task>;

    nb::class_<T>(m, name.c_str())  //
        .def(nb::new_([](std::shared_ptr<Task> task) { return T::create(std::move(task)); }), "task"_a);
}

template<typename Task>
void bind_state_repository(nb::module_& m, const std::string& name)
{
    using T = StateRepository<Task>;

    nb::class_<T>(m, name.c_str())  //
        .def(nb::new_([](std::shared_ptr<Task> task) { return T::create(std::move(task)); }), "task"_a)
        .def("get_initial_state", &T::get_initial_state, nb::rv_policy::move)
        .def("get_registered_state", &T::get_registered_state, nb::rv_policy::move, "state_index"_a)
        .def("get_axiom_evaluator", &T::get_axiom_evaluator, nb::rv_policy::copy);
}

template<typename Task>
void bind_successor_generator(nb::module_& m, const std::string& name)
{
    using T = SuccessorGenerator<Task>;

    nb::class_<T>(m, name.c_str())
        .def(nb::new_([](std::shared_ptr<Task> task) { return T::create(std::move(task)); }), "task"_a)
        .def("get_initial_node", &T::get_initial_node, nb::rv_policy::move)
        .def("get_labeled_successor_nodes", nb::overload_cast<const Node<Task>&>(&T::get_labeled_successor_nodes), nb::rv_policy::move, "node"_a)
        .def("get_state", &T::get_state, nb::rv_policy::move)
        .def("get_state_repository", &T::get_state_repository, nb::rv_policy::copy);
}

template<typename Task>
void bind_search_result(nb::module_& m, const std::string& name)
{
    using T = SearchResult<Task>;

    nb::class_<T>(m, name.c_str()).def(nb::init<>()).def_rw("status", &T::status).def_rw("plan", &T::plan).def_rw("goal_node", &T::goal_node);
}

template<typename Task>
void bind_heuristic(nb::module_& m, const std::string& name)
{
    using T = Heuristic<Task>;

    nb::class_<T>(m, name.c_str())  //
        .def("set_goal", &T::set_goal, "goal"_a)
        .def("evaluate", &T::evaluate, "state"_a);
}

template<typename Task>
void bind_blind_heuristic(nb::module_& m, const std::string& name)
{
    using T = BlindHeuristic<Task>;

    nb::class_<T, Heuristic<Task>>(m, name.c_str())  //
        .def(nb::new_([]() { return T::create(); }));
}

template<typename Task>
void bind_max_heuristic(nb::module_& m, const std::string& name)
{
    using T = MaxHeuristic<Task>;

    nb::class_<T, Heuristic<Task>>(m, name.c_str())  //
        .def(nb::new_([](std::shared_ptr<Task> task) { return T::create(std::move(task)); }), "task"_a);
}

template<typename Task>
void bind_add_heuristic(nb::module_& m, const std::string& name)
{
    using T = AddHeuristic<Task>;

    nb::class_<T, Heuristic<Task>>(m, name.c_str())  //
        .def(nb::new_([](std::shared_ptr<Task> task) { return T::create(std::move(task)); }), "task"_a);
}

template<typename Task>
void bind_ff_heuristic(nb::module_& m, const std::string& name)
{
    using T = FFHeuristic<Task>;

    nb::class_<T, Heuristic<Task>>(m, name.c_str())  //
        .def(nb::new_([](std::shared_ptr<Task> task) { return T::create(std::move(task)); }), "task"_a);
}

namespace astar_eager
{
template<typename Task>
void bind_options(nb::module_& m, const std::string& name)
{
    using T = Options<Task>;

    nb::class_<T>(m, name.c_str())
        .def(nb::init<>())
        .def_rw("start_node", &T::start_node)
        .def_rw("event_handler", &T::event_handler)
        .def_rw("pruning_strategy", &T::pruning_strategy)
        .def_rw("goal_strategy", &T::goal_strategy)
        .def_rw("max_num_states", &T::max_num_states)
        .def_rw("max_time", &T::max_time)
        .def_rw("stop_if_goal", &T::stop_if_goal)
        .def_rw("random_seed", &T::random_seed)
        .def_rw("shuffle_labeled_succ_nodes", &T::shuffle_labeled_succ_nodes);
}

template<typename Task>
void bind_find_solution(nb::module_& m, const std::string& py_name)
{
    m.def(
        py_name.c_str(),
        [](Task& task, SuccessorGenerator<Task>& successor_generator, Heuristic<Task>& heuristic, const Options<Task>& options)
        { return find_solution(task, successor_generator, heuristic, options); },
        nb::arg("task"),
        nb::arg("successor_generator"),
        nb::arg("heuristic"),
        nb::arg("options"));
}

template<typename Task>
void bind_event_handler(nb::module_& m, const std::string& name)
{
    nb::class_<EventHandler<Task>>(m, name.c_str());
}
}

}

#endif