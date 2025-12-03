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

#include <string>
#include <tyr/formalism/formalism.hpp>
#include <vector>

namespace tyr::tests
{
/*
 Create a program for gripper with 2 balls:
    (:predicates
        ; Static Predicates
        0 object / 1
        1 number / 1
        2 room / 1
        3 ball / 1
        4 gripper / 1
        ; Fluent Predicates
        0 at-robby / 1
        1 at / 2
        2 free / 1
        3 carry / 2
        4 move / 2
        5 pick / 3
        6 drop / 3
    )
    (:objects
        0 rooma
        1 roomb
        2 left
        3 right
        4 ball1
        5 ball2
    )
    (:init
        ; Static GroundAtoms
        0 (object rooma)
        1 (object roomb)
        2 (object left)
        3 (object right)
        4 (object ball1)
        5 (object ball2)
        6 (room rooma)
        7 (room roomb)
        8 (gripper left)
        9 (gripper right)
        10 (ball ball1)
        11 (ball ball2)
        ; Fluent GroundAtoms
        0 (free left)
        1 (free right)
        2 (at ball1 rooma)
        3 (at ball2 rooma)
        4 (at-robby rooma)
    )
    0 (:rule move
        :parameters
            (?from_0 ?to_0)
        :precondition
            (and (object ?from_0) (object ?to_0) (room ?from_0) (room ?to_0) (at-robby ?from_0))
        :effect
            (move ?from_0 ?to_0)
    )
    1 (:rule pick
        :parameters
            (?obj_0 ?room_0 ?gripper_0)
        :precondition
            (and (object ?obj_0) (object ?room_0) (object ?gripper_0) (ball ?obj_0) (room ?room_0)
            (gripper ?gripper_0) (at ?obj_0 ?room_0) (at-robby ?room_0) (free ?gripper_0))
        :effect
            (pick ?obj_0 ?room_0 ?gripper_0)
    )
    2 (:rule drop
        :parameters
            (?obj_0 ?room_0 ?gripper_0)
        :precondition
            (and (object ?obj_0) (object ?room_0) (object ?gripper_0) (ball ?obj_0) (room ?room_0)
                 (gripper ?gripper_0) (at-robby ?room_0) (carry ?obj_0 ?gripper_0))
        :effect
            (drop ?obj_0 ?room_0 ?gripper_0)
    )
*/

/*
    0 object / 1
    1 number / 1
    2 room / 1
    3 ball / 1
    4 gripper / 1
*/
inline IndexList<formalism::Predicate<formalism::StaticTag>> add_static_predicates(formalism::Repository& repository)
{
    auto result = IndexList<formalism::Predicate<formalism::StaticTag>> {};

    auto predicate_builder = Data<formalism::Predicate<formalism::StaticTag>> {};
    auto buffer = buffer::Buffer {};

    for (const auto& [name, arity] : std::vector<std::pair<std::string, size_t>> { { "object", 1 },  //
                                                                                   { "number", 1 },
                                                                                   { "room", 1 },
                                                                                   { "ball", 1 },
                                                                                   { "gripper", 1 } })
    {
        predicate_builder.name = name;
        predicate_builder.arity = arity;
        canonicalize(predicate_builder);
        result.push_back(repository.get_or_create(predicate_builder, buffer).first.get_index());
    }

    return result;
}

enum class GripperStaticPredicate : uint_t
{
    Object = 0,
    Number = 1,
    Room = 2,
    Ball = 3,
    Gripper = 4,
};

inline Index<formalism::Predicate<formalism::StaticTag>> convert(GripperStaticPredicate e) noexcept
{
    return Index<formalism::Predicate<formalism::StaticTag>>(static_cast<uint_t>(e));
}

/*
    0 at-robby / 1
    1 at / 2
    2 free / 1
    3 carry / 2
    4 move / 2
    5 pick / 3
    6 drop / 3
*/
inline IndexList<formalism::Predicate<formalism::FluentTag>> add_fluent_predicates(formalism::Repository& repository)
{
    auto result = IndexList<formalism::Predicate<formalism::FluentTag>> {};

    auto predicate_builder = Data<formalism::Predicate<formalism::FluentTag>> {};
    auto buffer = buffer::Buffer {};

    for (const auto& [name, arity] : std::vector<std::pair<std::string, size_t>> { { "at-robby", 1 },
                                                                                   { "at", 2 },
                                                                                   { "free", 1 },
                                                                                   { "carry", 2 },
                                                                                   { "move", 2 },
                                                                                   { "pick", 3 },
                                                                                   { "drop", 3 } })
    {
        predicate_builder.name = name;
        predicate_builder.arity = arity;
        canonicalize(predicate_builder);
        result.push_back(repository.get_or_create(predicate_builder, buffer).first.get_index());
    }

    return result;
}

enum class GripperFluentPredicate : uint_t
{
    AtRobby = 0,
    At = 1,
    Free = 2,
    Carry = 3,
    Move = 4,
    Pick = 5,
    Drop = 6,
};

inline Index<formalism::Predicate<formalism::FluentTag>> convert(GripperFluentPredicate e) noexcept
{
    return Index<formalism::Predicate<formalism::FluentTag>>(static_cast<uint_t>(e));
}

/*
    0 rooma
    1 roomb
    2 left
    3 right
    4 ball1
    5 ball2
*/
inline IndexList<formalism::Object> add_objects(formalism::Repository& repository)
{
    auto result = IndexList<formalism::Object> {};

    auto object_builder = Data<formalism::Object> {};
    auto buffer = buffer::Buffer {};

    for (const auto& name : std::vector<std::string> { "rooma", "roomb", "left", "right", "ball1", "ball2" })
    {
        object_builder.name = name;
        canonicalize(object_builder);
        result.push_back(repository.get_or_create(object_builder, buffer).first.get_index());
    }

    return result;
}

enum class GripperObject : uint_t
{
    RoomA = 0,
    RoomB = 1,
    Left = 2,
    Right = 3,
    Ball1 = 4,
    Ball2 = 5,
};

inline Index<formalism::Object> convert(GripperObject e) noexcept { return Index<formalism::Object>(static_cast<uint_t>(e)); }

/*
    0 (object rooma)
    1 (object roomb)
    2 (object left)
    3 (object right)
    4 (object ball1)
    5 (object ball2)
    6 (room rooma)
    7 (room roomb)
    8 (gripper left)
    9 (gripper right)
    10 (ball ball1)
    11 (ball ball2)
*/
inline IndexList<formalism::GroundAtom<formalism::StaticTag>> add_static_ground_atoms(formalism::Repository& repository)
{
    auto result = IndexList<formalism::GroundAtom<formalism::StaticTag>> {};

    auto ground_atom_builder = Data<formalism::GroundAtom<formalism::StaticTag>> {};
    auto buffer = buffer::Buffer {};

    for (const auto& [predicate, atoms] : std::vector<std::pair<GripperStaticPredicate, std::vector<std::vector<GripperObject>>>> {
             { GripperStaticPredicate::Object,
               { { GripperObject::RoomA },
                 { GripperObject::RoomB },
                 { GripperObject::Left },
                 { GripperObject::Right },
                 { GripperObject::Ball1 },
                 { GripperObject::Ball2 } } },
             { GripperStaticPredicate::Room, { { GripperObject::RoomA }, { GripperObject::RoomB } } },
             { GripperStaticPredicate::Gripper, { { GripperObject::Left }, { GripperObject::Right } } },
             { GripperStaticPredicate::Ball, { { GripperObject::Ball1 }, { GripperObject::Ball2 } } } })
    {
        ground_atom_builder.predicate = convert(predicate);

        for (const auto& atom : atoms)
        {
            ground_atom_builder.objects.clear();
            for (const auto& term : atom)
            {
                ground_atom_builder.objects.push_back(convert(term));
            }
            canonicalize(ground_atom_builder);
            result.push_back(repository.get_or_create(ground_atom_builder, buffer).first.get_index());
        }
    }

    return result;
}

/*
    0 (free left)
    1 (free right)
    2 (at ball1 rooma)
    3 (at ball2 rooma)
    4 (at-robby rooma)
*/
inline IndexList<formalism::GroundAtom<formalism::FluentTag>> add_fluent_ground_atoms(formalism::Repository& repository)
{
    auto result = IndexList<formalism::GroundAtom<formalism::FluentTag>> {};

    auto ground_atom_builder = Data<formalism::GroundAtom<formalism::FluentTag>> {};
    auto buffer = buffer::Buffer {};

    for (const auto& [predicate, atoms] : std::vector<std::pair<GripperFluentPredicate, std::vector<std::vector<GripperObject>>>> {
             { GripperFluentPredicate::Free, { { GripperObject::Left }, { GripperObject::Right } } },
             { GripperFluentPredicate::At, { { GripperObject::Ball1, GripperObject::RoomA }, { GripperObject::Ball2, GripperObject::RoomA } } },
             { GripperFluentPredicate::AtRobby, { { GripperObject::RoomA } } } })
    {
        ground_atom_builder.predicate = convert(predicate);
        for (const auto& atom : atoms)
        {
            ground_atom_builder.objects.clear();
            for (const auto& term : atom)
            {
                ground_atom_builder.objects.push_back(convert(term));
            }
            canonicalize(ground_atom_builder);
            result.push_back(repository.get_or_create(ground_atom_builder, buffer).first.get_index());
        }
    }
    return result;
}

/*
0 (:rule move
    :parameters
        (?from_0 ?to_0)
    :precondition
        (and (object ?from_0) (object ?to_0) (room ?from_0) (room ?to_0) (at-robby ?from_0))
    :effect
        (move ?from_0 ?to_0)
)
*/
inline Index<formalism::Rule> add_rule_move(formalism::Repository& repository)
{
    auto variable_builder = Data<formalism::Variable> {};
    auto static_atom_builder = Data<formalism::Atom<formalism::StaticTag>> {};
    auto fluent_atom_builder = Data<formalism::Atom<formalism::FluentTag>> {};
    auto static_literal_builder = Data<formalism::Literal<formalism::StaticTag>> {};
    auto fluent_literal_builder = Data<formalism::Literal<formalism::FluentTag>> {};
    auto conjunctive_condition_builder = Data<formalism::ConjunctiveCondition> {};
    auto rule_builder = Data<formalism::Rule> {};
    auto buffer = buffer::Buffer {};

    for (const auto& name : std::vector<std::string> { "?from_0", "?to_0" })
    {
        variable_builder.name = name;
        canonicalize(variable_builder);
        const auto variable = repository.get_or_create(variable_builder, buffer).first;
        conjunctive_condition_builder.variables.push_back(variable.get_index());
    }

    for (const auto& [predicate, polarity_and_params_per_atoms] :
         std::vector<std::pair<GripperStaticPredicate, std::vector<std::pair<bool, std::vector<size_t>>>>> {
             { GripperStaticPredicate::Object, { { true, { 0 } }, { true, { 1 } } } },
             { GripperStaticPredicate::Room, { { true, { 0 } }, { true, { 1 } } } } })
    {
        static_atom_builder.predicate = convert(predicate);

        for (const auto& [polarity, params] : polarity_and_params_per_atoms)
        {
            static_atom_builder.terms.clear();
            for (const auto& param : params)
            {
                static_atom_builder.terms.push_back(Data<formalism::Term>(formalism::ParameterIndex(param)));
            }
            canonicalize(static_atom_builder);
            const auto atom_i = repository.get_or_create(static_atom_builder, buffer).first;
            static_literal_builder.atom = atom_i.get_index();
            static_literal_builder.polarity = polarity;
            canonicalize(static_literal_builder);
            const auto literal_i = repository.get_or_create(static_literal_builder, buffer).first;
            conjunctive_condition_builder.static_literals.push_back(literal_i.get_index());
        }
    }

    for (const auto& [predicate, polarity_and_params_per_atoms] :
         std::vector<std::pair<GripperFluentPredicate, std::vector<std::pair<bool, std::vector<size_t>>>>> {
             { GripperFluentPredicate::AtRobby, { { true, { 0 } } } } })
    {
        fluent_atom_builder.predicate = convert(predicate);

        for (const auto& [polarity, params] : polarity_and_params_per_atoms)
        {
            fluent_atom_builder.terms.clear();
            for (const auto& param : params)
            {
                fluent_atom_builder.terms.push_back(Data<formalism::Term>(formalism::ParameterIndex(param)));
            }
            canonicalize(fluent_atom_builder);
            const auto atom_i = repository.get_or_create(fluent_atom_builder, buffer).first;
            fluent_literal_builder.atom = atom_i.get_index();
            fluent_literal_builder.polarity = polarity;
            canonicalize(fluent_literal_builder);
            const auto literal_i = repository.get_or_create(fluent_literal_builder, buffer).first;
            conjunctive_condition_builder.fluent_literals.push_back(literal_i.get_index());
        }
    }

    canonicalize(conjunctive_condition_builder);
    const auto conjunctive_condition = repository.get_or_create(conjunctive_condition_builder, buffer).first;
    rule_builder.body = conjunctive_condition.get_index();

    fluent_atom_builder.predicate = convert(GripperFluentPredicate::Move);
    fluent_atom_builder.terms.clear();
    fluent_atom_builder.terms.push_back(Data<formalism::Term>(formalism::ParameterIndex(0)));
    fluent_atom_builder.terms.push_back(Data<formalism::Term>(formalism::ParameterIndex(1)));
    canonicalize(fluent_atom_builder);
    const auto head = repository.get_or_create(fluent_atom_builder, buffer).first;
    rule_builder.head = head.get_index();

    canonicalize(rule_builder);
    return repository.get_or_create(rule_builder, buffer).first.get_index();
}

/*
1 (:rule pick
    :parameters
        (?obj_0 ?room_0 ?gripper_0)
    :precondition
        (and (object ?obj_0) (object ?room_0) (object ?gripper_0) (ball ?obj_0) (room ?room_0)
        (gripper ?gripper_0) (at ?obj_0 ?room_0) (at-robby ?room_0) (free ?gripper_0))
    :effect
        (pick ?obj_0 ?room_0 ?gripper_0)
)
*/
inline Index<formalism::Rule> add_rule_pick(formalism::Repository& repository)
{
    auto variable_builder = Data<formalism::Variable> {};
    auto static_atom_builder = Data<formalism::Atom<formalism::StaticTag>> {};
    auto fluent_atom_builder = Data<formalism::Atom<formalism::FluentTag>> {};
    auto static_literal_builder = Data<formalism::Literal<formalism::StaticTag>> {};
    auto fluent_literal_builder = Data<formalism::Literal<formalism::FluentTag>> {};
    auto conjunctive_condition_builder = Data<formalism::ConjunctiveCondition> {};
    auto rule_builder = Data<formalism::Rule> {};
    auto buffer = buffer::Buffer {};

    for (const auto& name : std::vector<std::string> { "?obj_0", "?room_0", "?gripper_0" })
    {
        variable_builder.name = name;
        canonicalize(variable_builder);
        const auto variable = repository.get_or_create(variable_builder, buffer).first;
        conjunctive_condition_builder.variables.push_back(variable.get_index());
    }

    for (const auto& [predicate, polarity_and_params_per_atoms] :
         std::vector<std::pair<GripperStaticPredicate, std::vector<std::pair<bool, std::vector<size_t>>>>> {
             { GripperStaticPredicate::Object, { { true, { 0 } }, { true, { 1 } }, { true, { 2 } } } },
             { GripperStaticPredicate::Ball, { { true, { 0 } } } },
             { GripperStaticPredicate::Room, { { true, { 1 } } } },
             { GripperStaticPredicate::Gripper, { { true, { 2 } } } } })
    {
        static_atom_builder.predicate = convert(predicate);

        for (const auto& [polarity, params] : polarity_and_params_per_atoms)
        {
            static_atom_builder.terms.clear();
            for (const auto& param : params)
            {
                static_atom_builder.terms.push_back(Data<formalism::Term>(formalism::ParameterIndex(param)));
            }
            canonicalize(static_atom_builder);
            const auto atom_i = repository.get_or_create(static_atom_builder, buffer).first;
            static_literal_builder.atom = atom_i.get_index();
            static_literal_builder.polarity = polarity;
            canonicalize(static_literal_builder);
            const auto literal_i = repository.get_or_create(static_literal_builder, buffer).first;
            conjunctive_condition_builder.static_literals.push_back(literal_i.get_index());
        }
    }

    for (const auto& [predicate, polarity_and_params_per_atoms] :
         std::vector<std::pair<GripperFluentPredicate, std::vector<std::pair<bool, std::vector<size_t>>>>> {
             { GripperFluentPredicate::At, { { true, { 0, 1 } } } },
             { GripperFluentPredicate::AtRobby, { { true, { 1 } } } },
             { GripperFluentPredicate::Free, { { true, { 2 } } } } })
    {
        fluent_atom_builder.predicate = convert(predicate);

        for (const auto& [polarity, params] : polarity_and_params_per_atoms)
        {
            fluent_atom_builder.terms.clear();
            for (const auto& param : params)
            {
                fluent_atom_builder.terms.push_back(Data<formalism::Term>(formalism::ParameterIndex(param)));
            }
            canonicalize(fluent_atom_builder);
            const auto atom_i = repository.get_or_create(fluent_atom_builder, buffer).first;
            fluent_literal_builder.atom = atom_i.get_index();
            fluent_literal_builder.polarity = polarity;
            canonicalize(fluent_literal_builder);
            const auto literal_i = repository.get_or_create(fluent_literal_builder, buffer).first;
            conjunctive_condition_builder.fluent_literals.push_back(literal_i.get_index());
        }
    }

    canonicalize(conjunctive_condition_builder);
    const auto conjunctive_condition = repository.get_or_create(conjunctive_condition_builder, buffer).first;
    rule_builder.body = conjunctive_condition.get_index();

    fluent_atom_builder.predicate = convert(GripperFluentPredicate::Pick);
    fluent_atom_builder.terms.clear();
    fluent_atom_builder.terms.push_back(Data<formalism::Term>(formalism::ParameterIndex(0)));
    fluent_atom_builder.terms.push_back(Data<formalism::Term>(formalism::ParameterIndex(1)));
    fluent_atom_builder.terms.push_back(Data<formalism::Term>(formalism::ParameterIndex(2)));
    canonicalize(fluent_atom_builder);
    const auto head = repository.get_or_create(fluent_atom_builder, buffer).first;
    rule_builder.head = head.get_index();

    canonicalize(rule_builder);
    return repository.get_or_create(rule_builder, buffer).first.get_index();
}

/*
2 (:rule drop
    :parameters
        (?obj_0 ?room_0 ?gripper_0)
    :precondition
        (and (object ?obj_0) (object ?room_0) (object ?gripper_0) (ball ?obj_0) (room ?room_0)
                (gripper ?gripper_0) (at-robby ?room_0) (carry ?obj_0 ?gripper_0))
    :effect
        (drop ?obj_0 ?room_0 ?gripper_0)
)
*/
inline Index<formalism::Rule> add_rule_drop(formalism::Repository& repository)
{
    auto variable_builder = Data<formalism::Variable> {};
    auto static_atom_builder = Data<formalism::Atom<formalism::StaticTag>> {};
    auto fluent_atom_builder = Data<formalism::Atom<formalism::FluentTag>> {};
    auto static_literal_builder = Data<formalism::Literal<formalism::StaticTag>> {};
    auto fluent_literal_builder = Data<formalism::Literal<formalism::FluentTag>> {};
    auto conjunctive_condition_builder = Data<formalism::ConjunctiveCondition> {};
    auto rule_builder = Data<formalism::Rule> {};
    auto buffer = buffer::Buffer {};

    for (const auto& name : std::vector<std::string> { "?obj_0", "?room_0", "?gripper_0" })
    {
        variable_builder.name = name;
        canonicalize(variable_builder);
        const auto variable = repository.get_or_create(variable_builder, buffer).first;
        conjunctive_condition_builder.variables.push_back(variable.get_index());
    }

    for (const auto& [predicate, polarity_and_params_per_atoms] :
         std::vector<std::pair<GripperStaticPredicate, std::vector<std::pair<bool, std::vector<size_t>>>>> {
             { GripperStaticPredicate::Object, { { true, { 0 } }, { true, { 1 } }, { true, { 2 } } } },
             { GripperStaticPredicate::Ball, { { true, { 0 } } } },
             { GripperStaticPredicate::Room, { { true, { 1 } } } },
             { GripperStaticPredicate::Gripper, { { true, { 2 } } } } })
    {
        static_atom_builder.predicate = convert(predicate);

        for (const auto& [polarity, params] : polarity_and_params_per_atoms)
        {
            static_atom_builder.terms.clear();
            for (const auto& param : params)
            {
                static_atom_builder.terms.push_back(Data<formalism::Term>(formalism::ParameterIndex(param)));
            }
            canonicalize(static_atom_builder);
            const auto atom_i = repository.get_or_create(static_atom_builder, buffer).first;
            static_literal_builder.atom = atom_i.get_index();
            static_literal_builder.polarity = polarity;
            canonicalize(static_literal_builder);
            const auto literal_i = repository.get_or_create(static_literal_builder, buffer).first;
            conjunctive_condition_builder.static_literals.push_back(literal_i.get_index());
        }
    }

    for (const auto& [predicate, polarity_and_params_per_atoms] :
         std::vector<std::pair<GripperFluentPredicate, std::vector<std::pair<bool, std::vector<size_t>>>>> {
             { GripperFluentPredicate::AtRobby, { { true, { 1 } } } },
             { GripperFluentPredicate::Carry, { { true, { 0, 2 } } } } })
    {
        fluent_atom_builder.predicate = convert(predicate);

        for (const auto& [polarity, params] : polarity_and_params_per_atoms)
        {
            fluent_atom_builder.terms.clear();
            for (const auto& param : params)
            {
                fluent_atom_builder.terms.push_back(Data<formalism::Term>(formalism::ParameterIndex(param)));
            }
            canonicalize(fluent_atom_builder);
            const auto atom_i = repository.get_or_create(fluent_atom_builder, buffer).first;
            fluent_literal_builder.atom = atom_i.get_index();
            fluent_literal_builder.polarity = polarity;
            canonicalize(fluent_literal_builder);
            const auto literal_i = repository.get_or_create(fluent_literal_builder, buffer).first;
            conjunctive_condition_builder.fluent_literals.push_back(literal_i.get_index());
        }
    }

    canonicalize(conjunctive_condition_builder);
    const auto conjunctive_condition = repository.get_or_create(conjunctive_condition_builder, buffer).first;
    rule_builder.body = conjunctive_condition.get_index();

    fluent_atom_builder.predicate = convert(GripperFluentPredicate::Drop);
    fluent_atom_builder.terms.clear();
    fluent_atom_builder.terms.push_back(Data<formalism::Term>(formalism::ParameterIndex(0)));
    fluent_atom_builder.terms.push_back(Data<formalism::Term>(formalism::ParameterIndex(1)));
    fluent_atom_builder.terms.push_back(Data<formalism::Term>(formalism::ParameterIndex(2)));
    canonicalize(fluent_atom_builder);
    const auto head = repository.get_or_create(fluent_atom_builder, buffer).first;
    rule_builder.head = head.get_index();

    canonicalize(rule_builder);
    return repository.get_or_create(rule_builder, buffer).first.get_index();
}

enum class GripperRule : uint_t
{
    Move = 0,
    Pick = 1,
    Drop = 2,
};

inline Index<formalism::Rule> convert(GripperRule e) noexcept { return Index<formalism::Rule>(static_cast<uint_t>(e)); }

inline std::pair<View<Index<formalism::Program>, formalism::Repository>, formalism::RepositoryPtr> create_example_problem()
{
    auto repository_ptr = std::make_shared<formalism::Repository>();
    auto& repository = *repository_ptr;
    auto program_builder = Data<formalism::Program> {};
    auto buffer = buffer::Buffer {};

    program_builder.static_predicates = add_static_predicates(repository);

    program_builder.fluent_predicates = add_fluent_predicates(repository);
    program_builder.objects = add_objects(repository);
    program_builder.static_atoms = add_static_ground_atoms(repository);
    program_builder.fluent_atoms = add_fluent_ground_atoms(repository);

    program_builder.rules.push_back(add_rule_move(repository));
    program_builder.rules.push_back(add_rule_pick(repository));
    program_builder.rules.push_back(add_rule_drop(repository));

    canonicalize(program_builder);
    auto program = repository.get_or_create(program_builder, buffer).first;

    // We cannot return view because repository hasnt find a stable location yet.
    return { program, repository_ptr };
}
}

#endif
