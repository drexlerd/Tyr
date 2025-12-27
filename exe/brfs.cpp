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
#include <chrono>
#include <queue>
#include <tyr/tyr.hpp>

using namespace tyr;

enum SearchNodeStatus : uint8_t
{
    GOAL = 0,
    DEAD_END = 1,
    CLOSED = 2,
    OPEN = 3,
    NEW = 4,
};

struct SearchNode
{
    tyr::uint_t g_value;
    planning::StateIndex parent;
    SearchNodeStatus status;
};

inline auto& get_or_create_search_node(planning::StateIndex state, SegmentedVector<SearchNode>& search_nodes)
{
    static auto default_node = SearchNode { tyr::uint_t(0), planning::StateIndex(), SearchNodeStatus::NEW };

    while (uint_t(state) >= search_nodes.size())
    {
        search_nodes.push_back(default_node);
    }
    return search_nodes[uint_t(state)];
}

int main(int argc, char** argv)
{
    auto program = argparse::ArgumentParser("BrFS search.");
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

    auto successor_generator = planning::SuccessorGenerator<planning::LiftedTask>(lifted_task);

    auto initial_node = successor_generator.get_initial_node();

    auto queue = std::deque<planning::StateIndex>();

    auto search_nodes = SegmentedVector<SearchNode> {};

    queue.emplace_back(initial_node.get_state().get_index());

    auto& initial_search_node = get_or_create_search_node(initial_node.get_state().get_index(), search_nodes);
    initial_search_node.status = SearchNodeStatus::OPEN;
    initial_search_node.g_value = 0;

    auto labeled_succ_nodes = std::vector<planning::LabeledNode<planning::LiftedTask>> {};

    auto g_value = uint_t(0);
    auto num_expanded = size_t(0);
    auto num_generated = size_t(0);
    auto found_solution = bool(false);

    while (!queue.empty())
    {
        auto state_index = queue.front();
        queue.pop_front();

        ++num_expanded;

        const auto& search_node = get_or_create_search_node(state_index, search_nodes);

        if (search_node.g_value > g_value)
        {
            std::cout << "Finished g-layer: " << search_node.g_value << " with num expanded = " << num_expanded << " and num generated = " << num_generated
                      << std::endl;
            g_value = search_node.g_value;
        }

        auto node = planning::Node<planning::LiftedTask>(successor_generator.get_state(state_index), search_node.g_value);

        labeled_succ_nodes.clear();
        successor_generator.get_labeled_successor_nodes(node, labeled_succ_nodes);

        for (const auto& labeled_succ_node : labeled_succ_nodes)
        {
            auto& labeled_succ_search_node = get_or_create_search_node(labeled_succ_node.node.get_state().get_index(), search_nodes);

            ++num_generated;

            if (labeled_succ_search_node.status != SearchNodeStatus::NEW)
                continue;

            labeled_succ_search_node.status = SearchNodeStatus::OPEN;
            labeled_succ_search_node.parent = state_index;
            labeled_succ_search_node.g_value = search_node.g_value + 1;

            if (planning::is_applicable(
                    lifted_task->get_task().get_goal(),
                    planning::StateContext { *lifted_task, labeled_succ_node.node.get_state().get_unpacked_state(), labeled_succ_search_node.g_value }))
            {
                std::cout << "Solution found!" << std::endl;
                std::cout << "Plan length: " << labeled_succ_search_node.g_value << std::endl;
                found_solution = true;
                break;
            }

            queue.emplace_back(labeled_succ_node.node.get_state().get_index());
        }
        if (found_solution)
            break;
    }

    std::cout << "Num expanded: " << num_expanded << std::endl;
    std::cout << "Num generated: " << num_generated << std::endl;

    return 0;
}