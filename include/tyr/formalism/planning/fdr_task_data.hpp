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

#ifndef TYR_FORMALISM_PLANNING_FDR_TASK_DATA_HPP_
#define TYR_FORMALISM_PLANNING_FDR_TASK_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_atom_index.hpp"
#include "tyr/formalism/ground_function_term_value_index.hpp"
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/planning/axiom_index.hpp"
#include "tyr/formalism/planning/domain_index.hpp"
#include "tyr/formalism/planning/fdr_fact_data.hpp"
#include "tyr/formalism/planning/fdr_task_index.hpp"
#include "tyr/formalism/planning/fdr_variable_index.hpp"
#include "tyr/formalism/planning/ground_action_index.hpp"
#include "tyr/formalism/planning/ground_axiom_index.hpp"
#include "tyr/formalism/planning/ground_fdr_conjunctive_condition_index.hpp"
#include "tyr/formalism/planning/metric_index.hpp"
#include "tyr/formalism/predicate_index.hpp"

namespace tyr
{

template<>
struct Data<formalism::FDRTask>
{
    using Tag = formalism::FDRTask;

    Index<formalism::FDRTask> index;
    ::cista::offset::string name;
    Index<formalism::Domain> domain;
    IndexList<formalism::Predicate<formalism::DerivedTag>> derived_predicates;
    IndexList<formalism::Object> objects;
    IndexList<formalism::GroundAtom<formalism::StaticTag>> static_atoms;
    IndexList<formalism::GroundAtom<formalism::FluentTag>> fluent_atoms;
    IndexList<formalism::GroundAtom<formalism::DerivedTag>> derived_atoms;
    IndexList<formalism::GroundFunctionTermValue<formalism::StaticTag>> static_fterm_values;
    IndexList<formalism::GroundFunctionTermValue<formalism::FluentTag>> fluent_fterm_values;
    ::cista::optional<Index<formalism::GroundFunctionTermValue<formalism::AuxiliaryTag>>> auxiliary_fterm_value;
    ::cista::optional<Index<formalism::Metric>> metric;
    IndexList<formalism::Axiom> axioms;

    /// FDR-related
    IndexList<formalism::FDRVariable<formalism::FluentTag>> fluent_variables;
    IndexList<formalism::FDRVariable<formalism::DerivedTag>> derived_variables;
    DataList<formalism::FDRFact<formalism::FluentTag>> fluent_facts;
    Index<formalism::GroundFDRConjunctiveCondition> goal;
    IndexList<formalism::GroundAction> ground_actions;
    IndexList<formalism::GroundAxiom> ground_axioms;

    Data() = default;
    Data(Index<formalism::FDRTask> index,
         ::cista::offset::string name,
         Index<formalism::Domain> domain,
         IndexList<formalism::Predicate<formalism::DerivedTag>> derived_predicates,
         IndexList<formalism::Object> objects,
         IndexList<formalism::GroundAtom<formalism::StaticTag>> static_atoms,
         IndexList<formalism::GroundAtom<formalism::FluentTag>> fluent_atoms,
         IndexList<formalism::GroundAtom<formalism::DerivedTag>> derived_atoms,
         IndexList<formalism::GroundFunctionTermValue<formalism::StaticTag>> static_fterm_values,
         IndexList<formalism::GroundFunctionTermValue<formalism::FluentTag>> fluent_fterm_values,
         ::cista::optional<Index<formalism::GroundFunctionTermValue<formalism::AuxiliaryTag>>> auxiliary_fterm_value,
         ::cista::optional<Index<formalism::Metric>> metric,
         IndexList<formalism::Axiom> axioms,
         IndexList<formalism::FDRVariable<formalism::FluentTag>> fluent_variables,
         IndexList<formalism::FDRVariable<formalism::DerivedTag>> derived_variables,
         DataList<formalism::FDRFact<formalism::FluentTag>> fluent_facts,
         Index<formalism::GroundFDRConjunctiveCondition> goal,
         IndexList<formalism::GroundAction> ground_actions,
         IndexList<formalism::GroundAxiom> ground_axioms) :
        index(index),
        name(std::move(name)),
        domain(domain),
        derived_predicates(std::move(derived_predicates)),
        objects(std::move(objects)),
        static_atoms(std::move(static_atoms)),
        fluent_atoms(std::move(fluent_atoms)),
        derived_atoms(std::move(derived_atoms)),
        static_fterm_values(std::move(static_fterm_values)),
        fluent_fterm_values(std::move(fluent_fterm_values)),
        auxiliary_fterm_value(auxiliary_fterm_value),
        metric(metric),
        axioms(std::move(axioms)),
        fluent_variables(std::move(fluent_variables)),
        derived_variables(std::move(derived_variables)),
        fluent_facts(std::move(fluent_facts)),
        goal(goal),
        ground_actions(std::move(ground_actions)),
        ground_axioms(std::move(ground_axioms))
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
        tyr::clear(derived_atoms);
        tyr::clear(static_fterm_values);
        tyr::clear(fluent_fterm_values);
        tyr::clear(auxiliary_fterm_value);
        tyr::clear(metric);
        tyr::clear(axioms);
        tyr::clear(fluent_variables);
        tyr::clear(derived_variables);
        tyr::clear(fluent_facts);
        tyr::clear(goal);
        tyr::clear(ground_actions);
        tyr::clear(ground_axioms);
    }

    template<formalism::FactKind T>
    const auto& get_atoms() const
    {
        if constexpr (std::same_as<T, formalism::StaticTag>)
            return static_atoms;
        else if constexpr (std::same_as<T, formalism::FluentTag>)
            return fluent_atoms;
        else if constexpr (std::same_as<T, formalism::DerivedTag>)
            return derived_atoms;
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

    template<formalism::FactKind T>
    const auto& get_variables() const
    {
        if constexpr (std::same_as<T, formalism::FluentTag>)
            return fluent_variables;
        else if constexpr (std::same_as<T, formalism::DerivedTag>)
            return derived_variables;
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
                        derived_atoms,
                        static_fterm_values,
                        fluent_fterm_values,
                        auxiliary_fterm_value,
                        metric,
                        axioms,
                        fluent_variables,
                        derived_variables,
                        fluent_facts,
                        goal,
                        ground_actions,
                        ground_axioms);
    }
    auto identifying_members() const noexcept
    {
        return std::tie(name,
                        domain,
                        derived_predicates,
                        objects,
                        static_atoms,
                        fluent_atoms,
                        derived_atoms,
                        static_fterm_values,
                        fluent_fterm_values,
                        auxiliary_fterm_value,
                        metric,
                        axioms,
                        fluent_variables,
                        derived_variables,
                        fluent_facts,
                        goal,
                        ground_actions,
                        ground_axioms);
    }
};
}

#endif
