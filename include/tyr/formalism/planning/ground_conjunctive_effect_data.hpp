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

#ifndef TYR_FORMALISM_PLANNING_GROUND_CONJUNCTIVE_EFFECT_DATA_HPP_
#define TYR_FORMALISM_PLANNING_GROUND_CONJUNCTIVE_EFFECT_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/ground_literal_index.hpp"
#include "tyr/formalism/planning/fdr_fact_data.hpp"
#include "tyr/formalism/planning/ground_conjunctive_effect_index.hpp"
#include "tyr/formalism/planning/ground_numeric_effect_operator_data.hpp"

namespace tyr
{

template<>
struct Data<formalism::GroundConjunctiveEffect>
{
    using Tag = formalism::GroundConjunctiveEffect;

    Index<formalism::GroundConjunctiveEffect> index;
    DataList<formalism::FDRFact<formalism::FluentTag>> facts;
    DataList<formalism::GroundNumericEffectOperator<formalism::FluentTag>> numeric_effects;
    ::cista::optional<Data<formalism::GroundNumericEffectOperator<formalism::AuxiliaryTag>>> auxiliary_numeric_effect;  // :action-cost

    Data() = default;
    Data(Index<formalism::GroundConjunctiveEffect> index,
         DataList<formalism::FDRFact<formalism::FluentTag>> facts,
         DataList<formalism::GroundNumericEffectOperator<formalism::FluentTag>> numeric_effects,
         ::cista::optional<Data<formalism::GroundNumericEffectOperator<formalism::AuxiliaryTag>>> auxiliary_numeric_effect) :
        index(index),
        facts(std::move(facts)),
        numeric_effects(std::move(numeric_effects)),
        auxiliary_numeric_effect(std::move(auxiliary_numeric_effect))
    {
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    void clear() noexcept
    {
        tyr::clear(index);
        tyr::clear(facts);
        tyr::clear(numeric_effects);
        tyr::clear(auxiliary_numeric_effect);
    }

    auto cista_members() const noexcept { return std::tie(index, facts, numeric_effects, auxiliary_numeric_effect); }
    auto identifying_members() const noexcept { return std::tie(facts, numeric_effects, auxiliary_numeric_effect); }
};
}

#endif
