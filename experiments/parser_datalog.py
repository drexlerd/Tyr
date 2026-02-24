#! /usr/bin/env python

from lab.parser import Parser
from lab import tools

import re


RE_SECTION = re.compile(r'^\[(?P<name>[^\]]+)\]\s+Summary$')

# ProgramStatistics
RE_PROG_N_EXEC = re.compile(r'^\[ProgramStatistics\]\s+N_exec\s*=\s*(?P<v>\d+)\s*.*$')
RE_PROG_T_SEQ =  re.compile(r'^\[ProgramStatistics\]\s+T_seq\s*=\s*(?P<ms>\d+)\s*ms\s*.*$')
RE_PROG_T_PAR =  re.compile(r'^\[ProgramStatistics\]\s+T_par\s*=\s*(?P<ms>\d+)\s*ms\s*.*$')
RE_PROG_T_TOT =  re.compile(r'^\[ProgramStatistics\]\s+T_tot\s*=\s*(?P<ms>\d+)\s*ms\s*.*$')
RE_PROG_T_AVG =  re.compile(r'^\[ProgramStatistics\]\s+T_avg\s*=\s*(?P<us>\d+)\s*us\s*.*$')
RE_PROG_PF =  re.compile(r'^\[ProgramStatistics\]\s+PF\s*=\s*(?P<v>[0-9]*\.?[0-9]+)\s*.*$')

# AggregatedRuleStatistics
RE_RULE_N_EXEC     = re.compile(r'^\[AggregatedRuleStatistics\]\s+N_exec\s*=\s*(?P<v>\d+)\s*.*$')
RE_RULE_N_SAMPLES     = re.compile(r'^\[AggregatedRuleStatistics\]\s+N_samples\s*=\s*(?P<v>\d+)\s*.*$')
RE_RULE_T_SEQ =  re.compile(r'^\[AggregatedRuleStatistics\]\s+T_seq\s*=\s*(?P<ms>\d+)\s*ms\s*.*$')
RE_RULE_T_PAR =  re.compile(r'^\[AggregatedRuleStatistics\]\s+T_par\s*=\s*(?P<ms>\d+)\s*ms\s*.*$')
RE_RULE_T_TOT =  re.compile(r'^\[AggregatedRuleStatistics\]\s+T_tot\s*=\s*(?P<ms>\d+)\s*ms\s*.*$')
RE_RULE_T_AVG =  re.compile(r'^\[AggregatedRuleStatistics\]\s+T_avg\s*=\s*(?P<us>\d+)\s*us\s*.*$')
RE_RULE_PF =  re.compile(r'^\[AggregatedRuleStatistics\]\s+PF\s*=\s*(?P<v>[0-9]*\.?[0-9]+)\s*.*$')
RE_RULE_SKEW_TOT =  re.compile(r'^\[AggregatedRuleStatistics\]\s+T_tot_skew\s*=\s*(?P<v>[0-9]*\.?[0-9]+)\s*.*$')
RE_RULE_SKEW_AVG =  re.compile(r'^\[AggregatedRuleStatistics\]\s+T_avg_skew\s*=\s*(?P<v>[0-9]*\.?[0-9]+)\s*.*$')

# AggregatedRuleWorkerStatistics
RE_RULE_WORKER_N_EXEC     = re.compile(r'^\[AggregatedRuleWorkerStatistics\]\s+N_exec\s*=\s*(?P<v>\d+)\s*.*$')
RE_RULE_WORKER_N_GEN     = re.compile(r'^\[AggregatedRuleWorkerStatistics\]\s+N_gen\s*=\s*(?P<v>\d+)\s*.*$')
RE_RULE_WORKER_N_PEN     = re.compile(r'^\[AggregatedRuleWorkerStatistics\]\s+N_pen\s*=\s*(?P<v>\d+)\s*.*$')
RE_RULE_WORKER_OA     = re.compile(r'^\[AggregatedRuleWorkerStatistics\]\s+OA\s*=\s*(?P<v>\d+)\s*.*$')

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

        m = RE_PROG_N_EXEC.match(line)
        if m: put("prog_n_exec", int(m.group("v"))); continue

        m = RE_PROG_T_SEQ.match(line)
        if m: put("prog_t_seq", int(m.group("ms"))); continue

        m = RE_PROG_T_PAR.match(line)
        if m: put("prog_t_par", int(m.group("ms"))); continue

        m = RE_PROG_T_TOT.match(line)
        if m: put("prog_t_tot", int(m.group("ms"))); continue

        m = RE_PROG_T_AVG.match(line)
        if m: put("prog_t_avg", int(m.group("us"))); continue

        m = RE_PROG_PF.match(line)
        if m: put("prog_pf", float(m.group("v"))); continue

        # AggregatedRuleStatistics

        m = RE_RULE_N_EXEC.match(line)
        if m: put("rule_n_exec", int(m.group("v"))); continue

        m = RE_RULE_N_SAMPLES.match(line)
        if m: put("rule_n_samples", int(m.group("v"))); continue

        m = RE_RULE_T_SEQ.match(line)
        if m: put("rule_t_seq", int(m.group("ms"))); continue

        m = RE_RULE_T_PAR.match(line)
        if m: put("rule_t_par", int(m.group("ms"))); continue

        m = RE_RULE_T_TOT.match(line)
        if m: put("rule_t_tot", int(m.group("ms"))); continue

        m = RE_RULE_T_AVG.match(line)
        if m: put("rule_t_avg", int(m.group("us"))); continue

        m = RE_RULE_PF.match(line)
        if m: put("rule_pf", float(m.group("v"))); continue

        m = RE_RULE_SKEW_TOT.match(line)
        if m: put("rule_skew_tot", float(m.group("v"))); continue

        m = RE_RULE_SKEW_AVG.match(line)
        if m: put("rule_skew_avg", float(m.group("v"))); continue

        # AggregatedRuleWorkerStatistics

        m = RE_RULE_WORKER_N_EXEC.match(line)
        if m: put("rule_worker_n_exec", int(m.group("v"))); continue

        m = RE_RULE_WORKER_N_GEN.match(line)
        if m: put("rule_worker_n_gen", int(m.group("v"))); continue

        m = RE_RULE_WORKER_N_PEN.match(line)
        if m: put("rule_worker_n_pen", int(m.group("v"))); continue

        m = RE_RULE_WORKER_OA.match(line)
        if m: put("rule_worker_oa", float(m.group("v"))); continue


class DatalogParser(Parser):
    """
    [Successor generator] Summary
    [ProgramStatistics] N_exec =         10    | executions
    [ProgramStatistics] T_seq  =          0 ms | sequential time
    [ProgramStatistics] T_par  =          1 ms | parallel time
    [ProgramStatistics] T_tot  =          1 ms | total time
    [ProgramStatistics] T_avg  =        192 us | average time
    [ProgramStatistics] PF     =       0.79    | parallel fraction (T_par / T_tot)
    [AggregatedRuleStatistics] N_exec     =         40    | executions
    [AggregatedRuleStatistics] N_samples  =          4    | samples
    [AggregatedRuleStatistics] T_seq      =          0 ms | sequential time
    [AggregatedRuleStatistics] T_par      =          1 ms | parallel time
    [AggregatedRuleStatistics] T_tot      =          1 ms | total time
    [AggregatedRuleStatistics] T_avg      =         37 us | average time
    [AggregatedRuleStatistics] PF         =       0.79    | parallel fraction (T_par / T_tot)
    [AggregatedRuleStatistics] T_tot_min  =          0 ms | minimum total time
    [AggregatedRuleStatistics] T_tot_max  =          0 ms | maximum total time
    [AggregatedRuleStatistics] T_tot_med  =          0 ms | median total time
    [AggregatedRuleStatistics] T_tot_skew =       2.87    | skew total time (T_tot_max / T_tot_med)
    [AggregatedRuleStatistics] T_avg_min  =          1 us | minimum average time
    [AggregatedRuleStatistics] T_avg_max  =         86 us | maximum average time
    [AggregatedRuleStatistics] T_avg_med  =         30 us | median average time
    [AggregatedRuleStatistics] T_avg_skew =       2.87    | skew average time (T_avg_max / T_avg_med)
    [AggregatedRuleWorkerStatistics] N_exec =          40 | executions
    [AggregatedRuleWorkerStatistics] N_gen  =        7488 | generated rules
    [AggregatedRuleWorkerStatistics] N_pen  =        2493 | pending rules
    [AggregatedRuleWorkerStatistics] OA     =        1.33 | overapproximation ratio (1 + N_pen / N_gen)
    [Axiom evaluator] Summary
    [ProgramStatistics] N_exec =       4996    | executions
    [ProgramStatistics] T_seq  =         20 ms | sequential time
    [ProgramStatistics] T_par  =          0 ms | parallel time
    [ProgramStatistics] T_tot  =         20 ms | total time
    [ProgramStatistics] T_avg  =          4 us | average time
    [ProgramStatistics] PF     =       0.01    | parallel fraction (T_par / T_tot)
    [AggregatedRuleStatistics] N_exec     =          0    | executions
    [AggregatedRuleStatistics] N_samples  =          0    | samples
    [AggregatedRuleStatistics] T_seq      =          0 ms | sequential time
    [AggregatedRuleStatistics] T_par      =          0 ms | parallel time
    [AggregatedRuleStatistics] T_tot      =          0 ms | total time
    [AggregatedRuleStatistics] T_avg      =          0 us | average time
    [AggregatedRuleStatistics] PF         =       1.00    | parallel fraction (T_par / T_tot)
    [AggregatedRuleStatistics] T_tot_min  =          0 ms | minimum total time
    [AggregatedRuleStatistics] T_tot_max  =          0 ms | maximum total time
    [AggregatedRuleStatistics] T_tot_med  =          0 ms | median total time
    [AggregatedRuleStatistics] T_tot_skew =       1.00    | skew total time (T_tot_max / T_tot_med)
    [AggregatedRuleStatistics] T_avg_min  =          0 us | minimum average time
    [AggregatedRuleStatistics] T_avg_max  =          0 us | maximum average time
    [AggregatedRuleStatistics] T_avg_med  =          0 us | median average time
    [AggregatedRuleStatistics] T_avg_skew =       1.00    | skew average time (T_avg_max / T_avg_med)
    [AggregatedRuleWorkerStatistics] N_exec =           0 | executions
    [AggregatedRuleWorkerStatistics] N_gen  =           0 | generated rules
    [AggregatedRuleWorkerStatistics] N_pen  =           0 | pending rules
    [AggregatedRuleWorkerStatistics] OA     =        1.00 | overapproximation ratio (1 + N_pen / N_gen)
    [FFHeuristic] Summary
    [ProgramStatistics] N_exec =          7    | executions
    [ProgramStatistics] T_seq  =       1478 ms | sequential time
    [ProgramStatistics] T_par  =       4296 ms | parallel time
    [ProgramStatistics] T_tot  =       5774 ms | total time
    [ProgramStatistics] T_avg  =     824915 us | average time
    [ProgramStatistics] PF     =       0.74    | parallel fraction (T_par / T_tot)
    [AggregatedRuleStatistics] N_exec     =        207    | executions
    [AggregatedRuleStatistics] N_samples  =         13    | samples
    [AggregatedRuleStatistics] T_seq      =        202 ms | sequential time
    [AggregatedRuleStatistics] T_par      =       4092 ms | parallel time
    [AggregatedRuleStatistics] T_tot      =       4295 ms | total time
    [AggregatedRuleStatistics] T_avg      =      20753 us | average time
    [AggregatedRuleStatistics] PF         =       0.95    | parallel fraction (T_par / T_tot)
    [AggregatedRuleStatistics] T_tot_min  =          0 ms | minimum total time
    [AggregatedRuleStatistics] T_tot_max  =       1812 ms | maximum total time
    [AggregatedRuleStatistics] T_tot_med  =         15 ms | median total time
    [AggregatedRuleStatistics] T_tot_skew =     117.36    | skew total time (T_tot_max / T_tot_med)
    [AggregatedRuleStatistics] T_avg_min  =         11 us | minimum average time
    [AggregatedRuleStatistics] T_avg_max  =      98525 us | maximum average time
    [AggregatedRuleStatistics] T_avg_med  =       1403 us | median average time
    [AggregatedRuleStatistics] T_avg_skew =      70.19    | skew average time (T_avg_max / T_avg_med)
    [AggregatedRuleWorkerStatistics] N_exec =         207 | executions
    [AggregatedRuleWorkerStatistics] N_gen  =     7013498 | generated rules
    [AggregatedRuleWorkerStatistics] N_pen  =        1496 | pending rules
    [AggregatedRuleWorkerStatistics] OA     =        1.00 | overapproximation ratio (1 + N_pen / N_gen)
    """
    def __init__(self):
        super().__init__()

        self.add_function(parse_datalog_summaries)
        