#! /usr/bin/env python

import sys 

from pathlib import Path

from lab.experiment import Experiment
from downward.reports.absolute import AbsoluteReport

from lab.reports import Attribute, geometric_mean

DIR = Path(__file__).resolve().parent
REPO = DIR.parent.parent

sys.path.append(str(DIR.parent.parent))

from experiments.suite import SUITE_IPC_SATISFICING_ADL, SUITE_IPC_SATISFICING_STRIPS

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


EXCLUDED_DOMAINS = [
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
    # Powerlifted: requirements = pddl.Requirements(opt[1:])
    "cavediving-14-adl",
]

# EXCLUDED_DOMAINS += SUITE_IPC_SATISFICING_ADL
# EXCLUDED_DOMAINS += SUITE_IPC_SATISFICING_STRIPS

def exclude_domains(run) -> bool:
    domain = run.get("domain")
    return domain not in EXCLUDED_DOMAINS

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

def normalize(run):
    if not exclude_domains(run):
        return False
    
    # merge_domain(run, "childsnack")
    # merge_domain(run, "visitall")
    # merge_domain(run, "genome-edit-distance")
    # merge_domain(run, "organic-synthesis")
    # merge_domain(run, "blocksworld")
    # merge_domain(run, "logistics")

    return True

exp = Experiment("2026-1-8-gbfs_lazy-profiling-tyr-pl-combined")
# exp = Experiment("2026-1-8-gbfs_lazy-tyr-pl-combined")

exp.add_fetcher("tmp/2026-1-9-lazy-gbfs-ff-pref-ff-profiling-eval", filter=normalize)
exp.add_fetcher("tmp/8-2026-1-8-gbfs_lazy-profiling-classical-combined-eval", filter=normalize)

# exp.add_fetcher("results/pl-2026-1-9-lazy-gbfs-ff-pref-ff-eval", filter=normalize)
# exp.add_fetcher("results/1-2026-1-8-gbfs_lazy-combined-eval", filter=normalize)



exp.add_report(BaseReport(attributes=ATTRIBUTES))

exp.run_steps()
