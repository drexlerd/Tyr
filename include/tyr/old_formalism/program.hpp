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
#include "tyr/formalism/function_index.hpp"
#include "tyr/formalism/ground_atom_index.hpp"
#include "tyr/formalism/ground_function_term_value_index.hpp"
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
    FunctionIndexList<StaticTag> static_functions;
    FunctionIndexList<FluentTag> fluent_functions;
    ObjectIndexList objects;
    GroundAtomIndexList<StaticTag> static_atoms;
    GroundAtomIndexList<FluentTag> fluent_atoms;
    GroundFunctionTermValueIndexList<StaticTag> static_function_values;
    GroundFunctionTermValueIndexList<FluentTag> fluent_function_values;
    RuleIndexList rules;

    Program() = default;
    Program(ProgramIndex index,
            PredicateIndexList<StaticTag> static_predicates,
            PredicateIndexList<FluentTag> fluent_predicates,
            FunctionIndexList<StaticTag> static_functions,
            FunctionIndexList<FluentTag> fluent_functions,
            ObjectIndexList objects,
            GroundAtomIndexList<StaticTag> static_atoms,
            GroundAtomIndexList<FluentTag> fluent_atoms,
            GroundFunctionTermValueIndexList<StaticTag> static_function_values,
            GroundFunctionTermValueIndexList<FluentTag> fluent_function_values,
            RuleIndexList rules) :
        index(index),
        static_predicates(std::move(static_predicates)),
        fluent_predicates(std::move(fluent_predicates)),
        static_functions(std::move(static_functions)),
        fluent_functions(std::move(fluent_functions)),
        objects(std::move(objects)),
        static_atoms(std::move(static_atoms)),
        fluent_atoms(std::move(fluent_atoms)),
        static_function_values(std::move(static_function_values)),
        fluent_function_values(std::move(fluent_function_values)),
        rules(std::move(rules))
    {
    }
    Program(const Program& other) = delete;
    Program& operator=(const Program& other) = delete;
    Program(Program&& other) = default;
    Program& operator=(Program&& other) = default;

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
    const auto& get_functions() const
    {
        if constexpr (std::same_as<T, StaticTag>)
        {
            return static_functions;
        }
        else if constexpr (std::same_as<T, FluentTag>)
        {
            return fluent_functions;
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

    template<IsStaticOrFluentTag T>
    const auto& get_function_values() const
    {
        if constexpr (std::same_as<T, StaticTag>)
        {
            return static_function_values;
        }
        else if constexpr (std::same_as<T, FluentTag>)
        {
            return fluent_function_values;
        }
    }

    auto cista_members() const noexcept
    {
        return std::tie(index,
                        static_predicates,
                        fluent_predicates,
                        static_functions,
                        fluent_functions,
                        objects,
                        static_atoms,
                        fluent_atoms,
                        static_function_values,
                        fluent_function_values,
                        rules);
    }
    auto identifying_members() const noexcept
    {
        return std::tie(static_predicates,
                        fluent_predicates,
                        static_functions,
                        fluent_functions,
                        objects,
                        static_atoms,
                        fluent_atoms,
                        static_function_values,
                        fluent_function_values,
                        rules);
    }
};

}

#endif
