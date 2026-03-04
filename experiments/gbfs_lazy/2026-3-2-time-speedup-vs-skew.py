#! /usr/bin/env python

import matplotlib.pyplot as plt
import json

from collections import defaultdict

import matplotlib.pyplot as plt
import numpy as np

MAX_MEMORY_MB = 20_000_000_000
MAX_AVG_NUM_STATE_VARIABLES = 200_000_000

def main():
    with open("results-combined/2026-1-8-gbfs_lazy-htg-tyr-pl-fd-eval/properties", 'r') as file:
    # with open("results-combined/2026-1-8-gbfs_lazy-tyr-pl-fd-eval/properties", 'r') as file:
    # with open("data/2026-1-8-gbfs_lazy-profiling-classical-1-eval/properties", 'r') as file:
        data = json.load(file)

        task_to_runs = defaultdict(list)
        for task, run in data.items():
            task_to_runs[tuple(run["id"][1:])].append(run)

        dfs = []

        per_domain_items = defaultdict(set)

        for task, runs in task_to_runs.items():
            domain = task[0]

            run_1 = None
            run_8 = None
            for run in runs:
                if run["algorithm"] == "gbfs-lazy-hff-pref-ff-1":
                    if run["coverage"] == 0:
                        continue
                    if run["total_time_s"] < 6:
                        continue

                    total_time_ms = run["total_time_ns"] / 1_000_000
                    axiom_prog_t_tot_ms = run["axiom_prog_t_tot_ms"]
                    succgen_prog_t_tot_ms = run["succgen_prog_t_tot_ms"]
                    ff_prog_t_tot_ms = run["ff_prog_t_tot_ms"]

                    df = (axiom_prog_t_tot_ms + succgen_prog_t_tot_ms + ff_prog_t_tot_ms) / total_time_ms
                    if df < 0.5:
                        continue
                    run_1 = run
                if run["algorithm"] == "gbfs-lazy-hff-pref-ff-8":
                    run_8 = run

            if run_1 is None or run_1["coverage"] == 0 or run_8["coverage"] == 0:
                continue


            speedup = run_1["total_time_ns"] / run_8["total_time_ns"]
            skew = run_1["ff_rule_t_tot_ms"] / (run_1["ff_rule_t_avg_us"] / 1000)

            per_domain_items[task[0]].add((skew, speedup))

        for domain, items in per_domain_items.items():
            print(domain)
            for item in items:
                print(f"({item[0]}, {item[1]})", end=" ")
            print()





if __name__ == "__main__":
    main()