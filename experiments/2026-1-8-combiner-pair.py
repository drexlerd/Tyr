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
    Attribute("search_time_per_expanded", function=geometric_mean),

    # Total
    Attribute("total_time", function=geometric_mean),
    "memory",

    # Datalog
    Attribute("axiom_prog_par_frac", function=geometric_mean, min_wins=False),
    "axiom_prog_num_exec",
    "axiom_prog_par_ms",
    "axiom_prog_T_total_ms",
    "axiom_prog_T_avg_us",
    "axiom_rule_samples",
    "axiom_rule_T_init_ms",
    "axiom_rule_T_generate_ms",
    "axiom_rule_T_pending_ms",
    "axiom_rule_T_total_ms",
    Attribute("axiom_rule_par_frac", function=geometric_mean, min_wins=False),
    Attribute("axiom_rule_total_skew", function=geometric_mean, min_wins=False),
    Attribute("axiom_rule_avg_skew", function=geometric_mean, min_wins=False),

    Attribute("ff_prog_par_frac", function=geometric_mean, min_wins=False),
    "ff_prog_num_exec",
    "ff_prog_par_ms",
    "ff_prog_T_total_ms",
    "ff_prog_T_avg_us",
    "ff_rule_samples",
    "ff_rule_T_init_ms",
    "ff_rule_T_generate_ms",
    "ff_rule_T_pending_ms",
    "ff_rule_T_total_ms",
    Attribute("ff_rule_par_frac", function=geometric_mean, min_wins=False),
    Attribute("ff_rule_total_skew", function=geometric_mean, min_wins=False),
    Attribute("ff_rule_avg_skew", function=geometric_mean, min_wins=False),

    Attribute("succgen_prog_par_frac", function=geometric_mean, min_wins=False),
    "succgen_prog_num_exec",
    "succgen_prog_par_ms",
    "succgen_prog_T_total_ms",
    "succgen_prog_T_avg_us",
    "succgen_rule_samples",
    "succgen_rule_T_init_ms",
    "succgen_rule_T_generate_ms",
    "succgen_rule_T_pending_ms",
    "succgen_rule_T_total_ms",
    Attribute("succgen_rule_par_frac", function=geometric_mean, min_wins=False),
    Attribute("succgen_rule_total_skew", function=geometric_mean, min_wins=False),
    Attribute("succgen_rule_avg_skew", function=geometric_mean, min_wins=False),
]

# 16 is baseline
exp = Experiment("gbfs_combine_21_22")

def rename_algorithm(properties):
    """Rename algorithm dynamically during fetching."""
    if properties["algorithm"] == "gbfs-lazy-ff-1":
        properties["algorithm"] = "old-gbfs-lazy-ff-1"
        properties["id"][0] = "old-gbfs-lazy-ff-1"
    elif properties["algorithm"] == "gbfs-lazy-ff-2":
        properties["algorithm"] = "old-gbfs-lazy-ff-2"
        properties["id"][0] = "old-gbfs-lazy-ff-2"
    elif properties["algorithm"] == "gbfs-lazy-ff-4":
        properties["algorithm"] = "old-gbfs-lazy-ff-4"
        properties["id"][0] = "old-gbfs-lazy-ff-4"
    elif properties["algorithm"] == "gbfs-lazy-ff-8":
        properties["algorithm"] = "old-gbfs-lazy-ff-8"
        properties["id"][0] = "old-gbfs-lazy-ff-8"
    return properties

exp.add_fetcher("../21-2026-1-8-gbfs_lazy-profiling-classical-combined-eval", filter=rename_algorithm)
exp.add_fetcher("../22-2026-1-8-gbfs_lazy-profiling-classical-combined-eval")

exp.add_report(BaseReport(attributes=ATTRIBUTES, filter_algorithm=[
    "old-gbfs-lazy-ff-1", 
    "gbfs-lazy-ff-1", 
    "old-gbfs-lazy-ff-2", 
    "gbfs-lazy-ff-2", 
    "old-gbfs-lazy-ff-4", 
    "gbfs-lazy-ff-4", 
    "old-gbfs-lazy-ff-8", 
    "gbfs-lazy-ff-8"]))

exp.run_steps()
