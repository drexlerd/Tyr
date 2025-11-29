
# Tyr

  

Tyr aims to become a weighted, annotated, and parallelizable datalog solver with a grounder based on k-clique enumeration in k-partite graphs (KPKC) with a focus on AI planning. It is currently unknown whether the KPKC grounder can efficiently support object creation, and hence, Tyr does not support arithmetic expressions in rule heads. However, it supports simple arithmetic expressions in rule bodies, which are sufficient for deciding whether a ground action in numeric planning is applicable. Tyr also provides a PDDL interface, which allows the grounding of an *overapproximation* of the applicable ground actions and axioms in a task, or, in a state, *precisely* the ones applicable in that state.

  

# Key Features

  

-  **Datalog Language Support**: relations over symbols, stratifiable programs

-  **Language Extensions**: weighted rule expansion, rule annotation, early termination

-  **Parallelized Architecture**: lock-free rule parallelization, zero-copy data serialization

-  **Program Analysis**: variable domain analysis, stratification, listeners

-  **Grounder Technology**: k-clique enumeration in k-partite graph (KPKC)

  

# Getting Started

  

## 1. Installation

  

TODO

  

## 2. Integration

  

TODO

  

## 3. Datalog Interface

The high level C++ datalog interface aims to be as follows

```cpp
#include <tyr/tyr.hpp>

auto parser = tyr::formalism::Parser("program.dl");

auto program = parser.get_program();
auto static_facts = parser.parse_static_facts("static_facts.dl")
auto fluent_facts = parser.parse_fluent_facts("fluent_facts.dl");
auto goal_facts = parser.parse_goal("goal_facts.dl");

const auto annotated = bool{true};
const auto weighted = bool{true};

// Solver can be reused for different sets of fluent facts
auto solver = tyr::solver::Solver(program, static_facts);

// Solution is a ground facts and rules annotated with achievers and cost
auto solution = solver.solve(fluent_facts, goal_facts, annotated, weighted);

```
  

## 4 PDDL Interface

The high level C++ planning interface aims to be as follows. 

## 4.1 Grounded Planning

```cpp
#include <tyr/tyr.hpp>

auto parser = tyr::formalism::planning::Parser("domain.pddl");
auto task = parse.parse_task("problem.pddl");

// Ground the task
auto ground_task = task.get_ground_task();

// Get the initial node (state + metric value)
auto initial_node = ground_task.get_initial_node();

// Get the labeled successor nodes (pairs of ground action and successor node)
auto successor_nodes = initial_node.get_labeled_successor_nodes();

```

## 4.2 Lifted Planning

```cpp
#include <tyr/tyr.hpp>

auto parser = tyr::formalism::planning::Parser("domain.pddl");
auto task = parse.parse_task("problem.pddl");

// Get the initial node (state + metric value)
auto initial_node = task.get_initial_node();

// Get the labeled successor nodes (pairs of ground action and successor node)
auto successor_nodes = initial_node.get_labeled_successor_nodes();

```
