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

using namespace tyr::buffer;
using namespace tyr::formalism;
using namespace tyr::planning;

namespace tyr::tests
{

static GroundTaskPtr compute_ground_task(const fs::path& domain_filepath, const fs::path& problem_filepath)
{
    auto parser = planning::Parser(domain_filepath);
    auto domain = parser.get_domain();
    auto lifted_task = parser.parse_task(problem_filepath);
    return lifted_task->get_ground_task();
}

static fs::path absolute(const std::string& subdir) { return fs::path(std::string(DATA_DIR)) / subdir; }

TEST(TyrTests, TyrPlanningGroundTaskAirport)
{
    auto ground_task = compute_ground_task(absolute("airport/domain.pddl"), absolute("airport/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 59);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 379);
    EXPECT_EQ(ground_task->get_num_actions(), 63);
    EXPECT_EQ(ground_task->get_num_axioms(), 420);
}

TEST(TyrTests, TyrPlanningGroundTaskAssembly)
{
    auto ground_task = compute_ground_task(absolute("assembly/domain.pddl"), absolute("assembly/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 7);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 8);
    EXPECT_EQ(ground_task->get_num_actions(), 6);
    EXPECT_EQ(ground_task->get_num_axioms(), 2);
}

TEST(TyrTests, TyrPlanningGroundTaskBarman)
{
    auto ground_task = compute_ground_task(absolute("barman/domain.pddl"), absolute("barman/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 26);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 84);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskBlocks3)
{
    auto ground_task = compute_ground_task(absolute("blocks_3/domain.pddl"), absolute("blocks_3/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 15);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 45);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskBlocks4)
{
    auto ground_task = compute_ground_task(absolute("blocks_4/domain.pddl"), absolute("blocks_4/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 19);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 24);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskChildsnack)
{
    auto ground_task = compute_ground_task(absolute("childsnack/domain.pddl"), absolute("childsnack/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 8);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 7);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskDelivery)
{
    auto ground_task = compute_ground_task(absolute("delivery/domain.pddl"), absolute("delivery/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 10);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 16);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskDriverlog)
{
    auto ground_task = compute_ground_task(absolute("driverlog/domain.pddl"), absolute("driverlog/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 10);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 14);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskFerry)
{
    auto ground_task = compute_ground_task(absolute("ferry/domain.pddl"), absolute("ferry/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 9);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 12);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskFoCounters)
{
    auto ground_task = compute_ground_task(absolute("fo-counters/domain.pddl"), absolute("fo-counters/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 0);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 12);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskGrid)
{
    auto ground_task = compute_ground_task(absolute("grid/domain.pddl"), absolute("grid/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 21);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 35);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskGripper)
{
    auto ground_task = compute_ground_task(absolute("gripper/domain.pddl"), absolute("gripper/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 12);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 20);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskHiking)
{
    auto ground_task = compute_ground_task(absolute("hiking/domain.pddl"), absolute("hiking/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 12);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 41);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskLogistics)
{
    auto ground_task = compute_ground_task(absolute("logistics/domain.pddl"), absolute("logistics/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 9);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 14);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskMiconic)
{
    auto ground_task = compute_ground_task(absolute("miconic/domain.pddl"), absolute("miconic/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 8);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 6);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskMiconicFulladl)
{
    auto ground_task = compute_ground_task(absolute("miconic-fulladl/domain.pddl"), absolute("miconic-fulladl/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 9);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 22);
    EXPECT_EQ(ground_task->get_num_actions(), 14);
    EXPECT_EQ(ground_task->get_num_axioms(), 15);
}

TEST(TyrTests, TyrPlanningGroundTaskMiconicSimpleadl)
{
    auto ground_task = compute_ground_task(absolute("miconic-simpleadl/domain.pddl"), absolute("miconic-simpleadl/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 4);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 4);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskPhilosophers)
{
    auto ground_task = compute_ground_task(absolute("philosophers/domain.pddl"), absolute("philosophers/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 50);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 21);
    EXPECT_EQ(ground_task->get_num_actions(), 34);
    EXPECT_EQ(ground_task->get_num_axioms(), 34);
}

TEST(TyrTests, TyrPlanningGroundTaskPushworld)
{
    //        auto ground_task = compute_ground_task(absolute("pushworld/domain.pddl"), absolute("pushworld/test_problem.pddl"));
    //
    //        EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 327);
    //        EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    //        EXPECT_EQ(ground_task->get_num_actions(), 6924);
    //        EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskRefuel)
{
    auto ground_task = compute_ground_task(absolute("refuel/domain.pddl"), absolute("refuel/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 0);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 1);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskRefuelAdl)
{
    auto ground_task = compute_ground_task(absolute("refuel-adl/domain.pddl"), absolute("refuel-adl/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 6);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 1);
    EXPECT_EQ(ground_task->get_num_actions(), 15);
    EXPECT_EQ(ground_task->get_num_axioms(), 3);
}

TEST(TyrTests, TyrPlanningGroundTaskReward)
{
    auto ground_task = compute_ground_task(absolute("reward/domain.pddl"), absolute("reward/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 7);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 6);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskRovers)
{
    auto ground_task = compute_ground_task(absolute("rovers/domain.pddl"), absolute("rovers/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 12);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 7);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskSatellite)
{
    auto ground_task = compute_ground_task(absolute("satellite/domain.pddl"), absolute("satellite/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 12);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 18);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskSchedule)
{
    auto ground_task = compute_ground_task(absolute("schedule/domain.pddl"), absolute("schedule/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 45);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 49);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskSokoban)
{
    auto ground_task = compute_ground_task(absolute("sokoban/domain.pddl"), absolute("sokoban/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 260);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 526);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskSpanner)
{
    auto ground_task = compute_ground_task(absolute("spanner/domain.pddl"), absolute("spanner/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 9);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 4);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskTpp)
{
    auto ground_task = compute_ground_task(absolute("tpp/numeric/domain.pddl"), absolute("tpp/numeric/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 6);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 56);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskTransport)
{
    auto ground_task = compute_ground_task(absolute("transport/domain.pddl"), absolute("transport/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 26);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 104);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskVisitall)
{
    auto ground_task = compute_ground_task(absolute("visitall/domain.pddl"), absolute("visitall/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 14);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 12);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskWoodworking)
{
    auto ground_task = compute_ground_task(absolute("woodworking/domain.pddl"), absolute("woodworking/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 19);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 57);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}

TEST(TyrTests, TyrPlanningGroundTaskZenotravel)
{
    auto ground_task = compute_ground_task(absolute("zenotravel/numeric/domain.pddl"), absolute("zenotravel/numeric/test_problem.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<FluentTag>(), 15);
    EXPECT_EQ(ground_task->get_num_atoms<DerivedTag>(), 0);
    EXPECT_EQ(ground_task->get_num_actions(), 37);
    EXPECT_EQ(ground_task->get_num_axioms(), 0);
}
}