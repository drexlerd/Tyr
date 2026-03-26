"""
Off-the-shelf A* eager with customization points explained.

Example usage (run from the repository root):

    python3 python/examples/planning/astar_eager.py \
        -d data/gripper/domain.pddl \
        -p data/gripper/test_problem.pddl

Author: Dominik Drexler (dominik.drexler@liu.se)
"""

import argparse

from pathlib import Path

from pytyr.common import (
    ExecutionContext
)

from pytyr.formalism.planning import (
    ParserOptions, 
    Parser, 
    GroundConjunctiveCondition, 
    GroundAction
)

# Note: we can easily switch between lifted and ground planning by swapping the submodule, 
# e.g., from pytyr.planning.ground import ..., and from pytyr.planning.ground.astar_eager import ...
# However, some heuristics, like MaxRPGHeuristic might not be available yet.
from pytyr.planning.lifted import (
    Task, 
    SuccessorGenerator, 
    Heuristic, 
    FFRPGHeuristic, 
    AddRPGHeuristic, 
    MaxRPGHeuristic, 
    GoalCountHeuristic, 
    PruningStrategy, 
    GoalStrategy, 
    TaskGoalStrategy, 
    State, 
    Node, 
    LabeledNode, 
    Plan, 
    Task
)

from pytyr.planning.lifted.astar_eager import (
    Options, 
    EventHandler, 
    DefaultEventHandler, 
    find_solution
)

# Lazy Greedy Best-First Search (GBFS) exposes an almost identical interface,
# although some options and EventHandler events differ slightly.
# To switch the search algorithm, typically only the following import needs to be changed:
#
# from pytyr.planning.lifted.gbfs_lazy import (
#     Options, 
#     EventHandler, 
#     DefaultEventHandler, 
#     find_solution
# )

class CustomHeuristic(Heuristic):
    """
    CustomHeuristic derives from Heuristic to implement custom heuristics.
    """
    def set_goal(self, goal : GroundConjunctiveCondition) -> None:
        pass

    def evaluate(self, state : State) -> float:
        pass

    def get_preferred_actions(self) -> set[GroundAction]:
        pass


class CustomPruningStrategy(PruningStrategy):
    """
    CustomPruningStrategy derives from PruningStrategy to implement custom strategies to prune states during search.

    A prominent example is Iterative Width (IW) that prunes state in a breadth-first (BrFS) search based on a state novelty criteria.
    """
    def should_prune_state(self, state: State) -> bool: 
        """ Is checked for the initial state. """
        pass

    def should_prune_successor_state(self, state: State, succ_state: State, is_new_succ_state: bool) -> bool:
        """ Is checked for every generated state. """
        pass


class CustomGoalStrategy(GoalStrategy):
    """
    CustomGoalStrategy derives from GoalStrategy to implement custom strategies to terminate search upon reaching a goal state.

    A prominent example is Serialized Iterative Width (SIW) that repeateadly calls IW to greedily achieve one goal atom at a time.
    """
    def is_static_goal_satisfied(self) -> bool: 
        """ Is checked before running a search. """
        pass

    def is_dynamic_goal_satisfied(self, state: State) -> bool: 
        """ Is checked for every generated state. """
        pass


class CustomEventHandler(EventHandler):
    """
    CustomEventHandler derives from EventHandler to implement custom ways to react on events triggered during an A* eager search.

    We can use it for instance to create a search tree, or even a complete state space 
    when running A* with a blind heuristic and a goal strategy that always returns False.
    We can also use it to collect custom search statistics.
    """
    def on_expand_node(self, node: Node) -> None: 
        pass

    def on_expand_goal_node(self, node: Node) -> None:
        pass

    def on_generate_node(self, labeled_succ_node: LabeledNode) -> None:
        pass

    def on_generate_node_relaxed(self, labeled_succ_node: LabeledNode) -> None:
        pass

    def on_generate_node_not_relaxed(self, labeled_succ_node: LabeledNode) -> None:
        pass

    def on_close_node(self, node: Node) -> None:
        pass

    def on_prune_node(self, node: Node) -> None:
        pass

    def on_start_search(self, node: Node, f_value: float) -> None:
        pass

    def on_finish_f_layer(self, f_value: float) -> None:
        pass

    def on_end_search(self) -> None:
        pass

    def on_solved(self, plan: Plan) -> None:
        pass

    def on_unsolvable(self) -> None:
        pass

    def on_exhausted(self) -> None:
        pass

    
def main():
    arg_parser = argparse.ArgumentParser(description="A* Eager Search.")
    arg_parser.add_argument("-d", "--domain-filepath", type=Path, required=True, help="Path to a PDDL domain file.")
    arg_parser.add_argument("-p", "--task-filepath", type=Path, required=True, help="Path to PDDL task file.")
    args = arg_parser.parse_args()

    domain_filepath : Path = args.domain_filepath
    task_filepath : Path = args.task_filepath

    execution_context = ExecutionContext(2)
    parser_options = ParserOptions()
    parser = Parser(domain_filepath, parser_options)
    lifted_task = Task(parser.parse_task(task_filepath, parser_options))
    heuristic = GoalCountHeuristic(lifted_task)
    successor_generator = SuccessorGenerator(lifted_task, execution_context)

    options = Options()                               # Lifted search is parallelized but only useful on large tasks.
    options.event_handler = DefaultEventHandler(0)         # Collects and prints statistics. If verbosity >= 2, then also prints labeled nodes.
    options.goal_strategy = TaskGoalStrategy(lifted_task)  # Terminates the search when reaching a state that satisfies the task's goal.
    options.pruning_strategy = PruningStrategy()           # Never prunes

    search_result = find_solution(lifted_task, successor_generator, heuristic, options)
 
    print("Search status:", search_result.status)

    plan = search_result.plan

    if plan is not None:
        print(f"Found plan with length {plan.get_length()} and cost {plan.get_cost()}")
        print(plan)
    else:
        print("No solution was found.")


if __name__ == "__main__":
    main()