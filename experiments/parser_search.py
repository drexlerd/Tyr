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



class SearchParser(Parser):
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

    ...

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
