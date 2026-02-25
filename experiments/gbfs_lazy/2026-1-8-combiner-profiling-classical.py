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

exp = Experiment("2026-1-8-gbfs_lazy-profiling-classical-combined")

exp.add_fetcher("data/2026-1-8-gbfs_lazy-profiling-classical-1-eval")
exp.add_fetcher("data/2026-1-8-gbfs_lazy-profiling-classical-2-eval")
exp.add_fetcher("data/2026-1-8-gbfs_lazy-profiling-classical-4-eval")
exp.add_fetcher("data/2026-1-8-gbfs_lazy-profiling-classical-8-eval")

exp.add_report(BaseReport(attributes=ATTRIBUTES))

exp.run_steps()
