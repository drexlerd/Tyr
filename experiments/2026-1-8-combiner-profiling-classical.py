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
    Attribute("axiom_par_frac", function=geometric_mean, min_wins=False),
    "axiom_num_exec",
    "axiom_par_ms",
    "axiom_total_ms",
    "axiom_avg_us",
    "axiom_rule_samples",
    "axiom_rule_T_initialize_delta_kpkc_ms",
    "axiom_rule_T_process_generate_ms",
    "axiom_rule_T_generate_clique_ms",
    "axiom_rule_T_process_pending_ms",
    "axiom_rule_T_process_clique_ms",
    "axiom_rule_T_total_ms",
    Attribute("axiom_rule_total_skew", function=geometric_mean, min_wins=False),
    "axiom_rule_total_max_ms",
    "axiom_rule_total_med_ms",
    "axiom_rule_total_min_ms",
    Attribute("axiom_rule_avg_skew", function=geometric_mean, min_wins=False),
    "axiom_rule_avg_max_ms",
    "axiom_rule_avg_med_ms",
    "axiom_rule_avg_min_ms",


    Attribute("ff_par_frac", function=geometric_mean, min_wins=False),
    "ff_num_exec",
    "ff_par_ms",
    "ff_total_ms",
    "ff_avg_us",
    "ff_rule_samples",
    "ff_rule_T_initialize_delta_kpkc_ms",
    "ff_rule_T_process_generate_ms",
    "ff_rule_T_generate_clique_ms",
    "ff_rule_T_process_pending_ms",
    "ff_rule_T_process_clique_ms",
    "ff_rule_T_total_ms",
    Attribute("ff_rule_total_skew", function=geometric_mean, min_wins=False),
    "ff_rule_total_max_ms",
    "ff_rule_total_med_ms",
    "ff_rule_total_min_ms",
    Attribute("ff_rule_avg_skew", function=geometric_mean, min_wins=False),
    "ff_rule_avg_max_ms",
    "ff_rule_avg_med_ms",
    "ff_rule_avg_min_ms",


    Attribute("succgen_par_frac", function=geometric_mean, min_wins=False),
    "succgen_num_exec",
    "succgen_par_ms",
    "succgen_total_ms",
    "succgen_avg_us",
    "succgen_rule_samples",
    "succgen_rule_T_initialize_delta_kpkc_ms",
    "succgen_rule_T_process_generate_ms",
    "succgen_rule_T_generate_clique_ms",
    "succgen_rule_T_process_pending_ms",
    "succgen_rule_T_process_clique_ms",
    "succgen_rule_T_total_ms",
    Attribute("succgen_rule_total_skew", function=geometric_mean, min_wins=False),
    "succgen_rule_total_max_ms",
    "succgen_rule_total_med_ms",
    "succgen_rule_total_min_ms",
    Attribute("succgen_rule_avg_skew", function=geometric_mean, min_wins=False),
    "succgen_rule_avg_max_ms",
    "succgen_rule_avg_med_ms",
    "succgen_rule_avg_min_ms",
]

exp = Experiment("2026-1-8-gbfs_lazy-profiling-classical-combined")

exp.add_fetcher("data/2026-1-8-gbfs_lazy-profiling-classical-1-eval")
exp.add_fetcher("data/2026-1-8-gbfs_lazy-profiling-classical-2-eval")
exp.add_fetcher("data/2026-1-8-gbfs_lazy-profiling-classical-4-eval")
exp.add_fetcher("data/2026-1-8-gbfs_lazy-profiling-classical-8-eval")

exp.add_report(BaseReport(attributes=ATTRIBUTES))

exp.run_steps()
