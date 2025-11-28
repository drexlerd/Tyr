# Tyr

Tyr aims to become a weighted, annotated, and parallelizable datalog solver with a grounder based on k-clique enumeration in k-partite graphs (KPKC) with a focus on AI planning. It is currently unknown whether the KPKC grounder can efficiently support object creation, and hence, Tyr does not support arithmetic expressions in rule heads. However, it supports simple arithmetic expressions in rule bodies, which are sufficient for deciding whether a ground action in numeric planning is applicable. Tyr also provides a PDDL interface, which allows the grounding of an *overapproximation* of the applicable ground actions and axioms in a task, or, in a state, *precisely* the ones applicable in that state.

# Key Features

- **Datalog Language Support**: relations over symbols, stratifiable programs
- **Language Extensions**: weighted rule expansion, rule annotation, early termination 
- **Parallelized Architecture**: lock-free rule parallelization, zero-copy data serialization
- **Program Analysis**: variable domain analysis, stratification, listeners
- **Grounder Technology**: k-clique enumeration in k-partite graph (KPKC)

# Getting Started

## 1. Installation

TODO

## 2. Integration

TODO

## 3. Example

See [here](https://github.com/drexlerd/Tyr/blob/main/tests/unit/utils.hpp) on how to programmatically instantiate a program to generate the applicable actions in the initial state of a planning task over the Gripper domain with two balls.
