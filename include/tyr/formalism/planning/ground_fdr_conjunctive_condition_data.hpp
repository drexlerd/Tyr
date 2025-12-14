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

#ifndef TYR_FORMALISM_PLANNING_GROUND_FDR_CONJUNCTIVE_CONDITION_DATA_HPP_
#define TYR_FORMALISM_PLANNING_GROUND_FDR_CONJUNCTIVE_CONDITION_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/binding_index.hpp"
#include "tyr/formalism/boolean_operator_data.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_literal_index.hpp"
#include "tyr/formalism/planning/fdr_fact_data.hpp"
#include "tyr/formalism/planning/ground_fdr_conjunctive_condition_index.hpp"

namespace tyr
{
template<>
struct Data<formalism::GroundFDRConjunctiveCondition>
{
    using Tag = formalism::GroundFDRConjunctiveCondition;

    Index<formalism::GroundFDRConjunctiveCondition> index;
    IndexList<formalism::GroundLiteral<formalism::StaticTag>> static_literals;
    DataList<formalism::FDRFact<formalism::FluentTag>> fluent_facts;
    IndexList<formalism::GroundLiteral<formalism::DerivedTag>> derived_literals;
    DataList<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>> numeric_constraints;

    Data() = default;
    Data(Index<formalism::GroundFDRConjunctiveCondition> index,
         IndexList<formalism::GroundLiteral<formalism::StaticTag>> static_literals,
         DataList<formalism::FDRFact<formalism::FluentTag>> fluent_facts,
         IndexList<formalism::GroundLiteral<formalism::DerivedTag>> derived_literals,
         DataList<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>> numeric_constraints) :
        index(index),
        static_literals(std::move(static_literals)),
        fluent_facts(std::move(fluent_facts)),
        derived_literals(std::move(derived_literals)),
        numeric_constraints(std::move(numeric_constraints))
    {
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    void clear() noexcept
    {
        tyr::clear(index);
        tyr::clear(static_literals);
        tyr::clear(fluent_facts);
        tyr::clear(derived_literals);
        tyr::clear(numeric_constraints);
    }

    template<formalism::FactKind T>
    const auto& get_facts() const
    {
        if constexpr (std::same_as<T, formalism::StaticTag>)
            return static_literals;
        else if constexpr (std::same_as<T, formalism::FluentTag>)
            return fluent_facts;
        else if constexpr (std::same_as<T, formalism::DerivedTag>)
            return derived_literals;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }

    auto cista_members() const noexcept { return std::tie(index, fluent_facts, static_literals, derived_literals, numeric_constraints); }
    auto identifying_members() const noexcept { return std::tie(fluent_facts, static_literals, derived_literals, numeric_constraints); }
};
}

#endif
