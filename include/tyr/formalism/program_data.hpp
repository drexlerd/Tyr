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

#ifndef TYR_FORMALISM_PROGRAM_DATA_HPP_
#define TYR_FORMALISM_PROGRAM_DATA_HPP_

#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function_index.hpp"
#include "tyr/formalism/ground_atom_index.hpp"
#include "tyr/formalism/ground_function_term_value_index.hpp"
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/predicate_index.hpp"
#include "tyr/formalism/program_index.hpp"
#include "tyr/formalism/rule_index.hpp"

namespace tyr
{
template<>
struct Data<formalism::Program>
{
    using Tag = formalism::Program;

    Index<formalism::Program> index;
    IndexList<formalism::Predicate<formalism::StaticTag>> static_predicates;
    IndexList<formalism::Predicate<formalism::FluentTag>> fluent_predicates;
    IndexList<formalism::Function<formalism::StaticTag>> static_functions;
    IndexList<formalism::Function<formalism::FluentTag>> fluent_functions;
    IndexList<formalism::Object> objects;
    IndexList<formalism::GroundAtom<formalism::StaticTag>> static_atoms;
    IndexList<formalism::GroundAtom<formalism::FluentTag>> fluent_atoms;
    IndexList<formalism::GroundFunctionTermValue<formalism::StaticTag>> static_function_values;
    IndexList<formalism::GroundFunctionTermValue<formalism::FluentTag>> fluent_function_values;
    IndexList<formalism::Rule> rules;

    Data() = default;
    Data(Index<formalism::Program> index,
         IndexList<formalism::Predicate<formalism::StaticTag>> static_predicates,
         IndexList<formalism::Predicate<formalism::FluentTag>> fluent_predicates,
         IndexList<formalism::Function<formalism::StaticTag>> static_functions,
         IndexList<formalism::Function<formalism::FluentTag>> fluent_functions,
         IndexList<formalism::Object> objects,
         IndexList<formalism::GroundAtom<formalism::StaticTag>> static_atoms,
         IndexList<formalism::GroundAtom<formalism::FluentTag>> fluent_atoms,
         IndexList<formalism::GroundFunctionTermValue<formalism::StaticTag>> static_function_values,
         IndexList<formalism::GroundFunctionTermValue<formalism::FluentTag>> fluent_function_values,
         IndexList<formalism::Rule> rules) :
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
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    template<formalism::IsStaticOrFluentTag T>
    const auto& get_predicates() const
    {
        if constexpr (std::same_as<T, formalism::StaticTag>)
        {
            return static_predicates;
        }
        else if constexpr (std::same_as<T, formalism::FluentTag>)
        {
            return fluent_predicates;
        }
    }

    template<formalism::IsStaticOrFluentTag T>
    const auto& get_functions() const
    {
        if constexpr (std::same_as<T, formalism::StaticTag>)
        {
            return static_functions;
        }
        else if constexpr (std::same_as<T, formalism::FluentTag>)
        {
            return fluent_functions;
        }
    }

    template<formalism::IsStaticOrFluentTag T>
    const auto& get_atoms() const
    {
        if constexpr (std::same_as<T, formalism::StaticTag>)
        {
            return static_atoms;
        }
        else if constexpr (std::same_as<T, formalism::FluentTag>)
        {
            return fluent_atoms;
        }
    }

    template<formalism::IsStaticOrFluentTag T>
    const auto& get_function_values() const
    {
        if constexpr (std::same_as<T, formalism::StaticTag>)
        {
            return static_function_values;
        }
        else if constexpr (std::same_as<T, formalism::FluentTag>)
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
