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

#ifndef TYR_FORMALISM_PLANNING_FDR_CONJUNCTIVE_CONDITION_DATA_HPP_
#define TYR_FORMALISM_PLANNING_FDR_CONJUNCTIVE_CONDITION_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/planning/boolean_operator_data.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/fdr_conjunctive_condition_index.hpp"
#include "tyr/formalism/planning/literal_index.hpp"
#include "tyr/formalism/variable_index.hpp"

namespace tyr
{
template<>
struct Data<formalism::planning::FDRConjunctiveCondition>
{
    Index<formalism::planning::FDRConjunctiveCondition> index;
    IndexList<formalism::Variable> variables;
    IndexList<formalism::planning::Literal<formalism::StaticTag>> static_literals;
    IndexList<formalism::planning::Literal<formalism::FluentTag>> fluent_literals;
    IndexList<formalism::planning::Literal<formalism::DerivedTag>> derived_literals;
    DataList<formalism::planning::BooleanOperator<Data<formalism::planning::FunctionExpression>>> numeric_constraints;

    Data() = default;
    Data(Index<formalism::planning::FDRConjunctiveCondition> index,
         IndexList<formalism::Variable> variables,
         IndexList<formalism::planning::Literal<formalism::StaticTag>> static_literals,
         IndexList<formalism::planning::Literal<formalism::FluentTag>> fluent_literals,
         IndexList<formalism::planning::Literal<formalism::DerivedTag>> derived_literals,
         DataList<formalism::planning::BooleanOperator<Data<formalism::planning::FunctionExpression>>> numeric_constraints) :
        index(index),
        variables(std::move(variables)),
        static_literals(std::move(static_literals)),
        fluent_literals(std::move(fluent_literals)),
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
        tyr::clear(variables);
        tyr::clear(static_literals);
        tyr::clear(fluent_literals);
        tyr::clear(derived_literals);
        tyr::clear(numeric_constraints);
    }

    template<formalism::FactKind T>
    const auto& get_literals() const
    {
        if constexpr (std::same_as<T, formalism::StaticTag>)
            return static_literals;
        else if constexpr (std::same_as<T, formalism::FluentTag>)
            return fluent_literals;
        else if constexpr (std::same_as<T, formalism::DerivedTag>)
            return derived_literals;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }

    auto cista_members() const noexcept { return std::tie(index, variables, static_literals, fluent_literals, derived_literals, numeric_constraints); }
    auto identifying_members() const noexcept { return std::tie(variables, static_literals, fluent_literals, derived_literals, numeric_constraints); }
};
}

#endif
