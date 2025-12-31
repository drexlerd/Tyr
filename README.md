
# Tyr

Tyr aims to become a weighted, annotated, and parallelizable datalog solver with a grounder based on k-clique enumeration in k-partite graphs (KPKC) with a focus on AI planning. Tyr also provides a PDDL interface, that employs the parallelized datalog solver to address three foundational problems within planning through compilations into datalog: 1) task grounding, 2) lifted axiom evaluation, 3) enumerating applicable actions in a state. For lifted states, Tyr employs a sparse representation, and for grounded planning, it employs a finite-domain representation.


# Key Features

-  **Datalog Language Support**: relations over symbols, stratifiable programs
-  **Language Extensions**: weighted rule expansion, rule annotation, early termination
-  **Parallelized Architecture**: lock-free rule parallelization, zero-copy data serialization
-  **Program Analysis**: variable domain analysis, stratification, listeners
-  **Grounder Technology**: k-clique enumeration in k-partite graph (KPKC)

  
# Getting Started

## 1. Installation

Instructions on how to build the library and exeutables is available [here](docs/BUILD.md).

## 2. Integration

TODO

## 3. Datalog Interface (Semi-Finished)

The high level C++ datalog interface is as follows:

```cpp
#include <tyr/tyr.hpp>

auto parser = tyr::formalism::Parser("program.dl");

auto program = parser.get_program();
// Fluent facts can optionally be parsed to override the ones in the program
auto fluent_facts = parser.parse_fluent_facts("fluent_facts.dl");
// Goal facts can optionally be parsed to trigger early termination
auto goal_facts = parser.parse_goal("goal_facts.dl");

// Initialize execution context. Fine-grained reinitialization with new fluent and goal facts possible.
// Only assumptions are fixed sets of objects and static facts.
auto execuction_context = tyr::datalog::ProgramExecutionContext(program, fluent_facts, goal_facts);

// Execution modes
const auto annotated = bool{true};
const auto weighted = bool{true};

// Solution is a set of ground facts and rules annotated with achievers and cost
auto solution = tyr::datalog::solve_bottomup(execuction_context, annotated, weighted);

```
  

## 4 PDDL Interface

The high level C++ planning interface is as follows:

## 4.1 Lifted Planning (Finished)

We obtain a lifted task by parsing the PDDL. Then, we translate the lifted task into three datalog program: P1) ground task program, P2) action program, P3) axiom program. Program P1 encodes the delete free task to approximate the set of applicable ground actions and axioms in the task. Program P2 encodes the action preconditions to overapproximate the set of ground applicable actions in a state. Program P3 encodes the axiom evaluation in a state. Given these three programs, the API allows to retrieve the extended initial node (sparse state + metric value) using program P3. Given a node, compute the labeled successor nodes (ground action + node) using programs P2 and P3.

```cpp
#include <tyr/tyr.hpp>

auto parser = tyr::formalism::Parser("domain.pddl");
auto task = parser.parse_task("problem.pddl");
auto successor_generator = SuccessorGenerator<tyr::planning::LiftedTask>(task);

// Get the initial node (state + metric value)
auto initial_node = successor_generator.get_initial_node();

// Get the labeled successor nodes (sequence of ground action + node)
auto successor_nodes = successor_generator.get_labeled_successor_nodes(initial_node);

```

## 4.1 Grounded Planning (Finished)

From the lifted task and using program P1, we can compute a ground task that overapproximates the delete-free reachable ground atoms, actions, and axioms. From those, we derived mutex groups, enabling us to form a more compact finite-domain representation (FDR). The remaining interface remains identical, but uses FDR instead of a sparse state representation.

```cpp
#include <tyr/tyr.hpp>

auto parser = tyr::formalism::Parser("domain.pddl");
auto task = parser.parse_task("problem.pddl");

// Ground the task
auto ground_task = task.get_ground_task();
auto successor_generator = SuccessorGenerator<tyr::planning::GroundTask>(ground_task);

// Get the initial node (state + metric value)
auto initial_node = successor_generator.get_initial_node();

// Get the labeled successor nodes (sequence of ground action + node)
auto successor_nodes = successor_generator.get_labeled_successor_nodes(initial_node);

```
