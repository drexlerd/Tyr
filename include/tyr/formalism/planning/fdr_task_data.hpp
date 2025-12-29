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
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/planning/axiom_index.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/domain_index.hpp"
#include "tyr/formalism/planning/fdr_fact_data.hpp"
#include "tyr/formalism/planning/fdr_task_index.hpp"
#include "tyr/formalism/planning/fdr_variable_index.hpp"
#include "tyr/formalism/planning/ground_action_index.hpp"
#include "tyr/formalism/planning/ground_atom_index.hpp"
#include "tyr/formalism/planning/ground_axiom_index.hpp"
#include "tyr/formalism/planning/ground_fdr_conjunctive_condition_index.hpp"
#include "tyr/formalism/planning/ground_function_term_index.hpp"
#include "tyr/formalism/planning/ground_function_term_value_index.hpp"
#include "tyr/formalism/planning/metric_index.hpp"
#include "tyr/formalism/predicate_index.hpp"

namespace tyr
{

template<>
struct Data<formalism::planning::FDRTask>
{
    Index<formalism::planning::FDRTask> index;
    ::cista::offset::string name;
    Index<formalism::planning::Domain> domain;
    IndexList<formalism::Predicate<formalism::DerivedTag>> derived_predicates;
    IndexList<formalism::Object> objects;
    IndexList<formalism::planning::GroundAtom<formalism::StaticTag>> static_atoms;
    IndexList<formalism::planning::GroundAtom<formalism::FluentTag>> fluent_atoms;
    IndexList<formalism::planning::GroundAtom<formalism::DerivedTag>> derived_atoms;
    IndexList<formalism::planning::GroundFunctionTerm<formalism::StaticTag>> static_fterms;
    IndexList<formalism::planning::GroundFunctionTerm<formalism::FluentTag>> fluent_fterms;
    ::cista::optional<Index<formalism::planning::GroundFunctionTerm<formalism::AuxiliaryTag>>> auxiliary_fterm;
    IndexList<formalism::planning::GroundFunctionTermValue<formalism::StaticTag>> static_fterm_values;
    IndexList<formalism::planning::GroundFunctionTermValue<formalism::FluentTag>> fluent_fterm_values;
    ::cista::optional<Index<formalism::planning::GroundFunctionTermValue<formalism::AuxiliaryTag>>> auxiliary_fterm_value;
    ::cista::optional<Index<formalism::planning::Metric>> metric;
    IndexList<formalism::planning::Axiom> axioms;

    /// FDR-related
    IndexList<formalism::planning::FDRVariable<formalism::FluentTag>> fluent_variables;
    DataList<formalism::planning::FDRFact<formalism::FluentTag>> fluent_facts;
    Index<formalism::planning::GroundFDRConjunctiveCondition> goal;
    IndexList<formalism::planning::GroundAction> ground_actions;
    IndexList<formalism::planning::GroundAxiom> ground_axioms;

    Data() = default;
    Data(Index<formalism::planning::FDRTask> index,
         ::cista::offset::string name,
         Index<formalism::planning::Domain> domain,
         IndexList<formalism::Predicate<formalism::DerivedTag>> derived_predicates,
         IndexList<formalism::Object> objects,
         IndexList<formalism::planning::GroundAtom<formalism::StaticTag>> static_atoms,
         IndexList<formalism::planning::GroundAtom<formalism::FluentTag>> fluent_atoms,
         IndexList<formalism::planning::GroundAtom<formalism::DerivedTag>> derived_atoms,
         IndexList<formalism::planning::GroundFunctionTerm<formalism::StaticTag>> static_fterms,
         IndexList<formalism::planning::GroundFunctionTerm<formalism::FluentTag>> fluent_fterms,
         ::cista::optional<Index<formalism::planning::GroundFunctionTerm<formalism::AuxiliaryTag>>> auxiliary_fterm,
         IndexList<formalism::planning::GroundFunctionTermValue<formalism::StaticTag>> static_fterm_values,
         IndexList<formalism::planning::GroundFunctionTermValue<formalism::FluentTag>> fluent_fterm_values,
         ::cista::optional<Index<formalism::planning::GroundFunctionTermValue<formalism::AuxiliaryTag>>> auxiliary_fterm_value,
         ::cista::optional<Index<formalism::planning::Metric>> metric,
         IndexList<formalism::planning::Axiom> axioms,
         IndexList<formalism::planning::FDRVariable<formalism::FluentTag>> fluent_variables,
         DataList<formalism::planning::FDRFact<formalism::FluentTag>> fluent_facts,
         Index<formalism::planning::GroundFDRConjunctiveCondition> goal,
         IndexList<formalism::planning::GroundAction> ground_actions,
         IndexList<formalism::planning::GroundAxiom> ground_axioms) :
        index(index),
        name(std::move(name)),
        domain(domain),
        derived_predicates(std::move(derived_predicates)),
        objects(std::move(objects)),
        static_atoms(std::move(static_atoms)),
        fluent_atoms(std::move(fluent_atoms)),
        derived_atoms(std::move(derived_atoms)),
        static_fterms(std::move(static_fterms)),
        fluent_fterms(std::move(fluent_fterms)),
        auxiliary_fterm(auxiliary_fterm),
        static_fterm_values(std::move(static_fterm_values)),
        fluent_fterm_values(std::move(fluent_fterm_values)),
        auxiliary_fterm_value(auxiliary_fterm_value),
        metric(metric),
        axioms(std::move(axioms)),
        fluent_variables(std::move(fluent_variables)),
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
        tyr::clear(static_fterms);
        tyr::clear(fluent_fterms);
        tyr::clear(auxiliary_fterm);
        tyr::clear(static_fterm_values);
        tyr::clear(fluent_fterm_values);
        tyr::clear(auxiliary_fterm_value);
        tyr::clear(metric);
        tyr::clear(axioms);
        tyr::clear(fluent_variables);
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
    const auto& get_fterms() const
    {
        if constexpr (std::same_as<T, formalism::StaticTag>)
            return static_fterms;
        else if constexpr (std::same_as<T, formalism::FluentTag>)
            return fluent_fterms;
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

    const auto& get_fluent_variables() const { return fluent_variables; }

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
                        static_fterms,
                        fluent_fterms,
                        auxiliary_fterm,
                        static_fterm_values,
                        fluent_fterm_values,
                        auxiliary_fterm_value,
                        metric,
                        axioms,
                        fluent_variables,
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
                        static_fterms,
                        fluent_fterms,
                        auxiliary_fterm,
                        static_fterm_values,
                        fluent_fterm_values,
                        auxiliary_fterm_value,
                        metric,
                        axioms,
                        fluent_variables,
                        fluent_facts,
                        goal,
                        ground_actions,
                        ground_axioms);
    }
};
}

#endif
