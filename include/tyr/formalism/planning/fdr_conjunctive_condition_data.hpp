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

#ifndef TYR_FORMALISM_FDR_CONJUNCTIVE_CONDITION_DATA_HPP_
#define TYR_FORMALISM_FDR_CONJUNCTIVE_CONDITION_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/binding_index.hpp"
#include "tyr/formalism/boolean_operator_data.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/planning/fdr_conjunctive_condition_index.hpp"
#include "tyr/formalism/planning/fdr_fact_data.hpp"

namespace tyr
{
template<>
struct Data<formalism::FDRConjunctiveCondition>
{
    using Tag = formalism::FDRConjunctiveCondition;

    Index<formalism::FDRConjunctiveCondition> index;
    DataList<formalism::FDRFact<formalism::FluentTag>> fluent_facts;
    DataList<formalism::FDRFact<formalism::DerivedTag>> derived_facts;
    DataList<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>> numeric_constraints;

    Data() = default;
    Data(Index<formalism::FDRConjunctiveCondition> index,
         DataList<formalism::FDRFact<formalism::FluentTag>> fluent_facts,
         DataList<formalism::FDRFact<formalism::DerivedTag>> derived_facts,
         DataList<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>> numeric_constraints) :
        index(index),
        fluent_facts(std::move(fluent_facts)),
        derived_facts(std::move(derived_facts)),
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
        tyr::clear(fluent_facts);
        tyr::clear(derived_facts);
        tyr::clear(numeric_constraints);
    }

    template<formalism::FactKind T>
    const auto& get_facts() const
    {
        if constexpr (std::same_as<T, formalism::FluentTag>)
            return fluent_facts;
        else if constexpr (std::same_as<T, formalism::DerivedTag>)
            return derived_facts;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }

    auto cista_members() const noexcept { return std::tie(index, fluent_facts, derived_facts, numeric_constraints); }
    auto identifying_members() const noexcept { return std::tie(fluent_facts, derived_facts, numeric_constraints); }
};
}

#endif
