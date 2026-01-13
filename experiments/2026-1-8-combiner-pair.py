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
    "plan_cost",
    "plan_length",

    # Search
    "search_time_ms",
    "expansions",
    "generated",

    # Total
    "total_time_ms",
    "peak_memory_usage_bytes",

    # Datalog
    Attribute("axiom_par_frac", function=geometric_mean, min_wins=False),
    "axiom_par_ms",
    "axiom_rule_samples",
    Attribute("axiom_rule_tot_skew", function=geometric_mean, min_wins=False),
    "axiom_rule_tot_max_ms",
    "axiom_rule_tot_med_ms",
    "axiom_rule_tot_min_ms",
    Attribute("axiom_rule_avg_skew", function=geometric_mean, min_wins=False),
    "axiom_rule_avg_max_ns",
    "axiom_rule_avg_med_ns",
    "axiom_rule_avg_min_ns",
    "axiom_seq_out_ms",
    "axiom_total_ms",

    Attribute("ff_par_frac", function=geometric_mean, min_wins=False),
    "ff_par_ms",
    "ff_rule_samples",
    Attribute("ff_rule_tot_skew", function=geometric_mean, min_wins=False),
    "ff_rule_tot_max_ms",
    "ff_rule_tot_med_ms",
    "ff_rule_tot_min_ms",
    Attribute("ff_rule_avg_skew", function=geometric_mean, min_wins=False),
    "ff_rule_avg_max_ns",
    "ff_rule_avg_med_ns",
    "ff_rule_avg_min_ns",
    "ff_seq_out_ms",
    "ff_total_ms",

    Attribute("succgen_par_frac", function=geometric_mean, min_wins=False),
    "succgen_par_ms",
    "succgen_rule_samples",
    Attribute("succgen_rule_tot_skew", function=geometric_mean, min_wins=False),
    "succgen_rule_tot_max_ms",
    "succgen_rule_tot_med_ms",
    "succgen_rule_tot_min_ms",
    Attribute("succgen_rule_avg_skew", function=geometric_mean, min_wins=False),
    "succgen_rule_avg_max_ns",
    "succgen_rule_avg_med_ns",
    "succgen_rule_avg_min_ns",
    "succgen_seq_out_ms",
    "succgen_total_ms",
]

exp = Experiment("gbfs_combine_2_3")

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

exp.add_fetcher("../proj-delta-2026-1-8-gbfs_lazy-1-eval", filter=rename_algorithm)
exp.add_fetcher("../proj-delta-pending-2026-1-8-gbfs_lazy-1-eval")

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
