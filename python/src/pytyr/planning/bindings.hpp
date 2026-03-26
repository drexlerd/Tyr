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

#include <nanobind/trampoline.h>

namespace tyr::planning
{

template<typename Task>
void bind_state(nb::module_& m, const std::string& name)
{
    using T = StateView<Task>;

    nb::class_<T>(m, name.c_str())  //
        .def("__str__", [](const T& self) { return to_string(self); })
        .def("get_index", &T::get_index, nb::rv_policy::copy)
        .def("get_repository", &T::get_repository, nb::rv_policy::copy)
        .def("get_state_repository", &T::get_state_repository, nb::rv_policy::copy)
        // AccessibleStateConcept
        .def("test", nb::overload_cast<formalism::planning::GroundAtomView<formalism::StaticTag>>(&T::test, nb::const_), nb::rv_policy::copy, "static_atom"_a)
        .def("test", nb::overload_cast<formalism::planning::GroundAtomView<formalism::DerivedTag>>(&T::test, nb::const_), nb::rv_policy::copy, "derived_atom"_a)
        .def("get",
             nb::overload_cast<formalism::planning::GroundFunctionTermView<formalism::StaticTag>>(&T::get, nb::const_),
             nb::rv_policy::copy,
             "static_fterm"_a)
        .def("get", nb::overload_cast<formalism::planning::FDRVariableView<formalism::FluentTag>>(&T::get, nb::const_), nb::rv_policy::copy, "fluent_fact"_a)
        .def("get",
             nb::overload_cast<formalism::planning::GroundFunctionTermView<formalism::FluentTag>>(&T::get, nb::const_),
             nb::rv_policy::copy,
             "fluent_fterm"_a)
        // IterableStateConcept
        .def(
            "static_atoms",
            [](const T& s)
            {
                auto range = s.get_static_atoms_view();
                return nb::make_iterator(nb::type<T>(), "static atom iterator", range);
            },
            nb::keep_alive<0, 1>())
        .def(
            "fluent_facts",
            [](const T& s)
            {
                auto range = s.get_fluent_facts_view();
                return nb::make_iterator(nb::type<T>(), "fluent facts iterator", range);
            },
            nb::keep_alive<0, 1>())
        .def(
            "derived_atoms",
            [](const T& s)
            {
                auto range = s.get_derived_atoms_view();
                return nb::make_iterator(nb::type<T>(), "derived atom iterator", range);
            },
            nb::keep_alive<0, 1>())
        .def(
            "static_fterm_values",
            [](const T& s)
            {
                auto range = s.get_static_fterm_values_view();
                return nb::make_iterator(nb::type<T>(), "static function term value iterator", range);
            },
            nb::keep_alive<0, 1>())
        .def(
            "fluent_fterm_values",
            [](const T& s)
            {
                auto range = s.get_fluent_fterm_values_view();
                return nb::make_iterator(nb::type<T>(), "fluent function term value iterator", range);
            },
            nb::keep_alive<0, 1>());
}

template<typename Task>
void bind_node(nb::module_& m, const std::string& name)
{
    using T = Node<Task>;

    nb::class_<T>(m, name.c_str())
        .def(nb::init<StateView<Task>, float_t>(), "state"_a, "metric_value"_a)
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
        .def(nb::init<Node<Task>>(), "start_node"_a)
        .def(nb::init<Node<Task>, LabeledNodeList<Task>>(), "start_node"_a, "labeled_succ_nodes"_a)
        .def("__str__", [](const T& self) { return to_string(self); })
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
        .def(nb::new_([](std::shared_ptr<Task> task, std::shared_ptr<ExecutionContext> execution_context)
                      { return T::create(std::move(task), std::move(execution_context)); }),
             "task"_a,
             "execution_context"_a);
}

template<typename Task>
void bind_state_repository(nb::module_& m, const std::string& name)
{
    using T = StateRepository<Task>;

    nb::class_<T>(m, name.c_str())  //
        .def(nb::new_([](std::shared_ptr<Task> task, std::shared_ptr<ExecutionContext> execution_context)
                      { return T::create(std::move(task), std::move(execution_context)); }),
             "task"_a,
             "execution_context"_a)
        .def("get_initial_state", &T::get_initial_state, nb::rv_policy::move)
        .def("get_registered_state", &T::get_registered_state, nb::rv_policy::move, "state_index"_a)
        .def("create_state",
             nb::overload_cast<const std::vector<formalism::planning::FDRFactView<formalism::FluentTag>>&,
                               const std::vector<formalism::planning::GroundFunctionTermViewValuePair<formalism::FluentTag>>&>(&T::create_state),
             nb::rv_policy::move,
             "fluent_fact"_a,
             "fterm_values"_a)
        .def("get_axiom_evaluator", &T::get_axiom_evaluator, nb::rv_policy::copy);
}

template<typename Task>
void bind_successor_generator(nb::module_& m, const std::string& name)
{
    using T = SuccessorGenerator<Task>;

    nb::class_<T>(m, name.c_str())
        .def(nb::new_([](std::shared_ptr<Task> task, std::shared_ptr<ExecutionContext> execution_context)
                      { return T::create(std::move(task), std::move(execution_context)); }),
             "task"_a,
             "execution_context"_a)
        .def("get_initial_node", &T::get_initial_node, nb::rv_policy::move)
        .def("get_labeled_successor_nodes",
             nb::overload_cast<const Node<Task>&>(&T::get_labeled_successor_nodes),
             nb::rv_policy::move,
             "node"_a,
             nb::call_guard<nb::gil_scoped_release>())
        .def("get_successor_node", &T::get_successor_node, "node"_a, "action"_a)
        .def("get_node", &T::get_node, nb::rv_policy::move, "state_index"_a)
        .def("get_state_repository", &T::get_state_repository, nb::rv_policy::copy);
}

template<typename Task>
void bind_search_result(nb::module_& m, const std::string& name)
{
    using T = SearchResult<Task>;

    nb::class_<T>(m, name.c_str()).def(nb::init<>()).def_rw("status", &T::status).def_rw("plan", &T::plan).def_rw("goal_node", &T::goal_node);
}

template<typename Task>
class PyGoalStrategy : public GoalStrategy<Task>
{
public:
    using Base = GoalStrategy<Task>;

    NB_TRAMPOLINE(Base, 2);

    /* Trampoline (need one for each virtual function) */
    bool is_static_goal_satisfied() override { NB_OVERRIDE_PURE(is_static_goal_satisfied); }

    bool is_dynamic_goal_satisfied(const StateView<Task>& state) override { NB_OVERRIDE_PURE(is_dynamic_goal_satisfied, state); }
};

template<typename Task>
void bind_goal_strategy(nb::module_& m, const std::string& name)
{
    using T = GoalStrategy<Task>;

    nb::class_<T, PyGoalStrategy<Task>>(m, name.c_str())  //
        .def("is_static_goal_satisfied", &T::is_static_goal_satisfied)
        .def("is_dynamic_goal_satisfied", &T::is_dynamic_goal_satisfied, "state"_a);
}

template<typename Task>
void bind_task_goal_strategy(nb::module_& m, const std::string& name)
{
    using T = TaskGoalStrategy<Task>;

    nb::class_<T, GoalStrategy<Task>>(m, name.c_str())  //
        .def(nb::init<const Task&>(), "task"_a);
}

template<typename Task>
class PyPruningStrategy : public PruningStrategy<Task>
{
public:
    using Base = PruningStrategy<Task>;

    NB_TRAMPOLINE(Base, 2);

    /* Trampoline (need one for each virtual function) */
    bool should_prune_state(const StateView<Task>& state) override { NB_OVERRIDE(should_prune_state, state); }

    bool should_prune_successor_state(const StateView<Task>& state, const StateView<Task>& succ_state, bool is_new_succ_state) override
    {
        NB_OVERRIDE(should_prune_successor_state, state, succ_state, is_new_succ_state);
    }
};

template<typename Task>
void bind_pruning_strategy(nb::module_& m, const std::string& name)
{
    using T = PruningStrategy<Task>;

    nb::class_<T, PyPruningStrategy<Task>>(m, name.c_str())  //
        .def(nb::init<>())
        .def("should_prune_state", nb::overload_cast<const StateView<Task>&>(&T::should_prune_state), "state"_a)
        .def("should_prune_successor_state",
             nb::overload_cast<const StateView<Task>&, const StateView<Task>&, bool>(&T::should_prune_successor_state),
             "state"_a,
             "succ_state"_a,
             "is_new_succ_state"_a);
}

template<typename Task>
class PyHeuristic : public Heuristic<Task>
{
public:
    using Base = Heuristic<Task>;

    NB_TRAMPOLINE(Base, 3);

    /* Trampoline (need one for each virtual function) */
    void set_goal(formalism::planning::GroundConjunctiveConditionView goal) override { NB_OVERRIDE_PURE(set_goal, goal); }

    float_t evaluate(const StateView<Task>& state) override { NB_OVERRIDE_PURE(evaluate, state); }

    const UnorderedSet<formalism::planning::GroundActionView>& get_preferred_action_views() override { NB_OVERRIDE(get_preferred_action_views); }
};

template<typename Task>
void bind_heuristic(nb::module_& m, const std::string& name)
{
    using T = Heuristic<Task>;

    nb::class_<T, PyHeuristic<Task>>(m, name.c_str())  //
        .def("set_goal", &T::set_goal, "goal"_a)
        .def("evaluate", &T::evaluate, "state"_a, nb::call_guard<nb::gil_scoped_release>())
        .def("get_preferred_actions", &T::get_preferred_action_views);
}

template<typename Task>
void bind_blind_heuristic(nb::module_& m, const std::string& name)
{
    using T = BlindHeuristic<Task>;

    nb::class_<T, Heuristic<Task>>(m, name.c_str())  //
        .def(nb::new_([]() { return T::create(); }));
}

template<typename Task>
void bind_goal_count_heuristic(nb::module_& m, const std::string& name)
{
    using T = GoalCountHeuristic<Task>;

    nb::class_<T, Heuristic<Task>>(m, name.c_str())  //
        .def(nb::new_([](std::shared_ptr<const Task> task) { return T::create(std::move(task)); }));
}

template<typename Task>
void bind_rpg_max_heuristic(nb::module_& m, const std::string& name)
{
    using T = MaxRPGHeuristic<Task>;

    nb::class_<T, Heuristic<Task>>(m, name.c_str())  //
        .def(nb::new_([](std::shared_ptr<Task> task, std::shared_ptr<ExecutionContext> execution_context)
                      { return T::create(std::move(task), std::move(execution_context)); }),
             "task"_a,
             "execution_context"_a);
}

template<typename Task>
void bind_rpg_add_heuristic(nb::module_& m, const std::string& name)
{
    using T = AddRPGHeuristic<Task>;

    nb::class_<T, Heuristic<Task>>(m, name.c_str())  //
        .def(nb::new_([](std::shared_ptr<Task> task, std::shared_ptr<ExecutionContext> execution_context)
                      { return T::create(std::move(task), std::move(execution_context)); }),
             "task"_a,
             "execution_context"_a);
}

template<typename Task>
void bind_rpg_ff_heuristic(nb::module_& m, const std::string& name)
{
    using T = FFRPGHeuristic<Task>;

    nb::class_<T, Heuristic<Task>>(m, name.c_str())  //
        .def(nb::new_([](std::shared_ptr<Task> task, std::shared_ptr<ExecutionContext> execution_context)
                      { return T::create(std::move(task), std::move(execution_context)); }),
             "task"_a,
             "execution_context"_a);
}

namespace astar_eager
{

template<typename Task>
class PyEventHandler : public EventHandler<Task>
{
public:
    using Base = EventHandler<Task>;

    NB_TRAMPOLINE(Base, 13);

    /* Trampoline (need one for each virtual function) */
    void on_expand_node(const Node<Task>& node) override { NB_OVERRIDE_PURE(on_expand_node, node); }

    void on_expand_goal_node(const Node<Task>& node) override { NB_OVERRIDE_PURE(on_expand_goal_node, node); }

    void on_generate_node(const LabeledNode<Task>& labeled_succ_node) override { NB_OVERRIDE_PURE(on_generate_node, labeled_succ_node); }

    void on_generate_node_relaxed(const LabeledNode<Task>& labeled_succ_node) override { NB_OVERRIDE_PURE(on_generate_node_relaxed, labeled_succ_node); };

    void on_generate_node_not_relaxed(const LabeledNode<Task>& labeled_succ_node) override
    {
        NB_OVERRIDE_PURE(on_generate_node_not_relaxed, labeled_succ_node);
    }

    void on_close_node(const Node<Task>& node) override { NB_OVERRIDE_PURE(on_close_node, node); }

    void on_prune_node(const Node<Task>& node) override { NB_OVERRIDE_PURE(on_prune_node, node); }

    void on_start_search(const Node<Task>& node, float_t f_value) override { NB_OVERRIDE_PURE(on_start_search, node, f_value); }

    void on_finish_f_layer(float_t f_value) override { NB_OVERRIDE_PURE(on_finish_f_layer, f_value); }

    void on_end_search() override { NB_OVERRIDE_PURE(on_end_search); }

    void on_solved(const Plan<Task>& plan) override { NB_OVERRIDE_PURE(on_solved, plan); }

    void on_unsolvable() override { NB_OVERRIDE_PURE(on_unsolvable); }

    void on_exhausted() override { NB_OVERRIDE_PURE(on_exhausted); }
};

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
        nb::call_guard<nb::gil_scoped_release>(),
        "task"_a,
        "successor_generator"_a,
        "heuristic"_a,
        "options"_a);
}

template<typename Task>
void bind_event_handler(nb::module_& m, const std::string& name)
{
    using T = EventHandler<Task>;

    nb::class_<T, PyEventHandler<Task>>(m, name.c_str())
        .def("on_expand_node", &T::on_expand_goal_node, "node"_a)
        .def("on_expand_goal_node", &T::on_expand_goal_node, "node"_a)
        .def("on_generate_node", &T::on_generate_node, "labeled_succ_node"_a)
        .def("on_generate_node_relaxed", &T::on_generate_node_relaxed, "labeled_succ_node"_a)
        .def("on_generate_node_not_relaxed", &T::on_generate_node_not_relaxed, "labeled_succ_node"_a)
        .def("on_close_node", &T::on_close_node, "node"_a)
        .def("on_prune_node", &T::on_prune_node, "node"_a)
        .def("on_start_search", &T::on_start_search, "node"_a, "f_value"_a)
        .def("on_finish_f_layer", &T::on_finish_f_layer, "f_value"_a)
        .def("on_end_search", &T::on_end_search)
        .def("on_solved", &T::on_solved, "plan"_a)
        .def("on_unsolvable", &T::on_unsolvable)
        .def("on_exhausted", &T::on_exhausted);
}

template<typename Task>
void bind_default_event_handler(nb::module_& m, const std::string& name)
{
    using T = DefaultEventHandler<Task>;

    nb::class_<T, EventHandler<Task>>(m, name.c_str())  //
        .def(nb::init<size_t>(), "verbosity"_a)
        .def("get_statistics", &T::get_statistics);
}
}

namespace gbfs_lazy
{
template<typename Task>
class PyEventHandler : public EventHandler<Task>
{
public:
    using Base = EventHandler<Task>;

    NB_TRAMPOLINE(Base, 10);

    /* Trampoline (need one for each virtual function) */
    void on_expand_node(const Node<Task>& node) override { NB_OVERRIDE_PURE(on_expand_node, node); }

    void on_expand_goal_node(const Node<Task>& node) override { NB_OVERRIDE_PURE(on_expand_goal_node, node); }

    void on_generate_node(const LabeledNode<Task>& labeled_succ_node) override { NB_OVERRIDE_PURE(on_generate_node, labeled_succ_node); }

    void on_prune_node(const Node<Task>& node) override { NB_OVERRIDE_PURE(on_prune_node, node); }

    void on_start_search(const Node<Task>& node, float_t h_value) override { NB_OVERRIDE_PURE(on_start_search, node, h_value); }

    void on_new_best_h_value(float_t h_value) override { NB_OVERRIDE_PURE(on_new_best_h_value, h_value); }

    void on_end_search() override { NB_OVERRIDE_PURE(on_end_search); }

    void on_solved(const Plan<Task>& plan) override { NB_OVERRIDE_PURE(on_solved, plan); }

    void on_unsolvable() override { NB_OVERRIDE_PURE(on_unsolvable); }

    void on_exhausted() override { NB_OVERRIDE_PURE(on_exhausted); }
};

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
        .def_rw("boost_preferred_queue", &T::boost_preferred_queue)
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
        nb::call_guard<nb::gil_scoped_release>(),
        "task"_a,
        "successor_generator"_a,
        "heuristic"_a,
        "options"_a);
}

template<typename Task>
void bind_event_handler(nb::module_& m, const std::string& name)
{
    using T = EventHandler<Task>;

    nb::class_<T, PyEventHandler<Task>>(m, name.c_str())
        .def("on_expand_node", &T::on_expand_node, "node"_a)
        .def("on_expand_goal_node", &T::on_expand_goal_node, "node"_a)
        .def("on_generate_node", &T::on_generate_node, "labeled_suc_node"_a)
        .def("on_prune_node", &T::on_prune_node, "node"_a)
        .def("on_start_search", &T::on_start_search, "node"_a, "h_value"_a)
        .def("on_new_best_h_value", &T::on_new_best_h_value, "h_value"_a)
        .def("on_end_search", &T::on_end_search)
        .def("on_solved", &T::on_solved, "plan"_a)
        .def("on_unsolvable", &T::on_unsolvable)
        .def("on_exhausted", &T::on_exhausted);
}

template<typename Task>
void bind_default_event_handler(nb::module_& m, const std::string& name)
{
    using T = DefaultEventHandler<Task>;

    nb::class_<T, EventHandler<Task>>(m, name.c_str())  //
        .def(nb::init<size_t>(), "verbosity"_a)
        .def("get_statistics", &T::get_statistics);
}
}

}

#endif