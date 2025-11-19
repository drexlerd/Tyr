/*
 * Copyright (C) 2025 Dominik Drexler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TYR_TESTS_UTILS_HPP_
#define TYR_TESTS_UTILS_HPP_

#include <tyr/formalism/formalism.hpp>

namespace tyr::tests
{
/*
 Create a program for gripper with 2 balls:
    (:predicates
        ; Static Predicates
        (object ?arg)
        (number ?arg)
        (room ?r)
        (ball ?b)
        (gripper ?g)
        ; Fluent Predicates
        (move ?from_0 ?to_0)
        (pick ?obj_0 ?room_0 ?gripper_0)
        (drop ?obj_0 ?room_0 ?gripper_0)
        (at-robby ?r)
        (at ?b ?r)
        (free ?g)
        (carry ?o ?g)
    )
    (:objects
        rooma roomb left right ball1 ball2
    )
    (:init
        (object rooma)
        (object roomb)
        (object left)
        (object right)
        (object ball1)
        (object ball2)
        (room rooma)
        (room roomb)
        (gripper left)
        (gripper right)
        (ball ball1)
        (ball ball2)
        (free left)
        (free right)
        (at ball1 rooma)
        (at ball2 rooma)
        (at-robby rooma)
    )
    (:rule move
        :parameters (?from_0 ?to_0)
        :precondition (and (object ?from_0) (object ?to_0) (room ?from_0) (room ?to_0) (at-robby ?from_0))
        :effect (move ?from_0 ?to_0)
    )
    (:rule pick
        :parameters (?obj_0 ?room_0 ?gripper_0)
        :precondition (and (object ?obj_0) (object ?room_0) (object ?gripper_0) (ball ?obj_0) (room ?room_0) (gripper ?gripper_0) (at ?obj_0 ?room_0) (at-robby
    ?room_0) (free ?gripper_0))
        :effect (pick ?obj_0 ?room_0 ?gripper_0)
        (and (carry ?obj_0 ?gripper_0) (not (at ?obj_0 ?room_0)) (not (free ?gripper_0)))
    )
    (:rule drop
        :parameters (?obj_0 ?room_0 ?gripper_0)
        :precondition (and (object ?obj_0) (object ?room_0) (object ?gripper_0) (ball ?obj_0) (room ?room_0) (gripper ?gripper_0) (at-robby ?room_0) (carry
    ?obj_0 ?gripper_0))
        :effect (drop ?obj_0 ?room_0 ?gripper_0)
    )
*/

formalism::ObjectIndexList create_objects(formalism::Object& builder) {}

formalism::PredicateIndex<formalism::StaticTag> create_static_predicates(formalism::Predicate<formalism::StaticTag>& builder) {}

std::pair<formalism::Program, formalism::Repository> create_example_problem()
{
    auto repository = formalism::Repository();
    auto variable_builder = formalism::Variable();
    auto object_builder = formalism::Object();
    auto static_predicate_builder = formalism::Predicate<formalism::StaticTag>();
    auto fluent_predicate_builder = formalism::Predicate<formalism::FluentTag>();
    auto static_atom_builder = formalism::Atom<formalism::StaticTag>();
    auto fluent_atom_builder = formalism::Atom<formalism::FluentTag>();
    auto static_literal_builder = formalism::Literal<formalism::StaticTag>();
    auto fluent_literal_builder = formalism::Literal<formalism::FluentTag>();
    auto static_ground_atom_builder = formalism::GroundAtom<formalism::StaticTag>();
    auto fluent_ground_atom_builder = formalism::GroundAtom<formalism::FluentTag>();
    auto static_ground_literal_builder = formalism::GroundLiteral<formalism::StaticTag>();
    auto fluent_ground_literal_builder = formalism::GroundLiteral<formalism::FluentTag>();
    auto rule_builder = formalism::Rule();
    auto program = formalism::Program();
}
}

#endif
