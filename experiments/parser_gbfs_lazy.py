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

def add_search_time_us_per_expanded(context, props):
    if "search_time_ns" in props:
        props["search_time_us_per_expanded"] = props["search_time_ns"] / 1000 / props["num_expanded"]

def add_memory(content, props):
    if "peak_memory_usage_bytes" in props:
        props["memory"] = props["peak_memory_usage_bytes"] / 1000000

def add_coverage(content, props):
    if "length" in props or props.get("unsolvable", 0):
        props["coverage"] = 1
    else:
        props["coverage"] = 0


RE_SECTION = re.compile(r'^\[(?P<name>[^\]]+)\]\s+Summary$')

# ProgramStatistics
RE_PROG_NUM_EXEC = re.compile(r'^\[ProgramStatistics\]\s+Num executions:\s*(?P<v>\d+)\s*$')
RE_PROG_PAR_MS   = re.compile(r'^\[ProgramStatistics\]\s+T_par\s*-\s*.*:\s*(?P<ms>\d+)\s*ms(?:\s*\(\s*(?P<ns>\d+)\s*ns\s*\))?\s*$')
RE_PROG_TOT_MS   = re.compile(r'^\[ProgramStatistics\]\s+T_total\s*-\s*.*:\s*(?P<ms>\d+)\s*ms(?:\s*\(\s*(?P<ns>\d+)\s*ns\s*\))?\s*$')
RE_PROG_AVG_US   = re.compile(r'^\[ProgramStatistics\]\s+T_avg\s*-\s*.*:\s*(?P<us>\d+)\s*us(?:\s*\(\s*(?P<ns>\d+)\s*ns\s*\))?\s*$')
RE_PROG_FRAC     = re.compile(r'^\[ProgramStatistics\]\s+T_par\s*/\s*T_total\s*-\s*Parallel fraction:\s*(?P<v>[0-9]*\.?[0-9]+)\s*$', re.I)

# AggregatedRuleStatistics header counts
RE_RULE_EXEC     = re.compile(r'^\[AggregatedRuleStatistics\]\s+Number of executions:\s*(?P<v>\d+)\s*$')
RE_RULE_BINDINGS = re.compile(r'^\[AggregatedRuleStatistics\]\s+Number of bindings:\s*(?P<v>\d+)\s*$')
RE_RULE_SAMPLES  = re.compile(r'^\[AggregatedRuleStatistics\]\s+Number of samples:\s*(?P<v>\d+)\s*$')

RE_RULE_INIT_MS = re.compile(r'^\[AggregatedRuleStatistics\]\s+T_initialize\s*-\s*.*:\s*(?P<ms>\d+)\s*ms(?:\s*\(\s*(?P<ns>\d+)\s*ns\s*\))?\s*$')
RE_RULE_GENERATE_MS = re.compile(r'^\[AggregatedRuleStatistics\]\s+T_generate\s*-\s*.*:\s*(?P<ms>\d+)\s*ms(?:\s*\(\s*(?P<ns>\d+)\s*ns\s*\))?\s*$')
RE_RULE_PENDING_MS = re.compile(r'^\[AggregatedRuleStatistics\]\s+T_pending\s*-\s*.*:\s*(?P<ms>\d+)\s*ms(?:\s*\(\s*(?P<ns>\d+)\s*ns\s*\))?\s*$')
RE_RULE_TOT_MS = re.compile(r'^\[AggregatedRuleStatistics\]\s+T_total\s*-\s*.*:\s*(?P<ms>\d+)\s*ms(?:\s*\(\s*(?P<ns>\d+)\s*ns\s*\))?\s*$')
RE_RULE_FRAC     = re.compile(r'^\[AggregatedRuleStatistics\]\s+T_par\s*/\s*T_total\s*-\s*Parallel fraction:\s*(?P<v>[0-9]*\.?[0-9]+)\s*$', re.I)

# Skews
RE_TOT_SKEW = re.compile(
    r'^\[AggregatedRuleStatistics\]\s+T_total_max\s*/\s*T_total_med.*Total skew:\s*(?P<v>(?:inf)|(?:[0-9]*\.?[0-9]+))\s*$',
    re.I
)
RE_AVG_SKEW = re.compile(
    r'^\[AggregatedRuleStatistics\]\s+T_avg_max\s*/\s*T_avg_med.*Average skew:\s*(?P<v>(?:inf)|(?:[0-9]*\.?[0-9]+))\s*$',
    re.I
)

# Map your section titles to prefixes in props
SECTION_MAP = {
    "Successor generator": "succgen",
    "Axiom evaluator": "axiom",
    "FFHeuristic": "ff",
}

def parse_datalog_summaries(content: str, props: dict):
    cur = None

    for raw in content.splitlines():
        line = raw.strip()
        if not line:
            continue

        m = RE_SECTION.match(line)
        if m:
            cur = SECTION_MAP.get(m.group("name"))
            continue

        if not cur:
            continue

        def put(suffix, value):
            props[f"{cur}_{suffix}"] = value

        # ProgramStatistics
        m = RE_PROG_NUM_EXEC.match(line)
        if m: put("prog_num_exec", int(m.group("v"))); continue

        m = RE_PROG_PAR_MS.match(line)
        if m:
            put("prog_par_ms", int(m.group("ms")))
            if m.group("ns") is not None:
                put("prog_par_ns", int(m.group("ns")))
            continue

        m = RE_PROG_TOT_MS.match(line)
        if m:
            put("prog_T_total_ms", int(m.group("ms")))
            if m.group("ns") is not None:
                put("prog_T_total_ns", int(m.group("ns")))
            continue

        m = RE_PROG_AVG_US.match(line)
        if m:
            put("prog_T_avg_us", int(m.group("us")))
            if m.group("ns") is not None:
                put("prog_T_avg_ns", int(m.group("ns")))
            continue

        m = RE_PROG_FRAC.match(line)
        if m: put("prog_par_frac", float(m.group("v"))); continue

        # Aggregated counts
        m = RE_RULE_EXEC.match(line)
        if m: put("rule_num_exec", int(m.group("v"))); continue

        m = RE_RULE_BINDINGS.match(line)
        if m: put("rule_num_bindings", int(m.group("v"))); continue

        m = RE_RULE_SAMPLES.match(line)
        if m: put("rule_samples", int(m.group("v"))); continue

        # Min/med/max totals (ms) + optional ns
        m = RE_RULE_INIT_MS.match(line)
        if m:
            put("rule_T_init_ms", int(m.group("ms")))
            if m.group("ns") is not None:
                put("rule_T_init_ns", int(m.group("ns")))
            continue

        m = RE_RULE_GENERATE_MS.match(line)
        if m:
            put("rule_T_generate_ms", int(m.group("ms")))
            if m.group("ns") is not None:
                put("rule_T_generate_ns", int(m.group("ns")))
            continue

        m = RE_RULE_PENDING_MS.match(line)
        if m:
            put("rule_T_pending_ms", int(m.group("ms")))
            if m.group("ns") is not None:
                put("rule_T_pending_ns", int(m.group("ns")))
            continue

        m = RE_RULE_TOT_MS.match(line)
        if m:
            put("rule_T_total_ms", int(m.group("ms")))
            if m.group("ns") is not None:
                put("rule_T_total_ns", int(m.group("ns")))
            continue

        m = RE_RULE_FRAC.match(line)
        if m: put("rule_par_frac", float(m.group("v"))); continue

        # Skews
        m = RE_TOT_SKEW.match(line)
        if m:
            v = m.group("v").lower()
            put("rule_total_skew", float("inf") if v == "inf" else float(v))
            continue

        m = RE_AVG_SKEW.match(line)
        if m:
            v = m.group("v").lower()
            put("rule_avg_skew", float("inf") if v == "inf" else float(v))
            continue


class GBFSLazyParser(Parser):
    """
    Num objects: 4
    [GBFS] Search started.
    [GBFS] Start node h_value: 3
    [GBFS] New best h_value: 2 with num expanded states 3 and num generated states 5 (0 ms)
    [GBFS] New best h_value: 1 with num expanded states 4 and num generated states 7 (0 ms)
    [GBFS] Search ended.
    [Search] Search time: 0 ms (743179 ns)
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
    [ProgramStatistics] T_par - wallclock time inside parallel: 0 ms (134942 ns)
    [ProgramStatistics] T_total - wallclock time total: 0 ms (165863 ns)
    [ProgramStatistics] T_avg - average wallclock time total: 23 us (23694 ns)
    [ProgramStatistics] T_par / T_total - Parallel fraction: 0.81
    [AggregatedRuleStatistics] Number of executions: 21
    [AggregatedRuleStatistics] Number of bindings: 28
    [AggregatedRuleStatistics] Number of samples: 3
    [AggregatedRuleStatistics] T_initialize - total wallclock time inside initialization of delta kpkc: 0 ms (29383 ns)
    [AggregatedRuleStatistics] T_generate - wallclock time to process generate: 0 ms (151003 ns)
    [AggregatedRuleStatistics] T_pending - wallclock time to process pending: 0 ms (2337 ns)
    [AggregatedRuleStatistics] T_par = T_generate: 0 ms (151003 ns)
    [AggregatedRuleStatistics] T_total - total wallclock time: 0 ms (193982 ns)
    [AggregatedRuleStatistics] T_par / T_total - Parallel fraction: 0.78
    [AggregatedRuleStatistics] T_total_min - minimum total wallclock time inside parallel: 0 ms (48983 ns)
    [AggregatedRuleStatistics] T_total_max - maximum total wallclock time inside parallel: 0 ms (84654 ns)
    [AggregatedRuleStatistics] T_total_med - median total wallclock time inside parallel: 0 ms (60345 ns)
    [AggregatedRuleStatistics] T_total_max / T_total_med_par - Total skew: 1.40
    [AggregatedRuleStatistics] T_avg_min - minimum average wallclock time inside parallel: 0 ms (6997 ns)
    [AggregatedRuleStatistics] T_avg_max - maximum average wallclock time inside parallel: 0 ms (12093 ns)
    [AggregatedRuleStatistics] T_avg_med - median average wallclock time inside parallel: 0 ms (8620 ns)
    [AggregatedRuleStatistics] T_avg_max / T_avg_med_par - Average skew: 1.40

    [Axiom evaluator] Summary
    [ProgramStatistics] Num executions: 29
    [ProgramStatistics] T_par - wallclock time inside parallel: 0 ms (3949 ns)
    [ProgramStatistics] T_total - wallclock time total: 0 ms (15678 ns)
    [ProgramStatistics] T_avg - average wallclock time total: 0 us (540 ns)
    [ProgramStatistics] T_par / T_total - Parallel fraction: 0.25
    [AggregatedRuleStatistics] Number of executions: 0
    [AggregatedRuleStatistics] Number of bindings: 0
    [AggregatedRuleStatistics] Number of samples: 0
    [AggregatedRuleStatistics] T_initialize - total wallclock time inside initialization of delta kpkc: 0 ms (0 ns)
    [AggregatedRuleStatistics] T_generate - wallclock time to process generate: 0 ms (0 ns)
    [AggregatedRuleStatistics] T_pending - wallclock time to process pending: 0 ms (0 ns)
    [AggregatedRuleStatistics] T_par = T_generate: 0 ms (0 ns)
    [AggregatedRuleStatistics] T_total - total wallclock time: 0 ms (0 ns)
    [AggregatedRuleStatistics] T_par / T_total - Parallel fraction: 1.00
    [AggregatedRuleStatistics] T_total_min - minimum total wallclock time inside parallel: 0 ms (0 ns)
    [AggregatedRuleStatistics] T_total_max - maximum total wallclock time inside parallel: 0 ms (0 ns)
    [AggregatedRuleStatistics] T_total_med - median total wallclock time inside parallel: 0 ms (0 ns)
    [AggregatedRuleStatistics] T_total_max / T_total_med_par - Total skew: 1.00
    [AggregatedRuleStatistics] T_avg_min - minimum average wallclock time inside parallel: 0 ms (0 ns)
    [AggregatedRuleStatistics] T_avg_max - maximum average wallclock time inside parallel: 0 ms (0 ns)
    [AggregatedRuleStatistics] T_avg_med - median average wallclock time inside parallel: 0 ms (0 ns)
    [AggregatedRuleStatistics] T_avg_max / T_avg_med_par - Average skew: 1.00

    [FFHeuristic] Summary
    [ProgramStatistics] Num executions: 5
    [ProgramStatistics] T_par - wallclock time inside parallel: 0 ms (855244 ns)
    [ProgramStatistics] T_total - wallclock time total: 0 ms (932627 ns)
    [ProgramStatistics] T_avg - average wallclock time total: 186 us (186525 ns)
    [ProgramStatistics] T_par / T_total - Parallel fraction: 0.92
    [AggregatedRuleStatistics] Number of executions: 74
    [AggregatedRuleStatistics] Number of bindings: 136
    [AggregatedRuleStatistics] Number of samples: 7
    [AggregatedRuleStatistics] T_initialize - total wallclock time inside initialization of delta kpkc: 0 ms (85984 ns)
    [AggregatedRuleStatistics] T_generate - wallclock time to process generate: 0 ms (630895 ns)
    [AggregatedRuleStatistics] T_pending - wallclock time to process pending: 0 ms (7855 ns)
    [AggregatedRuleStatistics] T_par = T_generate: 0 ms (630895 ns)
    [AggregatedRuleStatistics] T_total - total wallclock time: 0 ms (782940 ns)
    [AggregatedRuleStatistics] T_par / T_total - Parallel fraction: 0.81
    [AggregatedRuleStatistics] T_total_min - minimum total wallclock time inside parallel: 0 ms (76732 ns)
    [AggregatedRuleStatistics] T_total_max - maximum total wallclock time inside parallel: 0 ms (166988 ns)
    [AggregatedRuleStatistics] T_total_med - median total wallclock time inside parallel: 0 ms (116846 ns)
    [AggregatedRuleStatistics] T_total_max / T_total_med_par - Total skew: 1.43
    [AggregatedRuleStatistics] T_avg_min - minimum average wallclock time inside parallel: 0 ms (6294 ns)
    [AggregatedRuleStatistics] T_avg_max - maximum average wallclock time inside parallel: 0 ms (13484 ns)
    [AggregatedRuleStatistics] T_avg_med - median average wallclock time inside parallel: 0 ms (11684 ns)
    [AggregatedRuleStatistics] T_avg_max / T_avg_med_par - Average skew: 1.15

    [Total] Peak memory usage: 513306624 bytes
    [Total] Total time: 4 ms (4424855 ns)

    """
    def __init__(self):
        super().__init__()
        self.add_pattern("cost", r"\[GBFS\] Plan cost: (\d+)", type=int)
        self.add_pattern("length", r"\[GBFS\] Plan length: (\d+)", type=int)
        
        self.add_pattern("initial_h_value", r"\[GBFS\] Start node h_value: (\d+)", type=int)
        self.add_pattern("search_time_ms", r"\[Search\] Search time: (\d+) ms", type=int)
        self.add_pattern("search_time_ns", r"\[Search\] Search time: \d+ ms \((\d+) ns\)", type=int)
        self.add_pattern("num_expanded", r"\[Search\] Number of expanded states: (\d+)", type=int)
        self.add_pattern("num_generated", r"\[Search\] Number of generated states: (\d+)", type=int)

        self.add_pattern("total_time_ms", r"\[Total\] Total time: (\d+) ms", type=int)
        self.add_pattern("peak_memory_usage_bytes", r"\[Total\] Peak memory usage: (\d+) bytes", type=int)

        self.add_pattern("unsolvable", r"(Task is unsolvable!)", type=str)
        self.add_pattern("invalid", r"(Plan invalid)", type=str)

        self.add_pattern("num_objects", r"Num objects: (\d+)", type=int)
        
        self.add_function(process_invalid)
        self.add_function(process_unsolvable)
        self.add_function(add_search_time)
        self.add_function(add_total_time)
        self.add_function(add_search_time_us_per_expanded)
        self.add_function(add_memory)
        self.add_function(add_coverage)
        self.add_function(parse_datalog_summaries)
        