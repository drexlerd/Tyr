"""
Create planning formalism structures.

This example demonstrates how to create planning tasks over the gripper domain.

Example usage (run from the repository root):

    python3 python/examples/formalism/planning/builder.py

Author: Dominik Drexler (dominik.drexler@liu.se)
"""

from pytyr.formalism.planning import (
    ParameterIndex,
    RepositoryFactory,
    ObjectBuilder,
    VariableBuilder,
    TermBuilder,
    StaticPredicateBuilder,
    FluentPredicateBuilder,
    StaticPredicateBindingBuilder,
    FluentPredicateBindingBuilder,
    StaticAtomBuilder,
    FluentAtomBuilder,
    StaticGroundAtomBuilder,
    FluentGroundAtomBuilder,
    StaticLiteralBuilder,
    FluentLiteralBuilder,
    ConjunctiveConditionBuilder,
    ConjunctiveEffectBuilder,
    ConditionalEffectBuilder,
    ActionBuilder,
    DomainBuilder,
    GroundConjunctiveConditionBuilder,
    FluentFDRVariableBuilder,
    FluentFDRFactBuilder,
    LiftedTaskBuilder,
)


def get(repository, builder):
    value, _ = repository.get_or_create(builder)
    return value


def make_term(repository, value):
    return get(repository, TermBuilder(value))


def make_static_atom(repository, predicate, terms):
    return get(repository, StaticAtomBuilder(predicate, terms))


def make_fluent_atom(repository, predicate, terms):
    return get(repository, FluentAtomBuilder(predicate, terms))


def make_static_literal(repository, predicate, terms, polarity=True):
    atom = make_static_atom(repository, predicate, terms)
    return get(repository, StaticLiteralBuilder(atom, polarity))


def make_fluent_literal(repository, predicate, terms, polarity=True):
    atom = make_fluent_atom(repository, predicate, terms)
    return get(repository, FluentLiteralBuilder(atom, polarity))


def make_static_ground_atom(repository, predicate, objects):
    binding = get(repository, StaticPredicateBindingBuilder(predicate, objects))
    return get(repository, StaticGroundAtomBuilder(binding))


def make_fluent_ground_atom(repository, predicate, objects):
    binding = get(repository, FluentPredicateBindingBuilder(predicate, objects))
    return get(repository, FluentGroundAtomBuilder(binding))


def main():
    factory = RepositoryFactory()

    # --------------------------------------------------------------------------
    # 1. Build the domain
    # --------------------------------------------------------------------------

    domain_repository = factory.create_repository()

    # Static predicates
    room = get(domain_repository, StaticPredicateBuilder("room", 1))
    ball = get(domain_repository, StaticPredicateBuilder("ball", 1))
    gripper = get(domain_repository, StaticPredicateBuilder("gripper", 1))

    # Fluent predicates
    at_robby = get(domain_repository, FluentPredicateBuilder("at-robby", 1))
    at = get(domain_repository, FluentPredicateBuilder("at", 2))
    free = get(domain_repository, FluentPredicateBuilder("free", 1))
    carry = get(domain_repository, FluentPredicateBuilder("carry", 2))

    # Constants
    rooma = get(domain_repository, ObjectBuilder("rooma"))
    roomb = get(domain_repository, ObjectBuilder("roomb"))

    # --------------------------------------------------------------------------
    # Lifted variables
    # --------------------------------------------------------------------------

    v_from = get(domain_repository, VariableBuilder("?from"))
    v_to = get(domain_repository, VariableBuilder("?to"))

    v_obj = get(domain_repository, VariableBuilder("?obj"))
    v_room = get(domain_repository, VariableBuilder("?room"))
    v_gripper = get(domain_repository, VariableBuilder("?gripper"))

    # Terms
    t_from = TermBuilder(ParameterIndex(0))
    t_to = TermBuilder(ParameterIndex(1))

    t_obj = TermBuilder(ParameterIndex(0))
    t_room = TermBuilder(ParameterIndex(1))
    t_gripper = TermBuilder(ParameterIndex(2))

    # --------------------------------------------------------------------------
    # move action
    # Preconditions:
    #   (room ?from) (room ?to) (at-robby ?from)
    # Effects:
    #   (at-robby ?to) and not (at-robby ?from)
    # --------------------------------------------------------------------------

    move_condition = get(
        domain_repository,
        ConjunctiveConditionBuilder(
            variables=[v_from, v_to],
            static_literals=[
                make_static_literal(domain_repository, room, [t_from], True),
                make_static_literal(domain_repository, room, [t_to], True),
            ],
            fluent_literals=[
                make_fluent_literal(domain_repository, at_robby, [t_from], True),
            ],
            derived_literals=[],
            numeric_constraints=[],
        ),
    )

    move_effect = get(
        domain_repository,
        ConjunctiveEffectBuilder(
            fluent_literals=[
                make_fluent_literal(domain_repository, at_robby, [t_to], True),
                make_fluent_literal(domain_repository, at_robby, [t_from], False),
            ],
            fluent_numeric_effects=[],
            auxiliary_numeric_effect=None,
        ),
    )

    move_conditional_effect = get(
        domain_repository,
        ConditionalEffectBuilder(
            variables=[],
            condition=get(
                domain_repository,
                ConjunctiveConditionBuilder(
                    variables=[],
                    static_literals=[],
                    fluent_literals=[],
                    derived_literals=[],
                    numeric_constraints=[],
                ),
            ),
            effect=move_effect,
        ),
    )

    move = get(
        domain_repository,
        ActionBuilder(
            name="move",
            original_arity=2,
            variables=[v_from, v_to],
            condition=move_condition,
            effects=[move_conditional_effect],
        ),
    )

    # --------------------------------------------------------------------------
    # pick action
    # Preconditions:
    #   (ball ?obj) (room ?room) (gripper ?gripper)
    #   (at ?obj ?room) (at-robby ?room) (free ?gripper)
    # Effects:
    #   (carry ?obj ?gripper)
    #   not (at ?obj ?room)
    #   not (free ?gripper)
    # --------------------------------------------------------------------------

    pick_condition = get(
        domain_repository,
        ConjunctiveConditionBuilder(
            variables=[v_obj, v_room, v_gripper],
            static_literals=[
                make_static_literal(domain_repository, ball, [t_obj], True),
                make_static_literal(domain_repository, room, [t_room], True),
                make_static_literal(domain_repository, gripper, [t_gripper], True),
            ],
            fluent_literals=[
                make_fluent_literal(domain_repository, at, [t_obj, t_room], True),
                make_fluent_literal(domain_repository, at_robby, [t_room], True),
                make_fluent_literal(domain_repository, free, [t_gripper], True),
            ],
            derived_literals=[],
            numeric_constraints=[],
        ),
    )

    pick_effect = get(
        domain_repository,
        ConjunctiveEffectBuilder(
            fluent_literals=[
                make_fluent_literal(domain_repository, carry, [t_obj, t_gripper], True),
                make_fluent_literal(domain_repository, at, [t_obj, t_room], False),
                make_fluent_literal(domain_repository, free, [t_gripper], False),
            ],
            fluent_numeric_effects=[],
            auxiliary_numeric_effect=None,
        ),
    )

    pick_conditional_effect = get(
        domain_repository,
        ConditionalEffectBuilder(
            variables=[],
            condition=get(
                domain_repository,
                ConjunctiveConditionBuilder(
                    variables=[],
                    static_literals=[],
                    fluent_literals=[],
                    derived_literals=[],
                    numeric_constraints=[],
                ),
            ),
            effect=pick_effect,
        ),
    )

    pick = get(
        domain_repository,
        ActionBuilder(
            name="pick",
            original_arity=3,
            variables=[v_obj, v_room, v_gripper],
            condition=pick_condition,
            effects=[pick_conditional_effect],
        ),
    )

    # --------------------------------------------------------------------------
    # drop action
    # Preconditions:
    #   (ball ?obj) (room ?room) (gripper ?gripper)
    #   (carry ?obj ?gripper) (at-robby ?room)
    # Effects:
    #   (at ?obj ?room)
    #   (free ?gripper)
    #   not (carry ?obj ?gripper)
    # --------------------------------------------------------------------------

    drop_condition = get(
        domain_repository,
        ConjunctiveConditionBuilder(
            variables=[v_obj, v_room, v_gripper],
            static_literals=[
                make_static_literal(domain_repository, ball, [t_obj], True),
                make_static_literal(domain_repository, room, [t_room], True),
                make_static_literal(domain_repository, gripper, [t_gripper], True),
            ],
            fluent_literals=[
                make_fluent_literal(domain_repository, carry, [t_obj, t_gripper], True),
                make_fluent_literal(domain_repository, at_robby, [t_room], True),
            ],
            derived_literals=[],
            numeric_constraints=[],
        ),
    )

    drop_effect = get(
        domain_repository,
        ConjunctiveEffectBuilder(
            fluent_literals=[
                make_fluent_literal(domain_repository, at, [t_obj, t_room], True),
                make_fluent_literal(domain_repository, free, [t_gripper], True),
                make_fluent_literal(domain_repository, carry, [t_obj, t_gripper], False),
            ],
            fluent_numeric_effects=[],
            auxiliary_numeric_effect=None,
        ),
    )

    drop_conditional_effect = get(
        domain_repository,
        ConditionalEffectBuilder(
            variables=[],
            condition=get(
                domain_repository,
                ConjunctiveConditionBuilder(
                    variables=[],
                    static_literals=[],
                    fluent_literals=[],
                    derived_literals=[],
                    numeric_constraints=[],
                ),
            ),
            effect=drop_effect,
        ),
    )

    drop = get(
        domain_repository,
        ActionBuilder(
            name="drop",
            original_arity=3,
            variables=[v_obj, v_room, v_gripper],
            condition=drop_condition,
            effects=[drop_conditional_effect],
        ),
    )

    domain = get(
        domain_repository,
        DomainBuilder(
            name="gripper-strips",
            static_predicates=[room, ball, gripper],
            fluent_predicates=[at_robby, at, free, carry],
            derived_predicates=[],
            static_functions=[],
            fluent_functions=[],
            auxiliary_function=None,
            constants=[rooma, roomb],
            actions=[move, pick, drop],
            axioms=[],
        ),
    )

    print("DOMAIN")
    print(domain)
    print()

    # --------------------------------------------------------------------------
    # 2. Build the lifted task
    # --------------------------------------------------------------------------

    task_repository = factory.create_repository(domain_repository)

    left = get(task_repository, ObjectBuilder("left"))
    right = get(task_repository, ObjectBuilder("right"))
    ball1 = get(task_repository, ObjectBuilder("ball1"))

    # Static atoms from typing
    static_atoms = [
        make_static_ground_atom(task_repository, room, [rooma]),
        make_static_ground_atom(task_repository, room, [roomb]),
        make_static_ground_atom(task_repository, gripper, [left]),
        make_static_ground_atom(task_repository, gripper, [right]),
        make_static_ground_atom(task_repository, ball, [ball1]),
    ]

    # Initial fluent atoms
    fluent_atoms = [
        make_fluent_ground_atom(task_repository, free, [left]),
        make_fluent_ground_atom(task_repository, free, [right]),
        make_fluent_ground_atom(task_repository, at, [ball1, rooma]),
        make_fluent_ground_atom(task_repository, at_robby, [rooma]),
    ]

    # --------------------------------------------------------------------------
    # Goal
    #
    # Your GroundConjunctiveConditionBuilder expects fluent goals as FDR facts,
    # not as fluent ground atoms.
    #
    # For the goal (at ball1 roomb), we create an FDR variable whose domain is
    # [ (at ball1 rooma), (at ball1 roomb) ] and then select value 1.
    # --------------------------------------------------------------------------

    at_ball1_rooma = make_fluent_ground_atom(task_repository, at, [ball1, rooma])
    at_ball1_roomb = make_fluent_ground_atom(task_repository, at, [ball1, roomb])

    at_ball1_var = get(
        task_repository,
        FluentFDRVariableBuilder([at_ball1_rooma, at_ball1_roomb]),
    )

    goal_at_ball1_roomb = get(
        task_repository,
        FluentFDRFactBuilder(at_ball1_var, 1),
    )

    goal = get(
        task_repository,
        GroundConjunctiveConditionBuilder(
            static_literals=[],
            fluent_facts=[goal_at_ball1_roomb],
            derived_literals=[],
            numeric_constraints=[],
        ),
    )

    task = get(
        task_repository,
        LiftedTaskBuilder(
            name="gripper-1",
            domain=domain,
            derived_predicates=[],
            objects=[left, right, ball1],
            static_atoms=static_atoms,
            fluent_atoms=fluent_atoms,
            static_fterm_values=[],
            fluent_fterm_values=[],
            auxiliary_fterm_value=None,
            goal=goal,
            metric=None,
            axioms=[],
        ),
    )

    print("TASK")
    print(task)


if __name__ == "__main__":
    main()