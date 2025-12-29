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
#include "tyr/formalism/planning/ground_fdr_conjunctive_condition_index.hpp"
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
    Index<formalism::planning::GroundFDRConjunctiveCondition> goal;
    ::cista::optional<Index<formalism::planning::Metric>> metric;
    IndexList<formalism::planning::Axiom> axioms;

    Data() = default;
    Data(Index<formalism::planning::Task> index,
         ::cista::offset::string name,
         Index<formalism::planning::Domain> domain,
         IndexList<formalism::Predicate<formalism::DerivedTag>> derived_predicates,
         IndexList<formalism::Object> objects,
         IndexList<formalism::planning::GroundAtom<formalism::StaticTag>> static_atoms,
         IndexList<formalism::planning::GroundAtom<formalism::FluentTag>> fluent_atoms,
         IndexList<formalism::planning::GroundFunctionTermValue<formalism::StaticTag>> static_fterm_values,
         IndexList<formalism::planning::GroundFunctionTermValue<formalism::FluentTag>> fluent_fterm_values,
         ::cista::optional<Index<formalism::planning::GroundFunctionTermValue<formalism::AuxiliaryTag>>> auxiliary_fterm_value,
         Index<formalism::planning::GroundFDRConjunctiveCondition> goal,
         ::cista::optional<Index<formalism::planning::Metric>> metric,
         IndexList<formalism::planning::Axiom> axioms) :
        index(index),
        name(std::move(name)),
        domain(domain),
        derived_predicates(std::move(derived_predicates)),
        objects(std::move(objects)),
        static_atoms(std::move(static_atoms)),
        fluent_atoms(std::move(fluent_atoms)),
        static_fterm_values(std::move(static_fterm_values)),
        fluent_fterm_values(std::move(fluent_fterm_values)),
        auxiliary_fterm_value(auxiliary_fterm_value),
        goal(goal),
        metric(metric),
        axioms(std::move(axioms))
    {
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
}

#endif
