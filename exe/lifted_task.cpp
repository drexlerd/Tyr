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
#include <tyr/tyr.hpp>

using namespace tyr;

int main(int argc, char** argv)
{
    auto program = argparse::ArgumentParser("AStar search.");
    program.add_argument("-D", "--domain-filepath").required().help("The path to the PDDL domain file.");
    program.add_argument("-P", "--problem-filepath").required().help("The path to the PDDL problem file.");

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
    // oneapi::tbb::global_control control(oneapi::tbb::global_control::max_allowed_parallelism, 1);

    auto parser = planning::Parser(domain_filepath);
    auto domain = parser.get_domain();

    auto lifted_task = parser.parse_task(problem_filepath);

    // std::cout << *domain << std::endl;

    // std::cout << *lifted_task << std::endl;

    auto initial_node = lifted_task->get_initial_node();

    {
        const auto& action_program = lifted_task->get_action_program();

        auto program_execution_context = grounder::ProgramExecutionContext(action_program.get_program(), action_program.get_repository());

        const uint_t num_rules = action_program.get_program().get_rules().size();

        tbb::parallel_for(uint_t { 0 },
                          num_rules,
                          [&](uint_t i)
                          {
                              auto& facts_execution_context = program_execution_context.facts_execution_context;
                              auto& rule_execution_context = program_execution_context.rule_execution_contexts[i];
                              auto& thread_execution_context = program_execution_context.thread_execution_contexts.local();  // thread-local
                              thread_execution_context.clear();

                              grounder::ground(facts_execution_context, rule_execution_context, thread_execution_context);
                          });

        /**
         * Sequential merge.
         */

        auto builder = formalism::Builder();

        auto merge_cache = formalism::MergeCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository> {};
        auto merge_repository = formalism::Repository();
        auto merge_ground_rules = std::vector<View<Index<formalism::GroundRule>, formalism::Repository>> {};

        for (uint_t i = 0; i < action_program.get_program().get_rules().size(); ++i)
        {
            auto& rule_execution_context = program_execution_context.rule_execution_contexts[i];

            for (const auto ground_rule : rule_execution_context.ground_rules)
            {
                merge_ground_rules.push_back(formalism::merge(ground_rule, builder, merge_repository, merge_cache));
            }
        }

        // Now everything is moved from an OverlayRepository to a top-level Repository and we can perform the final merge step
        auto global_cache = formalism::MergeCache<formalism::Repository, formalism::Repository> {};
        auto global_ground_rules = std::vector<View<Index<formalism::GroundRule>, formalism::Repository>> {};

        for (const auto ground_rule : merge_ground_rules)
        {
            const auto global_ground_rule = formalism::merge(ground_rule, builder, *action_program.get_repository(), global_cache);

            std::cout << global_ground_rule << std::endl;

            global_ground_rules.push_back(global_ground_rule);
        }
    }

    {
        const auto& axiom_program = lifted_task->get_axiom_program();

        std::cout << axiom_program.get_program() << std::endl;

        auto program_execution_context = grounder::ProgramExecutionContext(axiom_program.get_program(), axiom_program.get_repository());

        const uint_t num_rules = axiom_program.get_program().get_rules().size();

        tbb::parallel_for(uint_t { 0 },
                          num_rules,
                          [&](uint_t i)
                          {
                              auto& facts_execution_context = program_execution_context.facts_execution_context;
                              auto& rule_execution_context = program_execution_context.rule_execution_contexts[i];
                              auto& thread_execution_context = program_execution_context.thread_execution_contexts.local();  // thread-local
                              thread_execution_context.clear();

                              grounder::ground(facts_execution_context, rule_execution_context, thread_execution_context);
                          });

        /**
         * Sequential merge.
         */

        auto builder = formalism::Builder();

        auto merge_cache = formalism::MergeCache<formalism::OverlayRepository<formalism::Repository>, formalism::Repository> {};
        auto merge_repository = formalism::Repository();
        auto merge_ground_rules = std::vector<View<Index<formalism::GroundRule>, formalism::Repository>> {};

        for (uint_t i = 0; i < axiom_program.get_program().get_rules().size(); ++i)
        {
            auto& rule_execution_context = program_execution_context.rule_execution_contexts[i];

            for (const auto ground_rule : rule_execution_context.ground_rules)
            {
                merge_ground_rules.push_back(formalism::merge(ground_rule, builder, merge_repository, merge_cache));
            }
        }

        // Now everything is moved from an OverlayRepository to a top-level Repository and we can perform the final merge step
        auto global_cache = formalism::MergeCache<formalism::Repository, formalism::Repository> {};
        auto global_ground_rules = std::vector<View<Index<formalism::GroundRule>, formalism::Repository>> {};

        for (const auto ground_rule : merge_ground_rules)
        {
            const auto global_ground_rule = formalism::merge(ground_rule, builder, *axiom_program.get_repository(), global_cache);

            std::cout << global_ground_rule << std::endl;

            global_ground_rules.push_back(global_ground_rule);
        }
    }
}