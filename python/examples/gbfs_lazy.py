
import pytyr.formalism.planning as fp
import pytyr.planning as p

from pathlib import Path

ROOT_DIR = (Path(__file__).parent.parent.parent).absolute()

def main():
    domain_filepath = ROOT_DIR / "data" / "gripper" / "domain.pddl"
    task_filepath = ROOT_DIR / "data" / "gripper" / "p-2-0.pddl"

    parser_options = p.ParserOptions()
    parser = p.Parser(domain_filepath, parser_options)
    lifted_task = parser.parse_task(task_filepath, parser_options)

    lifted_state_repository = p.lifted.StateRepository(lifted_task)
    lifted_init_state = lifted_state_repository.get_initial_state()
    print(lifted_init_state)

    lifted_succ_gen = p.lifted.SuccessorGenerator(lifted_task)
    lifted_init_node = lifted_succ_gen.get_initial_node()
    print(lifted_init_node)
    lifted_labeled_succ_nodes = lifted_succ_gen.get_labeled_successor_nodes(lifted_init_node)

    lifted_task.get_task()

    lifted_ff = p.lifted.FFHeuristic(lifted_task)
    lifted_astar_eager_options = p.lifted.astar_eager.Options()
    p.lifted.astar_eager.find_solution(lifted_task, lifted_succ_gen, lifted_ff, lifted_astar_eager_options)

    ground_task = lifted_task.instantiate_ground_task()

    ground_state_repository = p.ground.StateRepository(ground_task)
    ground_init_state = ground_state_repository.get_initial_state()
    print(ground_init_state)

    ground_succ_gen = p.ground.SuccessorGenerator(ground_task)
    ground_init_node = ground_succ_gen.get_initial_node()
    print(ground_init_node)
    ground_labeled_succ_nodes = ground_succ_gen.get_labeled_successor_nodes(ground_init_node)

    ground_blind = p.ground.BlindHeuristic()
    ground_astar_eager_options = p.ground.astar_eager.Options()
    p.ground.astar_eager.find_solution(ground_task, ground_succ_gen, ground_blind, ground_astar_eager_options)
    


if __name__ == "__main__":
    main()
