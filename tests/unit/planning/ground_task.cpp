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

#include <gtest/gtest.h>
#include <tyr/planning/planning.hpp>

namespace p = tyr::planning;
namespace f = tyr::formalism;

namespace tyr::tests
{

static p::GroundTaskPtr compute_ground_task(const fs::path& domain_filepath, const fs::path& problem_filepath)
{
    auto parser = p::Parser(domain_filepath);
    auto domain = parser.get_domain();
    auto lifted_task = parser.parse_task(problem_filepath);
    return lifted_task->get_ground_task();
}

static fs::path absolute(const std::string& subdir) { return fs::path(std::string(DATA_DIR)) / subdir; }

TEST(TyrTests, TyrPlanningGroundTaskAgricola)
{
    auto ground_task = compute_ground_task(absolute("agricola/domain.pddl"), absolute("agricola/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 141);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 12443);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 8);
}

TEST(TyrTests, TyrPlanningGroundTaskAirport)
{
    auto ground_task = compute_ground_task(absolute("airport/domain.pddl"), absolute("airport/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 59);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 379);
    EXPECT_EQ(ground_task->get_num_actions(), 43);
    EXPECT_EQ(ground_task->get_num_axioms(), 420);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 2);
}

TEST(TyrTests, TyrPlanningGroundTaskAssembly)
{
    auto ground_task = compute_ground_task(absolute("assembly/domain.pddl"), absolute("assembly/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 7);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 8);
    EXPECT_EQ(ground_task->get_num_actions(), 6);
    EXPECT_EQ(ground_task->get_num_axioms(), 2);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 3);
}

TEST(TyrTests, TyrPlanningGroundTaskBarman)
{
    auto ground_task = compute_ground_task(absolute("barman/domain.pddl"), absolute("barman/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 26);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 84);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 4);
}

TEST(TyrTests, TyrPlanningGroundTaskBlocks3)
{
    auto ground_task = compute_ground_task(absolute("blocks_3/domain.pddl"), absolute("blocks_3/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 15);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 45);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 2);
}

TEST(TyrTests, TyrPlanningGroundTaskBlocks4)
{
    auto ground_task = compute_ground_task(absolute("blocks_4/domain.pddl"), absolute("blocks_4/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 19);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 24);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 2);
}

TEST(TyrTests, TyrPlanningGroundTaskChildsnack)
{
    auto ground_task = compute_ground_task(absolute("childsnack/domain.pddl"), absolute("childsnack/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 8);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 7);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 3);
}

TEST(TyrTests, TyrPlanningGroundTaskDelivery)
{
    auto ground_task = compute_ground_task(absolute("delivery/domain.pddl"), absolute("delivery/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 10);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 16);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 2);
}

TEST(TyrTests, TyrPlanningGroundTaskDriverlog)
{
    auto ground_task = compute_ground_task(absolute("driverlog/domain.pddl"), absolute("driverlog/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 10);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 14);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 2);
}

TEST(TyrTests, TyrPlanningGroundTaskFerry)
{
    auto ground_task = compute_ground_task(absolute("ferry/domain.pddl"), absolute("ferry/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 9);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 10);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 3);
}

TEST(TyrTests, TyrPlanningGroundTaskFoCounters)
{
    auto ground_task = compute_ground_task(absolute("fo-counters/domain.pddl"), absolute("fo-counters/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 0);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 12);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 9);
}

TEST(TyrTests, TyrPlanningGroundTaskGrid)
{
    auto ground_task = compute_ground_task(absolute("grid/domain.pddl"), absolute("grid/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 21);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 35);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 1);
}

TEST(TyrTests, TyrPlanningGroundTaskGripper)
{
    auto ground_task = compute_ground_task(absolute("gripper/domain.pddl"), absolute("gripper/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 12);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 20);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 6);
}

TEST(TyrTests, TyrPlanningGroundTaskHiking)
{
    auto ground_task = compute_ground_task(absolute("hiking/domain.pddl"), absolute("hiking/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 12);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 41);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 18);
}

TEST(TyrTests, TyrPlanningGroundTaskLogistics)
{
    auto ground_task = compute_ground_task(absolute("logistics/domain.pddl"), absolute("logistics/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 9);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 14);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 6);
}

TEST(TyrTests, TyrPlanningGroundTaskMiconic)
{
    auto ground_task = compute_ground_task(absolute("miconic/domain.pddl"), absolute("miconic/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 8);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 6);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 3);
}

TEST(TyrTests, TyrPlanningGroundTaskMiconicFulladl)
{
    auto ground_task = compute_ground_task(absolute("miconic-fulladl/domain.pddl"), absolute("miconic-fulladl/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 9);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 22);
    EXPECT_EQ(ground_task->get_num_actions(), 10);
    EXPECT_EQ(ground_task->get_num_axioms(), 15);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 3);
}

TEST(TyrTests, TyrPlanningGroundTaskMiconicSimpleadl)
{
    auto ground_task = compute_ground_task(absolute("miconic-simpleadl/domain.pddl"), absolute("miconic-simpleadl/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 4);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 4);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 2);
}

TEST(TyrTests, TyrPlanningGroundTaskParcprinter)
{
    auto ground_task = compute_ground_task(absolute("parcprinter/domain.pddl"), absolute("parcprinter/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 43);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 25);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 1);
}

TEST(TyrTests, TyrPlanningGroundTaskPathways)
{
    auto ground_task = compute_ground_task(absolute("pathways/domain.pddl"), absolute("pathways/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 47);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 78);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 16);
}

TEST(TyrTests, TyrPlanningGroundTaskPhilosophers)
{
    auto ground_task = compute_ground_task(absolute("philosophers/domain.pddl"), absolute("philosophers/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 50);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 21);
    EXPECT_EQ(ground_task->get_num_actions(), 34);
    EXPECT_EQ(ground_task->get_num_axioms(), 34);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 2);
}

TEST(TyrTests, TyrPlanningGroundTaskPsrMiddle)
{
    auto ground_task = compute_ground_task(absolute("psr-middle/domain.pddl"), absolute("psr-middle/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 14);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 363);
    EXPECT_EQ(ground_task->get_num_actions(), 28);
    EXPECT_EQ(ground_task->get_num_axioms(), 467);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 1);
}

TEST(TyrTests, TyrPlanningGroundTaskPushworld)
{
    auto ground_task = compute_ground_task(absolute("pushworld/domain.pddl"), absolute("pushworld/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 228);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 8);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 4);
}

TEST(TyrTests, TyrPlanningGroundTaskRefuel)
{
    auto ground_task = compute_ground_task(absolute("refuel/domain.pddl"), absolute("refuel/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 0);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 1);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 1);
}

TEST(TyrTests, TyrPlanningGroundTaskRefuelAdl)
{
    auto ground_task = compute_ground_task(absolute("refuel-adl/domain.pddl"), absolute("refuel-adl/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 6);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 1);
    EXPECT_EQ(ground_task->get_num_actions(), 15);
    EXPECT_EQ(ground_task->get_num_axioms(), 3);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 5);
}

TEST(TyrTests, TyrPlanningGroundTaskReward)
{
    auto ground_task = compute_ground_task(absolute("reward/domain.pddl"), absolute("reward/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 7);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 6);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 1);
}

TEST(TyrTests, TyrPlanningGroundTaskRovers)
{
    auto ground_task = compute_ground_task(absolute("rovers/domain.pddl"), absolute("rovers/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 12);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 7);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 2);
}

TEST(TyrTests, TyrPlanningGroundTaskSatellite)
{
    auto ground_task = compute_ground_task(absolute("satellite/domain.pddl"), absolute("satellite/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 12);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 18);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 4);
}

TEST(TyrTests, TyrPlanningGroundTaskSchedule)
{
    auto ground_task = compute_ground_task(absolute("schedule/domain.pddl"), absolute("schedule/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 45);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 49);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 44);
}

TEST(TyrTests, TyrPlanningGroundTaskSokoban)
{
    auto ground_task = compute_ground_task(absolute("sokoban/domain.pddl"), absolute("sokoban/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 260);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 526);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 3);
}

TEST(TyrTests, TyrPlanningGroundTaskSpanner)
{
    auto ground_task = compute_ground_task(absolute("spanner/domain.pddl"), absolute("spanner/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 9);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 4);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 1);
}

TEST(TyrTests, TyrPlanningGroundTaskTpp)
{
    auto ground_task = compute_ground_task(absolute("tpp/numeric/domain.pddl"), absolute("tpp/numeric/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 6);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 56);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 5);
}

TEST(TyrTests, TyrPlanningGroundTaskTransport)
{
    auto ground_task = compute_ground_task(absolute("transport/domain.pddl"), absolute("transport/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 26);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 104);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 5);
}

TEST(TyrTests, TyrPlanningGroundTaskVisitall)
{
    auto ground_task = compute_ground_task(absolute("visitall/domain.pddl"), absolute("visitall/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 14);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 12);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 2);
}

TEST(TyrTests, TyrPlanningGroundTaskWoodworking)
{
    auto ground_task = compute_ground_task(absolute("woodworking/domain.pddl"), absolute("woodworking/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 52);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 198);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 8);
}

TEST(TyrTests, TyrPlanningGroundTaskZenotravel)
{
    auto ground_task = compute_ground_task(absolute("zenotravel/numeric/domain.pddl"), absolute("zenotravel/numeric/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), 15);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 37);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);

    auto successor_generator = p::SuccessorGenerator<p::GroundTask>(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 7);
}
}