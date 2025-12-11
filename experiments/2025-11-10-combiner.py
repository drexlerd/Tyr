#! /usr/bin/env python

from lab.experiment import Experiment
from downward.reports.absolute import AbsoluteReport

from lab.reports import Attribute, geometric_mean

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

exp = Experiment("2025-11-10-ground_task-combined")

exp.add_fetcher("data/2025-11-10-ground_task-1-eval")
exp.add_fetcher("data/2025-11-10-ground_task-2-eval")
exp.add_fetcher("data/2025-11-10-ground_task-4-eval")
exp.add_fetcher("data/2025-11-10-ground_task-8-eval")

exp.add_report(BaseReport(attributes=ATTRIBUTES))

exp.run_steps()
