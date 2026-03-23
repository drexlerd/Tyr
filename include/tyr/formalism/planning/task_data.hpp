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

#ifndef TYR_FORMALISM_PLANNING_TASK_DATA_HPP_
#define TYR_FORMALISM_PLANNING_TASK_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/planning/axiom_index.hpp"
#include "tyr/formalism/planning/domain_index.hpp"
#include "tyr/formalism/planning/ground_atom_index.hpp"
#include "tyr/formalism/planning/ground_conjunctive_condition_index.hpp"
#include "tyr/formalism/planning/ground_function_term_value_index.hpp"
#include "tyr/formalism/planning/metric_index.hpp"
#include "tyr/formalism/planning/task_index.hpp"
#include "tyr/formalism/predicate_index.hpp"

namespace tyr
{

template<>
struct Data<formalism::planning::Task>
{
    Index<formalism::planning::Task> index;
    ::cista::offset::string name;
    Index<formalism::planning::Domain> domain;
    IndexList<formalism::Predicate<formalism::DerivedTag>> derived_predicates;
    IndexList<formalism::Object> objects;
    IndexList<formalism::planning::GroundAtom<formalism::StaticTag>> static_atoms;
    IndexList<formalism::planning::GroundAtom<formalism::FluentTag>> fluent_atoms;
    IndexList<formalism::planning::GroundFunctionTermValue<formalism::StaticTag>> static_fterm_values;
    IndexList<formalism::planning::GroundFunctionTermValue<formalism::FluentTag>> fluent_fterm_values;
    ::cista::optional<Index<formalism::planning::GroundFunctionTermValue<formalism::AuxiliaryTag>>> auxiliary_fterm_value;
    Index<formalism::planning::GroundConjunctiveCondition> goal;
    ::cista::optional<Index<formalism::planning::Metric>> metric;
    IndexList<formalism::planning::Axiom> axioms;

    Data() = default;
    Data(::cista::offset::string name_,
         Index<formalism::planning::Domain> domain_,
         IndexList<formalism::Predicate<formalism::DerivedTag>> derived_predicates_,
         IndexList<formalism::Object> objects_,
         IndexList<formalism::planning::GroundAtom<formalism::StaticTag>> static_atoms_,
         IndexList<formalism::planning::GroundAtom<formalism::FluentTag>> fluent_atoms_,
         IndexList<formalism::planning::GroundFunctionTermValue<formalism::StaticTag>> static_fterm_values_,
         IndexList<formalism::planning::GroundFunctionTermValue<formalism::FluentTag>> fluent_fterm_values_,
         ::cista::optional<Index<formalism::planning::GroundFunctionTermValue<formalism::AuxiliaryTag>>> auxiliary_fterm_value_,
         Index<formalism::planning::GroundConjunctiveCondition> goal_,
         ::cista::optional<Index<formalism::planning::Metric>> metric_,
         IndexList<formalism::planning::Axiom> axioms_) :
        index(),
        name(std::move(name_)),
        domain(domain_),
        derived_predicates(std::move(derived_predicates_)),
        objects(std::move(objects_)),
        static_atoms(std::move(static_atoms_)),
        fluent_atoms(std::move(fluent_atoms_)),
        static_fterm_values(std::move(static_fterm_values_)),
        fluent_fterm_values(std::move(fluent_fterm_values_)),
        auxiliary_fterm_value(auxiliary_fterm_value_),
        goal(goal_),
        metric(metric_),
        axioms(std::move(axioms_))
    {
    }
    // Python constructor
    template<typename C>
    Data(const std::string& name_,
         View<Index<formalism::planning::Domain>, C> domain_,
         const std::vector<View<Index<formalism::Predicate<formalism::DerivedTag>>, C>>& derived_predicates_,
         const std::vector<View<Index<formalism::Object>, C>>& objects_,
         const std::vector<View<Index<formalism::planning::GroundAtom<formalism::StaticTag>>, C>>& static_atoms_,
         const std::vector<View<Index<formalism::planning::GroundAtom<formalism::FluentTag>>, C>>& fluent_atoms_,
         const std::vector<View<Index<formalism::planning::GroundFunctionTermValue<formalism::StaticTag>>, C>>& static_fterm_values_,
         const std::vector<View<Index<formalism::planning::GroundFunctionTermValue<formalism::FluentTag>>, C>>& fluent_fterm_values_,
         const std::optional<View<Index<formalism::planning::GroundFunctionTermValue<formalism::AuxiliaryTag>>, C>>& auxiliary_fterm_value_,
         View<Index<formalism::planning::GroundConjunctiveCondition>, C> goal_,
         const std::optional<View<Index<formalism::planning::Metric>, C>>& metric_,
         const std::vector<View<Index<formalism::planning::Axiom>, C>>& axioms_) :
        index(),
        name(name_),
        domain(),
        derived_predicates(),
        objects(),
        static_atoms(),
        fluent_atoms(),
        static_fterm_values(),
        fluent_fterm_values(),
        auxiliary_fterm_value(),
        goal(),
        metric(),
        axioms()
    {
        set(domain_, domain);
        set(derived_predicates_, derived_predicates);
        set(objects_, objects);
        set(static_atoms_, static_atoms);
        set(fluent_atoms_, fluent_atoms);
        set(static_fterm_values_, static_fterm_values);
        set(fluent_fterm_values_, fluent_fterm_values);
        set(auxiliary_fterm_value_, auxiliary_fterm_value);
        set(goal_, goal);
        set(metric_, metric);
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
        tyr::clear(domain);
        tyr::clear(derived_predicates);
        tyr::clear(objects);
        tyr::clear(static_atoms);
        tyr::clear(fluent_atoms);
        tyr::clear(static_fterm_values);
        tyr::clear(fluent_fterm_values);
        tyr::clear(auxiliary_fterm_value);
        tyr::clear(goal);
        tyr::clear(metric);
        tyr::clear(axioms);
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
                        name,
                        domain,
                        derived_predicates,
                        objects,
                        static_atoms,
                        fluent_atoms,
                        static_fterm_values,
                        fluent_fterm_values,
                        auxiliary_fterm_value,
                        goal,
                        metric,
                        axioms);
    }
    auto identifying_members() const noexcept
    {
        return std::tie(name,
                        domain,
                        derived_predicates,
                        objects,
                        static_atoms,
                        fluent_atoms,
                        static_fterm_values,
                        fluent_fterm_values,
                        auxiliary_fterm_value,
                        goal,
                        metric,
                        axioms);
    }
};

static_assert(!uses_trivial_storage_v<formalism::planning::Task>);
}

#endif
