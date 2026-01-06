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

#include <argparse/argparse.hpp>
#include <oneapi/tbb/global_control.h>
#include <tyr/tyr.hpp>

using namespace tyr;

int main(int argc, char** argv)
{
    auto program = argparse::ArgumentParser("AStar search.");
    program.add_argument("-D", "--domain-filepath").required().help("The path to the PDDL domain file.");
    program.add_argument("-P", "--problem-filepath").required().help("The path to the PDDL problem file.");
    program.add_argument("-N", "--num-worker-threads").default_value(size_t(1)).scan<'u', size_t>().help("The number of worker threads.");
    program.add_argument("-V", "--verbosity")
        .default_value(size_t(0))
        .scan<'u', size_t>()
        .help("The verbosity level. Defaults to minimal amount of debug output.");

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err)
    {
        std::cerr << err.what() << "\n";
        std::cerr << program;
        std::exit(1);
    }

    auto domain_filepath = program.get<std::string>("--domain-filepath");
    auto problem_filepath = program.get<std::string>("--problem-filepath");
    oneapi::tbb::global_control control(oneapi::tbb::global_control::max_allowed_parallelism, program.get<std::size_t>("--num-worker-threads"));
    auto verbosity = program.get<size_t>("--verbosity");

    auto parser = planning::Parser(domain_filepath);
    auto domain = parser.get_domain();

    auto lifted_task = parser.parse_task(problem_filepath);

    if (verbosity > 0)
        std::cout << *domain << std::endl;

    if (verbosity > 0)
        std::cout << *lifted_task << std::endl;

    auto successor_generator = planning::SuccessorGenerator<planning::LiftedTask>(lifted_task);
    auto add_heuristic = planning::Add(lifted_task);
    add_heuristic.set_goal(lifted_task->get_task().get_goal());
    auto max_heuristic = planning::Max(lifted_task);
    max_heuristic.set_goal(lifted_task->get_task().get_goal());
    auto ff_heuristic = planning::FF(lifted_task);
    ff_heuristic.set_goal(lifted_task->get_task().get_goal());

    auto initial_node = successor_generator.get_initial_node();

    std::cout << "Initial h_add estimate: " << add_heuristic.evaluate(initial_node.get_state()) << std::endl;
    std::cout << "Num h_add preferred actions: " << add_heuristic.get_preferred_actions().size() << std::endl;
    std::cout << "Initial h_max estimate: " << max_heuristic.evaluate(initial_node.get_state()) << std::endl;
    std::cout << "Num h_max preferred actions: " << max_heuristic.get_preferred_actions().size() << std::endl;
    std::cout << "Initial h_ff estimate: " << ff_heuristic.evaluate(initial_node.get_state()) << std::endl;
    std::cout << "Num h_ff preferred actions: " << ff_heuristic.get_preferred_actions().size() << std::endl;

    if (verbosity > 1)
        std::cout << to_string(initial_node) << std::endl;

    for (const auto& [ground_action, succ_node] : successor_generator.get_labeled_successor_nodes(initial_node))
    {
        if (verbosity > 1)
            std::cout << to_string(ground_action) << "\n"  //
                      << to_string(succ_node) << std::endl;
    }
}