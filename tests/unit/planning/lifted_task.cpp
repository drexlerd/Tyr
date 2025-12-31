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

namespace tyr::tests
{

static p::LiftedTaskPtr compute_lifted_task(const fs::path& domain_filepath, const fs::path& problem_filepath)
{
    auto parser = p::Parser(domain_filepath);
    auto domain = parser.get_domain();
    return parser.parse_task(problem_filepath);
}

static fs::path absolute(const std::string& subdir) { return fs::path(std::string(DATA_DIR)) / subdir; }

TEST(TyrTests, TyrPlanningLiftedTaskAgricola)
{
    auto lifted_task = compute_lifted_task(absolute("agricola/domain.pddl"), absolute("agricola/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 8);
}

TEST(TyrTests, TyrPlanningLiftedTaskAirport)
{
    auto lifted_task = compute_lifted_task(absolute("airport/domain.pddl"), absolute("airport/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 2);
}

TEST(TyrTests, TyrPlanningLiftedTaskAssembly)
{
    auto lifted_task = compute_lifted_task(absolute("assembly/domain.pddl"), absolute("assembly/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 3);
}

TEST(TyrTests, TyrPlanningLiftedTaskBarman)
{
    auto lifted_task = compute_lifted_task(absolute("barman/domain.pddl"), absolute("barman/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 4);
}

TEST(TyrTests, TyrPlanningLiftedTaskBlocks3)
{
    auto lifted_task = compute_lifted_task(absolute("blocks_3/domain.pddl"), absolute("blocks_3/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 2);
}

TEST(TyrTests, TyrPlanningLiftedTaskBlocks4)
{
    auto lifted_task = compute_lifted_task(absolute("blocks_4/domain.pddl"), absolute("blocks_4/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 2);
}

TEST(TyrTests, TyrPlanningLiftedTaskChildsnack)
{
    auto lifted_task = compute_lifted_task(absolute("childsnack/domain.pddl"), absolute("childsnack/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 3);
}

TEST(TyrTests, TyrPlanningLiftedTaskDelivery)
{
    auto lifted_task = compute_lifted_task(absolute("delivery/domain.pddl"), absolute("delivery/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 2);
}

TEST(TyrTests, TyrPlanningLiftedTaskDriverlog)
{
    auto lifted_task = compute_lifted_task(absolute("driverlog/domain.pddl"), absolute("driverlog/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 2);
}

TEST(TyrTests, TyrPlanningLiftedTaskFerry)
{
    auto lifted_task = compute_lifted_task(absolute("ferry/domain.pddl"), absolute("ferry/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 3);
}

TEST(TyrTests, TyrPlanningLiftedTaskFoCounters)
{
    auto lifted_task = compute_lifted_task(absolute("fo-counters/domain.pddl"), absolute("fo-counters/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 9);
}

TEST(TyrTests, TyrPlanningLiftedTaskGrid)
{
    auto lifted_task = compute_lifted_task(absolute("grid/domain.pddl"), absolute("grid/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 1);
}

TEST(TyrTests, TyrPlanningLiftedTaskGripper)
{
    auto lifted_task = compute_lifted_task(absolute("gripper/domain.pddl"), absolute("gripper/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 6);
}

TEST(TyrTests, TyrPlanningLiftedTaskHiking)
{
    auto lifted_task = compute_lifted_task(absolute("hiking/domain.pddl"), absolute("hiking/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 18);
}

TEST(TyrTests, TyrPlanningLiftedTaskLogistics)
{
    auto lifted_task = compute_lifted_task(absolute("logistics/domain.pddl"), absolute("logistics/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 6);
}

TEST(TyrTests, TyrPlanningLiftedTaskMiconic)
{
    auto lifted_task = compute_lifted_task(absolute("miconic/domain.pddl"), absolute("miconic/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 3);
}

TEST(TyrTests, TyrPlanningLiftedTaskMiconicFulladl)
{
    auto lifted_task = compute_lifted_task(absolute("miconic-fulladl/domain.pddl"), absolute("miconic-fulladl/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 3);
}

TEST(TyrTests, TyrPlanningLiftedTaskMiconicSimpleadl)
{
    auto lifted_task = compute_lifted_task(absolute("miconic-simpleadl/domain.pddl"), absolute("miconic-simpleadl/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 2);
}

TEST(TyrTests, TyrPlanningLiftedTaskParcprinter)
{
    auto lifted_task = compute_lifted_task(absolute("parcprinter/domain.pddl"), absolute("parcprinter/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 1);
}

TEST(TyrTests, TyrPlanningLiftedTaskPathways)
{
    auto lifted_task = compute_lifted_task(absolute("pathways/domain.pddl"), absolute("pathways/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 16);
}

TEST(TyrTests, TyrPlanningLiftedTaskPhilosophers)
{
    auto lifted_task = compute_lifted_task(absolute("philosophers/domain.pddl"), absolute("philosophers/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 2);
}

TEST(TyrTests, TyrPlanningLiftedTaskPsrMiddle)
{
    auto lifted_task = compute_lifted_task(absolute("psr-middle/domain.pddl"), absolute("psr-middle/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 1);
}

TEST(TyrTests, TyrPlanningLiftedTaskPushworld)
{
    auto lifted_task = compute_lifted_task(absolute("pushworld/domain.pddl"), absolute("pushworld/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 4);
}

TEST(TyrTests, TyrPlanningLiftedTaskRefuel)
{
    auto lifted_task = compute_lifted_task(absolute("refuel/domain.pddl"), absolute("refuel/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 1);
}

TEST(TyrTests, TyrPlanningLiftedTaskRefuelAdl)
{
    auto lifted_task = compute_lifted_task(absolute("refuel-adl/domain.pddl"), absolute("refuel-adl/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 5);
}

TEST(TyrTests, TyrPlanningLiftedTaskReward)
{
    auto lifted_task = compute_lifted_task(absolute("reward/domain.pddl"), absolute("reward/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 1);
}

TEST(TyrTests, TyrPlanningLiftedTaskRovers)
{
    auto lifted_task = compute_lifted_task(absolute("rovers/domain.pddl"), absolute("rovers/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 2);
}

TEST(TyrTests, TyrPlanningLiftedTaskSatellite)
{
    auto lifted_task = compute_lifted_task(absolute("satellite/domain.pddl"), absolute("satellite/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 4);
}

TEST(TyrTests, TyrPlanningLiftedTaskSchedule)
{
    auto lifted_task = compute_lifted_task(absolute("schedule/domain.pddl"), absolute("schedule/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 44);
}

TEST(TyrTests, TyrPlanningLiftedTaskSokoban)
{
    auto lifted_task = compute_lifted_task(absolute("sokoban/domain.pddl"), absolute("sokoban/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 3);
}

TEST(TyrTests, TyrPlanningLiftedTaskSpanner)
{
    auto lifted_task = compute_lifted_task(absolute("spanner/domain.pddl"), absolute("spanner/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 1);
}

TEST(TyrTests, TyrPlanningLiftedTaskTpp)
{
    auto lifted_task = compute_lifted_task(absolute("tpp/numeric/domain.pddl"), absolute("tpp/numeric/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 5);
}

TEST(TyrTests, TyrPlanningLiftedTaskTransport)
{
    auto lifted_task = compute_lifted_task(absolute("transport/domain.pddl"), absolute("transport/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 5);
}

TEST(TyrTests, TyrPlanningLiftedTaskVisitall)
{
    auto lifted_task = compute_lifted_task(absolute("visitall/domain.pddl"), absolute("visitall/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 2);
}

TEST(TyrTests, TyrPlanningLiftedTaskWoodworking)
{
    auto lifted_task = compute_lifted_task(absolute("woodworking/domain.pddl"), absolute("woodworking/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 8);
}

TEST(TyrTests, TyrPlanningLiftedTaskZenotravel)
{
    auto lifted_task = compute_lifted_task(absolute("zenotravel/numeric/domain.pddl"), absolute("zenotravel/numeric/test_problem.pddl"));

    auto successor_generator = p::SuccessorGenerator<p::LiftedTask>(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), 7);
}
}