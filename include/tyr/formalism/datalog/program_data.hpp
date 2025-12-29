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

#ifndef TYR_FORMALISM_DATALOG_PROGRAM_DATA_HPP_
#define TYR_FORMALISM_DATALOG_PROGRAM_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/ground_atom_index.hpp"
#include "tyr/formalism/datalog/ground_function_term_value_index.hpp"
#include "tyr/formalism/datalog/program_index.hpp"
#include "tyr/formalism/datalog/rule_index.hpp"
#include "tyr/formalism/function_index.hpp"
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/predicate_index.hpp"

namespace tyr
{
template<>
struct Data<formalism::datalog::Program>
{
    Index<formalism::datalog::Program> index;
    IndexList<formalism::Predicate<formalism::StaticTag>> static_predicates;
    IndexList<formalism::Predicate<formalism::FluentTag>> fluent_predicates;
    IndexList<formalism::Function<formalism::StaticTag>> static_functions;
    IndexList<formalism::Function<formalism::FluentTag>> fluent_functions;
    IndexList<formalism::Object> objects;
    IndexList<formalism::datalog::GroundAtom<formalism::StaticTag>> static_atoms;
    IndexList<formalism::datalog::GroundAtom<formalism::FluentTag>> fluent_atoms;
    IndexList<formalism::datalog::GroundFunctionTermValue<formalism::StaticTag>> static_fterm_values;
    IndexList<formalism::datalog::GroundFunctionTermValue<formalism::FluentTag>> fluent_fterm_values;
    IndexList<formalism::datalog::Rule> rules;

    Data() = default;
    Data(Index<formalism::datalog::Program> index,
         IndexList<formalism::Predicate<formalism::StaticTag>> static_predicates,
         IndexList<formalism::Predicate<formalism::FluentTag>> fluent_predicates,
         IndexList<formalism::Function<formalism::StaticTag>> static_functions,
         IndexList<formalism::Function<formalism::FluentTag>> fluent_functions,
         IndexList<formalism::Object> objects,
         IndexList<formalism::datalog::GroundAtom<formalism::StaticTag>> static_atoms,
         IndexList<formalism::datalog::GroundAtom<formalism::FluentTag>> fluent_atoms,
         IndexList<formalism::datalog::GroundFunctionTermValue<formalism::StaticTag>> static_fterm_values,
         IndexList<formalism::datalog::GroundFunctionTermValue<formalism::FluentTag>> fluent_fterm_values,
         IndexList<formalism::datalog::Rule> rules) :
        index(index),
        static_predicates(std::move(static_predicates)),
        fluent_predicates(std::move(fluent_predicates)),
        static_functions(std::move(static_functions)),
        fluent_functions(std::move(fluent_functions)),
        objects(std::move(objects)),
        static_atoms(std::move(static_atoms)),
        fluent_atoms(std::move(fluent_atoms)),
        static_fterm_values(std::move(static_fterm_values)),
        fluent_fterm_values(std::move(fluent_fterm_values)),
        rules(std::move(rules))
    {
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    void clear() noexcept
    {
        tyr::clear(index);
        tyr::clear(static_predicates);
        tyr::clear(fluent_predicates);
        tyr::clear(static_functions);
        tyr::clear(fluent_functions);
        tyr::clear(objects);
        tyr::clear(static_atoms);
        tyr::clear(fluent_atoms);
        tyr::clear(static_fterm_values);
        tyr::clear(fluent_fterm_values);
        tyr::clear(rules);
    }

    template<formalism::FactKind T>
    const auto& get_predicates() const
    {
        if constexpr (std::same_as<T, formalism::StaticTag>)
            return static_predicates;
        else if constexpr (std::same_as<T, formalism::FluentTag>)
            return fluent_predicates;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }

    template<formalism::FactKind T>
    const auto& get_functions() const
    {
        if constexpr (std::same_as<T, formalism::StaticTag>)
            return static_functions;
        else if constexpr (std::same_as<T, formalism::FluentTag>)
            return fluent_functions;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }

    template<formalism::FactKind T>
    const auto& get_atoms() const
    {
        if constexpr (std::same_as<T, formalism::StaticTag>)
            return static_atoms;
        else if constexpr (std::same_as<T, formalism::FluentTag>)
            return fluent_atoms;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }

    template<formalism::FactKind T>
    const auto& get_fterm_values() const
    {
        if constexpr (std::same_as<T, formalism::StaticTag>)
            return static_fterm_values;
        else if constexpr (std::same_as<T, formalism::FluentTag>)
            return fluent_fterm_values;
        else
            static_assert(dependent_false<T>::value, "Missing case");
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
                        static_fterm_values,
                        fluent_fterm_values,
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
                        static_fterm_values,
                        fluent_fterm_values,
                        rules);
    }
};

}

#endif
