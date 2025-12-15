#! /usr/bin/env python

import platform
import re
import os
import sys

from pathlib import Path

from downward import suites
from downward.reports.absolute import AbsoluteReport
from lab.environments import TetralithEnvironment, LocalEnvironment
from lab.experiment import Experiment
from lab.reports import Attribute, geometric_mean

DIR = Path(__file__).resolve().parent
REPO = DIR.parent

sys.path.append(str(DIR.parent))

from experiments.parser_ground_task import GroundTaskParser

from suite import SUITE_CNOT_SYNTHESIS, SUITE_IPC_OPTIMAL_STRIPS, SUITE_IPC_OPTIMAL_ADL, SUITE_IPC_SATISFICING_STRIPS, SUITE_IPC_LEARNING, SUITE_AUTOSCALE_OPTIMAL_STRIPS, SUITE_HTG, SUITE_IPC2023_NUMERIC, SUITE_PUSHWORLD, SUITE_BELUGA2025_SCALABILITY_DETERMINISTIC, SUITE_MINEPDDL
from suite_test import SUITE_CNOT_SYNTHESIS_TEST, SUITE_IPC_OPTIMAL_STRIPS_TEST, SUITE_IPC_OPTIMAL_ADL_TEST, SUITE_IPC_SATISFICING_STRIPS_TEST, SUITE_IPC_LEARNING_TEST, SUITE_AUTOSCALE_OPTIMAL_STRIPS_TEST, SUITE_HTG_TEST, SUITE_IPC2023_NUMERIC_TEST, SUITE_PUSHWORLD_TEST, SUITE_BELUGA2025_SCALABILITY_DETERMINISTIC_TEST, SUITE_MINEPDDL_TEST

# Create custom report class with suitable info and error attributes.
class BaseReport(AbsoluteReport):
    INFO_ATTRIBUTES = ["time_limit", "memory_limit"]
    ERROR_ATTRIBUTES = [
        "domain",
        "problem",
        "algorithm",
        "unexplained_errors",
        "error",
        "node",
    ]


BENCHMARKS_DIR = Path(os.environ["BENCHMARKS_PDDL"])

NODE = platform.node()
REMOTE = re.match(r"tetralith\d+.nsc.liu.se|n\d+", NODE)

NUM_THREADS = 4

if REMOTE:
    ENV = TetralithEnvironment(
        setup=TetralithEnvironment.DEFAULT_SETUP,
        memory_per_cpu="2840M",
        cpus_per_task=NUM_THREADS,
        extra_options="#SBATCH --account=naiss2025-22-1245")
    
else:
    ENV = LocalEnvironment(processes=6)

if REMOTE:
    SUITES = [
        ("cnot-synthesis", SUITE_CNOT_SYNTHESIS),
        ("downward-benchmarks", SUITE_IPC_OPTIMAL_STRIPS),
        ("downward-benchmarks", SUITE_IPC_OPTIMAL_ADL),
        ("downward-benchmarks", SUITE_IPC_SATISFICING_STRIPS),
        ("ipc2023-learning", SUITE_IPC_LEARNING),
        ("autoscale-benchmarks-main/21.11-optimal-strips", SUITE_AUTOSCALE_OPTIMAL_STRIPS),
        ("htg-domains/flat", SUITE_HTG),
        ("ipc2023-numeric", SUITE_IPC2023_NUMERIC),
        ("pushworld", SUITE_PUSHWORLD),
        ("beluga2025", SUITE_BELUGA2025_SCALABILITY_DETERMINISTIC),
        ("mine-pddl", SUITE_MINEPDDL),
        ("mine-pddl-numeric", SUITE_MINEPDDL)
    ]
    TIME_LIMIT = 10 * 60
else:
    SUITES = [
        #("cnot-synthesis", SUITE_CNOT_SYNTHESIS_TEST),
        ("downward-benchmarks", SUITE_IPC_OPTIMAL_STRIPS_TEST),
        ("downward-benchmarks", SUITE_IPC_OPTIMAL_ADL_TEST),
        #("downward-benchmarks", SUITE_IPC_SATISFICING_STRIPS_TEST),
        #("ipc2023-learning", SUITE_IPC_LEARNING_TEST),
        #("autoscale-benchmarks-main/21.11-optimal-strips", SUITE_AUTOSCALE_OPTIMAL_STRIPS_TEST),
        #("htg-domains/flat", SUITE_HTG_TEST),
        ("ipc2023-numeric", SUITE_IPC2023_NUMERIC_TEST),
        #("pushworld", SUITE_PUSHWORLD_TEST),
        #("beluga2025", SUITE_BELUGA2025_SCALABILITY_DETERMINISTIC_TEST),
        #("mine-pddl", SUITE_MINEPDDL_TEST),
        #("mine-pddl-numeric", SUITE_MINEPDDL_TEST)
    ]
    TIME_LIMIT = 5

ATTRIBUTES = [
    "run_dir",

    "num_fluent_atoms",
    "num_derived_atoms",
    "num_ground_actions",
    "num_ground_axioms",
    "total_task_grounding_time",

    "init_total_time_min",
    "init_total_time_max",
    "init_total_time_median",
    "ground_total_time_min",
    "ground_total_time_max",
    "ground_total_time_median",
    "num_rules",
    "ground_seq_total_time",
    "merge_seq_total_time",
    Attribute("parallel_fraction", function=geometric_mean, min_wins=False),
    Attribute("coverage", min_wins=False),
]

MEMORY_LIMIT = 8000

# Create a new experiment.
exp = Experiment(environment=ENV)
exp.add_parser(GroundTaskParser())

PLANNER_DIR = REPO / "build" / "exe" / "ground_task"

exp.add_resource("planner_exe", PLANNER_DIR)

for prefix, SUITE in SUITES:
    for task in suites.build_suite(BENCHMARKS_DIR / prefix, SUITE):
            ################ Grounded ################
            run = exp.add_run()
            run.add_resource("domain", task.domain_file, symlink=True)
            run.add_resource("problem", task.problem_file, symlink=True)

            run.add_command(
                f"ground_task-{NUM_THREADS}",
                [
                    "./{planner_exe}", 
                    "-D",
                    "{domain}", 
                    "-P",
                    "{problem}", 
                    "-N", 
                    str(NUM_THREADS)
                ],
                TIME_LIMIT,
                MEMORY_LIMIT,
            )
            # AbsoluteReport needs the following properties:
            # 'domain', 'problem', 'algorithm', 'coverage'.
            run.set_property("domain", task.domain)
            run.set_property("problem", task.problem)
            run.set_property("algorithm", f"ground_task-{NUM_THREADS}")
            # BaseReport needs the following properties:
            # 'time_limit', 'memory_limit'.
            run.set_property("time_limit", TIME_LIMIT)
            run.set_property("memory_limit", MEMORY_LIMIT)
            # Every run has to have a unique id in the form of a list.
            # The algorithm name is only really needed when there are
            # multiple algorithms.
            run.set_property("id", [f"ground_task-{NUM_THREADS}", task.domain, task.problem])

# Add step that writes experiment files to disk.
exp.add_step("build", exp.build)

# Add step that executes all runs.
exp.add_step("start", exp.start_runs)

exp.add_step("parse", exp.parse)

# Add step that collects properties from run directories and
# writes them to *-eval/properties.
exp.add_fetcher(name="fetch")

# Make a report.
exp.add_report(BaseReport(attributes=ATTRIBUTES), outfile="report.html")

# Parse the commandline and run the specified steps.
exp.run_steps()