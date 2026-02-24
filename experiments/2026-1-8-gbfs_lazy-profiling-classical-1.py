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
from lab.reports import Attribute, geometric_mean, arithmetic_mean

DIR = Path(__file__).resolve().parent
REPO = DIR.parent

sys.path.append(str(DIR.parent))

from experiments.parser_datalog import DatalogParser
from experiments.parser_search import SearchParser

from suite import SUITE_CNOT_SYNTHESIS, SUITE_IPC_OPTIMAL_STRIPS, SUITE_IPC_OPTIMAL_ADL, SUITE_IPC_SATISFICING_STRIPS, SUITE_IPC_LEARNING, SUITE_AUTOSCALE_OPTIMAL_STRIPS, SUITE_HTG, SUITE_IPC2023_NUMERIC, SUITE_PUSHWORLD, SUITE_BELUGA2025_SCALABILITY_DETERMINISTIC, SUITE_MINEPDDL, SUITE_IPC_SATISFICING_ADL
from suite_test import SUITE_CNOT_SYNTHESIS_TEST, SUITE_IPC_OPTIMAL_STRIPS_TEST, SUITE_IPC_OPTIMAL_ADL_TEST, SUITE_IPC_SATISFICING_STRIPS_TEST, SUITE_IPC_LEARNING_TEST, SUITE_AUTOSCALE_OPTIMAL_STRIPS_TEST, SUITE_HTG_TEST, SUITE_IPC2023_NUMERIC_TEST, SUITE_PUSHWORLD_TEST, SUITE_BELUGA2025_SCALABILITY_DETERMINISTIC_TEST, SUITE_MINEPDDL_TEST, SUITE_IPC_SATISFICING_ADL_TEST

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

NUM_THREADS = 1

if REMOTE:
    ENV = TetralithEnvironment(
        setup=TetralithEnvironment.DEFAULT_SETUP,
        memory_per_cpu="2840M",
        cpus_per_task=2,  # 2*2840 >= 5000
        extra_options="#SBATCH --account=naiss2025-22-1245")
    
else:
    ENV = LocalEnvironment(processes=6)

if REMOTE:
    SUITES = [
        #("cnot-synthesis", SUITE_CNOT_SYNTHESIS),
        #("downward-benchmarks", SUITE_IPC_OPTIMAL_STRIPS),
        #("downward-benchmarks", SUITE_IPC_OPTIMAL_ADL),
        ("downward-benchmarks", SUITE_IPC_SATISFICING_STRIPS),
        ("downward-benchmarks", SUITE_IPC_SATISFICING_ADL),
        #("ipc2023-learning", SUITE_IPC_LEARNING),
        #("autoscale-benchmarks-main/21.11-optimal-strips", SUITE_AUTOSCALE_OPTIMAL_STRIPS),
        ("htg-domains/flat", SUITE_HTG),
        #("pushworld", SUITE_PUSHWORLD),
        #("beluga2025", SUITE_BELUGA2025_SCALABILITY_DETERMINISTIC),
        #("mine-pddl", SUITE_MINEPDDL),
    ]
    WALL_TIME_LIMIT = 5 * 60
else:
    SUITES = [
        # ("downward-benchmarks", ["gripper:prob01.pddl"]), 
        #("cnot-synthesis", SUITE_CNOT_SYNTHESIS_TEST),
        #("downward-benchmarks", SUITE_IPC_OPTIMAL_STRIPS_TEST),
        #("downward-benchmarks", SUITE_IPC_OPTIMAL_ADL_TEST),
        ("downward-benchmarks", SUITE_IPC_SATISFICING_STRIPS_TEST),
        ("downward-benchmarks", SUITE_IPC_SATISFICING_ADL_TEST),
        #("ipc2023-learning", SUITE_IPC_LEARNING_TEST),
        #("autoscale-benchmarks-main/21.11-optimal-strips", SUITE_AUTOSCALE_OPTIMAL_STRIPS_TEST),
        #("htg-domains/flat", SUITE_HTG_TEST),
        #("pushworld", SUITE_PUSHWORLD_TEST),
        #("beluga2025", SUITE_BELUGA2025_SCALABILITY_DETERMINISTIC_TEST),
        #("mine-pddl", SUITE_MINEPDDL_TEST),
    ]
    WALL_TIME_LIMIT = 1

ATTRIBUTES = [
    "run_dir",

    "num_objects",

    Attribute("coverage", min_wins=False),
    # GBFS Lazy
    "cost",
    "length",
    "unsolvable",

    # VAL
    "invalid",

    # Search
    "initial_h_value",
    Attribute("search_time", function=geometric_mean),
    "num_expanded",
    "num_generated",
    Attribute("search_time_us_per_expanded", function=geometric_mean),

    # Total
    Attribute("total_time", function=geometric_mean),
    "memory",

    "succgen_prog_n_exec",
    "succgen_prog_t_seq",
    "succgen_prog_t_par",
    "succgen_prog_t_tot",
    "succgen_prog_t_avg",
    Attribute("succgen_prog_pf", function=geometric_mean, min_wins=False),

    "succgen_rule_n_exec",
    "succgen_rule_n_samples",
    "succgen_rule_t_seq",
    "succgen_rule_t_par",
    "succgen_rule_t_tot",
    "succgen_rule_t_avg",
    Attribute("succgen_rule_pf", function=geometric_mean, min_wins=False),
    "succgen_rule_skew_tot",
    "succgen_rule_skew_avg",

    "succgen_rule_worker_n_exec",
    "succgen_rule_worker_n_gen",
    "succgen_rule_worker_n_pen",
    Attribute("succgen_rule_worker_oa", function=geometric_mean, min_wins=False),

    "axiom_prog_n_exec",
    "axiom_prog_t_seq",
    "axiom_prog_t_par",
    "axiom_prog_t_tot",
    "axiom_prog_t_avg",
    Attribute("axiom_prog_pf", function=geometric_mean, min_wins=False),

    "axiom_rule_n_exec",
    "axiom_rule_n_samples",
    "axiom_rule_t_seq",
    "axiom_rule_t_par",
    "axiom_rule_t_tot",
    "axiom_rule_t_avg",
    Attribute("axiom_rule_pf", function=geometric_mean, min_wins=False),
    "axiom_rule_skew_tot",
    "axiom_rule_skew_avg",

    "axiom_rule_worker_n_exec",
    "axiom_rule_worker_n_gen",
    "axiom_rule_worker_n_pen",
    Attribute("axiom_rule_worker_oa", function=geometric_mean, min_wins=False),

    "ff_prog_n_exec",
    "ff_prog_t_seq",
    "ff_prog_t_par",
    "ff_prog_t_tot",
    "ff_prog_t_avg",
    Attribute("ff_prog_pf", function=geometric_mean, min_wins=False),

    "ff_rule_n_exec",
    "ff_rule_n_samples",
    "ff_rule_t_seq",
    "ff_rule_t_par",
    "ff_rule_t_tot",
    "ff_rule_t_avg",
    Attribute("ff_rule_pf", function=geometric_mean, min_wins=False),
    "ff_rule_skew_tot",
    "ff_rule_skew_avg",
    
    "ff_rule_worker_n_exec",
    "ff_rule_worker_n_gen",
    "ff_rule_worker_n_pen",
    Attribute("ff_rule_worker_oa", function=geometric_mean, min_wins=False),

]

MEMORY_LIMIT = 5000

# Create a new experiment.
exp = Experiment(environment=ENV)
exp.add_parser(SearchParser())
exp.add_parser(DatalogParser())

PLANNER_DIR = REPO / "build" / "exe" / "gbfs_lazy"

exp.add_resource("planner_exe", PLANNER_DIR)
exp.add_resource("run_planner", DIR / "gbfs_lazy.sh")

for prefix, SUITE in SUITES:
    for task in suites.build_suite(BENCHMARKS_DIR / prefix, SUITE):
            ################ Grounded ################
            run = exp.add_run()
            run.add_resource("domain", task.domain_file, symlink=True)
            run.add_resource("problem", task.problem_file, symlink=True)

            run.add_command(
                f"gbfs-lazy-ff-{NUM_THREADS}",
                [
                    "{run_planner}", 
                    "{planner_exe}", 
                    "{domain}", 
                    "{problem}",
                    "plan.out",
                    str(NUM_THREADS)
                ],
                time_limit=None,
                wall_time_limit=WALL_TIME_LIMIT,
                memory_limit=MEMORY_LIMIT,
            )
            # AbsoluteReport needs the following properties:
            # 'domain', 'problem', 'algorithm', 'coverage'.
            run.set_property("domain", task.domain)
            run.set_property("problem", task.problem)
            run.set_property("algorithm", f"gbfs-lazy-ff-{NUM_THREADS}")
            # BaseReport needs the following properties:
            # 'time_limit', 'memory_limit'.
            run.set_property("wall_time_limit", WALL_TIME_LIMIT)
            run.set_property("memory_limit", MEMORY_LIMIT)
            # Every run has to have a unique id in the form of a list.
            # The algorithm name is only really needed when there are
            # multiple algorithms.
            run.set_property("id", [f"gbfs-lazy-ff-{NUM_THREADS}", task.domain, task.problem])

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