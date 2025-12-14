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

#ifndef TYR_FORMALISM_PLANNING_GROUND_FDR_CONJUNCTIVE_CONDITION_INDEX_HPP_
#define TYR_FORMALISM_PLANNING_GROUND_FDR_CONJUNCTIVE_CONDITION_INDEX_HPP_

#include "tyr/common/index_mixins.hpp"
#include "tyr/common/types.hpp"
#include "tyr/formalism/declarations.hpp"

namespace tyr
{
template<>
struct Index<formalism::GroundFDRConjunctiveCondition> : IndexMixin<Index<formalism::GroundFDRConjunctiveCondition>>
{
    using Tag = formalism::GroundFDRConjunctiveCondition;

    // Inherit constructors
    using Base = IndexMixin<Index<formalism::GroundFDRConjunctiveCondition>>;
    using Base::Base;
};

}

#endif
