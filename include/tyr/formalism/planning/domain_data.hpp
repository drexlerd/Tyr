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

#ifndef TYR_FORMALISM_PLANNING_DOMAIN_DATA_HPP_
#define TYR_FORMALISM_PLANNING_DOMAIN_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function_index.hpp"
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/planning/action_index.hpp"
#include "tyr/formalism/planning/axiom_index.hpp"
#include "tyr/formalism/planning/domain_index.hpp"
#include "tyr/formalism/planning/task_index.hpp"
#include "tyr/formalism/predicate_index.hpp"

namespace tyr
{

template<>
struct Data<formalism::Domain>
{
    using Tag = formalism::Domain;

    Index<formalism::Domain> index;
    ::cista::offset::string name;
    IndexList<formalism::Predicate<formalism::StaticTag>> static_predicates;
    IndexList<formalism::Predicate<formalism::FluentTag>> fluent_predicates;
    IndexList<formalism::Predicate<formalism::DerivedTag>> derived_predicates;
    IndexList<formalism::Function<formalism::StaticTag>> static_functions;
    IndexList<formalism::Function<formalism::FluentTag>> fluent_functions;
    ::cista::optional<Index<formalism::Function<formalism::AuxiliaryTag>>> auxiliary_function;
    IndexList<formalism::Object> constants;
    IndexList<formalism::Action> actions;
    IndexList<formalism::Axiom> axioms;

    Data() = default;
    Data(Index<formalism::Domain> index,
         ::cista::offset::string name,
         IndexList<formalism::Predicate<formalism::StaticTag>> static_predicates,
         IndexList<formalism::Predicate<formalism::FluentTag>> fluent_predicates,
         IndexList<formalism::Predicate<formalism::DerivedTag>> derived_predicates,
         IndexList<formalism::Function<formalism::StaticTag>> static_functions,
         IndexList<formalism::Function<formalism::FluentTag>> fluent_functions,
         ::cista::optional<Index<formalism::Function<formalism::AuxiliaryTag>>> auxiliary_function,
         IndexList<formalism::Object> constants,
         IndexList<formalism::Action> actions,
         IndexList<formalism::Axiom> axioms) :
        index(index),
        name(std::move(name)),
        static_predicates(std::move(static_predicates)),
        fluent_predicates(std::move(fluent_predicates)),
        derived_predicates(std::move(derived_predicates)),
        static_functions(std::move(static_functions)),
        fluent_functions(std::move(fluent_functions)),
        auxiliary_function(auxiliary_function),
        constants(std::move(constants)),
        actions(std::move(actions)),
        axioms(std::move(axioms))
    {
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    void clear() noexcept
    {
        static_predicates.clear();
        fluent_predicates.clear();
        derived_predicates.clear();
        static_functions.clear();
        fluent_functions.clear();
        auxiliary_function = std::nullopt;
        constants.clear();
        actions.clear();
        axioms.clear();
    }

    template<formalism::FactKind T>
    const auto& get_predicates() const
    {
        if constexpr (std::same_as<T, formalism::StaticTag>)
            return static_predicates;
        else if constexpr (std::same_as<T, formalism::FluentTag>)
            return fluent_predicates;
        else if constexpr (std::same_as<T, formalism::DerivedTag>)
            return derived_predicates;
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

    auto cista_members() const noexcept
    {
        return std::tie(index,
                        name,
                        static_predicates,
                        fluent_predicates,
                        derived_predicates,
                        static_functions,
                        fluent_functions,
                        auxiliary_function,
                        constants,
                        actions,
                        axioms);
    }
    auto identifying_members() const noexcept
    {
        return std::tie(name,
                        static_predicates,
                        fluent_predicates,
                        derived_predicates,
                        static_functions,
                        fluent_functions,
                        auxiliary_function,
                        constants,
                        actions,
                        axioms);
    }
};
}

#endif
