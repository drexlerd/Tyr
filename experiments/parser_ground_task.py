#! /usr/bin/env python

from lab.parser import Parser
from lab import tools

def add_dummy_attribute(content, props):
    props["dummy_attribute"] = 1

def add_parallel_fraction(content, props):
    if "ground_seq_total_time" in props and "merge_seq_total_time" in props:
        total = (props["ground_seq_total_time"] + props["merge_seq_total_time"])
        if total > 0:
            props["parallel_fraction"] = props["ground_seq_total_time"] / total
        else:
            props["parallel_fraction"] = 0.0

def add_coverage(content, props):
    if "ground_seq_total_time" in props:
        props["coverage"] = 1
    else:
        props["coverage"] = 0

class GroundTaskParser(Parser):
    """
    Num fluent atoms: 90901
    Num derived atoms: 0
    Num ground actions: 180600
    Num ground axioms: 0
    Total task grounding time: 5049 ms
    """
    def __init__(self):
        super().__init__()
        self.add_pattern("num_fluent_atoms", r"Num fluent atoms: (\d+)", type=int)
        self.add_pattern("num_derived_atoms", r"Num derived atoms: (\d+)", type=int)
        self.add_pattern("num_ground_actions", r"Num ground actions: (\d+)", type=int)
        self.add_pattern("num_ground_axioms", r"Num ground axioms: (\d+)", type=int)
        self.add_pattern("total_task_grounding_time", r"Total task grounding time: (\d+) ms", type=int)

        self.add_pattern("init_total_time_min", r"init_total_time_min: (\d+) ms", type=int)
        self.add_pattern("init_total_time_max", r"init_total_time_max: (\d+) ms", type=int)
        self.add_pattern("init_total_time_median", r"init_total_time_median: (\d+) ms", type=int)
        self.add_pattern("ground_total_time_min", r"ground_total_time_min: (\d+) ms", type=int)
        self.add_pattern("ground_total_time_max", r"ground_total_time_max: (\d+) ms", type=int)
        self.add_pattern("ground_total_time_median", r"ground_total_time_median: (\d+) ms", type=int)
        self.add_pattern("num_rules", r"num_rules: (\d+)", type=int)
        self.add_pattern("ground_seq_total_time", r"ground_seq_total_time: (\d+) ms", type=int)
        self.add_pattern("merge_seq_total_time", r"merge_seq_total_time: (\d+) ms", type=int)
        self.add_function(add_dummy_attribute)
        self.add_function(add_parallel_fraction)
        self.add_function(add_coverage)
        