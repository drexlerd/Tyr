#! /usr/bin/env python

import sys 

from pathlib import Path

from lab.experiment import Experiment
from downward.reports.absolute import AbsoluteReport

from lab.reports import Attribute, geometric_mean

DIR = Path(__file__).resolve().parent
REPO = DIR.parent.parent

sys.path.append(str(DIR.parent.parent))

from experiments.parser_datalog import DatalogParser
from experiments.parser_search import SearchParser


# Create custom report class with suitable info and error attributes.
class BaseReport(AbsoluteReport):
    INFO_ATTRIBUTES = ["wall_time_limit", "memory_limit"]
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
]
ATTRIBUTES += SearchParser.get_attributes()
ATTRIBUTES += DatalogParser.get_attributes()

# 16 is baseline
exp = Experiment("gbfs_combine_tyr_4")


def rename_algorithm(properties):
    """Rename algorithm dynamically during fetching."""
    if properties["algorithm"] == "gbfs-lazy-hff-pref-ff-1":
        properties["algorithm"] = "old-gbfs-lazy-hff-pref-ff-1"
        properties["id"][0] = "old-gbfs-lazy-hff-pref-ff-1"
    elif properties["algorithm"] == "gbfs-lazy-hff-pref-ff-2":
        properties["algorithm"] = "old-gbfs-lazy-hff-pref-ff-2"
        properties["id"][0] = "old-gbfs-lazy-hff-pref-ff-2"
    elif properties["algorithm"] == "gbfs-lazy-hff-pref-ff-4":
        properties["algorithm"] = "old-gbfs-lazy-hff-pref-ff-4"
        properties["id"][0] = "old-gbfs-lazy-hff-pref-ff-4"
    elif properties["algorithm"] == "gbfs-lazy-hff-pref-ff-8":
        properties["algorithm"] = "old-gbfs-lazy-hff-pref-ff-8"
        properties["id"][0] = "old-gbfs-lazy-hff-pref-ff-8"
    return properties



# exp.add_fetcher("../../3-2026-1-8-gbfs_lazy-profiling-classical-1-eval", filter=rename_algorithm)
# exp.add_fetcher("../../4-2026-1-8-gbfs_lazy-profiling-classical-1-eval")

exp.add_fetcher("results_raw/tyr-2026-1-8-gbfs_lazy-combined-eval", filter=rename_algorithm)
exp.add_fetcher("results_raw/4-2026-1-8-gbfs_lazy-1-eval")

exp.add_report(BaseReport(attributes=ATTRIBUTES, filter_algorithm=[
    "gbfs-lazy-ff-1",
    "old-gbfs-lazy-hff-pref-ff-1", 
    "gbfs-lazy-hff-pref-ff-1", 
    "old-gbfs-lazy-hff-pref-ff-2", 
    "gbfs-lazy-hff-pref-ff-2", 
    "old-gbfs-lazy-hff-pref-ff-4", 
    "gbfs-lazy-hff-pref-ff-4", 
    "old-gbfs-lazy-hff-pref-ff-8", 
    "gbfs-lazy-hff-pref-ff-8"]))

exp.run_steps()
