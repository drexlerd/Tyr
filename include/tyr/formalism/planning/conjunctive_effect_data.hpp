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

#ifndef TYR_FORMALISM_PLANNING_CONJUNCTIVE_EFFECT_DATA_HPP_
#define TYR_FORMALISM_PLANNING_CONJUNCTIVE_EFFECT_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/literal_index.hpp"
#include "tyr/formalism/planning/conjunctive_effect_index.hpp"
#include "tyr/formalism/planning/numeric_effect_operator_data.hpp"

namespace tyr
{

template<>
struct Data<formalism::ConjunctiveEffect>
{
    using Tag = formalism::ConjunctiveEffect;

    Index<formalism::ConjunctiveEffect> index;
    IndexList<formalism::Literal<formalism::FluentTag>> literals;
    DataList<formalism::NumericEffectOperator<formalism::FluentTag>> numeric_effects;
    ::cista::optional<Data<formalism::NumericEffectOperator<formalism::AuxiliaryTag>>> auxiliary_numeric_effect;  // :action-cost

    Data() = default;
    Data(Index<formalism::ConjunctiveEffect> index,
         IndexList<formalism::Literal<formalism::FluentTag>> literals,
         DataList<formalism::NumericEffectOperator<formalism::FluentTag>> numeric_effects,
         ::cista::optional<Data<formalism::NumericEffectOperator<formalism::AuxiliaryTag>>> auxiliary_numeric_effect) :
        index(index),
        literals(std::move(literals)),
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
        literals.clear();
        numeric_effects.clear();
        auxiliary_numeric_effect = std::nullopt;
    }

    auto cista_members() const noexcept { return std::tie(index, literals, numeric_effects, auxiliary_numeric_effect); }
    auto identifying_members() const noexcept { return std::tie(literals, numeric_effects, auxiliary_numeric_effect); }
};
}

#endif
