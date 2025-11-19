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

#ifndef TYR_FORMALISM_PROGRAM_HPP_
#define TYR_FORMALISM_PROGRAM_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_atom_index.hpp"
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/predicate_index.hpp"
#include "tyr/formalism/program_index.hpp"
#include "tyr/formalism/rule_index.hpp"

namespace tyr::formalism
{
struct Program
{
    ProgramIndex index;
    PredicateIndexList<StaticTag> static_predicates;
    PredicateIndexList<FluentTag> fluent_predicates;
    ObjectIndexList objects;
    GroundAtomIndexList<StaticTag> static_atoms;
    GroundAtomIndexList<FluentTag> fluent_atoms;
    RuleIndexList rules;

    using IndexType = ProgramIndex;

    Program() = default;
    Program(ProgramIndex index,
            PredicateIndexList<StaticTag> static_predicates,
            PredicateIndexList<FluentTag> fluent_predicates,
            ObjectIndexList objects,
            GroundAtomIndexList<StaticTag> static_atoms,
            GroundAtomIndexList<FluentTag> fluent_atoms,
            RuleIndexList rules) :
        index(index),
        static_predicates(std::move(static_predicates)),
        fluent_predicates(std::move(fluent_predicates)),
        objects(std::move(objects)),
        static_atoms(std::move(static_atoms)),
        fluent_atoms(std::move(fluent_atoms)),
        rules(std::move(rules))
    {
    }

    template<IsStaticOrFluentTag T>
    const auto& get_predicates() const
    {
        if constexpr (std::same_as<T, StaticTag>)
        {
            return static_predicates;
        }
        else if constexpr (std::same_as<T, FluentTag>)
        {
            return fluent_predicates;
        }
    }

    template<IsStaticOrFluentTag T>
    const auto& get_atoms() const
    {
        if constexpr (std::same_as<T, StaticTag>)
        {
            return static_atoms;
        }
        else if constexpr (std::same_as<T, FluentTag>)
        {
            return fluent_atoms;
        }
    }

    auto cista_members() const noexcept { return std::tie(index, static_predicates, fluent_predicates, objects, static_atoms, fluent_atoms, rules); }
    auto identifying_members() const noexcept { return std::tie(static_predicates, fluent_predicates, objects, static_atoms, fluent_atoms, rules); }
};

}

#endif
