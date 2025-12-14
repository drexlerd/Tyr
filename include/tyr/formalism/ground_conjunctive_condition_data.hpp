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

#ifndef TYR_FORMALISM_GROUND_CONJUNCTIVE_CONDITION_DATA_HPP_
#define TYR_FORMALISM_GROUND_CONJUNCTIVE_CONDITION_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/binding_index.hpp"
#include "tyr/formalism/boolean_operator_data.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_conjunctive_condition_index.hpp"
#include "tyr/formalism/ground_literal_index.hpp"

namespace tyr
{
template<>
struct Data<formalism::GroundConjunctiveCondition>
{
    using Tag = formalism::GroundConjunctiveCondition;

    Index<formalism::GroundConjunctiveCondition> index;
    IndexList<formalism::GroundLiteral<formalism::StaticTag>> static_literals;
    IndexList<formalism::GroundLiteral<formalism::FluentTag>> fluent_literals;
    DataList<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>> numeric_constraints;

    Data() = default;
    Data(Index<formalism::GroundConjunctiveCondition> index,
         IndexList<formalism::GroundLiteral<formalism::StaticTag>> static_literals,
         IndexList<formalism::GroundLiteral<formalism::FluentTag>> fluent_literals,
         IndexList<formalism::GroundLiteral<formalism::DerivedTag>> derived_literals,
         DataList<formalism::BooleanOperator<Data<formalism::GroundFunctionExpression>>> numeric_constraints) :
        index(index),
        static_literals(std::move(static_literals)),
        fluent_literals(std::move(fluent_literals)),
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
        tyr::clear(fluent_literals);
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

    auto cista_members() const noexcept { return std::tie(index, static_literals, fluent_literals, numeric_constraints); }
    auto identifying_members() const noexcept { return std::tie(static_literals, fluent_literals, numeric_constraints); }
};
}

#endif
