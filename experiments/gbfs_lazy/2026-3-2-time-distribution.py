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

        sum_runs = 0
        sum_total = 0
        sum_preprocessing = 0
        sum_axiom_rule_t_par = 0
        sum_axiom_rule_t_tot = 0
        sum_axiom_t_par = 0
        sum_axiom_t_tot = 0
        sum_succgen_rule_t_par = 0
        sum_succgen_rule_t_tot = 0
        sum_succgen_t_par = 0
        sum_succgen_t_tot = 0
        sum_ff_rule_t_par = 0
        sum_ff_rule_t_tot = 0
        sum_ff_t_par = 0
        sum_ff_t_tot = 0

        sum_runs_le_0_5 = 0
        sum_total_le_0_5 = 0
        sum_preprocessing_le_0_5 = 0
        sum_axiom_rule_t_par_le_0_5 = 0
        sum_axiom_rule_t_tot_le_0_5 = 0
        sum_axiom_t_par_le_0_5 = 0
        sum_axiom_t_tot_le_0_5 = 0
        sum_succgen_rule_t_par_le_0_5 = 0
        sum_succgen_rule_t_tot_le_0_5 = 0
        sum_succgen_t_par_le_0_5 = 0
        sum_succgen_t_tot_le_0_5 = 0
        sum_ff_rule_t_par_le_0_5 = 0
        sum_ff_rule_t_tot_le_0_5 = 0
        sum_ff_t_par_le_0_5 = 0
        sum_ff_t_tot_le_0_5 = 0

        sum_runs_ge_0_5 = 0
        sum_total_ge_0_5 = 0
        sum_preprocessing_ge_0_5 = 0
        sum_axiom_rule_t_par_ge_0_5 = 0
        sum_axiom_rule_t_tot_ge_0_5 = 0
        sum_axiom_t_par_ge_0_5 = 0
        sum_axiom_t_tot_ge_0_5 = 0
        sum_succgen_rule_t_par_ge_0_5 = 0
        sum_succgen_rule_t_tot_ge_0_5 = 0
        sum_succgen_t_par_ge_0_5 = 0
        sum_succgen_t_tot_ge_0_5 = 0
        sum_ff_rule_t_par_ge_0_5 = 0
        sum_ff_rule_t_tot_ge_0_5 = 0
        sum_ff_t_par_ge_0_5 = 0
        sum_ff_t_tot_ge_0_5 = 0


        dfs = []

        for task, runs in task_to_runs.items():
            for run in runs:
                if run["coverage"] == 0:
                    continue
                if run["algorithm"] != "gbfs-lazy-hff-pref-ff-1":
                    continue         

                total_time_ms = run["total_time_ns"] / 1_000_000
                preprocessing_time_ms = run["preprocessing_time_s"] * 1000
                axiom_rule_t_par_ms = run["axiom_rule_t_par_ms"]
                axiom_rule_t_tot_ms = run["axiom_rule_t_tot_ms"]
                axiom_prog_t_par_ms = run["axiom_prog_t_par_ms"]
                axiom_prog_t_tot_ms = run["axiom_prog_t_tot_ms"]
                succgen_rule_t_par_ms = run["succgen_rule_t_par_ms"]
                succgen_rule_t_tot_ms = run["succgen_rule_t_tot_ms"]
                succgen_prog_t_par_ms = run["succgen_prog_t_par_ms"]
                succgen_prog_t_tot_ms = run["succgen_prog_t_tot_ms"]
                ff_rule_t_par_ms = run["ff_rule_t_par_ms"]
                ff_rule_t_tot_ms = run["ff_rule_t_tot_ms"]
                ff_prog_t_par_ms = run["ff_prog_t_par_ms"]
                ff_prog_t_tot_ms = run["ff_prog_t_tot_ms"]

                if total_time_ms < 6000:
                    continue

                sum_runs += 1
                sum_total += total_time_ms
                sum_preprocessing += preprocessing_time_ms
                sum_axiom_rule_t_par += axiom_rule_t_par_ms
                sum_axiom_rule_t_tot += axiom_rule_t_tot_ms
                sum_axiom_t_par += axiom_prog_t_par_ms
                sum_axiom_t_tot += axiom_prog_t_tot_ms
                sum_succgen_rule_t_par += succgen_rule_t_par_ms
                sum_succgen_rule_t_tot += succgen_rule_t_tot_ms
                sum_succgen_t_par += succgen_prog_t_par_ms
                sum_succgen_t_tot += succgen_prog_t_tot_ms
                sum_ff_rule_t_par += ff_rule_t_par_ms
                sum_ff_rule_t_tot += ff_rule_t_tot_ms
                sum_ff_t_par += ff_prog_t_par_ms
                sum_ff_t_tot += ff_prog_t_tot_ms

                df = (axiom_prog_t_tot_ms + succgen_prog_t_tot_ms + ff_prog_t_tot_ms) / total_time_ms
                dfs.append(df)

                if df < 0.5:
                    sum_runs_le_0_5 += 1
                    sum_total_le_0_5 += total_time_ms
                    sum_preprocessing_le_0_5 += preprocessing_time_ms
                    sum_axiom_rule_t_par_le_0_5 += axiom_rule_t_par_ms
                    sum_axiom_rule_t_tot_le_0_5 += axiom_rule_t_tot_ms
                    sum_axiom_t_par_le_0_5 += axiom_prog_t_par_ms
                    sum_axiom_t_tot_le_0_5 += axiom_prog_t_tot_ms
                    sum_succgen_rule_t_par_le_0_5 += succgen_rule_t_par_ms
                    sum_succgen_rule_t_tot_le_0_5 += succgen_rule_t_tot_ms
                    sum_succgen_t_par_le_0_5 += succgen_prog_t_par_ms
                    sum_succgen_t_tot_le_0_5 += succgen_prog_t_tot_ms
                    sum_ff_rule_t_par_le_0_5 += ff_rule_t_par_ms
                    sum_ff_rule_t_tot_le_0_5 += ff_rule_t_tot_ms
                    sum_ff_t_par_le_0_5 += ff_prog_t_par_ms
                    sum_ff_t_tot_le_0_5 += ff_prog_t_tot_ms
                else:
                    sum_runs_ge_0_5 += 1
                    sum_total_ge_0_5 += total_time_ms
                    sum_preprocessing_ge_0_5 += preprocessing_time_ms
                    sum_axiom_rule_t_par_ge_0_5 += axiom_rule_t_par_ms
                    sum_axiom_rule_t_tot_ge_0_5 += axiom_rule_t_tot_ms
                    sum_axiom_t_par_ge_0_5 += axiom_prog_t_par_ms
                    sum_axiom_t_tot_ge_0_5 += axiom_prog_t_tot_ms
                    sum_succgen_rule_t_par_ge_0_5 += succgen_rule_t_par_ms
                    sum_succgen_rule_t_tot_ge_0_5 += succgen_rule_t_tot_ms
                    sum_succgen_t_par_ge_0_5 += succgen_prog_t_par_ms
                    sum_succgen_t_tot_ge_0_5 += succgen_prog_t_tot_ms
                    sum_ff_rule_t_par_ge_0_5 += ff_rule_t_par_ms
                    sum_ff_rule_t_tot_ge_0_5 += ff_rule_t_tot_ms
                    sum_ff_t_par_ge_0_5 += ff_prog_t_par_ms
                    sum_ff_t_tot_ge_0_5 += ff_prog_t_tot_ms


        sum_other_t_tot = sum_total - (sum_preprocessing + sum_axiom_t_tot + sum_succgen_t_tot + sum_ff_t_tot)

        print("sum_runs:", sum_runs)
        print("sum_total:", sum_total)
        print("sum_preprocessing:", sum_preprocessing, "fraction:", sum_preprocessing / sum_total)
        # print("sum_axiom_rule_t_par:", sum_axiom_rule_t_par, "fraction:", sum_axiom_rule_t_par / sum_axiom_rule_t_tot)
        # print("sum_axiom_rule_t_tot:", sum_axiom_rule_t_tot)
        print("sum_axiom_t_par:", sum_axiom_t_par, "fraction:", sum_axiom_t_par / sum_axiom_t_tot)
        print("sum_axiom_t_tot:", sum_axiom_t_tot, "fraction:", sum_axiom_t_tot / sum_total)
        print("sum_succgen_rule_t_par:", sum_succgen_rule_t_par, "fraction:", sum_succgen_rule_t_par / sum_succgen_rule_t_tot)
        print("sum_succgen_rule_t_tot:", sum_succgen_rule_t_tot)
        print("sum_succgen_t_par:", sum_succgen_t_par, "fraction:", sum_succgen_t_par / sum_succgen_t_tot)
        print("sum_succgen_t_tot:", sum_succgen_t_tot, "fraction:", sum_succgen_t_tot / sum_total)
        print("sum_ff_rule_t_par:", sum_ff_rule_t_par, "fraction:", sum_ff_rule_t_par / sum_ff_rule_t_tot)
        print("sum_ff_rule_t_tot:", sum_ff_rule_t_tot)
        print("sum_ff_t_par:", sum_ff_t_par, "fraction:", sum_ff_t_par / sum_ff_t_tot)
        print("sum_ff_t_tot:", sum_ff_t_tot, "fraction:", sum_ff_t_tot / sum_total)
        print("sum_other_t_tot:", sum_other_t_tot, "fraction:", sum_other_t_tot / sum_total)

        sum_other_t_tot_le_0_5 = sum_total_le_0_5 - (sum_preprocessing_le_0_5 + sum_axiom_t_tot_le_0_5 + sum_succgen_t_tot_le_0_5 + sum_ff_t_tot_le_0_5)

        print("sum_runs_le_0_5:", sum_runs_le_0_5)
        print("sum_total_le_0_5:", sum_total_le_0_5)
        print("sum_preprocessing_le_0_5:", sum_preprocessing_le_0_5, "fraction:", sum_preprocessing_le_0_5 / sum_total_le_0_5)
        # print("sum_axiom_rule_t_par_le_0_5:", sum_axiom_rule_t_par_le_0_5, "fraction:", sum_axiom_rule_t_par_le_0_5 / sum_axiom_rule_t_tot_le_0_5)
        # print("sum_axiom_rule_t_tot_le_0_5:", sum_axiom_rule_t_tot_le_0_5)
        print("sum_axiom_t_par_le_0_5:", sum_axiom_t_par_le_0_5, "fraction:", sum_axiom_t_par_le_0_5 / sum_axiom_t_tot_le_0_5)
        print("sum_axiom_t_tot_le_0_5:", sum_axiom_t_tot_le_0_5, "fraction:", sum_axiom_t_tot_le_0_5 / sum_total_le_0_5)
        print("sum_succgen_rule_t_par_le_0_5:", sum_succgen_rule_t_par_le_0_5, "fraction:", sum_succgen_rule_t_par_le_0_5 / sum_succgen_rule_t_tot_le_0_5)
        print("sum_succgen_rule_t_tot_le_0_5:", sum_succgen_rule_t_tot_le_0_5)
        print("sum_succgen_t_par_le_0_5:", sum_succgen_t_par_le_0_5, "fraction:", sum_succgen_t_par_le_0_5 / sum_succgen_t_tot_le_0_5)
        print("sum_succgen_t_tot_le_0_5:", sum_succgen_t_tot_le_0_5, "fraction:", sum_succgen_t_tot_le_0_5 / sum_total_le_0_5)
        print("sum_ff_rule_t_par_le_0_5:", sum_ff_rule_t_par_le_0_5, "fraction:", sum_ff_rule_t_par_le_0_5 / sum_ff_rule_t_tot_le_0_5)
        print("sum_ff_rule_t_tot_le_0_5:", sum_ff_rule_t_tot_le_0_5)
        print("sum_ff_t_par_le_0_5:", sum_ff_t_par_le_0_5, "fraction:", sum_ff_t_par_le_0_5 / sum_ff_t_tot_le_0_5)
        print("sum_ff_t_tot_le_0_5:", sum_ff_t_tot_le_0_5, "fraction:", sum_ff_t_tot_le_0_5 / sum_total_le_0_5)
        print("sum_other_t_tot_le_0_5:", sum_other_t_tot_le_0_5, "fraction:", sum_other_t_tot_le_0_5 / sum_total_le_0_5)

        sum_other_t_tot_ge_0_5 = sum_total_ge_0_5 - (sum_preprocessing_ge_0_5 + sum_axiom_t_tot_ge_0_5 + sum_succgen_t_tot_ge_0_5 + sum_ff_t_tot_ge_0_5)

        print("sum_runs:", sum_runs_ge_0_5)
        print("sum_total:", sum_total_ge_0_5)
        print("sum_preprocessing:", sum_preprocessing_ge_0_5, "fraction:", sum_preprocessing_ge_0_5 / sum_total_ge_0_5)
        # print("sum_axiom_rule_t_par:", sum_axiom_rule_t_par_ge_0_5, "fraction:", sum_axiom_rule_t_par_ge_0_5 / sum_axiom_rule_t_tot_ge_0_5)
        # print("sum_axiom_rule_t_tot:", sum_axiom_rule_t_tot_ge_0_5)
        print("sum_axiom_t_par:", sum_axiom_t_par_ge_0_5, "fraction:", sum_axiom_t_par_ge_0_5 / sum_axiom_t_tot_ge_0_5)
        print("sum_axiom_t_tot:", sum_axiom_t_tot_ge_0_5, "fraction:", sum_axiom_t_tot_ge_0_5 / sum_total_ge_0_5)
        print("sum_succgen_rule_t_par:", sum_succgen_rule_t_par_ge_0_5, "fraction:", sum_succgen_rule_t_par_ge_0_5 / sum_succgen_rule_t_tot_ge_0_5)
        print("sum_succgen_rule_t_tot:", sum_succgen_rule_t_tot_ge_0_5)
        print("sum_succgen_t_par:", sum_succgen_t_par_ge_0_5, "fraction:", sum_succgen_t_par_ge_0_5 / sum_succgen_t_tot_ge_0_5)
        print("sum_succgen_t_tot:", sum_succgen_t_tot_ge_0_5, "fraction:", sum_succgen_t_tot_ge_0_5 / sum_total_ge_0_5)
        print("sum_ff_rule_t_par:", sum_ff_rule_t_par_ge_0_5, "fraction:", sum_ff_rule_t_par_ge_0_5 / sum_ff_rule_t_tot_ge_0_5)
        print("sum_ff_rule_t_tot:", sum_ff_rule_t_tot_ge_0_5)
        print("sum_ff_t_par:", sum_ff_t_par_ge_0_5, "fraction:", sum_ff_t_par_ge_0_5 / sum_ff_t_tot_ge_0_5)
        print("sum_ff_t_tot:", sum_ff_t_tot_ge_0_5, "fraction:", sum_ff_t_tot_ge_0_5 / sum_total_ge_0_5)
        print("sum_other_t_tot:", sum_other_t_tot_ge_0_5, "fraction:", sum_other_t_tot_ge_0_5 / sum_total_ge_0_5)

        bins = np.linspace(0, 1, 21)
        plt.figure()
        plt.hist(dfs, bins=bins, edgecolor="black")
        plt.xlabel("Parallelizable fraction p")
        plt.ylabel("Number of runs")
        plt.title("Distribution of parallelization potential")
        plt.savefig("parallel_fraction_histogram.png", dpi=300, bbox_inches="tight")
  
        print("DF:")
        for x in dfs:
            print(x)


if __name__ == "__main__":
    main()