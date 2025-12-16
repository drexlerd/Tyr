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
    "peak_memory_usage",
    
    Attribute("parallel_fraction", function=geometric_mean, min_wins=False),
    Attribute("coverage", min_wins=False),
]

exp = Experiment("6_7")

def rename_algorithm(properties):
    """Rename algorithm dynamically during fetching."""
    if properties["algorithm"] == "ground_task-1":
        properties["algorithm"] = "old-ground_task-1"
        properties["id"][0] = "old-ground_task-1"
    elif properties["algorithm"] == "ground_task-2":
        properties["algorithm"] = "old-ground_task-2"
        properties["id"][0] = "old-ground_task-2"
    elif properties["algorithm"] == "ground_task-4":
        properties["algorithm"] = "old-ground_task-4"
        properties["id"][0] = "old-ground_task-4"
    elif properties["algorithm"] == "ground_task-8":
        properties["algorithm"] = "old-ground_task-8"
        properties["id"][0] = "old-ground_task-8"
    return properties

exp.add_fetcher("../6-2025-11-10-ground_task-combined-033ced75ce2d3d8c85122119f77de42b3a94c99a-eval", filter=rename_algorithm)
exp.add_fetcher("../7-2025-11-10-ground_task-combined-4ef213ef7c18932f70adea037d0b12753605f0ea-eval")

exp.add_report(BaseReport(attributes=ATTRIBUTES, filter_algorithm=["old-ground_task-1", "ground_task-1", "old-ground_task-2", "ground_task-2", "old-ground_task-4", "ground_task-4", "old-ground_task-8", "ground_task-8"]))

exp.run_steps()
