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
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/fdr_fact_data.hpp"
#include "tyr/formalism/planning/ground_conjunctive_effect_index.hpp"
#include "tyr/formalism/planning/ground_literal_index.hpp"
#include "tyr/formalism/planning/ground_numeric_effect_operator_data.hpp"

namespace tyr
{

template<>
struct Data<formalism::planning::GroundConjunctiveEffect>
{
    Index<formalism::planning::GroundConjunctiveEffect> index;
    DataList<formalism::planning::FDRFact<formalism::FluentTag>> facts;
    DataList<formalism::planning::GroundNumericEffectOperator<formalism::FluentTag>> numeric_effects;
    ::cista::optional<Data<formalism::planning::GroundNumericEffectOperator<formalism::AuxiliaryTag>>> auxiliary_numeric_effect;  // :action-cost

    Data() = default;
    Data(DataList<formalism::planning::FDRFact<formalism::FluentTag>> facts_,
         DataList<formalism::planning::GroundNumericEffectOperator<formalism::FluentTag>> numeric_effects_,
         ::cista::optional<Data<formalism::planning::GroundNumericEffectOperator<formalism::AuxiliaryTag>>> auxiliary_numeric_effect_) :
        index(),
        facts(std::move(facts_)),
        numeric_effects(std::move(numeric_effects_)),
        auxiliary_numeric_effect(std::move(auxiliary_numeric_effect_))
    {
    }
    // Python constructor
    template<typename C>
    Data(const std::vector<View<Data<formalism::planning::FDRFact<formalism::FluentTag>>, C>>& facts_,
         const std::vector<View<Data<formalism::planning::GroundNumericEffectOperator<formalism::FluentTag>>, C>>& numeric_effects_,
         const std::optional<View<Data<formalism::planning::GroundNumericEffectOperator<formalism::AuxiliaryTag>>, C>>& auxiliary_numeric_effect_) :
        index(),
        facts(),
        numeric_effects(),
        auxiliary_numeric_effect()
    {
        set(facts_, facts);
        set(numeric_effects_, numeric_effects);
        set(auxiliary_numeric_effect_, auxiliary_numeric_effect);
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

static_assert(!uses_trivial_storage_v<formalism::planning::GroundConjunctiveEffect>);
}

#endif
