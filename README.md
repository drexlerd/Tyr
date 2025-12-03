
# Tyr

Tyr aims to become a weighted, annotated, and parallelizable datalog solver with a grounder based on k-clique enumeration in k-partite graphs (KPKC) with a focus on AI planning. It is currently unknown whether the KPKC grounder can efficiently support object creation, and hence, Tyr does not support arithmetic expressions in rule heads. However, it supports simple arithmetic expressions in rule bodies, which are sufficient for deciding whether a ground action in numeric planning is applicable. 

Tyr also aims to provides a PDDL interface, that employs the parallelized datalog solver to address three foundational problems within planning through compilations into datalog: 1) task grounding, 2) lifted axiom evaluation, 3) enumerating applicable actions in a state. Tyr's strongly typed and templated design enforces a clear separation between grounded and lifted planning for prototyping approaches to planning and evaluating them on both paradigms. For lifted states, Tyr employs a sparse representation, and for grounded planning, it employs a finite-domain representation.


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
auto solution = solver.solve(program, fluent_facts, goal_facts, annotated, weighted);

```
  

## 4 PDDL Interface

The high level C++ planning interface aims to be as follows. 

## 4.1 Lifted Planning

We obtain a lifted task by parsing the PDDL. We can then iteratively expand the search space, starting from the initial node (or some arbitrary instantiated node), by computing their successor nodes each labeled with their ground action that generates it.

```cpp
#include <tyr/tyr.hpp>

auto parser = tyr::formalism::Parser("domain.pddl");
auto task = parser.parse_task("problem.pddl");

// Get the initial node (state + metric value)
auto initial_node = task.get_initial_node();

// Get the labeled successor nodes (pairs of ground action and successor node)
auto successor_nodes = initial_node.get_labeled_successor_nodes();

```

## 4.1 Grounded Planning

We can obtain a ground task from the lifted task using a delete free exploration of the lifted task.
An additional mutex generation phase, allows translating binary atoms into finite domain variables, resulting in the finite domain state representation (FDR).

```cpp
#include <tyr/tyr.hpp>

auto parser = tyr::formalism::Parser("domain.pddl");
auto task = parser.parse_task("problem.pddl");

// Ground the task
auto ground_task = task.get_ground_task();

// Get the initial node (state + metric value)
auto initial_node = ground_task.get_initial_node();

// Get the labeled successor nodes (pairs of ground action and successor node)
auto successor_nodes = initial_node.get_labeled_successor_nodes();

```
