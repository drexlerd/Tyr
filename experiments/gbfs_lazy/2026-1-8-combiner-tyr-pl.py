#! /usr/bin/env python

import sys 

from pathlib import Path

from lab.experiment import Experiment
from downward.reports.absolute import AbsoluteReport

from lab.reports import Attribute, geometric_mean

DIR = Path(__file__).resolve().parent
REPO = DIR.parent.parent

sys.path.append(str(DIR.parent.parent))

from experiments.suite import SUITE_IPC_SATISFICING_ADL, SUITE_IPC_SATISFICING_STRIPS, SUITE_HTG, SUITE_AUTOSCALE_AGILE_STRIPS

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
]


UNSUPPORTED_DOMAINS = [
    # Powerlifted: Actions with negated preconditions not supported yet
    "data-network-sat18-strips",
    "pathways",
    "snake-sat18-strips",
    "termes-sat18-strips",
    "tetris-sat14-strips",
    "tidybot-sat11-strips",
    "data-network",
    "snake",
    "termes",
    "tetris",
    "tidybot",
    # Powerlifted: Derived predicates are not supported.
    "optical-telegraphs",
    "philosophers",
    "psr-large",
    "psr-middle",
    # Powerlifted: PDDL feature "when" not supported yet.
    "schedule",
    "spider-sat18-strips",
    # Powerlifted: PDDL feature "imply" not supported yet.
    "miconic-fulladl",
    # Powerlifted: PDDL feature "forall" not supported yet.
    "assembly",
    "caldera-sat18-adl",
    "caldera-split-sat18-adl",
    "citycar-sat14-adl",
    "flashfill-sat18-adl",
    "genome-edit-distance-positional",
    "maintenance-sat14-adl",
    "miconic-simpleadl",
    "nurikabe-sat18-adl",
    "openstacks",
    "openstacks-sat08-adl",
    "settlers-sat18-adl",
    "trucks",
    # Powerlifted: assert (not isinstance(arg.type_name, list))
    "storage",
    "zenotravel",
    # Powerlifted: requirements = pddl.Requirements(opt[1:])
    "cavediving-14-adl",
]

def exclude_domains(run, excluded_domains) -> bool:
    domain = run.get("domain")
    return domain not in excluded_domains

def merge_domain(run, name) -> bool:
    dom = run.get("domain", "")
    if dom.startswith(name):
        prob = run.get("problem", "")
        run["problem"] = f"{dom}_{prob}"
        run["domain"] = name
        if "id" in run and isinstance(run["id"], list):
            if len(run["id"]) > 1:
                run["id"][1] = name
            if len(run["id"]) > 2:
                run["id"][2] = run["problem"]

def merge_domains(run) -> bool:
    merge_domain(run, "childsnack")
    merge_domain(run, "visitall")
    merge_domain(run, "genome-edit-distance")
    merge_domain(run, "organic-synthesis")
    merge_domain(run, "blocksworld")
    merge_domain(run, "logistics")

    return True

def normalize_htg(run):
    if not exclude_domains(run, UNSUPPORTED_DOMAINS+ SUITE_AUTOSCALE_AGILE_STRIPS):
        return False  
    merge_domains(run)
    return True

def normalize_autoscale(run):
    if not exclude_domains(run, UNSUPPORTED_DOMAINS + SUITE_HTG):
        return False
    merge_domains(run)
    return True

def normalize(run):  
    if not exclude_domains(run, UNSUPPORTED_DOMAINS):
        return False 
    merge_domains(run)
    return True

# exp = Experiment("2026-1-8-gbfs_lazy-profiling-tyr-pl-combined")
exp_htg = Experiment("2026-1-8-gbfs_lazy-htg-tyr-pl-fd")
exp_htg.add_fetcher("results_raw/pl-2026-1-9-lazy-gbfs-ff-pref-ff-eval", filter=normalize_htg)
exp_htg.add_fetcher("results_raw/3-2026-1-8-gbfs_lazy-combined-eval", filter=normalize_htg)
exp_htg.add_fetcher("results_raw/fd-2026-2-28-gbfs-lazy-ff-pref-ff-eval", filter=normalize_htg)
exp_htg.add_report(BaseReport(attributes=ATTRIBUTES, filter_algorithm=["downward-gbfs-lazy-hff-pref-ff", "powerlifted-gbfs-lazy-hff-pref-ff", "gbfs-lazy-hff-pref-ff-1", "gbfs-lazy-hff-pref-ff-2", "gbfs-lazy-hff-pref-ff-4", "gbfs-lazy-hff-pref-ff-8"]))
exp_htg.run_steps()

exp_autoscale = Experiment("2026-1-8-gbfs_lazy-autoscale-tyr-pl-fd")
exp_autoscale.add_fetcher("results_raw/pl-2026-1-9-lazy-gbfs-ff-pref-ff-eval", filter=normalize_htg)
exp_autoscale.add_fetcher("results_raw/3-2026-1-8-gbfs_lazy-combined-eval", filter=normalize_htg)
exp_autoscale.add_fetcher("results_raw/fd-2026-2-28-gbfs-lazy-ff-pref-ff-eval", filter=normalize_htg)
exp_autoscale.add_report(BaseReport(attributes=ATTRIBUTES, filter_algorithm=["downward-gbfs-lazy-hff-pref-ff", "powerlifted-gbfs-lazy-hff-pref-ff", "gbfs-lazy-hff-pref-ff-1", "gbfs-lazy-hff-pref-ff-2", "gbfs-lazy-hff-pref-ff-4", "gbfs-lazy-hff-pref-ff-8"]))
exp_autoscale.run_steps()

exp = Experiment("2026-1-8-gbfs_lazy-tyr-pl-fd")
exp.add_fetcher("results_raw/pl-2026-1-9-lazy-gbfs-ff-pref-ff-eval", filter=normalize_htg)
exp.add_fetcher("results_raw/3-2026-1-8-gbfs_lazy-combined-eval", filter=normalize_htg)
exp.add_fetcher("results_raw/fd-2026-2-28-gbfs-lazy-ff-pref-ff-eval", filter=normalize_htg)
exp.add_report(BaseReport(attributes=ATTRIBUTES, filter_algorithm=["downward-gbfs-lazy-hff-pref-ff", "powerlifted-gbfs-lazy-hff-pref-ff", "gbfs-lazy-hff-pref-ff-1", "gbfs-lazy-hff-pref-ff-2", "gbfs-lazy-hff-pref-ff-4", "gbfs-lazy-hff-pref-ff-8"]))
exp.run_steps()