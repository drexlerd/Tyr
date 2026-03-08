# Tyr: Generalized Planning in C++20 and Python

Tyr is designed to address several challenges in modern planning systems:

1. **Unified grounded and lifted planning** within a type-safe API.

2. **Rapid prototyping** through Python bindings with type hints, backed by a high-performance C++ core.

3. **Support for expressive numeric planning formalisms** across both grounded and lifted reasoning paradigms (see [Supported PDDL Features](docs/PDDL_SUPPORT.md)).

4. **Scalable reasoning** through a parallel architecture with memory management based on hierarchical arenas (domain ← tasks ← workers) and object pools for recycling frequently allocated objects such as states.

5. **Integration of learning and reasoning** by supporting collections of planning tasks over a shared planning domain.
  
# Getting Started

The library consists of a **formalism** and a **planning** component. The formalism component is responsible for representing PDDL entities. The planning component provides functionality for implementing search algorithms, as well as off-the-shelf implementations of eager A*, lazy GBFS, and heuristics such as blind, max, add, and FF. Below is a basic overview of the Python and C++ APIs for implementing custom search algorithms.

## Python Interface

Pytyr is available at [Pypi.org](https://pypi.org/project/pytyr/) and can be installed with `pip install pytyr`. 

Detailed examples are available in the `python/examples` directory:

- [`structures.py`](python/examples/formalism/planning/structures.py) – Parse and traverse all planning formalism structures.
- [`astar_eager.py`](python/examples/planning/astar_eager.py) – Use and customize off-the-shelf search algorithms.
- [`gbfs_lazy.py`](python/examples/planning/gbfs_lazy.py) – Implement a custom search algorithm from scratch.

The Python interface for implementing search algorithms is:

### Lifted Planning

```py
# Recommended namespace aliases
import pytyr.formalism as tf
import pytyr.planning.lifted as tpl

# Parse and translate a task over a domain.
parser = tf.Parser("domain.pddl")
task = parser.parse_task("problem.pddl")

# Instantiate a lifted successor generator.
successor_generator = tpl.SuccessorGenerator(task)

# Get the initial node (state + metric value)
initial_node = successor_generator.get_initial_node()

# Get the labeled successor nodes (sequence of ground action + node)
labeled_successor_nodes = successor_generator.get_labeled_successor_nodes(initial_node)
```

### Grounded Planning

```py
# Recommended namespace aliases
import pytyr.formalism as tf
import pytyr.planning.ground as tpg

# Parse and translate a task over a domain.
parser = tf.Parser("domain.pddl")
task = parser.parse_task("problem.pddl")

# Instantiate a ground task.
ground_task = task.instantiate_ground_task()

# Instantiate a ground successor generator.
successor_generator = tpg.SuccessorGenerator(ground_task)

# Get the initial node (state + metric value)
initial_node = successor_generator.get_initial_node()

# Get the labeled successor nodes (sequence of ground action + node)
labeled_successor_nodes = successor_generator.get_labeled_successor_nodes(initial_node)
```

## C++ Interface

Instructions on how to build the library and executables are available [here](docs/BUILD.md).

The C++ interface for implementing search algorithms is:

### Lifted Planning

```cpp
#include <tyr/tyr.hpp>

// Recommended namespace aliases.
namespace tf = tyr::formalism;
namespace tp = tyr::planning;

// Parse and translate a task over a domain.
auto parser = tf::Parser("domain.pddl");
auto task = parser.parse_task("problem.pddl");

// Instantiate a lifted successor generator.
auto successor_generator = SuccessorGenerator<tp::LiftedTask>(task);

// Get the initial node (state + metric value).
auto initial_node = successor_generator.get_initial_node();

// Get the labeled successor nodes (sequence of ground action + node).
auto labeled_successor_nodes = successor_generator.get_labeled_successor_nodes(initial_node);

```

### Grounded Planning

```cpp
#include <tyr/tyr.hpp>

// Recommended namespace aliases.
namespace tf = tyr::formalism;
namespace tp = tyr::planning;

// Parse and translate a task over a domain.
auto parser = tf::Parser("domain.pddl");
auto task = parser.parse_task("problem.pddl");

// Instantiate a ground task.
auto ground_task = task.instantiate_ground_task();

// Instantiate a ground successor generator.
auto successor_generator = SuccessorGenerator<tp::GroundTask>(ground_task);

// Get the initial node (state + metric value).
auto initial_node = successor_generator.get_initial_node();

// Get the labeled successor nodes (sequence of ground action + node).
auto labeled_successor_nodes = successor_generator.get_labeled_successor_nodes(initial_node);

```
