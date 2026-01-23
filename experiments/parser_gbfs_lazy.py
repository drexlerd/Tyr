#! /usr/bin/env python

from lab.parser import Parser
from lab import tools

import re

def process_invalid(content, props):
    props["invalid"] = int("invalid" in props)

def process_unsolvable(content, props):
    props["unsolvable"] = int("unsolvable" in props)

def add_search_time(content, props):
    if "search_time_ms" in props:
        props["search_time"] = props["search_time_ms"] / 1000

def add_total_time(content, props):
    if "total_time_ms" in props:
        props["total_time"] = props["total_time_ms"] / 1000

def add_search_time_per_expanded(context, props):
    if "search_time" in props:
        props["search_time_per_expanded"] = props["search_time"] / props["num_expanded"]

def add_memory(content, props):
    if "peak_memory_usage_bytes" in props:
        props["memory"] = props["peak_memory_usage_bytes"] / 1000000

def add_coverage(content, props):
    if "length" in props or props.get("unsolvable", 0):
        props["coverage"] = 1
    else:
        props["coverage"] = 0

SECTION_MAP = {
    "Successor generator": "succgen",
    "Axiom evaluator": "axiom",
    "FFHeuristic": "ff",
}

RE_SECTION = re.compile(r'^\[(?P<name>[^\]]+)\]\s+Summary$')

RE_NUM_EXEC = re.compile(r'^\[ProgramStatistics\]\s+Num executions:\s*(?P<v>\d+)\s*$')
RE_PAR_MS = re.compile(r'^\[ProgramStatistics\].*T_par_region.*:\s*(?P<v>\d+)\s*ms\s*$')
RE_TOT_MS = re.compile(r'^\[ProgramStatistics\].*T_total.*:\s*(?P<v>\d+)\s*ms\s*$')
RE_AVG_US = re.compile(r'^\[ProgramStatistics\].*T_avg.*:\s*(?P<v>\d+)\s*us\s*$')
RE_FRAC = re.compile(r'^\[ProgramStatistics\].*Parallel fraction:\s*(?P<v>[0-9]*\.?[0-9]+)\s*$',re.I)


RE_SAMPLES = re.compile(r'^\[AggregatedRuleStatistics\].*Number of samples:\s*(?P<v>\d+)$')

# Totals (ms)
RE_TOT_MIN_MS = re.compile(r'^\[AggregatedRuleStatistics\].*T_tot_min_par_region.*:\s*(?P<v>\d+)\s*ms$')
RE_TOT_MAX_MS = re.compile(r'^\[AggregatedRuleStatistics\].*T_tot_max_par_region.*:\s*(?P<v>\d+)\s*ms$')
RE_TOT_MED_MS = re.compile(r'^\[AggregatedRuleStatistics\].*T_tot_med_par_region.*:\s*(?P<v>\d+)\s*ms$')

# Averages (ns)
RE_AVG_MIN_NS = re.compile(r'^\[AggregatedRuleStatistics\].*T_avg_min_par_region.*:\s*(?P<v>\d+)\s*ns$')
RE_AVG_MAX_NS = re.compile(r'^\[AggregatedRuleStatistics\].*T_avg_max_par_region.*:\s*(?P<v>\d+)\s*ns$')
RE_AVG_MED_NS = re.compile(r'^\[AggregatedRuleStatistics\].*T_avg_med_par_region.*:\s*(?P<v>\d+)\s*ns$')

# Skews
RE_TOT_SKEW = re.compile(
    r'^\[AggregatedRuleStatistics\].*T_tot_max_par_region\s*/\s*T_tot_med_par_region.*Total skew:\s*(?P<v>(?:inf)|(?:[0-9]*\.?[0-9]+))$',
    re.I
)
RE_AVG_SKEW = re.compile(
    r'^\[AggregatedRuleStatistics\].*T_avg_max_par_region\s*/\s*T_avg_med_par_region.*Average skew:\s*(?P<v>(?:inf)|(?:[0-9]*\.?[0-9]+))$',
    re.I
)

def parse_datalog_summaries(content, props):
    cur = None  # section prefix like "ff" / "succgen" / "axiom"

    for raw in content.splitlines():
        line = raw.strip()

        m = RE_SECTION.match(line)
        if m:
            name = m.group("name")
            cur = SECTION_MAP.get(name)  # ignore unknown sections
            continue

        if not cur:
            continue

        def put(suffix, value):
            props[f"{cur}_{suffix}"] = value

        m = RE_NUM_EXEC.match(line)
        if m:
            put("num_exec", int(m.group("v")))
            continue

        m = RE_PAR_MS.match(line)
        if m:
            put("par_ms", int(m.group("v")))
            continue

        m = RE_TOT_MS.match(line)
        if m:
            put("total_ms", int(m.group("v")))
            continue

        m = RE_AVG_US.match(line)
        if m:
            put("avg_us", int(m.group("v")))
            continue

        m = RE_FRAC.match(line)
        if m:
            put("par_frac", float(m.group("v")))
            continue

        m = RE_SAMPLES.match(line)
        if m:
            put("rule_samples", int(m.group("v")))
            continue

        # Totals (ms)
        m = RE_TOT_MIN_MS.match(line)
        if m:
            put("rule_tot_min_ms", int(m.group("v")))
            continue

        m = RE_TOT_MAX_MS.match(line)
        if m:
            put("rule_tot_max_ms", int(m.group("v")))
            continue

        m = RE_TOT_MED_MS.match(line)
        if m:
            put("rule_tot_med_ms", int(m.group("v")))
            continue

        # Averages (ns)
        m = RE_AVG_MIN_NS.match(line)
        if m:
            put("rule_avg_min_ns", int(m.group("v")))
            continue

        m = RE_AVG_MAX_NS.match(line)
        if m:
            put("rule_avg_max_ns", int(m.group("v")))
            continue

        m = RE_AVG_MED_NS.match(line)
        if m:
            put("rule_avg_med_ns", int(m.group("v")))
            continue

        # Skews
        m = RE_TOT_SKEW.match(line)
        if m:
            v = m.group("v").lower()
            put("rule_tot_skew", float("inf") if v == "inf" else float(v))
            continue

        m = RE_AVG_SKEW.match(line)
        if m:
            v = m.group("v").lower()
            put("rule_avg_skew", float("inf") if v == "inf" else float(v))
            continue

class GBFSLazyParser(Parser):
    """
    [GBFS] Search started.
    [GBFS] Start node h_value: 3
    [GBFS] New best h_value: 2 with num expanded states 2 and num generated states 5 (1 ms)
    [GBFS] New best h_value: 1 with num expanded states 3 and num generated states 7 (1 ms)
    [GBFS] Search ended.
    [Search] Search time: 1 ms
    [Search] Number of expanded states: 4
    [Search] Number of generated states: 7
    [Search] Number of pruned states: 0
    [GBFS] Plan found.
    [GBFS] Plan cost: 3
    [GBFS] Plan length: 3
    (pick ball2 rooma left)
    (move rooma roomb)
    (drop ball2 roomb left)

    [Successor generator] Summary
    [ProgramStatistics] Num executions: 7
    [ProgramStatistics] T_par_region - wallclock time inside parallel region: 0 ms
    [ProgramStatistics] T_total - wallclock time total: 0 ms
    [ProgramStatistics] T_avg - average wallclock time total: 40 us
    [ProgramStatistics] T_par_region / T_total - Parallel fraction: 1.00
    [AggregatedRuleStatistics] Number of samples: 3
    [AggregatedRuleStatistics] T_tot_min_par_region - minimum total wallclock time inside parallel region: 0 ms
    [AggregatedRuleStatistics] T_tot_max_par_region - maximum total wallclock time inside parallel region: 0 ms
    [AggregatedRuleStatistics] T_tot_med_par_region - median total wallclock time inside parallel region: 0 ms
    [AggregatedRuleStatistics] T_tot_max_par_region / T_tot_med_par_region - Total skew: 1.00
    [AggregatedRuleStatistics] T_avg_min_par_region - minimum average wallclock time inside parallel region: 7204 ns
    [AggregatedRuleStatistics] T_avg_max_par_region - maximum average wallclock time inside parallel region: 15266 ns
    [AggregatedRuleStatistics] T_avg_med_par_region - median average wallclock time inside parallel region: 8284 ns
    [AggregatedRuleStatistics] T_avg_max_par_region / T_avg_med_par_region - Average skew: 1.84
    [Axiom evaluator] Summary
    [ProgramStatistics] Num executions: 29
    [ProgramStatistics] T_par_region - wallclock time inside parallel region: 0 ms
    [ProgramStatistics] T_total - wallclock time total: 0 ms
    [ProgramStatistics] T_avg - average wallclock time total: 0 us
    [ProgramStatistics] T_par_region / T_total - Parallel fraction: 1.00
    [AggregatedRuleStatistics] Number of samples: 0
    [AggregatedRuleStatistics] T_tot_min_par_region - minimum total wallclock time inside parallel region: 0 ms
    [AggregatedRuleStatistics] T_tot_max_par_region - maximum total wallclock time inside parallel region: 0 ms
    [AggregatedRuleStatistics] T_tot_med_par_region - median total wallclock time inside parallel region: 0 ms
    [AggregatedRuleStatistics] T_tot_max_par_region / T_tot_med_par_region - Total skew: 1.00
    [AggregatedRuleStatistics] T_avg_min_par_region - minimum average wallclock time inside parallel region: 0 ns
    [AggregatedRuleStatistics] T_avg_max_par_region - maximum average wallclock time inside parallel region: 0 ns
    [AggregatedRuleStatistics] T_avg_med_par_region - median average wallclock time inside parallel region: 0 ns
    [AggregatedRuleStatistics] T_avg_max_par_region / T_avg_med_par_region - Average skew: 1.00
    [FFHeuristic] Summary
    [ProgramStatistics] Num executions: 5
    [ProgramStatistics] T_par_region - wallclock time inside parallel region: 1 ms
    [ProgramStatistics] T_total - wallclock time total: 1 ms
    [ProgramStatistics] T_avg - average wallclock time total: 321 us
    [ProgramStatistics] T_par_region / T_total - Parallel fraction: 1.00
    [AggregatedRuleStatistics] Number of samples: 7
    [AggregatedRuleStatistics] T_tot_min_par_region - minimum total wallclock time inside parallel region: 0 ms
    [AggregatedRuleStatistics] T_tot_max_par_region - maximum total wallclock time inside parallel region: 0 ms
    [AggregatedRuleStatistics] T_tot_med_par_region - median total wallclock time inside parallel region: 0 ms
    [AggregatedRuleStatistics] T_tot_max_par_region / T_tot_med_par_region - Total skew: 1.00
    [AggregatedRuleStatistics] T_avg_min_par_region - minimum average wallclock time inside parallel region: 6732 ns
    [AggregatedRuleStatistics] T_avg_max_par_region - maximum average wallclock time inside parallel region: 26896 ns
    [AggregatedRuleStatistics] T_avg_med_par_region - median average wallclock time inside parallel region: 9336 ns
    [AggregatedRuleStatistics] T_avg_max_par_region / T_avg_med_par_region - Average skew: 2.88
    [Total] Peak memory usage: 226709504 bytes
    [Total] Total time: 12 ms
    """
    def __init__(self):
        super().__init__()
        self.add_pattern("cost", r"\[GBFS\] Plan cost: (\d+)", type=int)
        self.add_pattern("length", r"\[GBFS\] Plan length: (\d+)", type=int)
        
        self.add_pattern("initial_h_value", r"\[GBFS\] Start node h_value: (\d+)", type=int)
        self.add_pattern("search_time_ms", r"\[Search\] Search time: (\d+) ms", type=int)
        self.add_pattern("num_expanded", r"\[Search\] Number of expanded states: (\d+)", type=int)
        self.add_pattern("num_generated", r"\[Search\] Number of generated states: (\d+)", type=int)

        self.add_pattern("total_time_ms", r"\[Total\] Total time: (\d+) ms", type=int)
        self.add_pattern("peak_memory_usage_bytes", r"\[Total\] Peak memory usage: (\d+) bytes", type=int)

        self.add_pattern("unsolvable", r"(Task is unsolvable!)", type=str)
        self.add_pattern("invalid", r"(Plan invalid)", type=str)
        
        self.add_function(process_invalid)
        self.add_function(process_unsolvable)
        self.add_function(add_search_time)
        self.add_function(add_total_time)
        self.add_function(add_search_time_per_expanded)
        self.add_function(add_memory)
        self.add_function(add_coverage)
        self.add_function(parse_datalog_summaries)
        