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
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/function_index.hpp"
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/planning/action_index.hpp"
#include "tyr/formalism/planning/axiom_index.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/domain_index.hpp"
#include "tyr/formalism/planning/task_index.hpp"
#include "tyr/formalism/predicate_index.hpp"

namespace tyr
{

template<>
struct Data<formalism::planning::Domain>
{
    Index<formalism::planning::Domain> index;
    ::cista::offset::string name;
    IndexList<formalism::Predicate<formalism::StaticTag>> static_predicates;
    IndexList<formalism::Predicate<formalism::FluentTag>> fluent_predicates;
    IndexList<formalism::Predicate<formalism::DerivedTag>> derived_predicates;
    IndexList<formalism::Function<formalism::StaticTag>> static_functions;
    IndexList<formalism::Function<formalism::FluentTag>> fluent_functions;
    ::cista::optional<Index<formalism::Function<formalism::AuxiliaryTag>>> auxiliary_function;
    IndexList<formalism::Object> constants;
    IndexList<formalism::planning::Action> actions;
    IndexList<formalism::planning::Axiom> axioms;

    Data() = default;
    Data(::cista::offset::string name_,
         IndexList<formalism::Predicate<formalism::StaticTag>> static_predicates_,
         IndexList<formalism::Predicate<formalism::FluentTag>> fluent_predicates_,
         IndexList<formalism::Predicate<formalism::DerivedTag>> derived_predicates_,
         IndexList<formalism::Function<formalism::StaticTag>> static_functions_,
         IndexList<formalism::Function<formalism::FluentTag>> fluent_functions_,
         ::cista::optional<Index<formalism::Function<formalism::AuxiliaryTag>>> auxiliary_function_,
         IndexList<formalism::Object> constants_,
         IndexList<formalism::planning::Action> actions_,
         IndexList<formalism::planning::Axiom> axioms_) :
        index(),
        name(std::move(name_)),
        static_predicates(std::move(static_predicates_)),
        fluent_predicates(std::move(fluent_predicates_)),
        derived_predicates(std::move(derived_predicates_)),
        static_functions(std::move(static_functions_)),
        fluent_functions(std::move(fluent_functions_)),
        auxiliary_function(auxiliary_function_),
        constants(std::move(constants_)),
        actions(std::move(actions_)),
        axioms(std::move(axioms_))
    {
    }
    // Python constructor
    template<typename C>
    Data(const std::string& name_,
         const std::vector<View<Index<formalism::Predicate<formalism::StaticTag>>, C>>& static_predicates_,
         const std::vector<View<Index<formalism::Predicate<formalism::FluentTag>>, C>>& fluent_predicates_,
         const std::vector<View<Index<formalism::Predicate<formalism::DerivedTag>>, C>>& derived_predicates_,
         const std::vector<View<Index<formalism::Function<formalism::StaticTag>>, C>>& static_functions_,
         const std::vector<View<Index<formalism::Function<formalism::FluentTag>>, C>>& fluent_functions_,
         const std::optional<View<Index<formalism::Function<formalism::AuxiliaryTag>>, C>>& auxiliary_function_,
         const std::vector<View<Index<formalism::Object>, C>>& constants_,
         const std::vector<View<Index<formalism::planning::Action>, C>>& actions_,
         const std::vector<View<Index<formalism::planning::Axiom>, C>>& axioms_) :
        index(),
        name(name_),
        static_predicates(),
        fluent_predicates(),
        derived_predicates(),
        static_functions(),
        fluent_functions(),
        auxiliary_function(),
        constants(),
        actions(),
        axioms()
    {
        set(static_predicates_, static_predicates);
        set(fluent_predicates_, fluent_predicates);
        set(derived_predicates_, derived_predicates);
        set(static_functions_, static_functions);
        set(fluent_functions_, fluent_functions);
        set(auxiliary_function_, auxiliary_function);
        set(constants_, constants);
        set(actions_, actions);
        set(axioms_, axioms);
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    void clear() noexcept
    {
        tyr::clear(index);
        tyr::clear(name);
        tyr::clear(static_predicates);
        tyr::clear(fluent_predicates);
        tyr::clear(derived_predicates);
        tyr::clear(static_functions);
        tyr::clear(fluent_functions);
        tyr::clear(auxiliary_function);
        tyr::clear(constants);
        tyr::clear(actions);
        tyr::clear(axioms);
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

static_assert(!uses_trivial_storage_v<formalism::planning::Domain>);
}

#endif
